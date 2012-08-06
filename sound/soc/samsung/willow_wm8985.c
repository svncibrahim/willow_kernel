/*
 *  willow_wm8985.c
 *
 *  Copyright (c) 2009 Samsung Electronics Co. Ltd
 *  Author: Jaswinder Singh <jassi.brar@samsung.com>
 *
 *  This program is free software; you can redistribute  it and/or modify it
 *  under  the terms of  the GNU General  Public License as published by the
 *  Free Software Foundation;  either version 2 of the  License, or (at your
 *  option) any later version.
 */

#include <sound/soc.h>
#include <sound/pcm_params.h>
#include <linux/clk.h>

#include <asm/mach-types.h>
#include <mach/regs-clock.h>

#include "../codecs/wm8985.h"
#include "i2s.h"

//#include "s5pc1xx-i2s.h"

#define I2S_FMT_LEFT_JUSTIFIED	1

static int willow_hw_params(struct snd_pcm_substream *substream,
	struct snd_pcm_hw_params *params)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct snd_soc_dai *cpu_dai = rtd->cpu_dai;
	struct snd_soc_dai *codec_dai = rtd->codec_dai;
	unsigned int pll_out;
	int bfs, rfs, psr, ret;
	struct clk    *clk_epll;

	switch (params_format(params)) {
	case SNDRV_PCM_FORMAT_U8:
	case SNDRV_PCM_FORMAT_S8:
		bfs = 16;
		break;
	case SNDRV_PCM_FORMAT_U16_LE:
	case SNDRV_PCM_FORMAT_S16_LE:
		bfs = 32;
		break;
	default:
		return -EINVAL;
	}

	/* The Fvco for WM8985 PLLs must fall within [90,100]MHz.
	 * This criterion can't be met if we request PLL output
	 * as {8000x256, 64000x256, 11025x256}Hz.
	 * As a wayout, we rather change rfs to a minimum value that
	 * results in (params_rate(params) * rfs), and itself, acceptable
	 * to both - the CODEC and the CPU.
	 */
	switch (params_rate(params)) {
	case 16000:
	case 22050:
	case 32000:
	case 44100:
	case 48000:
	case 88200:
	case 96000:
		rfs = 256;
		break;
	case 64000:
		rfs = 384;
		break;
	case 8000:
	case 11025:
		rfs = 512;
		break;
	default:
		return -EINVAL;
	}
	pll_out = params_rate(params) * rfs;

	switch (pll_out) {
        case 4096000:
        case 5644800:
        case 6144000:
        case 8467200:
        case 9216000:
                psr = 8;
                break;
        case 8192000:
        case 11289600:
        case 12288000:
        case 16934400:
        case 18432000:
                psr = 4;
                break;
        case 22579200:
        case 24576000:
        case 33868800:
        case 36864000:
                psr = 2;
                break;
        case 67737600:
        case 73728000:
                psr = 1;
                break;
        default:
                printk("Not yet supported!\n");
                return -EINVAL;
        }

        clk_epll = clk_get(NULL, "fout_epll");
        if (IS_ERR(clk_epll)) {
                printk(KERN_ERR
                        "failed to get fout_epll\n");
                return -EBUSY;
        }

        pll_out *= psr;

        clk_set_rate(clk_epll, pll_out);

	/* Set the Codec DAI configuration */
	ret = snd_soc_dai_set_fmt(codec_dai,
#if I2S_FMT_LEFT_JUSTIFIED
					SND_SOC_DAIFMT_LEFT_J
#else
					SND_SOC_DAIFMT_I2S
#endif
					 | SND_SOC_DAIFMT_NB_NF
					 | SND_SOC_DAIFMT_CBS_CFS);
	if (ret < 0)
		return ret;

	/* Set the AP DAI configuration */
	ret = snd_soc_dai_set_fmt(cpu_dai,
#if I2S_FMT_LEFT_JUSTIFIED
					SND_SOC_DAIFMT_RIGHT_J
#else
					SND_SOC_DAIFMT_I2S
#endif
					 | SND_SOC_DAIFMT_NB_NF
					 | SND_SOC_DAIFMT_CBS_CFS);
	if (ret < 0)
		return ret;

        /* Set the AP BFS */
        ret = snd_soc_dai_set_clkdiv(cpu_dai, SAMSUNG_I2S_DIV_BCLK, bfs);

        if (ret < 0) {
          printk(KERN_ERR "willow : AP bfs setting error!\n");
          return ret;
        }

        ret = snd_soc_dai_set_sysclk(cpu_dai, SAMSUNG_I2S_RCLKSRC_0,
                                        params_rate(params), SND_SOC_CLOCK_OUT);
        if (ret < 0) {
          printk(KERN_ERR "willow : AP set_sysclk  setting error!\n");
          return ret;
	}

        ret = snd_soc_dai_set_sysclk(cpu_dai, SAMSUNG_I2S_CDCLK,
                                        0, SND_SOC_CLOCK_OUT);
        if (ret < 0){
          printk(KERN_ERR "willow : SAMSUNG_I2S_CDCLK  setting error!\n");
          return ret;
        }

	ret = snd_soc_dai_set_sysclk(cpu_dai, SAMSUNG_I2S_OPCLK,
                                        params_rate(params), MOD_OPCLK_PCLK);
        if (ret < 0) {
          printk(KERN_ERR "willow : AP OP_CLK setting error!\n");
          return ret;
	}

