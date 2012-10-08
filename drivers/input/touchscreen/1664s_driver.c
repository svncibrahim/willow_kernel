/*
 *  Copyright (C) 2010, Samsung Electronics Co. Ltd. All Rights Reserved.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/input.h>
#include <linux/input/mt.h>
#include <linux/interrupt.h>
#include <linux/i2c.h>
#include <linux/delay.h>
#include <linux/earlysuspend.h>
#include <linux/slab.h>
#include <linux/gpio.h>
#include <linux/i2c/1664s_driver.h>
#include <asm/unaligned.h>
#include <linux/firmware.h>
#include <linux/string.h>

/* ATMEL Defined  Start */
#define OBJECT_TABLE_START_ADDRESS      7
#define OBJECT_TABLE_ELEMENT_SIZE       6

#define CMD_RESET_OFFSET                0
#define CMD_BACKUP_OFFSET               1
#define CMD_CALIBRATE_OFFSET    2
#define CMD_REPORTATLL_OFFSET   3
#define CMD_DEBUG_CTRL_OFFSET   4
#define CMD_DIAGNOSTIC_OFFSET   5


#define DETECT_MSG_MASK         0x80
#define PRESS_MSG_MASK                  0x40
#define RELEASE_MSG_MASK                0x20
#define MOVE_MSG_MASK                   0x10
#define AMPLITUDE_MSG_MASK      0x04
#define SUPPRESS_MSG_MASK               0x02

/* Slave addresses */
/* need to check +  */
#define MXT_APP_LOW             0x4a
#define MXT_APP_HIGH            0x4d
#define MXT_BOOT_LOW            0x24 //0x26
#define MXT_BOOT_HIGH           0x25 //0x27
/* need to check -  */

/* FIRMWARE NAME */
#define MXT_FW_NAME                     "tsp_atmel/mXT1664S.fw"
#define MXT_BOOT_VALUE          0xa5
#define MXT_BACKUP_VALUE                0x55

/* Bootloader mode status */
#define MXT_WAITING_BOOTLOAD_CMD        0xc0    /* valid 7 6 bit only */
#define MXT_WAITING_FRAME_DATA  0x80    /* valid 7 6 bit only */
#define MXT_FRAME_CRC_CHECK     0x02
#define MXT_FRAME_CRC_FAIL              0x03
#define MXT_FRAME_CRC_PASS              0x04
#define MXT_APP_CRC_FAIL                0x40    /* valid 7 8 bit only */
#define MXT_BOOT_STATUS_MASK    0x3f

/* Command to unlock bootloader */
#define MXT_UNLOCK_CMD_MSB              0xaa
#define MXT_UNLOCK_CMD_LSB              0xdc

#define ID_BLOCK_SIZE                   7

#define MXT_STATE_INACTIVE              -1
#define MXT_STATE_RELEASE               0
#define MXT_STATE_PRESS         1
#define MXT_STATE_MOVE          2

#define MAX_USING_FINGER_NUM 10

/* touch booster */
#if TOUCH_BOOSTER
#include <mach/cpufreq.h>
#define TOUCH_BOOSTER_TIME              3000
#define TOUCH_BOOSTER_LIMIT_CLK 500000

static bool tsp_press_status;
static bool touch_cpu_lock_status;
static int cpu_lv = -1;
#endif

/* use sumsize in T57 register */
#if USE_SUMSIZE
#define MAX_SUMSIZE     40
#define MAX_SINGLE_SUMSIZE              100

static uint16_t sum_size;
#endif
/* ATMEL Defined  end */

/* Firmware */
#if READ_FW_FROM_HEADER
static u8 firmware_mXT[] = {
        #include "mXT1664S-V0.5.2_.h"
};
#endif

#if ITDEV
static int driver_paused;
static int debug_enabled=0;
#endif

static int treat_median_error_status;

struct mxt_data *copy_data;
u8 max_report_id;
struct report_id_map_t *rid_map;
static bool rid_map_alloc;

int touch_is_pressed;
EXPORT_SYMBOL(touch_is_pressed);

static int mxt_enabled;
#if SYSFS
static bool g_debug_switch;
#endif
static u8 threshold;
static int firm_status_data;

/* ATMEL CONFIG STRUCTURE Start */
static u8 firmware_latest[] = {0x5, 0x2};       /* version, build_version */
static gen_commandprocessor_t6_config_t t6_config= {0, };
static gen_powerconfig_t7_config_t t7_config = {0, };
static gen_acquisitionconfig_t8_config_t t8_config = {0, };
static touch_multitouchscreen_t9_config_t t9_config= {0, };
static touch_keyarray_t15_config_t t15_config = {0, };
static spt_comcconfig_t18_config_t t18_config = {0, };
static spt_gpiopwm_t19_config_t t19_config={0, };
static proci_gripfacesuppression_t20_config_t t20_config={0, };
static procg_noisesuppression_t22_config_t t22_config={0, };
static proci_onetouchgestureprocessor_t24 t24_config={0, };
static spt_selftest_t25_config_t t25_config={0, };
static proci_twotouchgestureprocessor_t27_config_t t27_config={0, };
static spt_cteconfig_t28_config_t t28_config={0, };
static debug_diagnostic_t37_config_t t37_config={0, };
static spt_userdata_t38_config_t t38_config ;
static proci_gripsuppression_t40_config_t t40_config={0, };
static proci_touchsuppression_t42_config_t t42_config={0, };
static spt_digitizer_t43_t t43_config={0, };
static spt_cteconfig_t46_config_t t46_config={0, };
static proci_stylus_t47_config_t t47_config={0, };
static procg_noisesuppression_t48_config_t t48_config={0, };
static touch_proximity_t52_config_t t52_config={0, };
static proci_adaptivethreshold_t55_config_t t55_config={0, };
static proci_shieldless_t56_t t56_config={0, };
static proc_extratouchscreendata_t57_t t57_config={0, };
static spt_timer_t61_t t61_config={0, };
static proci_noisesupperssion_t62_t t62_config={0, };
static proci_activestylues_t63_t t63_config={0, };

static proci_lensbending_t65_t t65_config={0, };
static spt_goldenreferences_t66_t t66_config={0, };
static spt_serialdatacommand_t68_t t68_config={0, };
/* for registers in runtime mode */
struct mxt_runmode_registers_t runmode_regs = {0, };
/* ATMEL CONFIG STRUCTURE END */

int atmel_tsp_config[]=
{
#if 0
	DEBUG_DIAGNOSTIC_T37,
	SPT_SERIALDATACOMMAND_T68,
	GEN_POWERCONFIG_T7,
	GEN_ACQUISITIONCONFIG_T8,
	TOUCH_MULTITOUCHSCREEN_T9,
	TOUCH_KEYARRAY_T15,
	PROCI_ONETOUCHGESTUREPROCESSOR_T24,
	SPT_SELFTEST_T25,
	PROCI_TWOTOUCHGESTUREPROCESSOR_T27,
	PROCI_GRIPSUPPRESSION_T40,
	PROCI_TOUCHSUPPRESSION_T42,
	SPT_DIGITIZER_T43,
	SPT_CTECONFIG_T46,
	PROCI_STYLUS_T47,
	PROCI_ADAPTIVETHRESHOLD_T55,
	PROCI_SHIELDLESS_T56,
	PROCI_EXTRATOUCHSCREENDATA_T57,
	SPT_TIMER_T61,
	PROCG_NOISESUPPRESSION_T62,
	PROCI_ACTIVESTYLUS_T63,
	PROCI_LENSBENDING_T65,
	SPT_GOLDENREFERENCES_T66,
	RESERVED_T255,
#else
	DEBUG_DIAGNOSTIC_T37,
	SPT_USERDATA_T38,
	GEN_POWERCONFIG_T7,
	GEN_ACQUISITIONCONFIG_T8,
	TOUCH_MULTITOUCHSCREEN_T9,
	TOUCH_KEYARRAY_T15,
	SPT_COMMSCONFIG_T18,
	PROCI_ONETOUCHGESTUREPROCESSOR_T24,
	SPT_SELFTEST_T25,
	PROCI_TWOTOUCHGESTUREPROCESSOR_T27,
	PROCI_GRIPSUPPRESSION_T40,
	PROCI_TOUCHSUPPRESSION_T42,
	SPT_DIGITIZER_T43,
	SPT_CTECONFIG_T46,
	PROCI_STYLUS_T47,
	PROCI_ADAPTIVETHRESHOLD_T55,
	PROCI_SHIELDLESS_T56,
	PROCI_EXTRATOUCHSCREENDATA_T57,
	SPT_TIMER_T61,
	PROCG_NOISESUPPRESSION_T62,
	RESERVED_T255,
#endif	
};

int get_object_info(struct mxt_data *data, u8 object_type, u16 *size,
                                u16 *address)
{
        int i;

        for (i = 0; i < data->objects_len; i++) {
                if (data->objects[i].object_type == object_type) {
                        *size = data->objects[i].size + 1;
                        *address = data->objects[i].i2c_address;
                        return 0;
                }
        }

        return -ENODEV;
}
int mxt_write_block(struct i2c_client *client,
                    u16 addr,
                    u16 length,
                    u8 *value)
{
        int i;
        struct {
                __le16  le_addr;
                u8      data[256];

        } i2c_block_transfer;

        struct mxt_data *mxt;

        if (length > 256)
                return -EINVAL;

        mxt = i2c_get_clientdata(client);
        if (mxt != NULL)
                mxt->last_read_addr = -1;

        for (i = 0; i < length; i++)
                i2c_block_transfer.data[i] = *value++;

        i2c_block_transfer.le_addr = cpu_to_le16(addr);

        i = i2c_master_send(client, (u8 *) &i2c_block_transfer, length + 2);

        if (i == (length + 2)) {
#if ITDEV
                if (debug_enabled)
                        print_hex_dump(KERN_INFO, "MXT TX:", DUMP_PREFIX_NONE,
                                16, 1, &i2c_block_transfer, length+2, false);
#endif
                return length;
        } else
                return -EIO;
}

int mxt_GEN_POWERCONFIG_T7(struct mxt_data *mxt)

{
	struct i2c_client *client = mxt->client;
	u16 obj_addr=0, obj_size=0;
	int error=0;

    get_object_info(copy_data,
            GEN_POWERCONFIG_T7, &obj_size, &obj_addr);
	printk("mxt_GEN_POWERCONFIG_T7 obj_size=%d obj_addr=%d OBJECT_SIZE=4   OBJECT_ADDRESS=455 \n",obj_size,obj_addr );
	memset(&t7_config, 0, sizeof(t7_config));

	t7_config.nIDLEACQINT=255;
	t7_config.nACTVACQINT=255;
	t7_config.nACTV2IDLETO=5;
	t7_config.nCFG=0;

	error = mxt_write_block(client,
			obj_addr, obj_size, (u8 *)&t7_config);
	if (error < 0) {
		dev_err(&client->dev, "[TSP] mxt_write_block failed! "
				"(%s, %d)\n", __func__, __LINE__);
		return -EIO;
	}

	return 0;
}


int mxt_GEN_ACQUISITIONCONFIG_T8(struct mxt_data *mxt)

{
	struct i2c_client *client = mxt->client;
	u16 obj_addr=0, obj_size=0;
	int error=0;

    get_object_info(copy_data,
            GEN_ACQUISITIONCONFIG_T8, &obj_size, &obj_addr);
	printk("mxt_GEN_ACQUISITIONCONFIG_T8 obj_size=%d obj_addr=%d OBJECT_SIZE=10  OBJECT_ADDRESS=459 \n",obj_size,obj_addr );
	memset(&t8_config, 0, sizeof(t8_config));

#if 0
	t8_config.nCHRGTIME=150;
	t8_config.nATCHDRIFT=0;
	t8_config.nTCHDRIFT=5;
	t8_config.nDRIFTST=1;
	t8_config.nTCHAUTOCAL=0;
	t8_config.nSYNC=0;
	t8_config.nATCHCALST=0;
	t8_config.nATCHCALSTHR=35;
	t8_config.nATCHFRCCALTHR=0;
	t8_config.nATCHFRCCALRATIO=0;
#else
	t8_config.nCHRGTIME=100;
	t8_config.nATCHDRIFT=0;
	t8_config.nTCHDRIFT=20;
	t8_config.nDRIFTST=20;
	t8_config.nTCHAUTOCAL=0;
	t8_config.nSYNC=0;
	t8_config.nATCHCALST=0;
	t8_config.nATCHCALSTHR=1;
	t8_config.nATCHFRCCALTHR=0;
	t8_config.nATCHFRCCALRATIO=0;
#endif

	error = mxt_write_block(client,
			obj_addr, obj_size, (u8 *)&t8_config);
	if (error < 0) {
		dev_err(&client->dev, "[TSP] mxt_write_block failed! "
				"(%s, %d)\n", __func__, __LINE__);
		return -EIO;
	}

	return 0;
}


int mxt_TOUCH_MULTITOUCHSCREEN_T9(struct mxt_data *mxt)

{
	struct i2c_client *client = mxt->client;
	u16 obj_addr=0, obj_size=0;
	int error=0;

    get_object_info(copy_data,
            TOUCH_MULTITOUCHSCREEN_T9, &obj_size, &obj_addr);
	printk("mxt_TOUCH_MULTITOUCHSCREEN_T9 obj_size=%d obj_addr=%d  OBJECT_SIZE=36 OBJECT_ADDRESS=469  \n",obj_size,obj_addr );
	memset(&t9_config, 0, sizeof(t9_config));
#if 0
	t9_config.nCTRL=139;
	t9_config.nXORIGIN=0;
	t9_config.nYORIGIN=0;
	t9_config.nXSIZE=32;
	t9_config.nYSIZE=52;
	t9_config.nAKSCFG=0;
	t9_config.nBLEN=128;
	t9_config.nTCHTHR=75;
	t9_config.nTCHDI=2;
	t9_config.nORIENT=1;
	t9_config.nMRGTIMEOUT=0;
	t9_config.nMOVHYSTI=5;
	t9_config.nMOVHYSTN=1;
	t9_config.nMOVFILTER=65;
	t9_config.nNUMTOUCH=10;
	t9_config.nMRGHYST=10;
	t9_config.nMRGTHR=20;
	t9_config.nAMPHYST=20;
	t9_config.nXRANGE=1279;
	t9_config.nYRANGE=799;
	t9_config.nXLOCLIP=0;
	t9_config.nXHICLIP=0;
	t9_config.nYLOCLIP=0;
	t9_config.nYHICLIP=0;
	t9_config.nXEDGECTRL=0;
	t9_config.nXEDGEDIST=0;
	t9_config.nYEDGECTRL=0;
	t9_config.nYEDGEDIST=0;
	t9_config.nJUMPLIMIT=32;
	t9_config.nTCHHYST=15;
	t9_config.nXPITCH=42;
	t9_config.nYPITCH=42;
	t9_config.nNEXTTCHDI=0;
	t9_config.nCFG=0;
#else
	t9_config.nCTRL=131;
	t9_config.nXORIGIN=0;
	t9_config.nYORIGIN=0;
	t9_config.nXSIZE=32;
	t9_config.nYSIZE=52;
	t9_config.nAKSCFG=0;
	t9_config.nBLEN=123;
	t9_config.nTCHTHR=40;
	t9_config.nTCHDI=2;
	t9_config.nORIENT=5;
	t9_config.nMRGTIMEOUT=0;
	t9_config.nMOVHYSTI=131;
	t9_config.nMOVHYSTN=0;
	t9_config.nMOVFILTER=0;
	t9_config.nNUMTOUCH=16;
	t9_config.nMRGHYST=20;
	t9_config.nMRGTHR=20;
	t9_config.nAMPHYST=20;
	t9_config.nXRANGE=1279;
	t9_config.nYRANGE=799;
	t9_config.nXLOCLIP=0;
	t9_config.nXHICLIP=0;
	t9_config.nYLOCLIP=0;
	t9_config.nYHICLIP=0;
	t9_config.nXEDGECTRL=0;
	t9_config.nXEDGEDIST=0;
	t9_config.nYEDGECTRL=0;
	t9_config.nYEDGEDIST=0;
	t9_config.nJUMPLIMIT=15;
	t9_config.nTCHHYST=25;
	t9_config.nXPITCH=0;
	t9_config.nYPITCH=0;
	t9_config.nNEXTTCHDI=2;
	t9_config.nCFG=0;
#endif
	error = mxt_write_block(client, obj_addr,
			obj_size, (u8 *)&t9_config);
/*
	[TOUCH_MULTITOUCHSCREEN_T9 instance 1
	OBJECT_ADDRESS=505
	OBJECT_SIZE=36
*/
	memset(&t9_config, 0, sizeof(t9_config));
	error = mxt_write_block(client, 505,
			36, (u8 *)&t9_config);


	if (error < 0) {
		dev_err(&client->dev, "[TSP] mxt_write_block failed! "
				"(%s, %d)\n", __func__, __LINE__);
		return -EIO;
	}

	return 0;
}


int mxt_TOUCH_KEYARRAY_T15(struct mxt_data *mxt)

{
	struct i2c_client *client = mxt->client;
	u16 obj_addr=0, obj_size=0;
	int error=0;

    get_object_info(copy_data,
            TOUCH_KEYARRAY_T15, &obj_size, &obj_addr);
	printk("mxt_TOUCH_KEYARRAY_T15 obj_size=%d obj_addr=%d OBJECT_SIZE=11  OBJECT_ADDRESS=541\n",obj_size,obj_addr );
	memset(&t15_config, 0, sizeof(t15_config));
#if 0
	t15_config.nCTRL=0;
	t15_config.nXORIGIN=0;
	t15_config.nYORIGIN=0;
	t15_config.nXSIZE=0;
	t15_config.nYSIZE=0;
	t15_config.nAKSCFG=0;
	t15_config.nBLEN=0;
	t15_config.nTCHTHR=0;
	t15_config.nTCHDI=0;
	t15_config.nRESERVED[0]=0;
	t15_config.nRESERVED[1]=0;
#else
	t15_config.nCTRL=0;
	t15_config.nXORIGIN=8;
	t15_config.nYORIGIN=51;
	t15_config.nXSIZE=16;
	t15_config.nYSIZE=1;
	t15_config.nAKSCFG=1;
	t15_config.nBLEN=123;
	t15_config.nTCHTHR=40;
	t15_config.nTCHDI=2;
	t15_config.nRESERVED[0]=0;
	t15_config.nRESERVED[1]=0;
#endif
	error = mxt_write_block(client, obj_addr,
			obj_size, (u8 *)&t15_config);

	if (error < 0) {
		dev_err(&client->dev, "[TSP] mxt_write_block failed! "
				"(%s, %d)\n", __func__, __LINE__);
		return -EIO;
	}

	return 0;
}

int mxt_SPT_COMMSCONFIG_T18(struct mxt_data *mxt)

{
	struct i2c_client *client = mxt->client;
	u16 obj_addr=0, obj_size=0;
	int error=0;

    get_object_info(copy_data,
            SPT_COMMSCONFIG_T18, &obj_size, &obj_addr);
	printk("mxt_SPT_COMMSCONFIG_T18 obj_size=%d obj_addr=%d  OBJECT_SIZE=2  OBJECT_ADDRESS=552 \n",obj_size,obj_addr );
	memset(&t18_config, 0, sizeof(t18_config));
#if 0
	t18_config.nCTRL=0;
	t18_config.nCOMMAND=0;
#else
	t18_config.nCTRL=4;
	t18_config.nCOMMAND=0;
#endif
	error = mxt_write_block(client, obj_addr,
			obj_size, (u8 *)&t18_config);

	if (error < 0) {
		dev_err(&client->dev, "[TSP] mxt_write_block failed! "
				"(%s, %d)\n", __func__, __LINE__);
		return -EIO;
	}

	return 0;
}

int mxt_PROCI_ONETOUCHGESTUREPROCESSOR_T24(struct mxt_data *mxt)

