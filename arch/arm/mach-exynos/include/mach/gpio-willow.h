
#ifndef __ASM_ARCH_GPIO_WILLOW_H
#define __ASM_ARCH_GPIO_WILLOW_H __FILE__

#include "gpio.h"

/* WILLOW KEY MAP */
#define WILLOW_POWER_KEY		EXYNOS4_GPX1(3)
#define WILLOW_VOLUM_UP			EXYNOS4_GPX3(4)
#define WILLOW_VOLUM_DOWN		EXYNOS4_GPX3(3)

/* VIB KEY */
#define GPIO_VIB_PWR_EN			EXYNOS4_GPX0(7)
#define GPIO_VIB_PWM			EXYNOS4_GPD0(0)
#define GPIO_VIB_EN				EXYNOS4_GPX1(0)

/* DOCK DET */
#define DOCK_DET_N				EXYNOS4_GPX1(7)

/* BACKLIGHT */
#define BACKLIGHT_PWM_GPIO		EXYNOS4_GPD0(1)

/* PMIC GPIO */
#define PMIC_SET2				EXYNOS4_GPL0(0)
#define PMIC_SET3				EXYNOS4_GPL0(1)
#define PMIC_SET4				EXYNOS4_GPL0(2)
#define PMIC_DVS1				EXYNOS4_GPL0(3)
#define PMIC_DVS2				EXYNOS4_GPL0(4)
#define PMIC_DVS3				EXYNOS4_GPL0(6)
#define GPIO_PMIC_IRQ			EXYNOS4_GPX3(2)

#define GPIO_SPEAKER_AMP_OFF  EXYNOS4212_GPM1(2)
#define GPIO_POP_DISABLE     EXYNOS4212_GPM1(3)

#define GPIO_HW_VERSION_0           EXYNOS4_GPL2(2)     //IN
#define GPIO_HW_VERSION_1           EXYNOS4_GPL2(1)     //IN
#define GPIO_HW_VERSION_2           EXYNOS4_GPL2(0)     //IN

/* WIFI */
#define GPIO_WLAN_EN            EXYNOS4212_GPM0(4)
#define GPIO_WLAN_EN_AF         1
#define GPIO_WLAN_HOST_WAKE     EXYNOS4_GPX0(3)
#define GPIO_WLAN_HOST_WAKE_AF  0xF
#define GPIO_WLAN_SDIO_CLK      EXYNOS4_GPK3(0)
#define GPIO_WLAN_SDIO_CLK_AF   2
#define GPIO_WLAN_SDIO_CMD      EXYNOS4_GPK3(1)
#define GPIO_WLAN_SDIO_CMD_AF   2
#define GPIO_WLAN_SDIO_CD       EXYNOS4_GPK3(2)
#define GPIO_WLAN_SDIO_CD_AF    2
#define GPIO_WLAN_SDIO_D0       EXYNOS4_GPK3(3)
#define GPIO_WLAN_SDIO_D0_AF    2
#define GPIO_WLAN_SDIO_D1       EXYNOS4_GPK3(4)
#define GPIO_WLAN_SDIO_D1_AF    2
#define GPIO_WLAN_SDIO_D2       EXYNOS4_GPK3(5)
#define GPIO_WLAN_SDIO_D2_AF    2
#define GPIO_WLAN_SDIO_D3       EXYNOS4_GPK3(6)
#define GPIO_WLAN_SDIO_D3_AF    2

/* Bluetooth */
#define GPIO_BT_EN			EXYNOS4212_GPM0(5)
#define GPIO_BT_WAKE		EXYNOS4212_GPM0(7)
#define GPIO_BT_HOST_WAKE	EXYNOS4_GPX0(2)
#define IRQ_BT_HOST_WAKE	IRQ_EINT(2)
#define GPIO_BT_RXD			EXYNOS4_GPA0(0)
#define GPIO_BT_RXD_AF		2
#define GPIO_BT_TXD			EXYNOS4_GPA0(1)
#define GPIO_BT_TXD_AF		2
#define GPIO_BT_CTS			EXYNOS4_GPA0(2)
#define GPIO_BT_CTS_AF		2
#define GPIO_BT_RTS			EXYNOS4_GPA0(3)
#define GPIO_BT_RTS_AF		2

/* GPS */
#define GPIO_GPS_RXD		EXYNOS4_GPA0(4)
#define GPIO_GPS_RXD_AF		2
#define GPIO_GPS_TXD		EXYNOS4_GPA0(5)
#define GPIO_GPS_TXD_AF		2

/* ETC UART */
#define GPIO_AP_RXD			EXYNOS4_GPA1(0)
#define GPIO_AP_RXD_AF		2
#define GPIO_AP_TXD			EXYNOS4_GPA1(1)
#define GPIO_AP_TXD_AF		2
#define GPIO_TEST_RXD		EXYNOS4_GPA1(4)
#define GPIO_TEST_RXD_AF	2
#define GPIO_TEST_TXD		EXYNOS4_GPA1(5)
#define GPIO_TEST_TXD_AF	2

/* LCD */
#define GPIO_LCD_OFF_CHG		EXYNOS4_GPM0(3)
#define GPIO_nLVDS_PDN			EXYNOS4_GPM1(0)
#define GPIO_LCD_BL_PWM			EXYNOS4_GPD0(1)

/* max17040, max8903 (charger/fuel) */
#define nCHG_EN    EXYNOS4212_GPM0(2) //nCHG_EN   //CHGARGE_EN
#define nCHARGING  EXYNOS4_GPX2(5)    //nCHARGING //CHG
#define nDC_OK     EXYNOS4_GPX2(4)    //nDC_OK    //DOK
#define nUSB_OK    EXYNOS4_GPX2(6)    //nUSB_OK   //USB_OK
#define nBAT_FLT   EXYNOS4_GPX3(0)    //nBAT_FLT  //FLT

typedef enum
{
  WILLOW_HW_VERSION_NONE  = 0,
  WILLOW_HW_VERSION_WS    = 1,
  WILLOW_HW_VERSION_ES1   = 2,
  WILLOW_HW_VERSION_ES2   = 3,
  WILLOW_HW_VERSION_PP    = 4,
  WILLOW_HW_VERSION_MP    = 5,
  WILLOW_HW_VERSION_MAX   = 6
} willow_hw_version_type;

extern willow_hw_version_type WILLOW_get_hw_version( void );

extern void willow_config_sleep_gpio_table(void);
extern void willow_config_gpio_table(void);

#endif /* __ASM_ARCH_GPIO_WILLOW_H */