#if 0 //pll configurations are not required when 8580 is operating in slave mode 
	/* Set WM8580 to drive MCLK from its PLLA */
	ret = snd_soc_dai_set_clkdiv(codec_dai, WM8580_MCLK,
					WM8580_CLKSRC_PLLA);
	if (ret < 0)
		return ret;

	ret = snd_soc_dai_set_pll(codec_dai, WM8580_PLLA, 0,
					SMDK_WM8580_FREQ, pll_out);
	if (ret < 0)
		return ret;

#endif
	pll_out /= psr;
	ret = snd_soc_dai_set_sysclk(codec_dai, WM8985_CLKSRC_PLL,
				     pll_out, SND_SOC_CLOCK_IN);
	if (ret < 0)
		return ret;
	clk_put(clk_epll);
	return 0;
}

/*
 * WILLOW WM8985 DAI operations.
 */
static struct snd_soc_ops willow_ops = {
	.hw_params = willow_hw_params,
};

/* WILLOW Playback widgets */
static const struct snd_soc_dapm_widget wm8985_dapm_widgets_pbk[] = {
	SND_SOC_DAPM_HP("HP-L/R", NULL),
        SND_SOC_DAPM_SPK("Speaker-L/R", NULL),
};

/* WILLOW Capture widgets */
static const struct snd_soc_dapm_widget wm8985_dapm_widgets_cpt[] = {
	SND_SOC_DAPM_MIC("MicIn", NULL),
};

/* WILLOW-PAIFTX connections */
static const struct snd_soc_dapm_route audio_map_tx[] = {
	/* MicIn feeds AINL */
        {"RIN", NULL, "MicIn"},
        {"LIN", NULL, "MicIn"},
};

/* WILLOW-PAIFRX connections */
static const struct snd_soc_dapm_route audio_map_rx[] = {
	/* HP Left/Right */
        {"HP-L/R", NULL, "HPL"},
        {"HP-L/R", NULL, "HPR"},

        /* SPK Left/Right */
        {"Speaker-L/R", NULL, "SPKL"},
        {"Speaker-L/R", NULL, "SPKR"},
};

static int willow_wm8985_init_paiftx(struct snd_soc_pcm_runtime *rtd)
{
	struct snd_soc_codec *codec = rtd->codec;
	struct snd_soc_dapm_context *dapm = &codec->dapm;

	/* Add willow specific Capture widgets */
	snd_soc_dapm_new_controls(dapm, wm8985_dapm_widgets_cpt,
				  ARRAY_SIZE(wm8985_dapm_widgets_cpt));

	/* Set up PAIFTX audio path */
	snd_soc_dapm_add_routes(dapm, audio_map_tx, ARRAY_SIZE(audio_map_tx));

	/* signal a DAPM event */
	snd_soc_dapm_sync(dapm);

	return 0;
}

static int willow_wm8985_init_paifrx(struct snd_soc_pcm_runtime *rtd)
{
	struct snd_soc_codec *codec = rtd->codec;
	struct snd_soc_dapm_context *dapm = &codec->dapm;

	/* Add willow specific Playback widgets */
	snd_soc_dapm_new_controls(dapm, wm8985_dapm_widgets_pbk,
				  ARRAY_SIZE(wm8985_dapm_widgets_pbk));

	/* Set up PAIFRX audio path */
	snd_soc_dapm_add_routes(dapm, audio_map_rx, ARRAY_SIZE(audio_map_rx));

	/* signal a DAPM event */
	snd_soc_dapm_sync(dapm);

	return 0;
}

enum {
	PRI_PLAYBACK = 0,
	PRI_CAPTURE,
};

static struct snd_soc_dai_link willow_dai[] = {
	[PRI_PLAYBACK] = { /* Primary Playback i/f */
		.name = "WM8985 PAIF RX",
		.stream_name = "Playback",
		.cpu_dai_name = "samsung-i2s.0",
		.codec_dai_name = "wm8985-hifi",
		.platform_name = "samsung-audio",
		.codec_name = "wm8985.1-001a",
		.init = willow_wm8985_init_paifrx,
		.ops = &willow_ops,
	},
	[PRI_CAPTURE] = { /* Primary Capture i/f */
		.name = "WM8985 PAIF TX",
		.stream_name = "Capture",
		.cpu_dai_name = "samsung-i2s.0",
		.codec_dai_name = "wm8985-hifi",
		.platform_name = "samsung-audio",
		.codec_name = "wm8985.1-001a",
		.init = willow_wm8985_init_paiftx,
		.ops = &willow_ops,
	},
};

static struct snd_soc_card willow = {
	.name = "willow",
	.dai_link = willow_dai,
	.num_links = 2,
};

static struct platform_device *willow_snd_device;

static int __init willow_audio_init(void)
{
	int ret;
	char *str;

	willow_snd_device = platform_device_alloc("soc-audio", -1);
	if (!willow_snd_device)
		return -ENOMEM;

	platform_set_drvdata(willow_snd_device, &willow);
	ret = platform_device_add(willow_snd_device);

	if (ret)
		platform_device_put(willow_snd_device);

	return ret;
}
module_init(willow_audio_init);

static void __exit willow_audio_exit(void)
{
	platform_device_unregister(willow_snd_device);
}
module_exit(willow_audio_exit);

MODULE_AUTHOR("Jaswinder Singh, jassi.brar@samsung.com");
MODULE_DESCRIPTION("ALSA SoC WILLOW WM8985");
MODULE_LICENSE("GPL");