{
	struct i2c_client *client = mxt->client;
	u16 obj_addr=0, obj_size=0;
	int error=0;

    get_object_info(copy_data,
            PROCI_ONETOUCHGESTUREPROCESSOR_T24, &obj_size, &obj_addr);
	printk("PROCI_ONETOUCHGESTUREPROCESSOR_T24 obj_size=%d obj_addr=%d OBJECT_SIZE=19  OBJECT_ADDRESS=554 \n",obj_size,obj_addr );
	memset(&t24_config, 0, sizeof(t24_config));

#if 0
	t24_config.nCTRL=0;
	t24_config.nNUMGEST=0;
	t24_config.nGESTEN=0;
	t24_config.nPROCESS=0;
	t24_config.nTAPTO=0;
	t24_config.nFLICKTO=0;
	t24_config.nDRAGTO=0;
	t24_config.nSPRESSTO=0;
	t24_config.nLPRESSTO=0;
	t24_config.nREPPRESSTO=0;
	t24_config.nFLICKTHR=0;
	t24_config.nDRAGTHR=0;
	t24_config.nTAPTHR=0;
	t24_config.nTHROWTHR=0;
#endif

	error = mxt_write_block(client, obj_addr,
			obj_size, (u8 *)&t24_config);

/*
	[PROCI_ONETOUCHGESTUREPROCESSOR_T24 instance 1
	OBJECT_ADDRESS=573
	OBJECT_SIZE=19
*/

	memset(&t24_config, 0, sizeof(t24_config));
	error = mxt_write_block(client, 573,
			19, (u8 *)&t24_config);


	if (error < 0) {
		dev_err(&client->dev, "[TSP] mxt_write_block failed! "
				"(%s, %d)\n", __func__, __LINE__);
		return -EIO;
	}

	return 0;
}

int mxt_SPT_SELFTEST_T25(struct mxt_data *mxt)

{
	struct i2c_client *client = mxt->client;
	u16 obj_addr=0, obj_size=0;
	int error=0;

    get_object_info(copy_data,
            SPT_SELFTEST_T25, &obj_size, &obj_addr);
	printk("mxt_SPT_SELFTEST_T25 obj_size=%d obj_addr=%d   OBJECT_SIZE=15  OBJECT_ADDRESS=592\n",obj_size,obj_addr );
	memset(&t25_config, 0, sizeof(t25_config));

#if 0
	t25_config.nCTRL=0;
	t25_config.nCMD=0;
	t25_config.nUPSIGLIM[0]=0;
	t25_config.nLOSIGLIM[0]=0;
	t25_config.nUPSIGLIM[1]=0;
	t25_config.nLOSIGLIM[1]=0;
	t25_config.nUPSIGLIM[2]=0;
	t25_config.nLOSIGLIM[2]=0;
	t25_config.nPINDWELLUS=0;
#else
	t25_config.nCTRL=0;
	t25_config.nCMD=0;
	t25_config.nUPSIGLIM[0]=28000;
	t25_config.nLOSIGLIM[0]=20000;
	t25_config.nUPSIGLIM[1]=0;
	t25_config.nLOSIGLIM[1]=0;
	t25_config.nUPSIGLIM[2]=0;
	t25_config.nLOSIGLIM[2]=0;
	t25_config.nPINDWELLUS=0;
#endif

	error = mxt_write_block(client, obj_addr,
			obj_size, (u8 *)&t25_config);

	if (error < 0) {
		dev_err(&client->dev, "[TSP] mxt_write_block failed! "
				"(%s, %d)\n", __func__, __LINE__);
		return -EIO;
	}

	return 0;
}

int mxt_PROCI_TWOTOUCHGESTUREPROCESSOR_T27(struct mxt_data *mxt)

{
	struct i2c_client *client = mxt->client;
	u16 obj_addr=0, obj_size=0;
	int error=0;

    get_object_info(copy_data,
            PROCI_TWOTOUCHGESTUREPROCESSOR_T27, &obj_size, &obj_addr);
	printk("PROCI_TWOTOUCHGESTUREPROCESSOR_T27 obj_size=%d obj_addr=%d   OBJECT_SIZE=7  OBJECT_ADDRESS=607\n",obj_size,obj_addr );
	memset(&t27_config, 0, sizeof(t27_config));

	error = mxt_write_block(client, obj_addr,
			obj_size, (u8 *)&t25_config);

	memset(&t27_config, 0, sizeof(t27_config));
	error = mxt_write_block(client, 614,
			7, (u8 *)&t25_config);

	if (error < 0) {
		dev_err(&client->dev, "[TSP] mxt_write_block failed! "
				"(%s, %d)\n", __func__, __LINE__);
		return -EIO;
	}

	return 0;
}



int mxt_SPT_USERDATA_T38(struct mxt_data *mxt)

{
	struct i2c_client *client = mxt->client;
	u16 obj_addr=0, obj_size=0;
	int error=0;

    get_object_info(copy_data,
            SPT_USERDATA_T38, &obj_size, &obj_addr);
	printk("DEBUG_DIAGNOSTIC_T37 obj_size=%d obj_addr=%d OBJECT_SIZE=64 OBJECT_ADDRESS=391\n",obj_size,obj_addr );
	memset(&t38_config, 0, sizeof(t38_config));


	error = mxt_write_block(client, obj_addr,
			obj_size, (u8 *)&t38_config);

	if (error < 0) {
		dev_err(&client->dev, "[TSP] mxt_write_block failed! "
				"(%s, %d)\n", __func__, __LINE__);
		return -EIO;
	}

	return 0;
}

int mxt_DEBUG_DIAGNOSTIC_T37(struct mxt_data *mxt)

{
	struct i2c_client *client = mxt->client;
	u16 obj_addr=0, obj_size=0;
	int error=0;

    get_object_info(copy_data,
            DEBUG_DIAGNOSTIC_T37, &obj_size, &obj_addr);
	printk("DEBUG_DIAGNOSTIC_T37 obj_size=%d obj_addr=%d OBJECT_SIZE=130 OBJECT_ADDRESS=172\n",obj_size,obj_addr );
	memset(&t37_config, 0, sizeof(t37_config));


	error = mxt_write_block(client, obj_addr,
			obj_size, (u8 *)&t37_config);

	if (error < 0) {
		dev_err(&client->dev, "[TSP] mxt_write_block failed! "
				"(%s, %d)\n", __func__, __LINE__);
		return -EIO;
	}

	return 0;
}

int mxt_PROCI_GRIPSUPPRESSION_T40(struct mxt_data *mxt)
{
	struct i2c_client *client = mxt->client;
	u16 obj_addr=0, obj_size=0;
	int error=0;

    get_object_info(copy_data,
            PROCI_GRIPSUPPRESSION_T40, &obj_size, &obj_addr);
	printk("mxt_PROCI_GRIPSUPPRESSION_T40 obj_size=%d obj_addr=%d  OBJECT_SIZE=5 OBJECT_ADDRESS=621 \n",obj_size,obj_addr );
	memset(&t40_config, 0, sizeof(t40_config));

#if 1
	t40_config.nCTRL=0;
	t40_config.nXLOGRIP=20;
	t40_config.nXHIGRIP=20;
	t40_config.nYLOGRIP=20;
	t40_config.nYHIGRIP=20;
#endif
	error = mxt_write_block(client, obj_addr,
			obj_size, (u8 *)&t40_config);

	memset(&t40_config, 0, sizeof(t40_config));
	error = mxt_write_block(client, 626,
			5, (u8 *)&t40_config);

	if (error < 0) {
		dev_err(&client->dev, "[TSP] mxt_write_block failed! "
				"(%s, %d)\n", __func__, __LINE__);
		return -EIO;
	}

	return 0;
}

int mxt_PROCI_TOUCHSUPPRESSION_T42(struct mxt_data *mxt)
{
	struct i2c_client *client = mxt->client;
	u16 obj_addr=0, obj_size=0;
	int error=0;

    get_object_info(copy_data,
            PROCI_TOUCHSUPPRESSION_T42, &obj_size, &obj_addr);
	printk("mxt_PROCI_TOUCHSUPPRESSION_T42 obj_size=%d obj_addr=%d OBJECT_SIZE=10  OBJECT_ADDRESS=631 \n",obj_size,obj_addr );
	memset(&t42_config, 0, sizeof(t42_config));

#if 0
	t42_config.nCTRL=0;
	t42_config.nAPPRTHR=42;
	t42_config.nMAXAPPRAREA=50;
	t42_config.nMAXTCHAREA=50;
	t42_config.nSUPSTRENGTH=127;
	t42_config.nSUPEXTTO=0;
	t42_config.nMAXNUMTCHS=0;
	t42_config.nSHAPESTRENGTH=0;
	t42_config.nSUPDIST=5;
	t42_config.nDISTHYST=5;
#else
	t42_config.nCTRL=32;
	t42_config.nAPPRTHR=20;
	t42_config.nMAXAPPRAREA=0;
	t42_config.nMAXTCHAREA=0;
	t42_config.nSUPSTRENGTH=0;
	t42_config.nSUPEXTTO=20;
	t42_config.nMAXNUMTCHS=17;
	t42_config.nSHAPESTRENGTH=0;
	t42_config.nSUPDIST=3;
	t42_config.nDISTHYST=0;
#endif
	error = mxt_write_block(client, obj_addr,
			obj_size, (u8 *)&t42_config);

	memset(&t42_config, 0, sizeof(t42_config));
	error = mxt_write_block(client, 641,
			10, (u8 *)&t42_config);


	if (error < 0) {
		dev_err(&client->dev, "[TSP] mxt_write_block failed! "
				"(%s, %d)\n", __func__, __LINE__);
		return -EIO;
	}

	return 0;
}

int mxt_SPT_DIGITIZER_T43(struct mxt_data *mxt)
{
	struct i2c_client *client = mxt->client;
	u16 obj_addr=0, obj_size=0;
	int error=0;

    get_object_info(copy_data,
            SPT_DIGITIZER_T43, &obj_size, &obj_addr);
	printk("mxt_SPT_DIGITIZER_T43 obj_size=%d obj_addr=%d   OBJECT_SIZE=12 OBJECT_ADDRESS=651\n",obj_size,obj_addr );
	memset(&t43_config, 0, sizeof(t43_config));

#if 1
	t43_config.nCTRL=0;
	t43_config.nHIDIDLERATE=128;
	t43_config.nXLENGTH=137;
	t43_config.nYLENGTH=221;
	t43_config.nRWKRATE=128;
	t43_config.nHEIGHTSCALE=0;
	t43_config.nHEIGHTOFFSET=0;
	t43_config.nWIDTHSCALE=0;
	t43_config.nWIDTHOFFSET=0;
	t43_config.nRESERVED=0;
#endif

	error = mxt_write_block(client, obj_addr,
			obj_size, (u8 *)&t43_config);

	if (error < 0) {
		dev_err(&client->dev, "[TSP] mxt_write_block failed! "
				"(%s, %d)\n", __func__, __LINE__);
		return -EIO;
	}

	return 0;
}

int mxt_SPT_CTECONFIG_T46(struct mxt_data *mxt)
{
	struct i2c_client *client = mxt->client;
	u16 obj_addr=0, obj_size=0;
	int error=0;

    get_object_info(copy_data,
            SPT_CTECONFIG_T46, &obj_size, &obj_addr);
	printk("mxt_SPT_CTECONFIG_T46 obj_size=%d obj_addr=%d OBJECT_SIZE=11  OBJECT_ADDRESS=663 \n",obj_size,obj_addr );
	memset(&t46_config, 0, sizeof(t46_config));

#if 0
	t46_config.nCTRL=4;
	t46_config.nMODE=0;
	t46_config.nIDLESYNCSPERX=24;
	t46_config.nACTVSYNCSPERX=24;
	t46_config.nADCSPERSYNC=0;
	t46_config.nPULSESPERADC=0;
	t46_config.nXSLEW=1;
	t46_config.nSYNCDELAY=0;
	t46_config.nXVOLTAGE=0;
	t46_config.nADCCTRL=15;
#else
	t46_config.nCTRL=0;
	t46_config.nMODE=0;
	t46_config.nIDLESYNCSPERX=8;
	t46_config.nACTVSYNCSPERX=8;
	t46_config.nADCSPERSYNC=0;
	t46_config.nPULSESPERADC=0;
	t46_config.nXSLEW=1;
	t46_config.nSYNCDELAY=0;
	t46_config.nXVOLTAGE=0;
	t46_config.nADCCTRL=15;
#endif
	error = mxt_write_block(client, obj_addr,
			obj_size, (u8 *)&t46_config);

	if (error < 0) {
		dev_err(&client->dev, "[TSP] mxt_write_block failed! "
				"(%s, %d)\n", __func__, __LINE__);
		return -EIO;
	}

	return 0;
}


int mxt_PROCI_STYLUS_T47(struct mxt_data *mxt)
{
	struct i2c_client *client = mxt->client;
	u16 obj_addr=0, obj_size=0;
	int error=0;

    get_object_info(copy_data,
            PROCI_STYLUS_T47, &obj_size, &obj_addr);
	printk("mxt_PROCI_STYLUS_T47 obj_size=%d obj_addr=%d  OBJECT_SIZE=22  OBJECT_ADDRESS=674 \n",obj_size,obj_addr );
	memset(&t47_config, 0, sizeof(t47_config));

#if 0
	t47_config.nCTRL=73;
	t47_config.nCONTMIN=35;
	t47_config.nCONTMAX=60;
	t47_config.nSTABILITY=10;
	t47_config.nMAXTCHAREA=2;
	t47_config.nAMPLTHR=30;
	t47_config.nSTYSHAPE=0;
	t47_config.nHOVERSUP=120;
	t47_config.nCONFTHR=1;
	t47_config.nSYNCSPERX=24;
	t47_config.nXPOSADJ=0;
	t47_config.nYPOSADJ=0;
	t47_config.nCFG=15;
#else
	t47_config.nCTRL=0;
	t47_config.nCONTMIN=20;
	t47_config.nCONTMAX=35;
	t47_config.nSTABILITY=2;
	t47_config.nMAXTCHAREA=5;
	t47_config.nAMPLTHR=30;
	t47_config.nSTYSHAPE=1;
	t47_config.nHOVERSUP=120;
	t47_config.nCONFTHR=3;
	t47_config.nSYNCSPERX=16;
	t47_config.nXPOSADJ=0;
	t47_config.nYPOSADJ=0;
	t47_config.nCFG=0;
#endif
	error = mxt_write_block(client, obj_addr,
			obj_size, (u8 *)&t47_config);

	memset(&t47_config, 0, sizeof(t47_config));
	error = mxt_write_block(client, 696,
			22, (u8 *)&t47_config);


	if (error < 0) {
		dev_err(&client->dev, "[TSP] mxt_write_block failed! "
				"(%s, %d)\n", __func__, __LINE__);
		return -EIO;
	}

	return 0;
}

int mxt_PROCI_ADAPTIVETHRESHOLD_T55(struct mxt_data *mxt)
{
	struct i2c_client *client = mxt->client;
	u16 obj_addr=0, obj_size=0;
	int error=0;

    get_object_info(copy_data,
            PROCI_ADAPTIVETHRESHOLD_T55, &obj_size, &obj_addr);
	printk("ADAPTIVE_T55 obj_size=%d obj_addr=%d OBJECT_SIZE=7  OBJECT_ADDRESS=718\n",obj_size,obj_addr );
	memset(&t55_config, 0, sizeof(t55_config));

	t55_config.nCTRL=0;
	t55_config.nTARGETTHR=0;
	t55_config.nTHRADJLIM=0;
	t55_config.nRESETSTEPTIME=0;
	t55_config.nFORCECHGDIST=0;
	t55_config.nFORCECHGTIME=0;

	error = mxt_write_block(client, obj_addr,
			obj_size, (u8 *)&t55_config);

	memset(&t55_config, 0, sizeof(t55_config));
	error = mxt_write_block(client, 725,
			7, (u8 *)&t55_config);

	if (error < 0) {
		dev_err(&client->dev, "[TSP] mxt_write_block failed! "
				"(%s, %d)\n", __func__, __LINE__);
		return -EIO;
	}

	return 0;
}

int mxt_PROCI_SHIELDLESS_T56(struct mxt_data *mxt)
{
	struct i2c_client *client = mxt->client;
	u16 obj_addr=0, obj_size=0;
	int error=0;

    get_object_info(copy_data,
            PROCI_SHIELDLESS_T56, &obj_size, &obj_addr);
	printk("mxt_PROCI_SHIELDLESS_T56 obj_size=%d obj_addr=%d   OBJECT_SIZE=51 OBJECT_ADDRESS=732\n",obj_size,obj_addr );
	memset(&t56_config, 0, sizeof(t56_config));

#if 0
	t56_config.nCTRL=0;
	t56_config.nCOMMAND=0;
	t56_config.nOPTINT=0;
	t56_config.nINTTIME=55;
	t56_config.nINTDELAY[0]=27;
	t56_config.nINTDELAY[1]=27;
	t56_config.nINTDELAY[2]=27;
	t56_config.nINTDELAY[3]=27;
	t56_config.nINTDELAY[4]=27;
	t56_config.nINTDELAY[5]=27;
	t56_config.nINTDELAY[6]=27;
	t56_config.nINTDELAY[7]=27;
	t56_config.nINTDELAY[8]=27;
	t56_config.nINTDELAY[9]=26;
	t56_config.nINTDELAY[10]=26;
	t56_config.nINTDELAY[11]=26;
	t56_config.nINTDELAY[12]=26;
	t56_config.nINTDELAY[13]=26;
	t56_config.nINTDELAY[14]=25;
	t56_config.nINTDELAY[15]=25;
	t56_config.nINTDELAY[16]=25;
	t56_config.nINTDELAY[17]=24;
	t56_config.nINTDELAY[18]=24;
	t56_config.nINTDELAY[19]=24;
	t56_config.nINTDELAY[20]=23;
	t56_config.nINTDELAY[21]=23;
	t56_config.nINTDELAY[22]=23;
	t56_config.nINTDELAY[23]=23;
	t56_config.nINTDELAY[24]=22;
	t56_config.nINTDELAY[25]=22;
	t56_config.nINTDELAY[26]=22;
	t56_config.nINTDELAY[27]=21;
	t56_config.nINTDELAY[28]=21;
	t56_config.nINTDELAY[29]=21;
	t56_config.nINTDELAY[30]=21;
	t56_config.nINTDELAY[31]=21;
	t56_config.nMULTICUTGC=0;
	t56_config.nGCLIMIT=0;
	t56_config.nNCNCL=0;
	t56_config.nTOUCHBIAS=0;
	t56_config.nBASESCALE=0;
	t56_config.nSHIFTLIMIT=0;
	t56_config.nYLONOISEMUL=0;
	t56_config.nYLONOISEDIV=0;
	t56_config.nYHINOISEMUL=0;
	t56_config.nYHINOISEDIV=0;
	t56_config.nNCNCLMANIDX=0;
#else
	t56_config.nCTRL=2;
	t56_config.nCOMMAND=0;
	t56_config.nOPTINT=1;
	t56_config.nINTTIME=48;
	t56_config.nINTDELAY[0]=19;
	t56_config.nINTDELAY[1]=20;
	t56_config.nINTDELAY[2]=20;
	t56_config.nINTDELAY[3]=20;
	t56_config.nINTDELAY[4]=21;
	t56_config.nINTDELAY[5]=21;
	t56_config.nINTDELAY[6]=21;
	t56_config.nINTDELAY[7]=21;
	t56_config.nINTDELAY[8]=21;
	t56_config.nINTDELAY[9]=21;
	t56_config.nINTDELAY[10]=21;
	t56_config.nINTDELAY[11]=22;
	t56_config.nINTDELAY[12]=22;
	t56_config.nINTDELAY[13]=22;
	t56_config.nINTDELAY[14]=22;
	t56_config.nINTDELAY[15]=22;
	t56_config.nINTDELAY[16]=22;
	t56_config.nINTDELAY[17]=22;
	t56_config.nINTDELAY[18]=22;
	t56_config.nINTDELAY[19]=22;
	t56_config.nINTDELAY[20]=22;
	t56_config.nINTDELAY[21]=21;
	t56_config.nINTDELAY[22]=20;
	t56_config.nINTDELAY[23]=20;
	t56_config.nINTDELAY[24]=20;
	t56_config.nINTDELAY[25]=20;
	t56_config.nINTDELAY[26]=21;
	t56_config.nINTDELAY[27]=20;
	t56_config.nINTDELAY[28]=20;
	t56_config.nINTDELAY[29]=20;
	t56_config.nINTDELAY[30]=20;
	t56_config.nINTDELAY[31]=19;
	t56_config.nMULTICUTGC=0;
	t56_config.nGCLIMIT=0;
	t56_config.nNCNCL=1;
	t56_config.nTOUCHBIAS=2;
	t56_config.nBASESCALE=5;
	t56_config.nSHIFTLIMIT=5;
	t56_config.nYLONOISEMUL=0;
	t56_config.nYLONOISEDIV=0;
	t56_config.nYHINOISEMUL=0;
	t56_config.nYHINOISEDIV=0;
#endif 
	error = mxt_write_block(client, obj_addr,
			obj_size, (u8 *)&t56_config);

	if (error < 0) {
		dev_err(&client->dev, "[TSP] mxt_write_block failed! "
				"(%s, %d)\n", __func__, __LINE__);
		return -EIO;
	}

	return 0;
}

