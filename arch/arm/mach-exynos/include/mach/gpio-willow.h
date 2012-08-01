
#ifndef __ASM_ARCH_GPIO_WILLOW_H
#define __ASM_ARCH_GPIO_WILLOW_H __FILE__

#include "gpio.h"

/* WILLOW KEY MAP */
#define WILLOW_POWER_KEY		EXYNOS4_GPX1(3)
#define WILLOW_VOLUM_UP			EXYNOS4_GPX3(3)
#define WILLOW_VOLUM_DOWN		EXYNOS4_GPX3(4)

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

#endif /* __ASM_ARCH_GPIO_WILLOW_H */