int mxt_SPT_GENERICDATA_T57(struct mxt_data *mxt)
{
	struct i2c_client *client = mxt->client;
	u16 obj_addr=0, obj_size=0;
	int error=0;

    get_object_info(copy_data,
            PROCI_EXTRATOUCHSCREENDATA_T57, &obj_size, &obj_addr);
	printk("SPT_GENERICDATA_T57 obj_size=%d obj_addr=%d  OBJECT_SIZE=3  OBJECT_ADDRESS=783 \n",obj_size,obj_addr );
	memset(&t57_config, 0, sizeof(t57_config));

	error = mxt_write_block(client, obj_addr,
			obj_size, (u8 *)&t57_config);

	memset(&t57_config, 0, sizeof(t57_config));
	error = mxt_write_block(client, 786,
			3, (u8 *)&t57_config);


	if (error < 0) {
		dev_err(&client->dev, "[TSP] mxt_write_block failed! "
				"(%s, %d)\n", __func__, __LINE__);
		return -EIO;
	}

	return 0;
}


int mxt_SPT_TIMER_T61(struct mxt_data *mxt)
{
	struct i2c_client *client = mxt->client;
	u16 obj_addr=0, obj_size=0;
	int error=0;

    get_object_info(copy_data,
            SPT_TIMER_T61, &obj_size, &obj_addr);
	printk("SPT_TIMER_T61 obj_size=%d obj_addr=%d OBJECT_SIZE=5  OBJECT_ADDRESS=789 \n",obj_size,obj_addr );
	memset(&t61_config, 0, sizeof(t61_config));

	t61_config.nCTRL=0;
	t61_config.nCMD=0;
	t61_config.nMODE=0;
	t61_config.nPERIOD=0;

	error = mxt_write_block(client, obj_addr,
			obj_size, (u8 *)&t61_config);

	memset(&t61_config, 0, sizeof(t61_config));
	error = mxt_write_block(client, 789,
			5, (u8 *)&t61_config);


	if (error < 0) {
		dev_err(&client->dev, "[TSP] mxt_write_block failed! "
				"(%s, %d)\n", __func__, __LINE__);
		return -EIO;
	}

	return 0;
}

int mxt_PROCG_NOISESUPPRESSION_T62(struct mxt_data *mxt)
{
	struct i2c_client *client = mxt->client;
	u16 obj_addr=0, obj_size=0;
	int error=0;

    get_object_info(copy_data,
            PROCG_NOISESUPPRESSION_T62, &obj_size, &obj_addr);
	printk("mxt_PROCG_NOISESUPPRESSION_T62 obj_size=%d obj_addr=%d OBJECT_SIZE=74  OBJECT_ADDRESS=799 \n",obj_size,obj_addr );
	memset(&t62_config, 0, sizeof(t62_config));

#if 0
	t62_config.nCTRL=0;
	t62_config.nCALCFG1=0;
	t62_config.nCALCFG2=0;
	t62_config.nCALCFG3=31;
	t62_config.nCFG1=2;
	t62_config.nRESERVED1=0;
	t62_config.nRESERVED2=0;
	t62_config.nBASEFREQ=0;
	t62_config.nMAXSELFREQ=25;
	t62_config.nFREQ[0]=16;
	t62_config.nFREQ[1]=20;
	t62_config.nFREQ[2]=10;
	t62_config.nFREQ[3]=25;
	t62_config.nFREQ[4]=12;
	t62_config.nHOPCNT=5;
	t62_config.nRESERVED3=0;
	t62_config.nHOPCNTPER=10;
	t62_config.nHOPEVALTO=5;
	t62_config.nHOPST=5;
	t62_config.nNLGAIN=176;
	t62_config.nMINNLTHR=45;
	t62_config.nINCNLTHR=0;
	t62_config.nADCSPERXTHR=48;
	t62_config.nNLTHRMARGIN=15;
	t62_config.nMAXADCSPERX=63;
	t62_config.nACTVADCSVLDNOD=6;
	t62_config.nIDLEADCSVLDNOD=6;
	t62_config.nMINGCLIMIT=4;
	t62_config.nMAXGCLIMIT=64;
	t62_config.nRESERVED[0]=0;
	t62_config.nRESERVED[1]=0;
	t62_config.nRESERVED[2]=0;
	t62_config.nRESERVED[3]=0;
	t62_config.nRESERVED[4]=0;

	t62_config.nBLEN[0]=100;
	t62_config.nTCHTHR[0]=60;
	t62_config.nTCHDI[0]=2;
	t62_config.nMOVHYSTI[0]=5;
	t62_config.nMOVHYSTN[0]=1;
	t62_config.nMOVFILTER[0]=65;
	t62_config.nNUMTOUCH[0]=10;
	t62_config.nMRGHYST[0]=20;
	t62_config.nMRGTHR[0]=30;
	t62_config.nXLOCLIP[0]=0;
	t62_config.nXHICLIP[0]=0;
	t62_config.nYLOCLIP[0]=0;
	t62_config.nYHICLIP[0]=0;
	t62_config.nXEDGECTRL[0]=0;
	t62_config.nXEDGEDIST[0]=0;
	t62_config.nYEDGECTRL[0]=0;
	t62_config.nYEDGEDIST[0]=0;
	t62_config.nJUMPLIMIT[0]=32;
	t62_config.nTCHHYST[0]=15;
	t62_config.nNEXTTCHDI[0]=0;

	t62_config.nBLEN[1]=0;
	t62_config.nTCHTHR[1]=0;
	t62_config.nTCHDI[1]=0;
	t62_config.nMOVHYSTI[1]=0;
	t62_config.nMOVHYSTN[1]=0;
	t62_config.nMOVFILTER[1]=0;
	t62_config.nNUMTOUCH[1]=0;
	t62_config.nMRGHYST[1]=0;
	t62_config.nMRGTHR[1]=0;
	t62_config.nXLOCLIP[1]=0;
	t62_config.nXHICLIP[1]=0;
	t62_config.nYLOCLIP[1]=0;
	t62_config.nYHICLIP[1]=0;
	t62_config.nXEDGECTRL[1]=0;
	t62_config.nXEDGEDIST[1]=0;
	t62_config.nYEDGECTRL[1]=0;
	t62_config.nYEDGEDIST[1]=0;
	t62_config.nJUMPLIMIT[1]=0;
	t62_config.nTCHHYST[1]=0;
	t62_config.nNEXTTCHDI[1]=0;
#else
	t62_config.nCTRL=126;
	t62_config.nCALCFG1=11;
	t62_config.nCALCFG2=0;
	t62_config.nCALCFG3=23;
	t62_config.nCFG1=0;
	t62_config.nRESERVED1=0;
	t62_config.nRESERVED2=0;
	t62_config.nBASEFREQ=0;
	t62_config.nMAXSELFREQ=25;
	t62_config.nFREQ[0]=0;
	t62_config.nFREQ[1]=3;
	t62_config.nFREQ[2]=9;
	t62_config.nFREQ[3]=12;
	t62_config.nFREQ[4]=23;
	t62_config.nHOPCNT=5;
	t62_config.nRESERVED3=0;
	t62_config.nHOPCNTPER=10;
	t62_config.nHOPEVALTO=5;
	t62_config.nHOPST=5;
	t62_config.nNLGAIN=80;
	t62_config.nMINNLTHR=44;
	t62_config.nINCNLTHR=26;
	t62_config.nADCSPERXTHR=26;
	t62_config.nNLTHRMARGIN=26;
	t62_config.nMAXADCSPERX=63;
	t62_config.nACTVADCSVLDNOD=0;
	t62_config.nIDLEADCSVLDNOD=0;
	t62_config.nMINGCLIMIT=4;
	t62_config.nMAXGCLIMIT=64;
	t62_config.nRESERVED[0]=0;
	t62_config.nRESERVED[1]=0;
	t62_config.nRESERVED[2]=0;
	t62_config.nRESERVED[3]=0;
	t62_config.nRESERVED[4]=0;

	t62_config.nBLEN[0]=0;
	t62_config.nTCHTHR[0]=0;
	t62_config.nTCHDI[0]=2;
	t62_config.nMOVHYSTI[0]=0;
	t62_config.nMOVHYSTN[0]=0;
	t62_config.nMOVFILTER[0]=0;
	t62_config.nNUMTOUCH[0]=0;
	t62_config.nMRGHYST[0]=0;
	t62_config.nMRGTHR[0]=0;
	t62_config.nXLOCLIP[0]=0;
	t62_config.nXHICLIP[0]=0;
	t62_config.nYLOCLIP[0]=0;
	t62_config.nYHICLIP[0]=0;
	t62_config.nXEDGECTRL[0]=0;
	t62_config.nXEDGEDIST[0]=0;
	t62_config.nYEDGECTRL[0]=0;
	t62_config.nYEDGEDIST[0]=0;
	t62_config.nJUMPLIMIT[0]=0;
	t62_config.nTCHHYST[0]=0;
	t62_config.nNEXTTCHDI[0]=0;

#endif

	error = mxt_write_block(client, obj_addr,
			obj_size, (u8 *)&t62_config);

	if (error < 0) {
		dev_err(&client->dev, "[TSP] mxt_write_block failed! "
				"(%s, %d)\n", __func__, __LINE__);
		return -EIO;
	}

	return 0;
}

int mxt_PROCI_ACTIVESTYLUS_T63(struct mxt_data *mxt)
{
	struct i2c_client *client = mxt->client;
	u16 obj_addr=0, obj_size=0;
	int error=0;

    get_object_info(copy_data,
            PROCI_ACTIVESTYLUS_T63, &obj_size, &obj_addr);
	printk("PROCI_ACTIVESTYLUS_T63 obj_size=%d obj_addr=%d OBJECT_SIZE=12  OBJECT_ADDRESS=873 \n",obj_size,obj_addr );
	memset(&t63_config, 0, sizeof(t63_config));

	t63_config.nCTRL  =0;
	t63_config.nMAXTCHAREA  =0;
	t63_config.nSIGPWR  =0;
	t63_config.nSIGRATIO  =0;
	t63_config.nSIGCNTMAX  =0;
	t63_config.nXADJUST  =0;
	t63_config.nYADJUST  =0;
	t63_config.nCOMMSFILTER  =0;
	t63_config.nDETADCSPERX  =0;
	t63_config.nSUPDIST  =0;
	t63_config.nSUPDISTHYST  =0;
	t63_config.nSUPTO  =0;

	error = mxt_write_block(client, obj_addr,
			obj_size, (u8 *)&t63_config);

	memset(&t63_config, 0, sizeof(t63_config));
	error = mxt_write_block(client, 897,
			17, (u8 *)&t63_config);


	if (error < 0) {
		dev_err(&client->dev, "[TSP] mxt_write_block failed! "
				"(%s, %d)\n", __func__, __LINE__);
		return -EIO;
	}

	return 0;
}

int mxt_PROCI_LENSBENDING_T65(struct mxt_data *mxt)
{
	struct i2c_client *client = mxt->client;
	u16 obj_addr=0, obj_size=0;
	int error=0;

    get_object_info(copy_data,
            PROCI_LENSBENDING_T65, &obj_size, &obj_addr);
	printk("mxt_PROCI_LENSBENDING_T65 obj_size=%d obj_addr=%d OBJECT_SIZE=17  OBJECT_ADDRESS=897 \n",obj_size,obj_addr );
	memset(&t65_config, 0, sizeof(t65_config));

	error = mxt_write_block(client, obj_addr,
			obj_size, (u8 *)&t65_config);

	if (error < 0) {
		dev_err(&client->dev, "[TSP] mxt_write_block failed! "
				"(%s, %d)\n", __func__, __LINE__);
		return -EIO;
	}

	return 0;
}

int mxt_SPT_GOLDENREFERENCES_T66(struct mxt_data *mxt)
{
	struct i2c_client *client = mxt->client;
	u16 obj_addr=0, obj_size=0;
	int error=0;

    get_object_info(copy_data,
            SPT_GOLDENREFERENCES_T66, &obj_size, &obj_addr);
	printk("mxt_SPT_GOLDENREFERENCES_T66 obj_size=%d obj_addr=%d OBJECT_SIZE=5  OBJECT_ADDRESS=914 \n",obj_size,obj_addr );
	memset(&t66_config, 0, sizeof(t66_config));

	error = mxt_write_block(client, obj_addr,
			obj_size, (u8 *)&t66_config);

	if (error < 0) {
		dev_err(&client->dev, "[TSP] mxt_write_block failed! "
				"(%s, %d)\n", __func__, __LINE__);
		return -EIO;
	}

	return 0;
}

int mxt_SPT_SERIALDATACOMMAND_T68(struct mxt_data *mxt)
{
	struct i2c_client *client = mxt->client;
	u16 obj_addr=0, obj_size=0;
	int error=0;

    get_object_info(copy_data,
            SPT_SERIALDATACOMMAND_T68, &obj_size, &obj_addr);
	printk("SPT_SERIALDATACOMMAND_T68 obj_size=%d obj_addr=%d OBJECT_SIZE=73 ADDRESS=318 \n",obj_size,obj_addr );
	memset(&t68_config, 0, sizeof(t68_config));

	error = mxt_write_block(client, obj_addr,
			obj_size, (u8 *)&t68_config);

	if (error < 0) {
		dev_err(&client->dev, "[TSP] mxt_write_block failed! "
				"(%s, %d)\n", __func__, __LINE__);
		return -EIO;
	}

	return 0;
}

int mxt_config_settings(struct mxt_data *mxt)
{
	printk("[TSP] mxt_config_settings  \n");

	if (mxt_GEN_POWERCONFIG_T7(mxt) < 0)
	{
		printk("[TSP] mxt_GEN_POWERCONFIG_T7 error  \n");
		return -1;
	}
	if (mxt_GEN_ACQUISITIONCONFIG_T8(mxt) < 0)
	{
		printk("[TSP] mxt_GEN_ACQUISITIONCONFIG_T8 error  \n");
		return -1;
	}
	if (mxt_TOUCH_MULTITOUCHSCREEN_T9(mxt) < 0)
	{
		printk("[TSP] mxt_TOUCH_MULTITOUCHSCREEN_T9 error  \n");
		return -1;
	}
	if (mxt_TOUCH_KEYARRAY_T15(mxt) < 0)
	{
		printk("[TSP] mxt_TOUCH_KEYARRAY_T15 error  \n");
		return -1;
	}

	if (mxt_SPT_COMMSCONFIG_T18(mxt) < 0)
	{
		printk("[TSP] mxt_TOUCH_KEYARRAY_T15 error  \n");
		return -1;
	}
	
	if (mxt_PROCI_GRIPSUPPRESSION_T40(mxt) < 0)
	{
		printk("[TSP] mxt_PROCI_GRIPSUPPRESSION_T40 error  \n");
		return -1;
	}
	if (mxt_PROCI_TOUCHSUPPRESSION_T42(mxt) < 0)
	{
		printk("[TSP] mxt_PROCI_TOUCHSUPPRESSION_T42 error  \n");
		return -1;
	}
	if (mxt_SPT_DIGITIZER_T43(mxt) < 0)	
	{
		printk("[TSP] mxt_SPT_DIGITIZER_T43 error  \n");
		return -1;
	}
	if (mxt_SPT_CTECONFIG_T46(mxt) < 0)
	{
		printk("[TSP] mxt_SPT_CTECONFIG_T46 error  \n");
		return -1;
	}
	if (mxt_PROCI_STYLUS_T47(mxt) < 0)
	{
		printk("[TSP] mxt_PROCI_STYLUS_T47 error  \n");
		return -1;
	}
	if (mxt_PROCI_SHIELDLESS_T56(mxt) < 0)
	{
		printk("[TSP] mxt_PROCI_SHIELDLESS_T56 error  \n");
		return -1;
	}
	if (mxt_PROCG_NOISESUPPRESSION_T62(mxt) < 0)
	{
		printk("[TSP] mxt_PROCG_NOISESUPPRESSION_T62 error  \n");
		return -1;
	}
	return 0;
}


int mxt_defconfig_settings(struct mxt_data *mxt,int config_num )
{
	int err=0;

	switch(config_num)
	{
		case 	GEN_COMMANDPROCESSOR_T6:
			printk("[ATMEL] mxt_defconfig_settings Not Support.. \n");
			break;	
		case 	GEN_POWERCONFIG_T7:
			err=mxt_GEN_POWERCONFIG_T7(mxt);
			break;
		case 	GEN_ACQUISITIONCONFIG_T8:
			err=mxt_GEN_ACQUISITIONCONFIG_T8(mxt);
			break;
		case 	TOUCH_MULTITOUCHSCREEN_T9:
			err=mxt_TOUCH_MULTITOUCHSCREEN_T9(mxt);
			break;
		case 	TOUCH_KEYARRAY_T15:
			err=mxt_TOUCH_KEYARRAY_T15(mxt);
			break;
		case 	SPT_COMMSCONFIG_T18:
			err=mxt_SPT_COMMSCONFIG_T18(mxt);
			break;
		case 	SPT_GPIOPWM_T19:
			printk("[ATMEL] mxt_defconfig_settings Not Support.. \n");
			break;
		case 	PROCI_GRIPFACESUPPRESSION_T20:
			printk("[ATMEL] mxt_defconfig_settings Not Support.. \n");
			break;
		case 	PROCG_NOISESUPPRESSION_T22:
			printk("[ATMEL] mxt_defconfig_settings Not Support.. \n");
			break;
		case 	PROCI_ONETOUCHGESTUREPROCESSOR_T24:
			err=mxt_PROCI_ONETOUCHGESTUREPROCESSOR_T24(mxt);
			break;
		case 	SPT_SELFTEST_T25:
			err=mxt_SPT_SELFTEST_T25(mxt);
			break;
		case 	PROCI_TWOTOUCHGESTUREPROCESSOR_T27:
			err=mxt_PROCI_TWOTOUCHGESTUREPROCESSOR_T27(mxt);
			break;
		case 	SPT_CTECONFIG_T28:
			break;
		case 	DEBUG_DIAGNOSTIC_T37:
			err=mxt_DEBUG_DIAGNOSTIC_T37(mxt);
			break;
		case 	SPT_USERDATA_T38:
			err=mxt_SPT_USERDATA_T38(mxt);
			break;
		case 	PROCI_GRIPSUPPRESSION_T40:
			err=mxt_PROCI_GRIPSUPPRESSION_T40(mxt);
			break;
		case 	PROCI_TOUCHSUPPRESSION_T42:
			err=mxt_PROCI_TOUCHSUPPRESSION_T42(mxt);
			break;
		case 	SPT_DIGITIZER_T43:
			err=mxt_SPT_DIGITIZER_T43(mxt);
			break;
		case 	SPT_CTECONFIG_T46:
			err=mxt_SPT_CTECONFIG_T46(mxt);
			break;
		case 	PROCI_STYLUS_T47:
			err=mxt_PROCI_STYLUS_T47(mxt);
			break;
		case 	PROCG_NOISESUPPRESSION_T48:
			break;
		case 	TOUCH_PROXIMITY_KEY_T52:
			break;
		case 	PROCI_ADAPTIVETHRESHOLD_T55:
			err=mxt_PROCI_ADAPTIVETHRESHOLD_T55(mxt);
			break;
		case 	PROCI_SHIELDLESS_T56:
			err=mxt_PROCI_SHIELDLESS_T56(mxt);
			break;
		case 	PROCI_EXTRATOUCHSCREENDATA_T57:
			err=mxt_SPT_GENERICDATA_T57(mxt);
			break;
		case 	SPT_TIMER_T61:
			err=mxt_SPT_TIMER_T61(mxt);
			break;
		case 	PROCG_NOISESUPPRESSION_T62:
			err=mxt_PROCG_NOISESUPPRESSION_T62(mxt);
			break;
		case 	PROCI_ACTIVESTYLUS_T63:
			err=mxt_PROCI_ACTIVESTYLUS_T63(mxt);
			break;
		case 	PROCI_LENSBENDING_T65:
			err=mxt_PROCI_LENSBENDING_T65(mxt);
			break;
		case 	SPT_GOLDENREFERENCES_T66:
			err=mxt_SPT_GOLDENREFERENCES_T66(mxt);
			break;
		case 	SPT_SERIALDATACOMMAND_T68:
			err=mxt_SPT_SERIALDATACOMMAND_T68(mxt);
			break;
		
		default:
			err=-1;
			break;
		}
	return err;
}



#if DEBUG_INFO
static u8       *object_type_name[60] = {
/*      [0]     = "Reserved",   */
/*      [2]     = "T2 - Obsolete",      */
/*      [3]     = "T3 - Obsolete",      */
        [5]     = "GEN_MESSAGEPROCESSOR_T5",
        [6]     = "GEN_COMMANDPROCESSOR_T6",
        [7]     = "GEN_POWERCONFIG_T7",
        [8]     = "GEN_ACQUIRECONFIG_T8",
        [9]     = "TOUCH_MULTITOUCHSCREEN_T9",
        [15]    = "TOUCH_KEYARRAY_T15",
        [18]    = "SPT_COMCONFIG_T18",
/*      [22]    = "PROCG_NOISESUPPRESSION_T22",*/
/*      [23]    = "TOUCH_PROXIMITY_T23",*/
/*      [24]    = "PROCI_ONETOUCHGESTUREPROCESSOR_T24",*/
        [25]    = "SPT_SELFTEST_T25",
/*      [26]    = "T26 - Obsolete",*/
/*      [27]    = "PROCI_TWOTOUCHGESTUREPROCESSOR_T27",*/
/*      [28]    = "SPT_CTECONFIG_T28",*/
        [37]    = "DEBUG_DIAGNOSTICS_T37",
        [38]    = "USER_DATA_T38",
        [40]    = "PROCI_GRIPSUPPRESSION_T40",
        [42]    = "PROCI_TOUCHSUPPRESSION_T42",
        [43]    = "SPT_DIGITIZER_T43",
        [46]    = "SPT_CTECONFIG_T46",
        [48]    = "PROCG_NOISESUPPRESSION_T48",
        [56]    = "PROCI_SHIELDLESS_T56",
        [57]    = "SPT_GENERICDATA_T57",
        [62]   = "PROCG_NOISESUPPRESSION_T62",
};
#endif

/* declare function proto type */
static void mxt_ta_probe(int ta_status);
static void report_input_data(struct mxt_data *data);

static int read_mem(struct mxt_data *data, u16 reg, u8 len, u8 *buf)
{
        int ret;
        u16 le_reg = cpu_to_le16(reg);
        struct i2c_msg msg[2] = {
                {
                        .addr = data->client->addr,
                        .flags = 0,
                        .len = 2,
                        .buf = (u8 *)&le_reg,
                },
                {
                        .addr = data->client->addr,
                        .flags = I2C_M_RD,
                        .len = len,
                        .buf = buf,
                },
        };

        ret = i2c_transfer(data->client->adapter, msg, 2);

        if (ret < 0) {
                pr_err("i2c failed ret = %d\n", ret);
                return ret;
        }
        return ret == 2 ? 0 : -EIO;
}

static int write_mem(struct mxt_data *data, u16 reg, u8 len, const u8 *buf)
{
        int ret;
        u8 tmp[len + 2];

        put_unaligned_le16(cpu_to_le16(reg), tmp);
        memcpy(tmp + 2, buf, len);

        ret = i2c_master_send(data->client, tmp, sizeof(tmp));

        if (ret < 0)
                return ret;

        return ret == sizeof(tmp) ? 0 : -EIO;
}

static int __devinit mxt_reset(struct mxt_data *data)
{
        u8 buf = 1u;
        return write_mem(data, data->cmd_proc + CMD_RESET_OFFSET, 1, &buf);
}

static int __devinit mxt_backup(struct mxt_data *data)
{
        u8 buf = 0x55u;
        return write_mem(data, data->cmd_proc + CMD_BACKUP_OFFSET, 1, &buf);
}




static int write_config(struct mxt_data *data, u8 type, const u8 *cfg)
{
        int ret;
        u16 address = 0;
        u16 size = 0;

        ret = get_object_info(data, type, &size, &address);

        if (size == 0 && address == 0)
                return 0;
        else
                return write_mem(data, address, size, cfg);
}

static int check_instance(struct mxt_data *data, u8 object_type)
{
        int i;

        for (i = 0; i < data->objects_len; i++) {
                if (data->objects[i].object_type == object_type)
                        return data->objects[i].instances;
        }
        return 0;
}

static int init_write_config(struct mxt_data *data, u8 type, const u8 *cfg)
{
        int ret;
        u16 address = 0;
        u16 size = 0;
        u8 *temp;

        ret = get_object_info(data, type, &size, &address);

        if ((size == 0) || (address == 0)) {
                pr_err("%s error object_type(%d)\n", __func__, type);
                return -ENODEV;
        }

        ret = write_mem(data, address, size, cfg);

        if (check_instance(data, type)) {
#if DEBUG_INFO
                pr_info("exist instance1 object (%d)\n", type);
#endif
                temp = kmalloc(size * sizeof(u8), GFP_KERNEL);
                memset(temp, 0, size);
                ret |= write_mem(data, address+size, size, temp);
                kfree(temp);
        }

        return ret;
}

static int change_config(struct mxt_data *data,
                        u16 reg, u8 offeset, u8 change_value)
{
        u8 value = 0;

        value = change_value;
        return write_mem(data, reg+offeset, 1, &value);
}

static u32 __devinit crc24(u32 crc, u8 byte1, u8 byte2)
{
        static const u32 crcpoly = 0x80001B;
        u32 res;
        u16 data_word;

        data_word = (((u16)byte2) << 8) | byte1;
        res = (crc << 1) ^ (u32)data_word;

        if (res & 0x1000000)
                res ^= crcpoly;

        return res;
}

static int __devinit calculate_infoblock_crc(struct mxt_data *data,
                                                        u32 *crc_pointer)
{
        u32 crc = 0;
        u8 mem[7 + data->objects_len * 6];
        int status;
        int i;

        status = read_mem(data, 0, sizeof(mem), mem);

        if (status)
                return status;

        for (i = 0; i < sizeof(mem) - 1; i += 2)
                crc = crc24(crc, mem[i], mem[i + 1]);

        *crc_pointer = crc24(crc, mem[i], 0) & 0x00FFFFFF;

        return 0;
}

uint8_t calibrate_chip_e(void)
{
        u8 cal_data = 1;
        int ret = 0;
        /* send calibration command to the chip */
        ret = write_mem(copy_data,
                copy_data->cmd_proc + CMD_CALIBRATE_OFFSET,
                1, &cal_data);
        /* set flag for calibration lockup
        recovery if cal command was successful */
        if (!ret)
                printk("calibration success!!!\n");
        return ret;
}

static void treat_error_status(void)
{
        bool ta_status = 0;
#if !(FOR_BRINGUP)			
        u16 size;
        u16 obj_address = 0;
#endif
        int error = 0;
        struct mxt_data *data = copy_data;

        data->read_ta_status(&ta_status);

        if (treat_median_error_status) {
                pr_err("Error status already treated\n");
                return;
        } else
                treat_median_error_status = 1;
#if DEBUG_INFO
        printk("[ATMEL]_______Error status TA is[%d]\n", ta_status);
#endif
        if (ta_status) {
#if !(FOR_BRINGUP)
                get_object_info(data,
                        GEN_POWERCONFIG_T7, &size, &obj_address);
                /* 1:ACTVACQINT */
                error = change_config(data, obj_address, 1, 255);

                get_object_info(data,
                        GEN_ACQUISITIONCONFIG_T8, &size, &obj_address);
                /* 0:CHRGTIME */
                error |= change_config(data, obj_address, 0, 64);

                /* 8:ATCHFRCCALTHR*/
                error |= change_config(data, obj_address, 8, 50);
                /* 9:ATCHFRCCALRATIO*/
                error |= change_config(data, obj_address, 9, 0);

                get_object_info(data,
                        PROCI_TOUCHSUPPRESSION_T42, &size, &obj_address);
                /* 0:CTRL */
                error |= change_config(data, obj_address, 0, 3);

                get_object_info(data,
                        SPT_CTECONFIG_T46, &size, &obj_address);
                /* 2:IDLESYNCSPERX */
                error |= change_config(data, obj_address, 2, 48);
                /* 3:ACTVSYNCSPERX */
                error |= change_config(data, obj_address, 3, 48);

                get_object_info(data,
                        PROCG_NOISESUPPRESSION_T48, &size, &obj_address);
                /* 2:CALCFG */
                error |= change_config(data, obj_address, 2, 114);
                /* 3:BASEFREQ */
                error |= change_config(data, obj_address, 3, 15);
                /* 8:MFFREQ[0] */
                error |= change_config(data, obj_address, 8, 3);
                /* 9:MFFREQ[1] */
                error |= change_config(data, obj_address, 9, 5);
                /* 10:NLGAIN*/
                error |= change_config(data, obj_address, 10, 96);
                /* 11:NLTHR*/
                error |= change_config(data, obj_address, 11, 30);
                /* 17:GCMAXADCSPERX */
                error |= change_config(data, obj_address, 17, 100);
                /* 34:BLEN[0] */
                error |= change_config(data, obj_address, 34, 80);
                /* 35:TCHTHR[0] */
                error |= change_config(data, obj_address, 35, 40);
                /* 36:TCHDI[0] */
                error |= change_config(data, obj_address, 36, 2);
                /* 39:MOVFILTER[0] */
                error |= change_config(data, obj_address, 39, 65);
                /* 41:MRGHYST[0] */
                error |= change_config(data, obj_address, 41, 40);
                /* 42:MRGTHR[0] */
                error |= change_config(data, obj_address, 42, 50);
                /* 43:XLOCLIP[0] */
                error |= change_config(data, obj_address, 43, 5);
                /* 44:XHICLIP[0] */
                error |= change_config(data, obj_address, 44, 5);
                /* 51:JUMPLIMIT[0] */
                error |= change_config(data, obj_address, 51, 25);
                /* 52:TCHHYST[0] */
                error |= change_config(data, obj_address, 52, 15);
#endif
                if (error < 0)
                        pr_err("failed to write error status\n");
        } else {
#if !(FOR_BRINGUP)
                get_object_info(data,
                        GEN_POWERCONFIG_T7, &size, &obj_address);
                /* 1:ACTVACQINT */
                error = change_config(data, obj_address, 1, 255);

                get_object_info(data,
                        GEN_ACQUISITIONCONFIG_T8, &size, &obj_address);
                /* 0:CHRGTIME */
                error |= change_config(data, obj_address, 0, 64);

                /* 8:ATCHFRCCALTHR*/
                error |= change_config(data, obj_address, 8, 50);
                /* 9:ATCHFRCCALRATIO*/
                error |= change_config(data, obj_address, 9, 0);

                get_object_info(data,
                        TOUCH_MULTITOUCHSCREEN_T9, &size, &obj_address);
                /* 31:TCHHYST */
                error |= change_config(data, obj_address, 31, 15);

                get_object_info(data,
                        PROCI_TOUCHSUPPRESSION_T42, &size, &obj_address);
                /* 0:CTRL */
                error |= change_config(data, obj_address, 0, 3);

                get_object_info(data,
                        SPT_CTECONFIG_T46, &size, &obj_address);
                /* 2:IDLESYNCSPERX */
                error |= change_config(data, obj_address, 2, 48);
                /* 3:ACTVSYNCSPERX */
                error |= change_config(data, obj_address, 3, 48);

                get_object_info(data,
                        PROCG_NOISESUPPRESSION_T48, &size, &obj_address);
                /* 2:CALCFG */
                error |= change_config(data, obj_address, 2, 242);
                /* 3:BASEFREQ */
                error |= change_config(data, obj_address, 3, 15);
                /* 8:MFFREQ[0] */
                error |= change_config(data, obj_address, 8, 3);
                /* 9:MFFREQ[1] */
                error |= change_config(data, obj_address, 9, 5);
                /* 10:NLGAIN*/
                error |= change_config(data, obj_address, 10, 112);
                /* 11:NLTHR*/
                error |= change_config(data, obj_address, 11, 25);
                /* 17:GCMAXADCSPERX */
                error |= change_config(data, obj_address, 17, 100);
                /* 34:BLEN[0] */
                error |= change_config(data, obj_address, 34, 112);
                /* 35:TCHTHR[0] */
                error |= change_config(data, obj_address, 35, 40);
                /* 41:MRGHYST[0] */
                error |= change_config(data, obj_address, 41, 40);
                /* 42:MRGTHR[0] */
                error |= change_config(data, obj_address, 42, 50);
                /* 51:JUMPLIMIT[0] */
                error |= change_config(data, obj_address, 51, 25);
                /* 52:TCHHYST[0] */
                error |= change_config(data, obj_address, 52, 15);
#endif
                if (error < 0)
                        pr_err("failed to write error status\n");
        }
}

#if TOUCH_BOOSTER
static void mxt_set_dvfs_off(struct work_struct *work)
{
        struct mxt_data *data =
                container_of(work, struct mxt_data, dvfs_dwork.work);

        if (mxt_enabled) {
                disable_irq(IRQ_EINT(4));
                if (touch_cpu_lock_status
                        && !tsp_press_status){
                        exynos_cpufreq_lock_free(DVFS_LOCK_ID_TSP);
                        touch_cpu_lock_status = 0;
                }
                enable_irq(IRQ_EINT(4));
        }
}

static void mxt_set_dvfs_on(struct mxt_data *data)
{
        cancel_delayed_work(&data->dvfs_dwork);
        if (cpu_lv < 0)
                exynos_cpufreq_get_level(TOUCH_BOOSTER_LIMIT_CLK, &cpu_lv);
        exynos_cpufreq_lock(DVFS_LOCK_ID_TSP, cpu_lv);
        touch_cpu_lock_status = 1;
}
#endif

static void mxt_ta_probe(int ta_status)
{
#if !(FOR_BRINGUP)
        u16 obj_address = 0;
        u16 size;
        int error;

        struct mxt_data *data = copy_data;

        if (!mxt_enabled) {
                pr_err("%s mxt_enabled is 0\n", __func__);
                return;
        }

        if (treat_median_error_status)
                treat_median_error_status = 0;
        if (ta_status) {
                error = write_config(data, data->t48_config_chrg[0],
                                        data->t48_config_chrg + 1);

                threshold = data->tchthr_charging;

                if (error < 0)
                        pr_err("ta_probe write config Error!!\n");
        } else {
                error |= write_config(data, data->t48_config_batt[0],
                                        data->t48_config_batt + 1);

                threshold = data->tchthr_batt;

                if (error < 0)
                        pr_err("%s write config Error!!\n", __func__);
        }
#endif
        pr_info("%s : threshold[%d]\n", __func__, threshold);
};

uint8_t reportid_to_type(struct mxt_data *data, u8 report_id, u8 *instance)
{
        struct report_id_map_t *report_id_map;
        report_id_map = rid_map;

        if (report_id <= max_report_id) {
                *instance = report_id_map[report_id].instance;
                return report_id_map[report_id].object_type;
        } else
                return 0;
}

static int __devinit mxt_init_touch_driver(struct mxt_data *data)
{
        struct object_t *object_table;
        struct report_id_map_t *report_id_map_t;
        u32 read_crc = 0;
        u32 calc_crc;
        u16 crc_address;
        u16 dummy;
        int i, j;
        u8 id[ID_BLOCK_SIZE];
        int ret;
        u8 type_count = 0;
        u8 tmp;
        int current_report_id, start_report_id;

        ret = read_mem(data, 0, sizeof(id), id);
        if (ret)
                return ret;
#if DEBUG_INFO
        printk("[ATMEL]_______family = %#02x, variant = %#02x, version "
                        "= %#02x, build = %#02x, "
                        "matrix X,Y size = %d,%d objects_len=%d\n"
                        , id[0], id[1], id[2], id[3], id[4], id[5],id[6]);
#endif

        data->family_id = id[0];
        data->tsp_variant = id[1];
        data->tsp_version = id[2];
        data->tsp_build = id[3];
        data->objects_len = id[6];

        object_table = kmalloc(data->objects_len * sizeof(*object_table),
                                GFP_KERNEL);
        if (!object_table)
                return -ENOMEM;

        ret = read_mem(data, OBJECT_TABLE_START_ADDRESS,
                        data->objects_len * sizeof(*object_table),
                        (u8 *)object_table);
        if (ret)
                goto err;

        max_report_id = 0;

        for (i = 0; i < data->objects_len; i++) {
                object_table[i].i2c_address =
                        le16_to_cpu(object_table[i].i2c_address);
                max_report_id += object_table[i].num_report_ids *
                                                (object_table[i].instances + 1);
                tmp = 0;
                if (object_table[i].num_report_ids) {
                        tmp = type_count + 1;
                        type_count += object_table[i].num_report_ids *
                                                (object_table[i].instances + 1);
                }
                switch (object_table[i].object_type) {
                case TOUCH_MULTITOUCHSCREEN_T9:
                        data->finger_type = tmp;
                        pr_info("Finger type = %d\n",
                                                data->finger_type);
                        break;
                case GEN_MESSAGEPROCESSOR_T5:
#if ITDEV
                        data->msg_proc_addr = object_table[i].i2c_address;
#endif
                        data->msg_object_size = object_table[i].size + 1;
                        break;
                }
        }
        if (rid_map_alloc) {
                rid_map_alloc = false;
                kfree(rid_map);
        }
        rid_map = kmalloc((sizeof(report_id_map_t) * max_report_id + 1),
                                        GFP_KERNEL);

        if (!rid_map) {
                kfree(object_table);
                return -ENOMEM;
        }
        rid_map_alloc = true;
        rid_map[0].instance = 0;
        rid_map[0].object_type = 0;
        current_report_id = 1;

        for (i = 0; i < data->objects_len; i++) {
                if (object_table[i].num_report_ids != 0) {
                        for (j = 0; j <= object_table[i].instances; j++) {
                                for (start_report_id = current_report_id;
                                     current_report_id <
                                     (start_report_id +
                                      object_table[i].num_report_ids);
                                     current_report_id++) {
                                        rid_map[current_report_id].instance = j;
                                        rid_map[current_report_id].object_type =
                                            object_table[i].object_type;
                                }
                        }
                }
        }
        data->objects = object_table;

        /* Verify CRC */
        crc_address = OBJECT_TABLE_START_ADDRESS +
                        data->objects_len * OBJECT_TABLE_ELEMENT_SIZE;

#ifdef __BIG_ENDIAN
#error The following code will likely break on a big endian machine
#endif
        ret = read_mem(data, crc_address, 3, (u8 *)&read_crc);
        if (ret)
                goto err;

        read_crc = le32_to_cpu(read_crc);

        ret = calculate_infoblock_crc(data, &calc_crc);
        if (ret)
                goto err;

        if (read_crc != calc_crc) {
                pr_err("CRC error\n");
                ret = -EFAULT;
                goto err;
        }

        ret = get_object_info(data, GEN_MESSAGEPROCESSOR_T5, &dummy,
                                        &data->msg_proc);
        if (ret)
                goto err;

        ret = get_object_info(data, GEN_COMMANDPROCESSOR_T6, &dummy,
                                        &data->cmd_proc);
        if (ret)
                goto err;

#if DEBUG_INFO
        pr_info("maXTouch: %d Objects\n",
                        data->objects_len);

        for (i = 0; i < data->objects_len; i++) {
                pr_info("Type:\t\t\t[%d]: %s\n",
                         object_table[i].object_type,
                         object_type_name[object_table[i].object_type]);
                pr_info("\tAddress:\t0x%04X\n",
                         object_table[i].i2c_address);
                pr_info("\tSize:\t\t%d Bytes\n",
                         object_table[i].size);
                pr_info("\tInstances:\t%d\n",
                         object_table[i].instances);
                pr_info("\tReport Id's:\t%d\n",
                         object_table[i].num_report_ids);
        }
#endif

        return 0;

err:
        kfree(object_table);
        return ret;
}

static void report_input_data(struct mxt_data *data)
{
        int i;
        int count = 0;
        int report_count = 0;

        for (i = 0; i < data->num_fingers; i++) {
                if (data->fingers[i].state == MXT_STATE_INACTIVE)
                        continue;

                if (data->fingers[i].state == MXT_STATE_RELEASE) {
                        input_mt_slot(data->input_dev, i);
                        input_mt_report_slot_state(data->input_dev,
                                        MT_TOOL_FINGER, false);
#if DEBUG_INFO
					        printk("[ATMEL]_______report_input_data release slot =%d \n",i);
#endif
                } else {
                        input_mt_slot(data->input_dev, i);
                        input_mt_report_slot_state(data->input_dev,
                                        MT_TOOL_FINGER, true);
                        input_report_abs(data->input_dev, ABS_MT_POSITION_X,
                                        data->fingers[i].x);
                        input_report_abs(data->input_dev, ABS_MT_POSITION_Y,
                                        data->fingers[i].y);
                        input_report_abs(data->input_dev, ABS_MT_TOUCH_MAJOR,
                                        data->fingers[i].z);
                        input_report_abs(data->input_dev, ABS_MT_PRESSURE,data->fingers[i].w);
#if DEBUG_INFO
					        printk("[ATMEL]_______report_input_data x=%d, y=%d w=%d\n",data->fingers[i].x,data->fingers[i].y,data->fingers[i].w);
#endif
                }
                #ifdef _SUPPORT_SHAPE_TOUCH_
                input_report_abs(data->input_dev, ABS_MT_COMPONENT,
                        data->fingers[i].component);
                #endif
					//input_mt_sync(data->input_dev);

                report_count++;

#if SHOW_COORDINATE
                switch (data->fingers[i].state) {
                case MXT_STATE_PRESS:
                        pr_info("P: "
                                "id[%d],x=%d,y=%d,w=%d\n",
                                i, data->fingers[i].x, data->fingers[i].y
                                , data->fingers[i].w);
                        break;
/*
                case MXT_STATE_MOVE:
                        pr_info("M: "
                                "id[%d],x=%d,y=%d,w=%d,mc=%d\n",
                                i, data->fingers[i].x, data->fingers[i].y
                                , data->fingers[i].w, data->fingers[i].mcount);
                        break;
*/
                case MXT_STATE_RELEASE:
                        pr_info("R: "
                                "id[%d],mc=%d\n",
                                i, data->fingers[i].mcount);
                        break;
                default:
                        break;
                }
#else
                if (data->fingers[i].state == MXT_STATE_PRESS)
                        pr_info("P: id[%d],w=%d\n"
                                , i, data->fingers[i].w);
                else if (data->fingers[i].state == MXT_STATE_RELEASE)
                        pr_info("R: id[%d],mc=%d\n"
                                , i, data->fingers[i].mcount);
#endif
                if (data->fingers[i].state == MXT_STATE_RELEASE) {
                        data->fingers[i].state = MXT_STATE_INACTIVE;
                        data->fingers[i].mcount = 0;
                } else {
                        data->fingers[i].state = MXT_STATE_MOVE;
                        count++;
                }
        }
        if (report_count > 0) {
#if ITDEV
                if (!driver_paused)
#endif
                        input_sync(data->input_dev);
        }

        if (count)
                touch_is_pressed = 1;
        else
                touch_is_pressed = 0;

#if TOUCH_BOOSTER
        if (count == 0) {
                if (touch_cpu_lock_status) {
                        cancel_delayed_work(&data->dvfs_dwork);
                        schedule_delayed_work(&data->dvfs_dwork,
                                msecs_to_jiffies(TOUCH_BOOSTER_TIME));
                }
                tsp_press_status = 0;
        } else
                tsp_press_status = 1;
#endif
        data->finger_mask = 0;
}

static irqreturn_t mxt_irq_thread(int irq, void *ptr)
{
        struct mxt_data *data = ptr;
        int id;
        u8 msg[data->msg_object_size];
        u8 touch_message_flag = 0;
        u16 obj_address = 0;
        u16 size;
        u8 value;
        int error;
        u8 object_type, instance;

        do {

                touch_message_flag = 0;
                if (read_mem(data, data->msg_proc, sizeof(msg), msg)) {
#if TOUCH_BOOSTER
                        if (touch_cpu_lock_status) {
                                exynos_cpufreq_lock_free(DVFS_LOCK_ID_TSP);
                                touch_cpu_lock_status = 0;
                        }
#endif
                        return IRQ_HANDLED;
                }
#if ITDEV
                if (debug_enabled)
                        print_hex_dump(KERN_INFO, "MXT MSG:",
                        DUMP_PREFIX_NONE, 16, 1, msg, sizeof(msg), false);
#endif

                object_type = reportid_to_type(data, msg[0] , &instance);
                if (object_type == GEN_COMMANDPROCESSOR_T6) {
                        if (msg[1] == 0x00) /* normal mode */
                                printk("normal mode\n");
                        if ((msg[1]&0x04) == 0x04) /* I2C checksum error */
                                printk("I2C checksum error\n");
                        if ((msg[1]&0x08) == 0x08) /* config error */
                                printk("config error\n");
                        if ((msg[1]&0x10) == 0x10) /* calibration */
                                printk("calibration is"
                                        " on going !!\n");
                        if ((msg[1]&0x20) == 0x20) /* signal error */
                                printk("signal error\n");
                        if ((msg[1]&0x40) == 0x40) /* overflow */
                                printk("overflow detected\n");
                        if ((msg[1]&0x80) == 0x80) /* reset */
                                printk("reset is ongoing\n");
                }

                if (object_type == PROCI_TOUCHSUPPRESSION_T42) {
                        get_object_info(data, GEN_ACQUISITIONCONFIG_T8,
                                                        &size, &obj_address);
                        if ((msg[1] & 0x01) == 0x00) {
                                /* Palm release */
                                printk("palm touch released\n");
                                touch_is_pressed = 0;

                        } else if ((msg[1] & 0x01) == 0x01) {
                                /* Palm Press */
                                printk("palm touch detected\n");
                                touch_is_pressed = 1;
                                touch_message_flag = 1;
                        }
                }

                if (object_type == PROCG_NOISESUPPRESSION_T48) {
                        /* pr_info("T48 [STATUS]:%#02x"
                                "[ADCSPERX]:%#02x[FRQ]:%#02x"
                                "[STATE]:%#02x[NLEVEL]:%#02x\n"
                                , msg[1], msg[2], msg[3], msg[4], msg[5]);*/

                        if (msg[4] == 5) { /* Median filter error */
                                printk("Median filter error\n");
                                if ((data->family_id == 0xA1)
                                        && ((data->tsp_version == 0x13)
                                        || (data->tsp_version == 0x20))) {
                                        if (data->read_ta_status)
                                                treat_error_status();
                                } else {
                                        get_object_info(data,
                                                PROCG_NOISESUPPRESSION_T48,
                                                &size, &obj_address);
                                        value = data->calcfg_batt;
                                        error = write_mem(data,
                                                obj_address+2, 1, &value);
                                        msleep(20);
                                        value |= 0x20;
                                        error |= write_mem(data,
                                                obj_address+2, 1, &value);
                                        if (error)
                                                printk("failed to"
                                                        "reenable CHRGON\n");
                                }
                        }
                }

#if USE_SUMSIZE
                if (object_type == SPT_GENERICDATA_T57) {
                        sum_size = msg[1];
                        sum_size +=  (msg[2]<<8);
                }
#endif

                if (object_type == TOUCH_MULTITOUCHSCREEN_T9) {
                        id = msg[0] - data->finger_type;
							//printk("[ATMEL] id=%d data->num_fingers=%d \n",id,data->num_fingers);
                        /* If not a touch event, then keep going */
                        if (id < 0 || id >= data->num_fingers)
                                        continue;
                        if (data->finger_mask & (1U << id))
                                report_input_data(data);

                        if (msg[1] & RELEASE_MSG_MASK) {
                                data->fingers[id].z = 0;
                                data->fingers[id].w = msg[5];
                                data->finger_mask |= 1U << id;
                                data->fingers[id].state = MXT_STATE_RELEASE;
                        } else if ((msg[1] & DETECT_MSG_MASK) && (msg[1] &
                                        (PRESS_MSG_MASK | MOVE_MSG_MASK))) {
#if TOUCH_BOOSTER
                                if (!touch_cpu_lock_status)
                                        mxt_set_dvfs_on(data);
#endif
                                touch_message_flag = 1;
                                data->fingers[id].z = msg[6];
                                data->fingers[id].w = msg[5];
                                data->fingers[id].x =
                                        (((msg[2] << 4) | (msg[4] >> 4))
                                        >> data->x_dropbits);
                                data->fingers[id].y =
                                        (((msg[3] << 4) | (msg[4] & 0xF))
                                        >> data->y_dropbits);

                                data->finger_mask |= 1U << id;

                                if (msg[1] & PRESS_MSG_MASK) {
                                        data->fingers[id].state =
                                                MXT_STATE_PRESS;
                                        data->fingers[id].mcount = 0;

                                } else if (msg[1] & MOVE_MSG_MASK)
                                        data->fingers[id].mcount += 1;

                                #ifdef _SUPPORT_SHAPE_TOUCH_
                                data->fingers[id].component = msg[7];
                                #endif

                        } else if ((msg[1] & SUPPRESS_MSG_MASK)
                        && (data->fingers[id].state != MXT_STATE_INACTIVE)) {
                                data->fingers[id].z = 0;
                                data->fingers[id].w = msg[5];
                                data->fingers[id].state = MXT_STATE_RELEASE;
                                data->finger_mask |= 1U << id;
                        } else {
                                /* ignore changed amplitude message */
                                if (!((msg[1] & DETECT_MSG_MASK)
                                        && (msg[1] & AMPLITUDE_MSG_MASK)))
                                        pr_err("Unknown state %#02x %#02x\n",
                                                msg[0], msg[1]);
                                continue;
                        }
                }
        } while (!gpio_get_value(data->gpio_read_done));
		 //printk("[ATMEL] data->finger_mask=%d \n",data->finger_mask);
        if (data->finger_mask) {
#if USE_SUMSIZE
                if (sum_size > 0) {
                        /* case of normal configuration */
                        u8 num_finger = 0;
                        u8 i;
                        uint16_t t9_sum_size = 0;

                        for (i = 0; i < data->num_fingers; i++) {
                                if (data->fingers[i].state == MXT_STATE_INACTIVE
                                || data->fingers[i].state == MXT_STATE_RELEASE)
                                        continue;
                                t9_sum_size += data->fingers[i].w;

                                num_finger++;
                        }

                        if ((num_finger == 1)
                                && ((sum_size-t9_sum_size) >= MAX_SUMSIZE)) {
                                pr_info(
                                        "recalibrate for max sumsize[%d]"
                                        "t9_sum_size[%d]\n"
                                        , sum_size, t9_sum_size);
                                calibrate_chip_e();
                        } else {
                                report_input_data(data);
                        }

                        t9_sum_size = 0;
                        sum_size = 0;
                } else {
                        /* case of recovery configuration */
                        report_input_data(data);
                }
#else
                report_input_data(data);
#endif
        }

        return IRQ_HANDLED;
}

static int mxt_internal_suspend(struct mxt_data *data)
{
        int i;
        int count = 0;

        for (i = 0; i < data->num_fingers; i++) {
                if (data->fingers[i].state == MXT_STATE_INACTIVE)
                        continue;
                data->fingers[i].z = 0;
                data->fingers[i].state = MXT_STATE_RELEASE;
                count++;
        }
        if (count)
                report_input_data(data);

#if TOUCH_BOOSTER
        cancel_delayed_work(&data->dvfs_dwork);
        tsp_press_status = 0;
        if (touch_cpu_lock_status) {
                exynos_cpufreq_lock_free(DVFS_LOCK_ID_TSP);
                touch_cpu_lock_status = 0;
        }
#endif
        data->power_off();

        return 0;
}

static int mxt_internal_resume(struct mxt_data *data)
{
        data->power_on();

        return 0;
}

#ifdef CONFIG_HAS_EARLYSUSPEND
#define mxt_suspend     NULL
#define mxt_resume      NULL

static void mxt_early_suspend(struct early_suspend *h)
{
        struct mxt_data *data = container_of(h, struct mxt_data,
                                                                early_suspend);
		printk("mxt_early_suspend  mxt_enabled=%d \n", mxt_enabled);
        if (mxt_enabled == 1) {
                pr_info("%s\n", __func__);
                mxt_enabled = 0;
                touch_is_pressed = 0;
                disable_irq(IRQ_EINT(4));
                mxt_internal_suspend(data);
        } else
                pr_err("%s. but touch already off\n", __func__);
}

static void mxt_late_resume(struct early_suspend *h)
{
        bool ta_status = 0;
        struct mxt_data *data = container_of(h, struct mxt_data,
                                                                early_suspend);
		printk("mxt_late_resume  mxt_enabled=%d \n", mxt_enabled);
        if (mxt_enabled == 0) {
                pr_info("%s\n", __func__);
                mxt_internal_resume(data);

                mxt_enabled = 1;

                if (data->read_ta_status) {
                        data->read_ta_status(&ta_status);
                        pr_info("ta_status is %d\n", ta_status);
                        mxt_ta_probe(ta_status);
                }

                treat_median_error_status = 0;
                calibrate_chip_e();

                enable_irq(IRQ_EINT(4));
        } else
                pr_err("%s. but touch already on\n", __func__);
}
#else
static int mxt_suspend(struct device *dev)
{
        struct i2c_client *client = to_i2c_client(dev);
        struct mxt_data *data = i2c_get_clientdata(client);

        mxt_enabled = 0;
        touch_is_pressed = 0;
#if TOUCH_BOOSTER
        tsp_press_status = 0;
#endif
        return mxt_internal_suspend(data);
}

static int mxt_resume(struct device *dev)
{
        int ret = 0;
        bool ta_status = 0;
        struct i2c_client *client = to_i2c_client(dev);
        struct mxt_data *data = i2c_get_clientdata(client);

        ret = mxt_internal_resume(data);

        mxt_enabled = 1;

        if (data->read_ta_status) {
                data->read_ta_status(&ta_status);
                pr_info("ta_status is %d\n", ta_status);
                mxt_ta_probe(ta_status);
        }
        return ret;
}
#endif

void Mxt_force_released(void)
{
        struct mxt_data *data = copy_data;
        int i;

        if (!mxt_enabled) {
                pr_err("mxt_enabled is 0\n");
                return;
        }

        for (i = 0; i < data->num_fingers; i++) {
                if (data->fingers[i].state == MXT_STATE_INACTIVE)
                        continue;
                data->fingers[i].z = 0;
                data->fingers[i].state = MXT_STATE_RELEASE;
        }
        report_input_data(data);

        calibrate_chip_e();
};

#if SYSFS
static ssize_t mxt_debug_setting(struct device *dev,
                                        struct device_attribute *attr,
                                        const char *buf, size_t count)
{
        g_debug_switch = !g_debug_switch;
        return 0;
}

static ssize_t mxt_object_setting(struct device *dev,
                                        struct device_attribute *attr,
                                        const char *buf, size_t count)
{
        struct mxt_data *data = dev_get_drvdata(dev);
        unsigned int object_type;
        unsigned int object_register;
        unsigned int register_value;
        u8 value;
        u8 val;
        int ret;
        u16 address;
        u16 size;
        sscanf(buf, "%u%u%u", &object_type, &object_register, &register_value);
        pr_info("object type T%d", object_type);
        pr_info("object register ->Byte%d\n", object_register);
        pr_info("register value %d\n", register_value);
        ret = get_object_info(data, (u8)object_type, &size, &address);
        if (ret) {
                pr_err("fail to get object_info\n");
                return count;
        }

        size = 1;
        value = (u8)register_value;
        write_mem(data, address+(u16)object_register, size, &value);
        read_mem(data, address+(u16)object_register, (u8)size, &val);

        pr_info("T%d Byte%d is %d\n",
                object_type, object_register, val);
        return count;
}

static ssize_t mxt_object_show(struct device *dev,
                                        struct device_attribute *attr,
                                        const char *buf, size_t count)
{
        struct mxt_data *data = dev_get_drvdata(dev);
        unsigned int object_type;
        u8 val;
        int ret;
        u16 address;
        u16 size;
        u16 i;
        sscanf(buf, "%u", &object_type);
        pr_info("object type T%d\n", object_type);
        ret = get_object_info(data, (u8)object_type, &size, &address);
        if (ret) {
                pr_err("fail to get object_info\n");
                return count;
        }
        for (i = 0; i < size; i++) {
                read_mem(data, address+i, 1, &val);
                pr_info("Byte %u --> %u\n", i, val);
        }
        return count;
}

struct device *sec_touchscreen;
struct device *mxt_noise_test;
/*
        top_left, top_right, center, bottom_left, bottom_right
*/
unsigned int test_node[5] = {642, 98, 367, 668, 124};

uint16_t qt_refrence_node[768] = { 0 };
uint16_t qt_delta_node[768] = { 0 };

void diagnostic_chip(u8 mode)
{
        int error;
        u16 t6_address = 0;
        u16 size_one;
        int ret;
        u8 value;
        u16 t37_address = 0;

        ret = get_object_info(copy_data,
                GEN_COMMANDPROCESSOR_T6, &size_one, &t6_address);

        size_one = 1;
        error = write_mem(copy_data, t6_address+5, (u8)size_one, &mode);
        /* QT602240_COMMAND_DIAGNOSTIC, mode); */
        if (error < 0) {
                pr_err("error %s: write_object\n", __func__);
        } else {
                get_object_info(copy_data,
                        DEBUG_DIAGNOSTIC_T37, &size_one, &t37_address);
                size_one = 1;
                /* pr_info("diagnostic_chip setting success\n"); */
                read_mem(copy_data, t37_address, (u8)size_one, &value);
                /* pr_info("dianostic_chip mode is %d\n",value); */
        }
}

uint8_t read_uint16_t(struct mxt_data *data, uint16_t address, uint16_t *buf)
{
        uint8_t status;
        uint8_t temp[2];

        status = read_mem(data, address, 2, temp);
        *buf = ((uint16_t)temp[1]<<8) + (uint16_t)temp[0];

        return status;
}

void read_dbg_data(uint8_t dbg_mode , uint16_t node, uint16_t *dbg_data)
{
        u8 read_page, read_point;
        uint8_t mode, page;
        u16 size;
        u16 diagnostic_addr = 0;

        if (!mxt_enabled) {
                pr_err("read_dbg_data. mxt_enabled is 0\n");
                return;
        }

        get_object_info(copy_data,
                DEBUG_DIAGNOSTIC_T37, &size, &diagnostic_addr);

        read_page = node / 64;
        node %= 64;
        read_point = (node * 2) + 2;

        /* Page Num Clear */
        diagnostic_chip(MXT_CTE_MODE);
        msleep(20);

        do {
                if (read_mem(copy_data, diagnostic_addr, 1, &mode)) {
                        pr_info("READ_MEM_FAILED\n");
                        return;
                }
        } while (mode != MXT_CTE_MODE);

        diagnostic_chip(dbg_mode);
        msleep(20);

        do {
                if (read_mem(copy_data, diagnostic_addr, 1, &mode)) {
                        pr_info("READ_MEM_FAILED\n");
                        return;
                }
        } while (mode != dbg_mode);

        for (page = 1; page <= read_page; page++) {
                diagnostic_chip(MXT_PAGE_UP);
                msleep(20);
                do {
                        if (read_mem(copy_data,
                                diagnostic_addr + 1, 1, &mode)) {
                                pr_info("READ_MEM_FAILED\n");
                                return;
                        }
                } while (mode != page);
        }

        if (read_uint16_t(copy_data, diagnostic_addr + read_point, dbg_data)) {
                pr_info("READ_MEM_FAILED\n");
                return;
        }
}

#define MIN_VALUE 19744
#define MAX_VALUE 28884
#define MIN_VALUE_TA_ERROR_MODE 19125

#define T48_CALCFG_CHRGON               0x20

/* caution : should check the sensor level
this value is depend on tsp tunning state */
#define SENSOR_GAIN_TAERROR     5

int read_all_data(uint16_t dbg_mode)
{
        u8 read_page, read_point;
        u16 max_value = MIN_VALUE, min_value = MAX_VALUE;
        u16 ref_max_value = MAX_VALUE;
        u16 ref_min_value = MIN_VALUE;
        u16 object_address = 0;
        u8 data_buffer[2] = { 0 };
        u8 node = 0;
        int state = 0;
        int num = 0;
        int ret;
        u16 size;
        u8 val = 0;
        u8 sensor_gain = 0;
        bool ta_status = 0;

        if (!mxt_enabled) {
                pr_err("%s : mxt_enabled is 0\n", __func__);
                return 1;
        }

        if (copy_data->read_ta_status) {
                copy_data->read_ta_status(&ta_status);
                pr_info("ta_status is %d\n", ta_status);
        }

        /* check the CHRG_ON bit is set or not */
        /* when CHRG_ON is setted dual x is on so skip read last line*/
        get_object_info(copy_data,
                PROCG_NOISESUPPRESSION_T48, &size, &object_address);
        ret = read_mem(copy_data, object_address+2 , 1, &val);
        if (ret < 0)
                pr_err("TSP read fail : %s", __func__);

        pr_info("%s CALCFG[%#02x]\n", __func__, val);
        val = val & T48_CALCFG_CHRGON;

        /* read sensor gain to check reference value */
        if (val == T48_CALCFG_CHRGON) {

                ret = read_mem(copy_data, object_address+34 , 1, &sensor_gain);
                if (ret < 0)
                        pr_err("TSP read fail : %s", __func__);
                sensor_gain = (sensor_gain&0xF0)>>4;

                pr_info("%s BLEN[%d]\n", __func__, sensor_gain);
                if (sensor_gain == SENSOR_GAIN_TAERROR)
                        ref_min_value = MIN_VALUE_TA_ERROR_MODE;
        }

        /* Page Num Clear */
        diagnostic_chip(MXT_CTE_MODE);
        msleep(30);/* msleep(20);  */

        diagnostic_chip(dbg_mode);
        msleep(30);/* msleep(20);  */

        ret = get_object_info(copy_data,
                DEBUG_DIAGNOSTIC_T37, &size, &object_address);

        msleep(50); /* msleep(20);  */

        /* 768/64 */
        for (read_page = 0 ; read_page < 12; read_page++) {
                for (node = 0; node < 64; node++) {
                        read_point = (node * 2) + 2;
                        read_mem(copy_data,
                                object_address+(u16)read_point, 2, data_buffer);
                        qt_refrence_node[num] = ((uint16_t)data_buffer[1]<<8)
                                + (uint16_t)data_buffer[0];

                        /* last X line has 1/2 reference during
                                TA mode So do not check min/max value */
                        if ((val != T48_CALCFG_CHRGON)
                                || (val == T48_CALCFG_CHRGON && (num < 736))) {
                                if ((qt_refrence_node[num] < ref_min_value)
                                || (qt_refrence_node[num] > ref_max_value)) {
                                        if (sensor_gain == SENSOR_GAIN_TAERROR)
                                                state = 2;
                                        else
                                                state = 1;
                                        pr_err(
                                                "qt_refrence_node[%3d] = %5d\n"
                                                , num, qt_refrence_node[num]);
                                }

                                if (data_buffer[0] != 0) {
                                        if (qt_refrence_node[num] > max_value)
                                                max_value =
                                                        qt_refrence_node[num];
                                        if (qt_refrence_node[num] < min_value)
                                                min_value =
                                                        qt_refrence_node[num];
                                }
                        }

                        num++;
                        /* all node => 24 * 32 = 768 => (12page * 64) */
                        /* if ((read_page == 11) && (node == 64))
                                break; */
                        if (qt_refrence_node[num-1] == 0)
                                pr_err("qt_refrence_node"
                                        "[%d] = 0\n", num);
                        if (num == 768)
                                break;
                }
                diagnostic_chip(MXT_PAGE_UP);
                msleep(35);
                if (num == 768)
                        break;
        }

        if ((max_value - min_value) > 4500) {
                pr_err("diff = %d, max_value = %d, min_value = %d\n",
                        (max_value - min_value), max_value, min_value);
                state = 1;
        }

        return state;
}

int read_all_delta_data(uint16_t dbg_mode)
{
        u8 read_page, read_point;
        u16 object_address = 0;
        u8 data_buffer[2] = { 0 };
        u8 node = 0;
        int state = 0;
        int num = 0;
        int ret;
        u16 size;

        if (!mxt_enabled) {
                pr_err("%s : mxt_enabled is 0\n", __func__);
                return 1;
        }

        /* Page Num Clear */
        diagnostic_chip(MXT_CTE_MODE);
        msleep(30);/* msleep(20);  */

        diagnostic_chip(dbg_mode);
        msleep(30);/* msleep(20);  */

        ret = get_object_info(copy_data,
                DEBUG_DIAGNOSTIC_T37, &size, &object_address);
/*jerry no need to leave it */
#if 0
        for (i = 0; i < 5; i++) {
                if (data_buffer[0] == dbg_mode)
                        break;

                msleep(20);
        }
#else
        msleep(50); /* msleep(20);  */
#endif

        /* 768/64 */
        for (read_page = 0 ; read_page < 12; read_page++) {
                for (node = 0; node < 64; node++) {
                        read_point = (node * 2) + 2;
                        read_mem(copy_data,
                                object_address+(u16)read_point, 2, data_buffer);
                                qt_delta_node[num] =
                                        ((uint16_t)data_buffer[1]<<8)
                                        + (uint16_t)data_buffer[0];

                        num++;

                        /* all node => 24 * 32 = 768 => (12page * 64) */
                        /*if ((read_page == 11) && (node == 64))
                                break;*/
                }
                diagnostic_chip(MXT_PAGE_UP);
                msleep(35);
        }

        return state;
}

int find_channel(uint16_t dbg_mode)
{
        u8 read_page, read_point;
        u16 object_address = 0;
        u8 data_buffer[2] = { 0 };
        u8 node = 0;
        int state = 0;
        int num = 0;
        int ret;
        u16 size;
        u16 delta_val = 0;
        u16 max_val = 0;

        if (!mxt_enabled) {
                pr_err("%s : mxt_enabled is 0\n", __func__);
                return 1;
        }

        /* Page Num Clear */
        diagnostic_chip(MXT_CTE_MODE);
        msleep(30);/* msleep(20);  */

        diagnostic_chip(dbg_mode);
        msleep(30);/* msleep(20);  */

        ret = get_object_info(copy_data,
                DEBUG_DIAGNOSTIC_T37, &size, &object_address);
/*jerry no need to leave it */
#if 0
        for (i = 0; i < 5; i++) {
                if (data_buffer[0] == dbg_mode)
                        break;

                msleep(20);
        }
#else
        msleep(50); /* msleep(20);  */
#endif

        /* 768/64 */
        for (read_page = 0 ; read_page < 12; read_page++) {
                for (node = 0; node < 64; node++) {
                        read_point = (node * 2) + 2;
                        read_mem(copy_data,
                                object_address+(u16)read_point, 2, data_buffer);
                                delta_val = ((uint16_t)data_buffer[1]<<8)
                                        + (uint16_t)data_buffer[0];

                                if (delta_val > 32767)
                                        delta_val = 65535 - delta_val;
                                if (delta_val > max_val) {
                                        max_val = delta_val;
                                        state = (read_point - 2)/2 +
                                                (read_page * 64);
                                }

                        num++;

                        /* all node => 24 * 32 = 768 => (12page * 64) */
                        /*if ((read_page == 11) && (node == 64))
                                break;*/
                }
                diagnostic_chip(MXT_PAGE_UP);
                msleep(35);
        }

        return state;
}

static ssize_t find_channel_show(struct device *dev,
                                        struct device_attribute *attr,
                                        char *buf)
{
        int status = 0;

        status = find_channel(MXT_DELTA_MODE);

        return sprintf(buf, "%u\n", status);
}
#endif

static int mxt_check_bootloader(struct i2c_client *client,
                                        unsigned int state)
{
        u8 val;
        u8 temp;

recheck:
        if (i2c_master_recv(client, &val, 1) != 1)
                return -EIO;

        if (val & 0x20) {
                if (i2c_master_recv(client, &temp, 1) != 1)
                        return -EIO;

                if (i2c_master_recv(client, &temp, 1) != 1)
                        return -EIO;
                val &= ~0x20;
        }

        if ((val & 0xF0) == MXT_APP_CRC_FAIL) {
                pr_info("MXT_APP_CRC_FAIL\n");
                if (i2c_master_recv(client, &val, 1) != 1)
                        return -EIO;

                if (val & 0x20) {
                        if (i2c_master_recv(client, &temp, 1) != 1)
                                return -EIO;

                        if (i2c_master_recv(client, &temp, 1) != 1)
                                return -EIO;
                        val &= ~0x20;
                }
        }

        switch (state) {
        case MXT_WAITING_BOOTLOAD_CMD:
        case MXT_WAITING_FRAME_DATA:
                val &= ~MXT_BOOT_STATUS_MASK;
                break;
        case MXT_FRAME_CRC_PASS:
                if (val == MXT_FRAME_CRC_CHECK)
                        goto recheck;
                break;
        default:
                return -EINVAL;
        }

        if (val != state) {
                pr_err("Unvalid bootloader mode state\n");
                return -EINVAL;
        }

        return 0;
}

static int mxt_unlock_bootloader(struct i2c_client *client)
{
        u8 buf[2];

        buf[0] = MXT_UNLOCK_CMD_LSB;
        buf[1] = MXT_UNLOCK_CMD_MSB;

        if (i2c_master_send(client, buf, 2) != 2) {
                pr_err("%s: i2c send failed\n",
                        __func__);

                return -EIO;
        }

        return 0;
}

static int mxt_fw_write(struct i2c_client *client,
                                const u8 *data, unsigned int frame_size)
{
        if (i2c_master_send(client, data, frame_size) != frame_size) {
                pr_err("%s: i2c send failed\n", __func__);
                return -EIO;
        }

        return 0;
}

static int mxt_load_fw(struct device *dev, const char *fn)
{
        struct mxt_data *data = copy_data;
        struct i2c_client *client = copy_data->client;
        unsigned int frame_size;
        unsigned int pos = 0;
        int ret;
        u16 obj_address = 0;
        u16 size_one;
        u8 value;
        unsigned int object_register;
        int check_frame_crc_error = 0;
        int check_wating_frame_data_error = 0;

#if READ_FW_FROM_HEADER
        struct firmware *fw = NULL;

        pr_info("mxt_load_fw start from header!!!\n");
        fw = kzalloc(sizeof(struct firmware), GFP_KERNEL);

        fw->data = firmware_mXT;
        fw->size = sizeof(firmware_mXT);
#else
        const struct firmware *fw = NULL;

        pr_info("mxt_load_fw startl!!!\n");
        ret = request_firmware(&fw, fn, &client->dev);
        if (ret) {
                pr_err("Unable to open firmware %s\n", fn);
                return ret;
        }
#endif
        /* Change to the bootloader mode */
        object_register = 0;
        value = (u8)MXT_BOOT_VALUE;
        ret = get_object_info(data,
                GEN_COMMANDPROCESSOR_T6, &size_one, &obj_address);
        if (ret) {
                pr_err("fail to get object_info\n");
                release_firmware(fw);
                return ret;
        }
        size_one = 1;
        write_mem(data, obj_address+(u16)object_register, (u8)size_one, &value);
        msleep(MXT_SW_RESET_TIME);

        /* Change to slave address of bootloader */
        if (client->addr == MXT_APP_LOW)
                client->addr = MXT_BOOT_LOW;
        else
                client->addr = MXT_BOOT_HIGH;

        ret = mxt_check_bootloader(client, MXT_WAITING_BOOTLOAD_CMD);
        if (ret)
                goto out;

        /* Unlock bootloader */
        mxt_unlock_bootloader(client);

        while (pos < fw->size) {
                ret = mxt_check_bootloader(client,
                                                MXT_WAITING_FRAME_DATA);
                if (ret) {
                        check_wating_frame_data_error++;
                        if (check_wating_frame_data_error > 10) {
                                pr_err("firm update fail. wating_frame_data err\n");
                                goto out;
                        } else {
                                pr_err("check_wating_frame_data_error = %d, retry\n",
                                        check_wating_frame_data_error);
                                continue;
                        }
                }

                frame_size = ((*(fw->data + pos) << 8) | *(fw->data + pos + 1));

                /* We should add 2 at frame size as the the firmware data is not
                * included the CRC bytes.
                */
                frame_size += 2;

                /* Write one frame to device */
                mxt_fw_write(client, fw->data + pos, frame_size);

                ret = mxt_check_bootloader(client,
                                                MXT_FRAME_CRC_PASS);
                if (ret) {
                        check_frame_crc_error++;
                        if (check_frame_crc_error > 10) {
                                pr_err("firm update fail. frame_crc err\n");
                                goto out;
                        } else {
                                pr_err("check_frame_crc_error = %d, retry\n",
                                        check_frame_crc_error);
                                continue;
                        }
                }

                pos += frame_size;

                pr_info("Updated %d bytes / %zd bytes\n",
                        pos, fw->size);

                msleep(20);
        }

out:
#if READ_FW_FROM_HEADER
        kfree(fw);
#else
        release_firmware(fw);
#endif
        /* Change to slave address of application */
        if (client->addr == MXT_BOOT_LOW)
                client->addr = MXT_APP_LOW;
        else
                client->addr = MXT_APP_HIGH;
        return ret;
}

static int mxt_load_fw_bootmode(struct device *dev, const char *fn)
{
        struct i2c_client *client = copy_data->client;
        unsigned int frame_size;
        unsigned int pos = 0;
        int ret;

        int check_frame_crc_error = 0;
        int check_wating_frame_data_error = 0;

#if READ_FW_FROM_HEADER
        struct firmware *fw = NULL;
        pr_info("mxt_load_fw start from header!!!\n");
        fw = kzalloc(sizeof(struct firmware), GFP_KERNEL);

        fw->data = firmware_mXT;
        fw->size = sizeof(firmware_mXT);
#else
        const struct firmware *fw = NULL;
        pr_info("mxt_load_fw start!!!\n");

        ret = request_firmware(&fw, fn, &client->dev);
        if (ret) {
                pr_err("Unable to open firmware %s\n", fn);
                return ret;
        }
#endif
        /* Unlock bootloader */
        mxt_unlock_bootloader(client);

        while (pos < fw->size) {
                ret = mxt_check_bootloader(client,
                                                MXT_WAITING_FRAME_DATA);
                if (ret) {
                        check_wating_frame_data_error++;
                        if (check_wating_frame_data_error > 10) {
                                pr_err("firm update fail. wating_frame_data err\n");
                                goto out;
                        } else {
                                pr_err("check_wating_frame_data_error = %d, retry\n",
                                        check_wating_frame_data_error);
                                continue;
                        }
                }

                frame_size = ((*(fw->data + pos) << 8) | *(fw->data + pos + 1));

                /* We should add 2 at frame size as the the firmware data is not
                * included the CRC bytes.
                */
                frame_size += 2;

                /* Write one frame to device */
                mxt_fw_write(client, fw->data + pos, frame_size);

                ret = mxt_check_bootloader(client,
                                                MXT_FRAME_CRC_PASS);
                if (ret) {
                        check_frame_crc_error++;
                        if (check_frame_crc_error > 10) {
                                pr_err("firm update fail. frame_crc err\n");
                                goto out;
                        } else {
                                pr_err("check_frame_crc_error = %d, retry\n",
                                        check_frame_crc_error);
                                continue;
                        }
                }

                pos += frame_size;

                pr_info("Updated %d bytes / %zd bytes\n",
                        pos, fw->size);

                msleep(20);
        }

out:
#if READ_FW_FROM_HEADER
        kfree(fw);
#else
        release_firmware(fw);
#endif
        /* Change to slave address of application */
        if (client->addr == MXT_BOOT_LOW)
                client->addr = MXT_APP_LOW;
        else
                client->addr = MXT_APP_HIGH;
        return ret;
}

#if SYSFS
static ssize_t set_refer0_mode_show(struct device *dev,
        struct device_attribute *attr, char *buf)
{
        uint16_t mxt_reference = 0;
        read_dbg_data(MXT_REFERENCE_MODE, test_node[0], &mxt_reference);
        return sprintf(buf, "%u\n", mxt_reference);
}

static ssize_t set_refer1_mode_show(struct device *dev,
        struct device_attribute *attr, char *buf)
{
        uint16_t mxt_reference = 0;
        read_dbg_data(MXT_REFERENCE_MODE, test_node[1], &mxt_reference);
        return sprintf(buf, "%u\n", mxt_reference);
}

static ssize_t set_refer2_mode_show(struct device *dev,
        struct device_attribute *attr, char *buf)
{
        uint16_t mxt_reference = 0;
        read_dbg_data(MXT_REFERENCE_MODE, test_node[2], &mxt_reference);
        return sprintf(buf, "%u\n", mxt_reference);
}

static ssize_t set_refer3_mode_show(struct device *dev,
        struct device_attribute *attr, char *buf)
{
        uint16_t mxt_reference = 0;
        read_dbg_data(MXT_REFERENCE_MODE, test_node[3], &mxt_reference);
        return sprintf(buf, "%u\n", mxt_reference);
}

static ssize_t set_refer4_mode_show(struct device *dev,
        struct device_attribute *attr, char *buf)
{
        uint16_t mxt_reference = 0;
        read_dbg_data(MXT_REFERENCE_MODE, test_node[4], &mxt_reference);
        return sprintf(buf, "%u\n", mxt_reference);
}

static ssize_t set_delta0_mode_show(struct device *dev,
        struct device_attribute *attr, char *buf)
{
        uint16_t mxt_delta = 0;
        read_dbg_data(MXT_DELTA_MODE, test_node[0], &mxt_delta);
        if (mxt_delta < 32767)
                return sprintf(buf, "%u\n", mxt_delta);
        else
                mxt_delta = 65535 - mxt_delta;

        if (mxt_delta)
                return sprintf(buf, "-%u\n", mxt_delta);
        else
                return sprintf(buf, "%u\n", mxt_delta);
}

static ssize_t set_delta1_mode_show(struct device *dev,
        struct device_attribute *attr, char *buf)
{
        uint16_t mxt_delta = 0;
        read_dbg_data(MXT_DELTA_MODE, test_node[1], &mxt_delta);
        if (mxt_delta < 32767)
                return sprintf(buf, "%u\n", mxt_delta);
        else
                mxt_delta = 65535 - mxt_delta;

        if (mxt_delta)
                return sprintf(buf, "-%u\n", mxt_delta);
        else
                return sprintf(buf, "%u\n", mxt_delta);
}

static ssize_t set_delta2_mode_show(struct device *dev,
        struct device_attribute *attr, char *buf)
{
        uint16_t mxt_delta = 0;
        read_dbg_data(MXT_DELTA_MODE, test_node[2], &mxt_delta);
        if (mxt_delta < 32767)
                return sprintf(buf, "%u\n", mxt_delta);
        else
                mxt_delta = 65535 - mxt_delta;

        if (mxt_delta)
                return sprintf(buf, "-%u\n", mxt_delta);
        else
                return sprintf(buf, "%u\n", mxt_delta);
}

static ssize_t set_delta3_mode_show(struct device *dev,
        struct device_attribute *attr, char *buf)
{
        uint16_t mxt_delta = 0;
        read_dbg_data(MXT_DELTA_MODE, test_node[3], &mxt_delta);
        if (mxt_delta < 32767)
                return sprintf(buf, "%u\n", mxt_delta);
        else
                mxt_delta = 65535 - mxt_delta;

        if (mxt_delta)
                return sprintf(buf, "-%u\n", mxt_delta);
        else
                return sprintf(buf, "%u\n", mxt_delta);
}

static ssize_t set_delta4_mode_show(struct device *dev,
        struct device_attribute *attr, char *buf)
{
        uint16_t mxt_delta = 0;
        read_dbg_data(MXT_DELTA_MODE, test_node[4], &mxt_delta);
        if (mxt_delta < 32767)
                return sprintf(buf, "%u\n", mxt_delta);
        else
                mxt_delta = 65535 - mxt_delta;

        if (mxt_delta)
                return sprintf(buf, "-%u\n", mxt_delta);
        else
                return sprintf(buf, "%u\n", mxt_delta);
}

static ssize_t set_threshold_mode_show(struct device *dev,
        struct device_attribute *attr, char *buf)
{
        return sprintf(buf, "%u\n", threshold);
}

static ssize_t set_all_refer_mode_show(struct device *dev,
        struct device_attribute *attr, char *buf)
{
        int status = 0;

        status = read_all_data(MXT_REFERENCE_MODE);

        return sprintf(buf, "%u\n", status);
}

static int index_reference;

static int atoi(const char *str)
{
        int result = 0;
        int count = 0;
        if (str == NULL)
                return -1;
        while (str[count] && str[count] >= '0' && str[count] <= '9') {
                result = result * 10 + str[count] - '0';
                ++count;
        }
        return result;
}

ssize_t disp_all_refdata_show(struct device *dev,
        struct device_attribute *attr, char *buf)
{
        return sprintf(buf, "%u\n",  qt_refrence_node[index_reference]);
}

ssize_t disp_all_refdata_store(struct device *dev,
        struct device_attribute *attr, const char *buf, size_t size)
{
        index_reference = atoi(buf);
        return size;
}

static ssize_t set_all_delta_mode_show(struct device *dev,
        struct device_attribute *attr, char *buf)
{
        int status = 0;

        status = read_all_delta_data(MXT_DELTA_MODE);

        return sprintf(buf, "%u\n", status);
}

static int index_delta;

ssize_t disp_all_deltadata_show(struct device *dev,
        struct device_attribute *attr, char *buf)
{
        if (qt_delta_node[index_delta] < 32767)
                return sprintf(buf, "%u\n", qt_delta_node[index_delta]);
        else
                qt_delta_node[index_delta] = 65535 - qt_delta_node[index_delta];

        return sprintf(buf, "-%u\n", qt_delta_node[index_delta]);
}

ssize_t disp_all_deltadata_store(struct device *dev,
        struct device_attribute *attr, const char *buf, size_t size)
{
        index_delta = atoi(buf);
        return size;
}

static ssize_t set_firm_version_show(struct device *dev,
        struct device_attribute *attr, char *buf)
{
        struct mxt_data *data = copy_data;

        u8 id[ID_BLOCK_SIZE];
        u8 value;
        int ret;
        u8 i;

        if (mxt_enabled == 1) {
                disable_irq(IRQ_EINT(4));
                for (i = 0; i < 4; i++) {
                        ret = read_mem(copy_data, 0, sizeof(id), id);
                        if (!ret)
                                break;
                }
                enable_irq(IRQ_EINT(4));
                if (ret < 0) {
                        pr_err("TSP read fail : %s", __func__);
                        value = 0;
                        return sprintf(buf, "%d\n", value);
                } else {
                        pr_info("%s : %#02x\n",
                                __func__, id[2]);
                        return sprintf(buf, "%#02x\n", id[2]);
                }
        } else {
                pr_err("TSP power off : %s", __func__);
                value = 0;
                return sprintf(buf, "%d\n", value);
        }
}

static ssize_t set_module_off_store(struct device *dev,
        struct device_attribute *attr, const char *buf, size_t size)
{
        struct mxt_data *data = copy_data;
        pr_info("%s!!\n", __func__);

        if (*buf != 'S' && *buf != 'F') {
                pr_err("Invalid values\n");
                return -EINVAL;
        }
        if (mxt_enabled == 1) {
                mxt_enabled = 0;
                touch_is_pressed = 0;
#if TOUCH_BOOSTER
                tsp_press_status = 0;
#endif
                disable_irq(IRQ_EINT(4));
                mxt_internal_suspend(data);
        }
        return size;
}

static ssize_t set_module_on_store(struct device *dev,
        struct device_attribute *attr, const char *buf, size_t size)
{
        struct mxt_data *data = copy_data;
        bool ta_status = 0;

        pr_info("%s!!\n", __func__);

        if (*buf != 'S' && *buf != 'F') {
                pr_err("Invalid values\n");
                return -EINVAL;
        }

        if (mxt_enabled == 0) {
                mxt_internal_resume(data);
                enable_irq(IRQ_EINT(4));

                mxt_enabled = 1;

                if (data->read_ta_status) {
                        data->read_ta_status(&ta_status);
                        pr_info("ta_status is %d\n", ta_status);
                        mxt_ta_probe(ta_status);
                }
                calibrate_chip_e();
        }

        return size;
}

#if FOR_DEBUGGING_TEST_DOWNLOADFW_BIN
#include <linux/uaccess.h>

#define MXT_FW_BIN_NAME "/sdcard/mxt.bin"

static int mxt_download(const u8 *pBianry, const u32 unLength)
{
        struct mxt_data *data = copy_data;
        struct i2c_client *client = copy_data->client;
        unsigned int frame_size;
        unsigned int pos = 0;
        int ret;
        u16 obj_address = 0;
        u16 size_one;
        u8 value;
        unsigned int object_register;
        int check_frame_crc_error = 0;
        int check_wating_frame_data_error = 0;

        pr_info("mxt_download start!!!\n");

        /* Change to the bootloader mode */
        object_register = 0;
        value = (u8)MXT_BOOT_VALUE;
        ret = get_object_info(data,
                GEN_COMMANDPROCESSOR_T6, &size_one, &obj_address);
        if (ret) {
                pr_err("fail to get object_info\n");
                return ret;
        }
        size_one = 1;

        write_mem(data, obj_address+(u16)object_register, (u8)size_one, &value);
        msleep(MXT_SW_RESET_TIME);

        /* Change to slave address of bootloader */
        if (client->addr == MXT_APP_LOW)
                client->addr = MXT_BOOT_LOW;
        else
                client->addr = MXT_BOOT_HIGH;
        ret = mxt_check_bootloader(client, MXT_WAITING_BOOTLOAD_CMD);

        if (ret)
                goto out;

        /* Unlock bootloader */
        mxt_unlock_bootloader(client);

        while (pos < unLength) {
                ret = mxt_check_bootloader(client,
                                                MXT_WAITING_FRAME_DATA);
                if (ret) {
                        check_wating_frame_data_error++;
                        if (check_wating_frame_data_error > 10) {
                                pr_err("firm update fail. wating_frame_data err\n");
                                goto out;
                        } else {
                                pr_info("check_wating_frame_data_error=%d, retry\n",
                                        check_wating_frame_data_error);
                                continue;
                        }
                }

                frame_size = ((*(pBianry + pos) << 8) | *(pBianry + pos + 1));

                /* We should add 2 at frame size as the the firmware data is not
                * included the CRC bytes.
                */
                frame_size += 2;

                /* Write one frame to device */
                mxt_fw_write(client, pBianry + pos, frame_size);

                ret = mxt_check_bootloader(client,
                                                MXT_FRAME_CRC_PASS);
                if (ret) {
                        check_frame_crc_error++;
                        if (check_frame_crc_error > 10) {
                                pr_err("firm update fail. frame_crc err\n");
                                goto out;
                        } else {
                                pr_info("check_frame_crc_error = %d, retry\n",
                                        check_frame_crc_error);
                                continue;
                        }
                }

                pos += frame_size;

                pr_info("Updated %d bytes / %zd bytes\n", pos, unLength);

                msleep(20);
        }

out:
        /* Change to slave address of application */
        if (client->addr == MXT_BOOT_LOW)
                client->addr = MXT_APP_LOW;
        else
                client->addr = MXT_APP_HIGH;
        return ret;
}

int mxt_binfile_download(void)
{
        int nRet = 0;
        int retry_cnt = 0;
        long fw1_size = 0;
        unsigned char *fw_data1;
        struct file *filp;
        loff_t  pos;
        int     ret = 0;
        mm_segment_t oldfs;
        spinlock_t      lock;

        oldfs = get_fs();
        set_fs(get_ds());

        filp = filp_open(MXT_FW_BIN_NAME, O_RDONLY, 0);
        if (IS_ERR(filp)) {
                pr_err("file open error:%d\n", (s32)filp);
                return -1;
        }

        fw1_size = filp->f_path.dentry->d_inode->i_size;
        pr_info("Size of the file : %ld(bytes)\n", fw1_size);

        fw_data1 = kmalloc(fw1_size, GFP_KERNEL);
        memset(fw_data1, 0, fw1_size);

        pos = 0;
        memset(fw_data1, 0, fw1_size);
        ret = vfs_read(filp, (char __user *)fw_data1, fw1_size, &pos);

        if (ret != fw1_size) {
                pr_err("Failed to read file %s (ret = %d)\n",
                        MXT_FW_BIN_NAME, ret);
                kfree(fw_data1);
                filp_close(filp, current->files);
                return -1;
        }

        filp_close(filp, current->files);

        set_fs(oldfs);

        for (retry_cnt = 0; retry_cnt < 3; retry_cnt++) {
                pr_info("ADB - MASTER CHIP Firmware update! try : %d",
                        retry_cnt+1);
                nRet = mxt_download((const u8 *)fw_data1, (const u32)fw1_size);
                if (nRet)
                        continue;
                break;
        }

        kfree(fw_data1);
        return nRet;
}
#endif

static ssize_t set_mxt_firm_update_store(struct device *dev,
        struct device_attribute *attr, const char *buf, size_t size)
{
        struct mxt_data *data = dev_get_drvdata(dev);
        u8 **tsp_config = (u8 **)data->pdata->config;
        int i = 0;
        int error = 0;

        pr_info("set_mxt_update_show start!!\n");
        if (*buf != 'S' && *buf != 'F') {
                pr_err("Invalid values\n");
                return -EINVAL;
        }

        disable_irq(IRQ_EINT(4));
        firm_status_data = 1;
#if FOR_DEBUGGING_TEST_DOWNLOADFW_BIN
        error = mxt_binfile_download();
#else
        if (*buf != 'F' && data->tsp_version >= firmware_latest[0]
                && data->tsp_build >= firmware_latest[1]) {
                pr_err("latest firmware\n");
                firm_status_data = 2;
                enable_irq(IRQ_EINT(4));
                return size;
        }
        pr_info("fm_update\n");
        error = mxt_load_fw(dev, MXT_FW_NAME);
#endif

        if (error) {
                firm_status_data = 3;
                pr_err("The firmware update failed(%d)\n", error);
                return error;
        } else {
                firm_status_data = 2;
                pr_info("The firmware update succeeded\n");

                /* Wait for reset */
                msleep(MXT_SW_RESET_TIME);

                mxt_init_touch_driver(data);
                /* mxt224_initialize(data); */
        }

        for (i = 0; tsp_config[i][0] != RESERVED_T255; i++) {
                error = init_write_config(data, tsp_config[i][0],
                                                        tsp_config[i] + 1);
                if (error) {
                        pr_err("init_write_config error\n");
                        firm_status_data = 3;
                        return error;
                }
        }

        error = mxt_backup(data);
        if (error) {
                pr_err("mxt_backup fail!!!\n");
                return error;
        }

        /* reset the touch IC. */
        error = mxt_reset(data);
        if (error) {
                pr_err("mxt_reset fail!!!\n");
                return error;
        }

        msleep(MXT_SW_RESET_TIME);
        enable_irq(IRQ_EINT(4));

        return size;
}

static ssize_t set_mxt_firm_status_show(struct device *dev,
        struct device_attribute *attr, char *buf)
{

        int count;
        pr_info("Enter firmware_status_show by Factory command\n");

        if (firm_status_data == 1)
                count = sprintf(buf, "DOWNLOADING\n");
        else if (firm_status_data == 2)
                count = sprintf(buf, "PASS\n");
        else if (firm_status_data == 3)
                count = sprintf(buf, "FAIL\n");
        else
                count = sprintf(buf, "PASS\n");

        return count;
}

static ssize_t key_threshold_show(struct device *dev,
        struct device_attribute *attr, char *buf)
{
        return sprintf(buf, "%u\n", threshold);
}

static ssize_t key_threshold_store(struct device *dev,
        struct device_attribute *attr, const char *buf, size_t size)
{
        /*TO DO IT*/
        unsigned int object_register = 7;
        u8 value;
        u8 val;
        int ret;
        u16 address = 0;
        u16 size_one;
        int num;
        if (sscanf(buf, "%d", &num) == 1) {
                threshold = num;
                pr_info("threshold value %d\n", threshold);
                ret = get_object_info(copy_data,
                        TOUCH_MULTITOUCHSCREEN_T9, &size_one, &address);
                size_one = 1;
                value = (u8)threshold;
                write_mem(copy_data,
                        address+(u16)object_register, size_one, &value);
                read_mem(copy_data,
                        address+(u16)object_register, (u8)size_one, &val);
                pr_err("T9 Byte%d is %d\n", object_register, val);
        }
        return size;
}

static ssize_t set_mxt_firm_version_show(struct device *dev,
        struct device_attribute *attr, char *buf)
{
        pr_info("phone's version : %#02x,%#02x\n"
                , firmware_latest[0], firmware_latest[1]);
        return sprintf(buf, "%#02x\n", firmware_latest[0]);
}

static ssize_t set_mxt_firm_version_read_show(struct device *dev,
        struct device_attribute *attr, char *buf)
{
        struct mxt_data *data = dev_get_drvdata(dev);
        pr_info("phone's version : %#02x,%#02x\n"
                , data->tsp_version, data->tsp_build);
        return sprintf(buf, "%#02x\n", data->tsp_version);
}

static ssize_t mxt_touchtype_show(struct device *dev,
        struct device_attribute *attr, char *buf)
{
        char temp[15];

        sprintf(temp, "ATMEL,MXT1664S\n");
        strcat(buf, temp);

        return strlen(buf);
}

static ssize_t x_line_show(struct device *dev,
        struct device_attribute *attr, char *buf)
{
        u8 data = 24;
        return sprintf(buf, "%d\n", data);
}

static ssize_t y_line_show(struct device *dev,
        struct device_attribute *attr, char *buf)
{
        u8 data = 32;
        return sprintf(buf, "%d\n", data);
}

#if ITDEV
/* Functions for mem_access interface */
struct bin_attribute mem_access_attr;

static int mxt_read_block(struct i2c_client *client,
                   u16 addr,
                   u16 length,
                   u8 *value)
{
        struct i2c_adapter *adapter = client->adapter;
        struct i2c_msg msg[2];
        __le16  le_addr;
        struct mxt_data *mxt;

        mxt = i2c_get_clientdata(client);

        if (mxt != NULL) {
                if ((mxt->last_read_addr == addr) &&
                        (addr == mxt->msg_proc_addr)) {
                        if  (i2c_master_recv(client, value, length) == length) {
#if ITDEV
                                if (debug_enabled)
                                        print_hex_dump(KERN_INFO, "MXT RX:",
                                                DUMP_PREFIX_NONE, 16, 1,
                                                value, length, false);
#endif
                                return 0;
                        } else
                                return -EIO;
                } else {
                        mxt->last_read_addr = addr;
                }
        }

        le_addr = cpu_to_le16(addr);
        msg[0].addr  = client->addr;
        msg[0].flags = 0x00;
        msg[0].len   = 2;
        msg[0].buf   = (u8 *) &le_addr;

        msg[1].addr  = client->addr;
        msg[1].flags = I2C_M_RD;
        msg[1].len   = length;
        msg[1].buf   = (u8 *) value;
        if (i2c_transfer(adapter, msg, 2) == 2) {
#if ITDEV
                if (debug_enabled) {
                        print_hex_dump(KERN_INFO, "MXT TX:", DUMP_PREFIX_NONE,
                                16, 1, msg[0].buf, msg[0].len, false);
                        print_hex_dump(KERN_INFO, "MXT RX:", DUMP_PREFIX_NONE,
                                16, 1, msg[1].buf, msg[1].len, false);
                }
#endif
                return 0;
        } else
                return -EIO;
}

/* Writes a block of bytes (max 256) to given address in mXT chip. */



static ssize_t mem_access_read(struct file *filp, struct kobject *kobj,
        struct bin_attribute *bin_attr, char *buf, loff_t off, size_t count)
{
        int ret = 0;
        struct i2c_client *client;

        pr_info("mem_access_read p=%p off=%lli c=%zi\n", buf, off, count);

        if (off >= 32768)
                return -EIO;

        if (off + count > 32768)
                count = 32768 - off;

        if (count > 256)
                count = 256;

        if (count > 0)  {
                client = to_i2c_client(container_of(kobj, struct device, kobj));
                ret = mxt_read_block(client, off, count, buf);
        }

        return ret >= 0 ? count : ret;
}

static ssize_t mem_access_write(struct file *filp, struct kobject *kobj,
        struct bin_attribute *bin_attr, char *buf, loff_t off, size_t count)
{
        int ret = 0;
        struct i2c_client *client;

        pr_info("mem_access_write p=%p off=%lli c=%zi\n", buf, off, count);

        if (off >= 32768)
                return -EIO;

        if (off + count > 32768)
                count = 32768 - off;

        if (count > 256)
                count = 256;

        if (count > 0) {
                client = to_i2c_client(container_of(kobj, struct device, kobj));
                ret = mxt_write_block(client, off, count, buf);
        }

        return ret >= 0 ? count : 0;
}

static ssize_t pause_show(struct device *dev,
        struct device_attribute *attr, char *buf)
{
        int count = 0;

        count += sprintf(buf + count, "%d", driver_paused);
        count += sprintf(buf + count, "\n");

        return count;
}

static ssize_t pause_store(struct device *dev,
        struct device_attribute *attr, const char *buf, size_t count)
{
        int i;
        if (sscanf(buf, "%u", &i) == 1 && i < 2) {
                driver_paused = i;

                pr_info("%s\n", i ? "paused" : "unpaused");
        } else {
                pr_info("pause_driver write error\n");
        }

        return count;
}

static ssize_t debug_enable_show(struct device *dev,
        struct device_attribute *attr, char *buf)
{
        int count = 0;

        count += sprintf(buf + count, "%d", debug_enabled);
        count += sprintf(buf + count, "\n");

        return count;
}

static ssize_t debug_enable_store(struct device *dev,
        struct device_attribute *attr, const char *buf, size_t count)
{
        int i;
        if (sscanf(buf, "%u", &i) == 1 && i < 2) {
                debug_enabled = i;

                pr_info("%s\n",
                        i ? "debug enabled" : "debug disabled");
        } else {
                pr_info("debug_enabled write error\n");
        }

        return count;
}

static ssize_t command_calibrate_store(struct device *dev,
        struct device_attribute *attr, const char *buf, size_t count)
{
        int ret;

        ret = calibrate_chip_e();

        return (ret < 0) ? ret : count;
}

static ssize_t command_reset_store(struct device *dev,
        struct device_attribute *attr, const char *buf, size_t count)
{
        struct i2c_client *client;
        struct mxt_data   *mxt;
        int ret;

        client = to_i2c_client(dev);
        mxt = i2c_get_clientdata(client);

        ret = mxt_reset(mxt);

        return (ret < 0) ? ret : count;
}

static ssize_t command_backup_store(struct device *dev,
        struct device_attribute *attr, const char *buf, size_t count)
{
        struct i2c_client *client;
        struct mxt_data   *mxt;
        int ret;

        client = to_i2c_client(dev);
        mxt = i2c_get_clientdata(client);

#if 0
        ret = backup_to_nv(mxt);
#else
        ret = mxt_backup(mxt);
#endif

        return (ret < 0) ? ret : count;
}
#endif

static DEVICE_ATTR(set_refer0, S_IRUGO,
        set_refer0_mode_show, NULL);
static DEVICE_ATTR(set_delta0, S_IRUGO,
        set_delta0_mode_show, NULL);
static DEVICE_ATTR(set_refer1, S_IRUGO,
        set_refer1_mode_show, NULL);
static DEVICE_ATTR(set_delta1, S_IRUGO,
        set_delta1_mode_show, NULL);
static DEVICE_ATTR(set_refer2, S_IRUGO,
        set_refer2_mode_show, NULL);
static DEVICE_ATTR(set_delta2, S_IRUGO,
        set_delta2_mode_show, NULL);
static DEVICE_ATTR(set_refer3, S_IRUGO,
        set_refer3_mode_show, NULL);
static DEVICE_ATTR(set_delta3, S_IRUGO,
        set_delta3_mode_show, NULL);
static DEVICE_ATTR(set_refer4, S_IRUGO,
        set_refer4_mode_show, NULL);
static DEVICE_ATTR(set_delta4, S_IRUGO,
        set_delta4_mode_show, NULL);
static DEVICE_ATTR(set_all_refer, S_IRUGO,
        set_all_refer_mode_show, NULL);
static DEVICE_ATTR(disp_all_refdata, S_IRUGO | S_IWUSR | S_IWGRP,
        disp_all_refdata_show, disp_all_refdata_store);
static DEVICE_ATTR(set_all_delta, S_IRUGO,
        set_all_delta_mode_show, NULL);
static DEVICE_ATTR(disp_all_deltadata, S_IRUGO | S_IWUSR | S_IWGRP,
        disp_all_deltadata_show, disp_all_deltadata_store);
static DEVICE_ATTR(set_firm_version, S_IRUGO | S_IWUSR | S_IWGRP,
        set_firm_version_show, NULL);
static DEVICE_ATTR(set_module_off, S_IRUGO | S_IWUSR | S_IWGRP,
        NULL, set_module_off_store);
static DEVICE_ATTR(set_module_on, S_IRUGO | S_IWUSR | S_IWGRP,
        NULL, set_module_on_store);
static DEVICE_ATTR(mxt_touchtype, S_IRUGO | S_IWUSR | S_IWGRP,
        mxt_touchtype_show, NULL);
static DEVICE_ATTR(set_threshold, S_IRUGO,
        set_threshold_mode_show, NULL);
/* firmware update */
static DEVICE_ATTR(tsp_firm_update, S_IWUSR | S_IWGRP,
        NULL, set_mxt_firm_update_store);
/* firmware update status return */
static DEVICE_ATTR(tsp_firm_update_status, S_IRUGO,
        set_mxt_firm_status_show, NULL);
/* touch threshold return, store */
static DEVICE_ATTR(tsp_threshold, S_IRUGO | S_IWUSR | S_IWGRP,
        key_threshold_show, key_threshold_store);
/* PHONE*/      /* firmware version resturn in phone driver version */
static DEVICE_ATTR(tsp_firm_version_phone, S_IRUGO,
        set_mxt_firm_version_show, NULL);
/*PART*/        /* firmware version resturn in TSP panel version */
static DEVICE_ATTR(tsp_firm_version_panel, S_IRUGO,
        set_mxt_firm_version_read_show, NULL);
static DEVICE_ATTR(object_show, S_IWUSR | S_IWGRP, NULL,
        mxt_object_show);
static DEVICE_ATTR(object_write, S_IWUSR | S_IWGRP, NULL,
        mxt_object_setting);
static DEVICE_ATTR(dbg_switch, S_IWUSR | S_IWGRP, NULL,
        mxt_debug_setting);
static DEVICE_ATTR(find_delta_channel, S_IRUGO,
        find_channel_show, NULL);
static DEVICE_ATTR(x_line, S_IRUGO,
        x_line_show, NULL);
static DEVICE_ATTR(y_line, S_IRUGO,
        y_line_show, NULL);
#if ITDEV
/* Sysfs files for libmaxtouch interface */
static DEVICE_ATTR(pause_driver, 0666,
        pause_show, pause_store);
static DEVICE_ATTR(debug_enable, 0666,
        debug_enable_show, debug_enable_store);
static DEVICE_ATTR(command_calibrate, 0666,
        NULL, command_calibrate_store);
static DEVICE_ATTR(command_reset, 0666,
        NULL, command_reset_store);
static DEVICE_ATTR(command_backup, 0666,
        NULL, command_backup_store);

static struct attribute *libmaxtouch_attributes[] = {
        &dev_attr_pause_driver.attr,
        &dev_attr_debug_enable.attr,
        &dev_attr_command_calibrate.attr,
        &dev_attr_command_reset.attr,
        &dev_attr_command_backup.attr,
        NULL,
};

static struct attribute_group libmaxtouch_attr_group = {
        .attrs = libmaxtouch_attributes,
};
#endif

static struct attribute *mxt_attrs[] = {
        &dev_attr_object_show.attr,
        &dev_attr_object_write.attr,
        &dev_attr_dbg_switch.attr,
        NULL
};

static const struct attribute_group mxt_attr_group = {
        .attrs = mxt_attrs,
};

#endif
extern int mxt_config_settings(struct mxt_data *mxt);
static int mxt_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
        struct mxt_platform_data *pdata = client->dev.platform_data;
        struct mxt_data *data;
        struct input_dev *input_dev;
        int ret;
        int i;
#if !(FOR_BRINGUP)
        bool ta_status = 0;
#endif
        u8 **tsp_config;

        touch_is_pressed = 0;
#if TOUCH_BOOSTER
        tsp_press_status = 0;
#endif
        if (!pdata) {
                printk("missing platform data\n");
                return -ENODEV;
        }

        if (pdata->max_finger_touches <= 0)
                return -EINVAL;

        data = kzalloc(sizeof(*data) + pdata->max_finger_touches *
                                        sizeof(*data->fingers), GFP_KERNEL);
        if (!data)
        {
#if DEBUG_INFO        
                printk("[ATEML]___mxt_probe kzalloc error\n");
#endif
                return -ENOMEM;
        }
        data->pdata = pdata;
        data->num_fingers = pdata->max_finger_touches;
        data->power_on = pdata->power_on;
        data->power_off = pdata->power_off;
#if !(FOR_BRINGUP)
        /*data->register_cb = pdata->register_cb;*/
#endif
        data->read_ta_status = pdata->read_ta_status;

        data->client = client;
        i2c_set_clientdata(client, data);

        input_dev = input_allocate_device();
        if (!input_dev) {
                ret = -ENOMEM;
#if DEBUG_INFO        							
                printk("[ATMEL]____mxt_prob_input device allocation failed\n");
#endif
                goto err_alloc_dev;
        }

				
		data->input_dev = input_dev;
		input_dev->name = "atmel_1664";
		input_dev->id.bustype = BUS_I2C;
		input_dev->dev.parent = &client->dev;
		
        set_bit(EV_SYN, input_dev->evbit);
        set_bit(EV_ABS, input_dev->evbit);
        set_bit(EV_KEY, input_dev->evbit);
        set_bit(MT_TOOL_FINGER, input_dev->keybit);
        set_bit(INPUT_PROP_DIRECT, input_dev->propbit);

		pdata->min_x=pdata->min_y=0;
		pdata->max_x=1279;		
		pdata->max_y=799;
        input_mt_init_slots(input_dev, MAX_USING_FINGER_NUM);

        input_set_abs_params(input_dev, ABS_MT_POSITION_X, pdata->min_x,
                             pdata->max_x, 0, 0);
        input_set_abs_params(input_dev, ABS_MT_POSITION_Y, pdata->min_y,
                             pdata->max_y, 0, 0);
        input_set_abs_params(input_dev, ABS_MT_TOUCH_MAJOR, 0,
                            255, 0, 0);
        input_set_abs_params(input_dev, ABS_MT_PRESSURE, 0, 255, 0, 0);

        input_set_drvdata(input_dev, data);
				
#ifdef _SUPPORT_SHAPE_TOUCH_
        input_set_abs_params(input_dev, ABS_MT_COMPONENT, 0, 255, 0, 0);
#endif
        ret = input_register_device(input_dev);
        if (ret) {
                input_free_device(input_dev);
                goto err_reg_dev;
        }

        data->gpio_read_done = pdata->gpio_read_done;

        data->power_on();

        copy_data = data;

        if (client->addr == MXT_APP_LOW)
                client->addr = MXT_BOOT_LOW;
        else
                client->addr = MXT_BOOT_HIGH;
		printk("[ATMEL] client->addr =%x \n", client->addr ); 
        ret = mxt_check_bootloader(client, MXT_WAITING_BOOTLOAD_CMD);
        if (ret >= 0) {
                pr_info("boot mode. firm update excute\n");
                mxt_load_fw_bootmode(NULL, MXT_FW_NAME);
                msleep(MXT_SW_RESET_TIME);
        } else {
                if (client->addr == MXT_BOOT_LOW)
                        client->addr = MXT_APP_LOW;
                else
                        client->addr = MXT_APP_HIGH;
        }
        ret = mxt_init_touch_driver(data);

#if !(FOR_BRINGUP)
        data->register_cb(mxt_ta_probe);
#endif

        if (ret) {
#if DEBUG_INFO        							
                printk("[ATMEL]____ mxt_prob chip initialization failed\n");
#endif
                goto err_init_drv;
        }
				
#if 1
        /* tsp_family_id - 0xA2 : MXT-1446-S series */
        if (data->family_id == 0xA2) {
                tsp_config = (u8 **)data->pdata->config;

#if !(FOR_BRINGUP)
                data->t48_config_batt = pdata->t48_config_batt;
                data->t48_config_chrg = pdata->t48_config_chrg;
                data->tchthr_batt = pdata->tchthr_batt;
                data->tchthr_charging = pdata->tchthr_charging;
                data->calcfg_batt = pdata->calcfg_batt;
                data->calcfg_charging = pdata->calcfg_charging;
#endif
#if UPDATE_ON_PROBE
#if !(FOR_DEBUGGING_TEST_DOWNLOADFW_BIN)
                if (data->tsp_version < firmware_latest[0]
                        || (data->tsp_version == firmware_latest[0]
                                && data->tsp_build != firmware_latest[1])) {
                        pr_info("force firmware update\n");
                        if (mxt_load_fw(NULL, MXT_FW_NAME))
                                goto err_config;
                        else {
                                msleep(MXT_SW_RESET_TIME);
                                mxt_init_touch_driver(data);
                        }
                }
#endif
#endif
#if TOUCH_BOOSTER
                INIT_DELAYED_WORK(&data->dvfs_dwork,
                        mxt_set_dvfs_off);
#endif
        } else {
                printk("ERROR : There is no valid TSP ID\n");
                goto err_config;
        }
#if 0
        for (i = 0; tsp_config[i][0] != RESERVED_T255; i++) {
                ret = init_write_config(data, tsp_config[i][0],
                                                        tsp_config[i] + 1);
                if (ret)
                        goto err_config;

                if (tsp_config[i][0] == TOUCH_MULTITOUCHSCREEN_T9) {
                        /* Are x and y inverted? */
                        if (tsp_config[i][10] & 0x1) {
                                data->x_dropbits =
                                        (!(tsp_config[i][22] & 0xC)) << 1;
                                data->y_dropbits =
                                        (!(tsp_config[i][20] & 0xC)) << 1;
                        } else {
                                data->x_dropbits =
                                        (!(tsp_config[i][20] & 0xC)) << 1;
                                data->y_dropbits =
                                        (!(tsp_config[i][22] & 0xC)) << 1;
                        }
                }
        }
        ret = mxt_backup(data);
        if (ret)
                goto err_backup;
#else

#if 0
        for (i = 0; atmel_tsp_config[i] != RESERVED_T255; i++) {
                ret = mxt_defconfig_settings(data, atmel_tsp_config[i]);

			if(ret<0)
				printk("[ATMEL] mxt_defconfig_settings error  +++++ \n");
							
        }
#endif
#if 0
        data->x_dropbits = (255 &0xc) << 1;
               //!(tsp_config[i][20] & 0xC)) << 1;
        data->y_dropbits =(0 &0xc) << 1;
               //!(tsp_config[i][22] & 0xC)) << 1;
#else
        data->x_dropbits = (0 &0xc) << 1;
               //!(tsp_config[i][20] & 0xC)) << 1;
        data->y_dropbits = 2; //(8 &0xc) << 1;
               //!(tsp_config[i][22] & 0xC)) << 1;
#endif               
		//mxt_config_settings(data);
        //ret = mxt_backup(data);
        if (ret)
                goto err_backup;
#endif
		msleep(100);
        /* reset the touch IC. */
        ret = mxt_reset(data);
        if (ret)
                goto err_reset;

        msleep(MXT_SW_RESET_TIME);

        calibrate_chip_e();

#endif
        for (i = 0; i < data->num_fingers; i++)
                data->fingers[i].state = MXT_STATE_INACTIVE;

        ret = request_threaded_irq(IRQ_EINT(4), NULL, mxt_irq_thread,
                IRQF_TRIGGER_LOW | IRQF_ONESHOT, "mxt_ts", data);

        if (ret < 0)
                goto err_irq;

#if SYSFS
        ret = sysfs_create_group(&client->dev.kobj, &mxt_attr_group);
        if (ret)
                pr_err("sysfs_create_group()is falled\n");


#if ITDEV
        ret = sysfs_create_group(&client->dev.kobj, &libmaxtouch_attr_group);
        if (ret) {
                pr_err("Failed to create libmaxtouch sysfs group\n");
                goto err_irq;
        }

        sysfs_bin_attr_init(&mem_access_attr);
        mem_access_attr.attr.name = "mem_access";
        mem_access_attr.attr.mode = S_IRUGO | S_IWUGO;
        mem_access_attr.read = mem_access_read;
        mem_access_attr.write = mem_access_write;
        mem_access_attr.size = 65535;

        if (sysfs_create_bin_file(&client->dev.kobj, &mem_access_attr) < 0) {
                pr_err("Failed to create device file(%s)!\n",
                        mem_access_attr.attr.name);
                goto err_irq;
        }
#endif

        sec_touchscreen = device_create(sec_class,
                NULL, 0, NULL, "atmel_1664");
        dev_set_drvdata(sec_touchscreen, data);
        if (IS_ERR(sec_touchscreen))
                pr_err("Failed to create device(sec_touchscreen)!\n");

        if (device_create_file(sec_touchscreen,
                                &dev_attr_tsp_firm_update) < 0)
                pr_err("Failed to create device file(%s)!\n",
                        dev_attr_tsp_firm_update.attr.name);

        if (device_create_file(sec_touchscreen,
                                &dev_attr_tsp_firm_update_status) < 0)
                pr_err("Failed to create device file(%s)!\n",
                        dev_attr_tsp_firm_update_status.attr.name);

        if (device_create_file(sec_touchscreen,
                                &dev_attr_tsp_threshold) < 0)
                pr_err("Failed to create device file(%s)!\n",
                        dev_attr_tsp_threshold.attr.name);

        if (device_create_file(sec_touchscreen,
                                &dev_attr_tsp_firm_version_phone) < 0)
                pr_err("Failed to create device file(%s)!\n",
                        dev_attr_tsp_firm_version_phone.attr.name);

        if (device_create_file(sec_touchscreen,
                                &dev_attr_tsp_firm_version_panel) < 0)
                pr_err("Failed to create device file(%s)!\n",
                        dev_attr_tsp_firm_version_panel.attr.name);

        if (device_create_file(sec_touchscreen,
                                &dev_attr_mxt_touchtype) < 0)
                pr_err("Failed to create device file(%s)!\n",
                        dev_attr_mxt_touchtype.attr.name);

        mxt_noise_test = device_create(sec_class,
                NULL, 0, NULL, "tsp_noise_test");

        if (IS_ERR(mxt_noise_test))
                pr_err("Failed to create device(mxt_noise_test)!\n");

        if (device_create_file(mxt_noise_test, &dev_attr_set_refer0) < 0)
                pr_err("Failed to create device file(%s)!\n",
                        dev_attr_set_refer0.attr.name);

        if (device_create_file(mxt_noise_test, &dev_attr_set_delta0) < 0)
                pr_err("Failed to create device file(%s)!\n",
                        dev_attr_set_delta0.attr.name);

        if (device_create_file(mxt_noise_test, &dev_attr_set_refer1) < 0)
                pr_err("Failed to create device file(%s)!\n",
                        dev_attr_set_refer1.attr.name);

        if (device_create_file(mxt_noise_test, &dev_attr_set_delta1) < 0)
                pr_err("Failed to create device file(%s)!\n",
                        dev_attr_set_delta1.attr.name);

        if (device_create_file(mxt_noise_test, &dev_attr_set_refer2) < 0)
                pr_err("Failed to create device file(%s)!\n",
                        dev_attr_set_refer2.attr.name);

        if (device_create_file(mxt_noise_test, &dev_attr_set_delta2) < 0)
                pr_err("Failed to create device file(%s)!\n",
                        dev_attr_set_delta2.attr.name);

        if (device_create_file(mxt_noise_test, &dev_attr_set_refer3) < 0)
                pr_err("Failed to create device file(%s)!\n",
                        dev_attr_set_refer3.attr.name);

        if (device_create_file(mxt_noise_test, &dev_attr_set_delta3) < 0)
                pr_err("Failed to create device file(%s)!\n",
                        dev_attr_set_delta3.attr.name);

        if (device_create_file(mxt_noise_test, &dev_attr_set_refer4) < 0)
                pr_err("Failed to create device file(%s)!\n",
                        dev_attr_set_refer4.attr.name);

        if (device_create_file(mxt_noise_test, &dev_attr_set_delta4) < 0)
                pr_err("Failed to create device file(%s)!\n",
                        dev_attr_set_delta4.attr.name);

        if (device_create_file(mxt_noise_test, &dev_attr_set_all_refer) < 0)
                pr_err("Failed to create device file(%s)!\n",
                        dev_attr_set_all_refer.attr.name);

        if (device_create_file(mxt_noise_test, &dev_attr_disp_all_refdata) < 0)
                pr_err("Failed to create device file(%s)!\n",
                        dev_attr_disp_all_refdata.attr.name);

        if (device_create_file(mxt_noise_test, &dev_attr_set_all_delta) < 0)
                pr_err("Failed to create device file(%s)!\n",
                        dev_attr_set_all_delta.attr.name);

        if (device_create_file(mxt_noise_test,
                                &dev_attr_disp_all_deltadata) < 0)
                pr_err("Failed to create device file(%s)!\n",
                        dev_attr_disp_all_deltadata.attr.name);

        if (device_create_file(mxt_noise_test, &dev_attr_set_threshold) < 0)
                pr_err("Failed to create device file(%s)!\n",
                        dev_attr_set_threshold.attr.name);

        if (device_create_file(mxt_noise_test, &dev_attr_set_firm_version) < 0)
                pr_err("Failed to create device file(%s)!\n",
                        dev_attr_set_firm_version.attr.name);

        if (device_create_file(mxt_noise_test, &dev_attr_set_module_off) < 0)
                pr_err("Failed to create device file(%s)!\n",
                        dev_attr_set_module_off.attr.name);

        if (device_create_file(mxt_noise_test, &dev_attr_set_module_on) < 0)
                pr_err("Failed to create device file(%s)!\n",
                        dev_attr_set_module_on.attr.name);

        if (device_create_file(mxt_noise_test, &dev_attr_x_line) < 0)
                pr_err("Failed to create device file(%s)!\n",
                        dev_attr_x_line.attr.name);

        if (device_create_file(mxt_noise_test, &dev_attr_y_line) < 0)
                pr_err("Failed to create device file(%s)!\n",
                        dev_attr_y_line.attr.name);

        if (device_create_file(mxt_noise_test,
                                &dev_attr_find_delta_channel) < 0)
                pr_err("Failed to create device file(%s)!\n",
                        dev_attr_find_delta_channel.attr.name);
#endif
#ifdef CONFIG_HAS_EARLYSUSPEND
        data->early_suspend.level = EARLY_SUSPEND_LEVEL_BLANK_SCREEN + 1;
        data->early_suspend.suspend = mxt_early_suspend;
        data->early_suspend.resume = mxt_late_resume;
        register_early_suspend(&data->early_suspend);
#endif
        mxt_enabled = 1;
#if !(FOR_BRINGUP)
        if (data->read_ta_status) {
                data->read_ta_status(&ta_status);
                pr_info("ta_status is %d\n", ta_status);
                mxt_ta_probe(ta_status);
        }
#endif

        return 0;

err_irq:
		printk("[ATMEL]_____mxt_prob	err_irq\n");
err_reset:
		printk("[ATMEL]_____mxt_prob 	err_reset\n");
err_backup:
		printk("[ATMEL]_____mxt_prob   err_reset\n");
err_config:
		printk("[ATMEL]_____mxt_prob   err_config\n");
        kfree(data->objects);
err_init_drv:
		printk("[ATMEL]_____mxt_prob    err_init_drv\n");
        gpio_free(data->gpio_read_done);
/* err_gpio_req:
        data->power_off();
        input_unregister_device(input_dev); */
err_reg_dev:
		printk("[ATMEL]_____mxt_prob   err_reg_dev\n");
err_alloc_dev:
		printk("[ATMEL]_____mxt_prob   err_alloc_dev\n");	
        kfree(data);
        return ret;
}

static int __devexit mxt_remove(struct i2c_client *client)
{
        struct mxt_data *data = i2c_get_clientdata(client);

#ifdef CONFIG_HAS_EARLYSUSPEND
        unregister_early_suspend(&data->early_suspend);
#endif
        free_irq(IRQ_EINT(4), data);
        kfree(data->objects);
        gpio_free(data->gpio_read_done);
        data->power_off();
        input_unregister_device(data->input_dev);
        kfree(data);

        return 0;
}

static struct i2c_device_id mxt_idtable[] = {
        {"atmel_1664", 0},
        {},
};

MODULE_DEVICE_TABLE(i2c, mxt_idtable);

static const struct dev_pm_ops mxt_pm_ops = {
        .suspend = mxt_suspend,
        .resume = mxt_resume,
};

static struct i2c_driver mxt_i2c_driver = {
        .id_table = mxt_idtable,
        .probe = mxt_probe,
        .remove = __devexit_p(mxt_remove),
        .driver = {
                .owner  = THIS_MODULE,
                .name   = MXT_DEV_NAME,
                //.pm     = &mxt_pm_ops,
        },
};

static int __init mxt_init(void)
{
	return i2c_add_driver(&mxt_i2c_driver);
}

static void __exit mxt_exit(void)
{
        i2c_del_driver(&mxt_i2c_driver);
}
module_init(mxt_init);
module_exit(mxt_exit);

MODULE_DESCRIPTION("Atmel MaXTouch driver");
MODULE_AUTHOR("ki_won.kim<ki_won.kim@samsung.com>");
MODULE_LICENSE("GPL");

