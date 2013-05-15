/* linux/arch/arm/mach-exynos/mach-smdk4x12.c
 *
 *  Modified by Kevin Chen< k.chen "at" dataltd "dot" com >
 *
 *
 * Copyright (c) 2011 Samsung Electronics Co., Ltd.
 *		http://www.samsung.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#include <linux/platform_device.h>
#include <linux/serial_core.h>
#include <linux/spi/spi.h>
#include <linux/spi/spi_gpio.h>
#include <linux/clk.h>
#include <linux/lcd.h>
#include <linux/gpio.h>
#include <linux/gpio_event.h>
#include <linux/i2c.h>
#include <linux/pwm_backlight.h>
#include <linux/input.h>
#include <linux/mmc/host.h>
#include <linux/regulator/machine.h>
#include <linux/regulator/max8649.h>
#include <linux/regulator/fixed.h>
#include <linux/mfd/wm8994/pdata.h>
#include <linux/mfd/max77686.h>
#include <linux/v4l2-mediabus.h>
#include <linux/duv060_eport_pdata.h>
#include <linux/si4704_pdata.h>
#include <linux/as3039s07_pdata.h>
#include <linux/bh1750fvi_pdata.h>
#include <linux/memblock.h>
#include <linux/delay.h>
#include <linux/smsc911x.h>
#include <linux/notifier.h>
#include <linux/reboot.h>
#include <linux/irq.h>

#include <asm/mach/arch.h>
#include <asm/mach-types.h>

#include <plat/regs-serial.h>
#include <plat/exynos4.h>
#include <plat/cpu.h>
#include <plat/clock.h>
#include <plat/keypad.h>
#include <plat/devs.h>
#include <plat/fb.h>
#include <plat/fb-s5p.h>
#include <plat/fb-core.h>
#include <plat/regs-fb-v4.h>
#include <plat/backlight.h>
#include <plat/gpio-cfg.h>
#include <plat/gpio-core.h>
#include <plat/regs-adc.h>
#include <plat/adc.h>
#include <plat/iic.h>
#include <plat/pd.h>
#include <plat/sdhci.h>
#include <plat/mshci.h>
#include <plat/ehci.h>
#include <plat/usbgadget.h>
#include <plat/s3c64xx-spi.h>
#if defined(CONFIG_VIDEO_FIMC)
#include <plat/fimc.h>
#elif defined(CONFIG_VIDEO_SAMSUNG_S5P_FIMC)
#include <plat/fimc-core.h>
#include <media/s5p_fimc.h>
#endif
#if defined(CONFIG_VIDEO_FIMC_MIPI)
#include <plat/csis.h>
#endif
#include <plat/tvout.h>
#include <plat/media.h>
#include <plat/regs-srom.h>
#include <plat/sysmmu.h>
#include <plat/tv-core.h>
#if defined(CONFIG_VIDEO_SAMSUNG_S5P_MFC) || defined(CONFIG_VIDEO_MFC5X)
#include <plat/s5p-mfc.h>
#endif

#include <media/s5k4ba_platform.h>
#include <media/s5k4ea_platform.h>
#include <media/ov5640_platform.h> 
#include <media/as3643.h>
#include <media/exynos_flite.h>
#include <media/exynos_fimc_is.h>
#include <video/platform_lcd.h>
#include <media/m5mo_platform.h>
#include <media/m5mols.h>
#include <mach/board_rev.h>
#include <mach/map.h>
#include <mach/spi-clocks.h>
#include <mach/exynos-ion.h>
#include <mach/regs-pmu.h>
#ifdef CONFIG_EXYNOS4_DEV_DWMCI
#include <mach/dwmci.h>
#endif

#ifdef CONFIG_EXYNOS4_CONTENT_PATH_PROTECTION
#include <mach/secmem.h>
#endif
#include <mach/dev.h>
#include <mach/ppmu.h>
#ifdef CONFIG_EXYNOS_C2C
#include <mach/c2c.h>
#endif
#include <plat/fimg2d.h>
#include <mach/dev-sysmmu.h>
#include <mach/mtk_wcn_cmb_stub.h>

#ifdef CONFIG_MPU_SENSORS_MPU6050B1
#include <linux/mpu.h>
#endif
#ifdef CONFIG_VIDEO_S5K3H2
extern struct s3c_platform_camera s5k3h2;
#endif
#ifdef CONFIG_VIDEO_S5K6AA
extern struct s3c_platform_camera s5k6aa;
#endif
#ifdef CONFIG_VIDEO_S5K4ECGX
extern struct s3c_platform_camera s5k4ecgx;
#endif

/* For Audio */
#define GPIO_VDD5V_AUDIO EXYNOS4_GPZ(6)
#define GPIO_SPK5V_EN EXYNOS4_GPA1(4)

/* max77686 */
static struct regulator_consumer_supply max77686_buck1 =
REGULATOR_SUPPLY("vdd_mif", NULL);

static struct regulator_consumer_supply max77686_buck2 =
REGULATOR_SUPPLY("vdd_arm", NULL);

static struct regulator_consumer_supply max77686_buck3 =
REGULATOR_SUPPLY("vdd_int", NULL);

static struct regulator_consumer_supply max77686_buck4 =
REGULATOR_SUPPLY("vdd_g3d", NULL);

static struct regulator_consumer_supply max77686_buck5 =
REGULATOR_SUPPLY("vdd_m12on", NULL);

static struct regulator_consumer_supply max77686_buck6 =
REGULATOR_SUPPLY("vdd_buck6", NULL);

static struct regulator_consumer_supply max77686_buck7 =
REGULATOR_SUPPLY("vdd_buck7", NULL);

static struct regulator_consumer_supply max77686_buck8 =
REGULATOR_SUPPLY("vddf_mmc", NULL);

static struct regulator_consumer_supply max77686_buck9 =
REGULATOR_SUPPLY("vdd_buck9", NULL);

static struct regulator_consumer_supply __initdata max77686_ldo1_consumer =
REGULATOR_SUPPLY("vdd_ldo1", NULL);

static struct regulator_consumer_supply __initdata max77686_ldo2_consumer =
REGULATOR_SUPPLY("vdd_ldo2", NULL);

static struct regulator_consumer_supply __initdata max77686_ldo3_consumer =
REGULATOR_SUPPLY("vdd_ldo3", NULL);

static struct regulator_consumer_supply __initdata max77686_ldo4_consumer =
REGULATOR_SUPPLY("vdd_ldo4", NULL);

static struct regulator_consumer_supply __initdata max77686_ldo5_consumer =
REGULATOR_SUPPLY("vdd_ldo5", NULL);

static struct regulator_consumer_supply __initdata max77686_ldo6_consumer =
REGULATOR_SUPPLY("vdd_ldo6", NULL);

static struct regulator_consumer_supply __initdata max77686_ldo7_consumer =
REGULATOR_SUPPLY("vdd_ldo7", NULL);

static struct regulator_consumer_supply __initdata max77686_ldo8_consumer =
REGULATOR_SUPPLY("vdd_ldo8", NULL);

static struct regulator_consumer_supply __initdata max77686_ldo9_consumer =
REGULATOR_SUPPLY("vdd_ldo9", NULL);

static struct regulator_consumer_supply __initdata max77686_ldo10_consumer =
REGULATOR_SUPPLY("vdd_ldo10", NULL);

static struct regulator_consumer_supply __initdata max77686_ldo11_consumer =
REGULATOR_SUPPLY("vdd_ldo11", NULL);

static struct regulator_consumer_supply __initdata max77686_ldo12_consumer =
REGULATOR_SUPPLY("vdd_ldo12", NULL);

static struct regulator_consumer_supply __initdata max77686_ldo13_consumer =
REGULATOR_SUPPLY("vdd_ldo13", NULL);

static struct regulator_consumer_supply __initdata max77686_ldo14_consumer =
REGULATOR_SUPPLY("vdd_ldo14", NULL);

static struct regulator_consumer_supply __initdata max77686_ldo15_consumer =
REGULATOR_SUPPLY("vdd_ldo15", NULL);

static struct regulator_consumer_supply __initdata max77686_ldo16_consumer =
REGULATOR_SUPPLY("vdd_ldo16", NULL);

static struct regulator_consumer_supply __initdata max77686_ldo17_consumer =
REGULATOR_SUPPLY("vdd_ldo17", NULL);

static struct regulator_consumer_supply __initdata max77686_ldo18_consumer =
REGULATOR_SUPPLY("vdd_ldo18", NULL);

static struct regulator_consumer_supply __initdata max77686_ldo19_consumer =
REGULATOR_SUPPLY("vdd_ldo19", NULL);

static struct regulator_consumer_supply __initdata max77686_ldo20_consumer =
REGULATOR_SUPPLY("vdd_ldo20", NULL);

static struct regulator_consumer_supply __initdata max77686_ldo21_consumer =
REGULATOR_SUPPLY("vdd_ldo21", NULL);

static struct regulator_consumer_supply __initdata max77686_ldo22_consumer =
REGULATOR_SUPPLY("vdd_ldo22", NULL);

static struct regulator_consumer_supply __initdata max77686_ldo23_consumer =
REGULATOR_SUPPLY("vdd_ldo23", NULL);

static struct regulator_consumer_supply __initdata max77686_ldo24_consumer =
REGULATOR_SUPPLY("vdd_ldo24", NULL);

static struct regulator_consumer_supply __initdata max77686_ldo25_consumer =
REGULATOR_SUPPLY("vdd_ldo25", NULL);

static struct regulator_consumer_supply __initdata max77686_ldo26_consumer =
REGULATOR_SUPPLY("vdd_ldo26", NULL);

static struct regulator_consumer_supply __initdata max77686_en32khz_ap_consumer =
REGULATOR_SUPPLY("vdd_en32khz_ap", NULL);

static struct regulator_consumer_supply __initdata max77686_en32khz_cp_consumer =
REGULATOR_SUPPLY("vdd_en32khz_cp", NULL);

static struct regulator_consumer_supply __initdata max77686_p32kh_consumer =
REGULATOR_SUPPLY("vdd_p32kh", NULL);

static struct regulator_init_data max77686_buck1_data = {
	.constraints = {
		.name = "vdd_mif range",
		.min_uV = 800000,
		.max_uV = 1050000,
		.always_on = 1,
		.boot_on = 1,
		.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE |
				REGULATOR_CHANGE_STATUS,
	},
	.num_consumer_supplies = 1,
	.consumer_supplies = &max77686_buck1,
};

static struct regulator_init_data max77686_buck2_data = {
	.constraints = {
		.name = "vdd_arm range",
		.min_uV = 800000,
		.max_uV = 1350000,
		.always_on = 1,
		.boot_on = 1,
		.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE,
	},
	.num_consumer_supplies = 1,
	.consumer_supplies = &max77686_buck2,
};

static struct regulator_init_data max77686_buck3_data = {
	.constraints = {
		.name = "vdd_int range",
		.min_uV = 800000,
		.max_uV = 1150000,
		.always_on = 1,
		.boot_on = 1,
		.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE,
	},
	.num_consumer_supplies = 1,
	.consumer_supplies = &max77686_buck3,
};

static struct regulator_init_data max77686_buck4_data = {
	.constraints = {
		.name = "vdd_g3d range",
		.min_uV = 850000,
		.max_uV = 1200000,
		.boot_on = 1,
		.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE |
				  REGULATOR_CHANGE_STATUS,
		.state_mem = {
			.disabled = 1,
		},
	},
	.num_consumer_supplies = 1,
	.consumer_supplies = &max77686_buck4,
};

static struct regulator_init_data max77686_buck5_data = {
	.constraints = {
		.name = "vdd_m12on range",
		.min_uV = 1200000,
		.max_uV = 1200000,
		.always_on	= 1,
		.boot_on = 1,
		.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE |
				  REGULATOR_CHANGE_STATUS,
		.state_mem = {
			.disabled = 1,
		},
	},
	.num_consumer_supplies = 1,
	.consumer_supplies = &max77686_buck5,
};

static struct regulator_init_data max77686_buck6_data = {
	.constraints = {
		.name = "vdd_buck6 range",
		.min_uV = 1350000,
		.max_uV = 1350000,
		.always_on	= 1,
		.boot_on    = 1,
		.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE |
				  REGULATOR_CHANGE_STATUS,
		.state_mem = {
			.disabled = 1,
		},
	},
	.num_consumer_supplies = 1,
	.consumer_supplies = &max77686_buck6,
};

static struct regulator_init_data max77686_buck7_data = {
	.constraints = {
		.name = "vdd_buck7 range",
		.min_uV = 2000000,
		.max_uV = 2000000,
		.always_on	= 1,
		.boot_on    = 1,
		.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE |
				  REGULATOR_CHANGE_STATUS,
		.state_mem = {
			.disabled = 1,
		},
	},
	.num_consumer_supplies = 1,
	.consumer_supplies = &max77686_buck7,
};

static struct regulator_init_data max77686_buck8_data = {
	.constraints = {
		.name = "vddf_mmc range",
		.min_uV = 2850000,
		.max_uV = 2850000,
		.always_on	= 1,
		.boot_on = 1,
		.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE |
				  REGULATOR_CHANGE_STATUS,
		.state_mem = {
			.disabled = 1,
		},
	},
	.num_consumer_supplies = 1,
	.consumer_supplies = &max77686_buck8,
};

static struct regulator_init_data max77686_buck9_data = {
	.constraints = {
		.name = "vdd_buck9 range",
		.min_uV     = 3300000,
		.max_uV     = 3300000,
		.always_on	= 1,
		.boot_on    = 1,
		.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE |
				  REGULATOR_CHANGE_STATUS,
		.state_mem = {
			.disabled = 1,
		},
	},
	.num_consumer_supplies = 1,
	.consumer_supplies = &max77686_buck9,
};

static struct regulator_init_data __initdata max77686_ldo1_data = {
	.constraints	= {
		.name		= "vdd_ldo1 range",
		.min_uV		= 1000000,
		.max_uV		= 1000000,
		.apply_uV	= 1,
		.always_on	= 1,
		.state_mem	= {
			.enabled	= 1,
		},
	},
	.num_consumer_supplies	= 1,
	.consumer_supplies	= &max77686_ldo1_consumer,
};

static struct regulator_init_data __initdata max77686_ldo2_data = {
	.constraints	= {
		.name		= "vdd_ldo2 range",
		.min_uV		= 1200000,
		.max_uV		= 1200000,
		.apply_uV	= 1,
		.always_on	= 1,
		.state_mem	= {
			.enabled	= 1,
		},
	},
	.num_consumer_supplies	= 1,
	.consumer_supplies	= &max77686_ldo2_consumer,
};

static struct regulator_init_data __initdata max77686_ldo3_data = {
	.constraints	= {
		.name		= "vdd_ldo3 range",
		.min_uV		= 1800000,
		.max_uV		= 1800000,
		.apply_uV	= 1,
		.always_on	= 1,
		.state_mem	= {
			.enabled	= 1,
		},
	},
	.num_consumer_supplies	= 1,
	.consumer_supplies	= &max77686_ldo3_consumer,
};

static struct regulator_init_data __initdata max77686_ldo4_data = {
	.constraints	= {
		.name		= "vdd_ldo4 range",
		.min_uV		= 2800000,
		.max_uV		= 2800000,
		.apply_uV	= 1,
		.always_on	= 1,
		.state_mem	= {
			.enabled	= 1,
		},
	},
	.num_consumer_supplies	= 1,
	.consumer_supplies	= &max77686_ldo4_consumer,
};

static struct regulator_init_data __initdata max77686_ldo5_data = {
	.constraints	= {
		.name		= "vdd_ldo5 range",
		.min_uV		= 1800000,
		.max_uV		= 1800000,
		.apply_uV	= 1,
		.always_on	= 1,
		.state_mem	= {
			.enabled	= 1,
		},
	},
	.num_consumer_supplies	= 1,
	.consumer_supplies	= &max77686_ldo5_consumer,
};

static struct regulator_init_data __initdata max77686_ldo6_data = {
	.constraints	= {
		.name		= "vdd_ldo6 range",
		.min_uV		= 1000000,
		.max_uV		= 1000000,
		.apply_uV	= 1,
		.always_on	= 1,
		.state_mem	= {
			.enabled	= 1,
		},
	},
	.num_consumer_supplies	= 1,
	.consumer_supplies	= &max77686_ldo6_consumer,
};

static struct regulator_init_data __initdata max77686_ldo7_data = {
	.constraints	= {
		.name		= "vdd_ldo7 range",
		.min_uV		= 1100000,
		.max_uV		= 1100000,
		.apply_uV	= 1,
		.always_on	= 1,
		.state_mem	= {
			.enabled	= 1,
		},
	},
	.num_consumer_supplies	= 1,
	.consumer_supplies	= &max77686_ldo7_consumer,
};

static struct regulator_init_data __initdata max77686_ldo8_data = {
	.constraints	= {
		.name		= "vdd_ldo8 range",
		.min_uV		= 1000000,
		.max_uV		= 1000000,
		.apply_uV	= 1,
		.always_on	= 1,
		.state_mem	= {
			.enabled	= 1,
		},
	},
	.num_consumer_supplies	= 1,
	.consumer_supplies	= &max77686_ldo8_consumer,
};

static struct regulator_init_data __initdata max77686_ldo9_data = {
	.constraints	= {
		.name		= "vdd_ldo9 range",
		.min_uV		= 1800000,
		.max_uV		= 1800000,
		.apply_uV	= 1,
		.always_on	= 1,
		.state_mem	= {
			.enabled	= 1,
		},
	},
	.num_consumer_supplies	= 1,
	.consumer_supplies	= &max77686_ldo9_consumer,
};

static struct regulator_init_data __initdata max77686_ldo10_data = {
	.constraints	= {
		.name		= "vdd_ldo10 range",
		.min_uV		= 1800000,
		.max_uV		= 1800000,
		.apply_uV	= 1,
		.always_on	= 1,
		.state_mem	= {
			.enabled	= 1,
		},
	},
	.num_consumer_supplies	= 1,
	.consumer_supplies	= &max77686_ldo10_consumer,
};

static struct regulator_init_data __initdata max77686_ldo11_data = {
	.constraints	= {
		.name		= "vdd_ldo11 range",
		.min_uV		= 1800000,
		.max_uV		= 1800000,
		.apply_uV	= 1,
		.always_on	= 1,
		.state_mem	= {
			.enabled	= 1,
		},
	},
	.num_consumer_supplies	= 1,
	.consumer_supplies	= &max77686_ldo11_consumer,
};

static struct regulator_init_data __initdata max77686_ldo12_data = {
	.constraints	= {
		.name		= "vdd_ldo12 range",
		.min_uV		= 3300000,
		.max_uV		= 3300000,
		.apply_uV	= 1,
		.always_on	= 1,
		.state_mem	= {
			.enabled	= 1,
		},
	},
	.num_consumer_supplies	= 1,
	.consumer_supplies	= &max77686_ldo12_consumer,
};

static struct regulator_init_data __initdata max77686_ldo13_data = {
	.constraints	= {
		.name		= "vdd_ldo13 range",
		.min_uV		= 1800000,
		.max_uV		= 1800000,
		.apply_uV	= 1,
		.always_on	= 1,
		.state_mem	= {
			.enabled	= 1,
		},
	},
	.num_consumer_supplies	= 1,
	.consumer_supplies	= &max77686_ldo13_consumer,
};

static struct regulator_init_data __initdata max77686_ldo14_data = {
	.constraints	= {
		.name		= "vdd_ldo14 range",
		.min_uV		= 1800000,
		.max_uV		= 1800000,
		.apply_uV	= 1,
		.always_on	= 1,
		.state_mem	= {
			.enabled	= 1,
		},
	},
	.num_consumer_supplies	= 1,
	.consumer_supplies	= &max77686_ldo14_consumer,
};

static struct regulator_init_data __initdata max77686_ldo15_data = {
	.constraints	= {
		.name		= "vdd_ldo15 range",
		.min_uV		= 1000000,
		.max_uV		= 1000000,
		.apply_uV	= 1,
		.always_on	= 1,
		.state_mem	= {
			.enabled	= 1,
		},
	},
	.num_consumer_supplies	= 1,
	.consumer_supplies	= &max77686_ldo15_consumer,
};

static struct regulator_init_data __initdata max77686_ldo16_data = {
	.constraints	= {
		.name		= "vdd_ldo16 range",
		.min_uV		= 1800000,
		.max_uV		= 1800000,
		.apply_uV	= 1,
		.always_on	= 1,
		.state_mem	= {
			.enabled	= 1,
		},
	},
	.num_consumer_supplies	= 1,
	.consumer_supplies	= &max77686_ldo16_consumer,
};

static struct regulator_init_data __initdata max77686_ldo17_data = {
	.constraints	= {
		.name		= "vdd_ldo17 range",
		.min_uV		= 1800000,
		.max_uV		= 1800000,
		.apply_uV	= 1,
		.always_on	= 0,
		.valid_ops_mask = REGULATOR_CHANGE_STATUS,
		.state_mem	= {
			.enabled	= 1,
		},
	},
	.num_consumer_supplies	= 1,
	.consumer_supplies	= &max77686_ldo17_consumer,
};

static struct regulator_init_data __initdata max77686_ldo18_data = {
	.constraints	= {
		.name		= "vdd_ldo18 range",
		.min_uV		= 1800000,
		.max_uV		= 1800000,
		.apply_uV	= 1,
		.always_on	= 1,
		.state_mem	= {
			.enabled	= 1,
		},
	},
	.num_consumer_supplies	= 1,
	.consumer_supplies	= &max77686_ldo18_consumer,
};

static struct regulator_init_data __initdata max77686_ldo19_data = {
	.constraints	= {
		.name		= "vdd_ldo19 range",
		.min_uV		= 1800000,
		.max_uV		= 1800000,
		.apply_uV	= 1,
		.always_on	= 1,
		.state_mem	= {
			.enabled	= 1,
		},
	},
	.num_consumer_supplies	= 1,
	.consumer_supplies	= &max77686_ldo19_consumer,
};

static struct regulator_init_data __initdata max77686_ldo20_data = {
	.constraints	= {
		.name		= "vdd_ldo20 range",
		.min_uV		= 1800000,
		.max_uV		= 1800000,
		.apply_uV	= 1,
		.always_on	= 1,
		.state_mem	= {
			.enabled	= 1,
		},
	},
	.num_consumer_supplies	= 1,
	.consumer_supplies	= &max77686_ldo20_consumer,
};

static struct regulator_init_data __initdata max77686_ldo21_data = {
	.constraints	= {
		.name		= "vdd_ldo21 range",
		.min_uV		= 2800000,
		.max_uV		= 2800000,
		.apply_uV	= 1,
		.always_on	= 0,
		.state_mem	= {
			.enabled	= 1,
		},
	},
	.num_consumer_supplies	= 1,
	.consumer_supplies	= &max77686_ldo21_consumer,
};

static struct regulator_init_data __initdata max77686_ldo22_data = {
	.constraints	= {
		.name		= "vdd_ldo22 range",
		.min_uV		= 2800000,
		.max_uV		= 2800000,
		.apply_uV	= 1,
		.always_on	= 1,
		.state_mem	= {
			.enabled	= 1,
		},
	},
	.num_consumer_supplies	= 1,
	.consumer_supplies	= &max77686_ldo22_consumer,
};

static struct regulator_init_data __initdata max77686_ldo23_data = {
	.constraints	= {
		.name		= "vdd_ldo23 range",
		.min_uV		= 1800000,
		.max_uV		= 1800000,
		.apply_uV	= 1,
		.always_on	= 0,
		.valid_ops_mask = REGULATOR_CHANGE_STATUS,
		.state_mem	= {
			.enabled	= 1,
		},
	},
	.num_consumer_supplies	= 1,
	.consumer_supplies	= &max77686_ldo23_consumer,
};

static struct regulator_init_data __initdata max77686_ldo24_data = {
	.constraints	= {
		.name		= "vdd_ldo24 range",
		.min_uV		= 1800000,
		.max_uV		= 1800000,
		.apply_uV	= 1,
		.always_on	= 1,
		.state_mem	= {
			.enabled	= 1,
		},
	},
	.num_consumer_supplies	= 1,
	.consumer_supplies	= &max77686_ldo24_consumer,
};

static struct regulator_init_data __initdata max77686_ldo25_data = {
	.constraints	= {
		.name		= "vdd_ldo25 range",
		.min_uV		= 2800000,
		.max_uV		= 2800000,
		.apply_uV	= 1,
		.always_on	= 1,
		.state_mem	= {
			.enabled	= 1,
		},
	},
	.num_consumer_supplies	= 1,
	.consumer_supplies	= &max77686_ldo25_consumer,
};

static struct regulator_init_data __initdata max77686_ldo26_data = {
	.constraints	= {
		.name		= "vdd_ldo26 range",
		.min_uV		= 2800000,
		.max_uV		= 2800000,
		.apply_uV	= 1,
		.always_on	= 0,
		.valid_ops_mask = REGULATOR_CHANGE_STATUS,
		.state_mem	= {
			.enabled	= 1,
		},
	},
	.num_consumer_supplies	= 1,
	.consumer_supplies	= &max77686_ldo26_consumer,
};

static struct regulator_init_data __initdata max77686_en32khz_ap_data = {
	.constraints	= {
		.name		= "vdd_en32khz ap",
		.always_on	= 1,
		.valid_ops_mask = REGULATOR_CHANGE_STATUS,
		.state_mem	= {
			.enabled	= 1,
		},
	},
	.num_consumer_supplies	= 1,
	.consumer_supplies	= &max77686_en32khz_ap_consumer,
};

static struct regulator_init_data __initdata max77686_en32khz_cp_data = {
	.constraints	= {
		.name		= "vdd_en32khz cp",
		.always_on	= 1,
		.valid_ops_mask = REGULATOR_CHANGE_STATUS,
		.state_mem	= {
			.enabled	= 1,
		},
	},
	.num_consumer_supplies	= 1,
	.consumer_supplies	= &max77686_en32khz_cp_consumer,
};

static struct regulator_init_data __initdata max77686_p32kh_data = {
	.constraints	= {
		.name		= "vdd_p32kh",
		.always_on	= 0,
		.valid_ops_mask = REGULATOR_CHANGE_STATUS,
		.state_mem	= {
			.enabled	= 1,
		},
	},
	.num_consumer_supplies	= 1,
	.consumer_supplies	= &max77686_p32kh_consumer,
};

static struct max77686_regulator_data max77686_regulators[] = {
	{MAX77686_BUCK1, &max77686_buck1_data,},
	{MAX77686_BUCK2, &max77686_buck2_data,},
	{MAX77686_BUCK3, &max77686_buck3_data,},
	{MAX77686_BUCK4, &max77686_buck4_data,},
	{MAX77686_BUCK5, &max77686_buck5_data,},
	{MAX77686_BUCK6, &max77686_buck6_data,},
	{MAX77686_BUCK7, &max77686_buck7_data,},
	{MAX77686_BUCK8, &max77686_buck8_data,},
	{MAX77686_BUCK9, &max77686_buck9_data,},
	{MAX77686_LDO1 , &max77686_ldo1_data, },
	{MAX77686_LDO2 , &max77686_ldo2_data, },
	{MAX77686_LDO3 , &max77686_ldo3_data, },
	{MAX77686_LDO4 , &max77686_ldo4_data, },
	{MAX77686_LDO5 , &max77686_ldo5_data, },
	{MAX77686_LDO6 , &max77686_ldo6_data, },
	{MAX77686_LDO7 , &max77686_ldo7_data, },
	{MAX77686_LDO8 , &max77686_ldo8_data, },
	{MAX77686_LDO9 , &max77686_ldo9_data, },
	{MAX77686_LDO10, &max77686_ldo10_data, },
	{MAX77686_LDO11, &max77686_ldo11_data, },
	{MAX77686_LDO12, &max77686_ldo12_data, },
	{MAX77686_LDO13, &max77686_ldo13_data, },
	{MAX77686_LDO14, &max77686_ldo14_data, },
	{MAX77686_LDO15, &max77686_ldo15_data, },
	{MAX77686_LDO16, &max77686_ldo16_data, },
	{MAX77686_LDO17, &max77686_ldo17_data, },
	{MAX77686_LDO18, &max77686_ldo18_data, },
	{MAX77686_LDO19, &max77686_ldo19_data, },
	{MAX77686_LDO20, &max77686_ldo20_data, },
	{MAX77686_LDO21, &max77686_ldo21_data, },
	{MAX77686_LDO22, &max77686_ldo22_data, },
	{MAX77686_LDO23, &max77686_ldo23_data, },
	{MAX77686_LDO24, &max77686_ldo24_data, },
	{MAX77686_LDO25, &max77686_ldo25_data, },
	{MAX77686_LDO26, &max77686_ldo26_data, },
	{MAX77686_EN32KHZ_AP, &max77686_en32khz_ap_data, },
	{MAX77686_EN32KHZ_CP, &max77686_en32khz_cp_data, },
	{MAX77686_P32KH, &max77686_p32kh_data, },
};

struct max77686_opmode_data max77686_opmode_datax[MAX77686_REG_MAX] = {
	[MAX77686_LDO1]   = {MAX77686_LDO1, MAX77686_OPMODE_STANDBY},
	[MAX77686_LDO2]   = {MAX77686_LDO2, MAX77686_OPMODE_STANDBY},
	[MAX77686_LDO3]   = {MAX77686_LDO3, MAX77686_OPMODE_STANDBY},
	[MAX77686_LDO4]   = {MAX77686_LDO4, MAX77686_OPMODE_STANDBY},
	[MAX77686_LDO5]   = {MAX77686_LDO5, MAX77686_OPMODE_STANDBY},
	[MAX77686_LDO6]   = {MAX77686_LDO6, MAX77686_OPMODE_STANDBY},
	[MAX77686_LDO7]   = {MAX77686_LDO7, MAX77686_OPMODE_STANDBY},
	[MAX77686_LDO8]   = {MAX77686_LDO8, MAX77686_OPMODE_STANDBY},
	[MAX77686_LDO9]   = {MAX77686_LDO9, MAX77686_OPMODE_STANDBY},
	[MAX77686_LDO10]  = {MAX77686_LDO10, MAX77686_OPMODE_STANDBY},
	[MAX77686_LDO11]  = {MAX77686_LDO11, MAX77686_OPMODE_STANDBY},
	[MAX77686_LDO12]  = {MAX77686_LDO12, MAX77686_OPMODE_STANDBY},
	[MAX77686_LDO13]  = {MAX77686_LDO13, MAX77686_OPMODE_STANDBY},
	[MAX77686_LDO14]  = {MAX77686_LDO14, MAX77686_OPMODE_STANDBY},
	[MAX77686_LDO15]  = {MAX77686_LDO15, MAX77686_OPMODE_STANDBY},
	[MAX77686_LDO16]  = {MAX77686_LDO16, MAX77686_OPMODE_STANDBY},
	[MAX77686_LDO17]  = {MAX77686_LDO17, MAX77686_OPMODE_STANDBY},
	[MAX77686_LDO18]  = {MAX77686_LDO18, MAX77686_OPMODE_STANDBY},
	[MAX77686_LDO19]  = {MAX77686_LDO19, MAX77686_OPMODE_STANDBY},
	[MAX77686_LDO20]  = {MAX77686_LDO20, MAX77686_OPMODE_STANDBY},
	[MAX77686_LDO21]  = {MAX77686_LDO21, MAX77686_OPMODE_STANDBY},
	[MAX77686_LDO22]  = {MAX77686_LDO22, MAX77686_OPMODE_STANDBY},
	[MAX77686_LDO23]  = {MAX77686_LDO23, MAX77686_OPMODE_STANDBY},
	[MAX77686_LDO24]  = {MAX77686_LDO24, MAX77686_OPMODE_STANDBY},
	[MAX77686_LDO25]  = {MAX77686_LDO25, MAX77686_OPMODE_STANDBY},
	[MAX77686_LDO26]  = {MAX77686_LDO26, MAX77686_OPMODE_STANDBY},
	[MAX77686_BUCK1]  = {MAX77686_BUCK1, MAX77686_OPMODE_STANDBY},
	[MAX77686_BUCK2]  = {MAX77686_BUCK2, MAX77686_OPMODE_STANDBY},
	[MAX77686_BUCK3]  = {MAX77686_BUCK3, MAX77686_OPMODE_STANDBY},
	[MAX77686_BUCK4]  = {MAX77686_BUCK4, MAX77686_OPMODE_STANDBY},
	[MAX77686_BUCK5]  = {MAX77686_BUCK5, MAX77686_OPMODE_STANDBY},
	[MAX77686_BUCK6]  = {MAX77686_BUCK6, MAX77686_OPMODE_STANDBY},
	[MAX77686_BUCK7]  = {MAX77686_BUCK7, MAX77686_OPMODE_STANDBY},
	[MAX77686_BUCK8]  = {MAX77686_BUCK8, MAX77686_OPMODE_STANDBY},
	[MAX77686_BUCK9]  = {MAX77686_BUCK9, MAX77686_OPMODE_STANDBY},
	[MAX77686_EN32KHZ_AP] = {MAX77686_EN32KHZ_AP, MAX77686_OPMODE_STANDBY},
	[MAX77686_EN32KHZ_CP] = {MAX77686_EN32KHZ_CP, MAX77686_OPMODE_STANDBY},
	[MAX77686_P32KH]      = {MAX77686_P32KH,      MAX77686_OPMODE_STANDBY},
};

static struct max77686_platform_data max77686_pdata = {
	.num_regulators = ARRAY_SIZE(max77686_regulators),
	.regulators = max77686_regulators,
	.irq_gpio	= 0,
	.irq_base	= 0,
	.wakeup		= 0,

	.opmode_data = max77686_opmode_datax,
	.ramp_rate = MAX77686_RAMP_RATE_27MV,

	.buck2_voltage[0] = 1300000,	/* 1.3V */
	.buck2_voltage[1] = 1000000,	/* 1.0V */
	.buck2_voltage[2] = 950000,	/* 0.95V */
	.buck2_voltage[3] = 900000,	/* 0.9V */
	.buck2_voltage[4] = 1000000,	/* 1.0V */
	.buck2_voltage[5] = 1000000,	/* 1.0V */
	.buck2_voltage[6] = 950000,	/* 0.95V */
	.buck2_voltage[7] = 900000,	/* 0.9V */

	.buck3_voltage[0] = 1037500,	/* 1.0375V */
	.buck3_voltage[1] = 1000000,	/* 1.0V */
	.buck3_voltage[2] = 950000,	/* 0.95V */
	.buck3_voltage[3] = 900000,	/* 0.9V */
	.buck3_voltage[4] = 1000000,	/* 1.0V */
	.buck3_voltage[5] = 1000000,	/* 1.0V */
	.buck3_voltage[6] = 950000,	/* 0.95V */
	.buck3_voltage[7] = 900000,	/* 0.9V */

	.buck4_voltage[0] = 1100000,	/* 1.1V */
	.buck4_voltage[1] = 1000000,	/* 1.0V */
	.buck4_voltage[2] = 950000,	/* 0.95V */
	.buck4_voltage[3] = 900000,	/* 0.9V */
	.buck4_voltage[4] = 1000000,	/* 1.0V */
	.buck4_voltage[5] = 1000000,	/* 1.0V */
	.buck4_voltage[6] = 950000,	/* 0.95V */
	.buck4_voltage[7] = 900000,	/* 0.9V */
};

#if defined(CONFIG_MPU_SENSORS_MPU6000) || defined(CONFIG_MPU_SENSORS_MPU6000_MODULE)

#define SENSOR_MPU_NAME "mpu6000"

static struct mpu3050_platform_data mpu_data = {
	.int_config  = 0x10,
	.orientation = {
		-1,  0,  0,
		0,  1,  0,
		0,  0, -1
	},
	/* accel */
	.accel = {
#ifdef CONFIG_MPU_SENSORS_MPU6000_MODULE
		.get_slave_descr = NULL,
#else
		.get_slave_descr = get_accel_slave_descr,
#endif
		.adapt_num   = 2,
		.bus         = EXT_SLAVE_BUS_PRIMARY,
		.address     = 0x68,
		.orientation = {
			-1,  0,  0,
			0,  1,  0,
			0,  0, -1
		},
	},
	/* compass */
	.compass = {
#ifdef CONFIG_MPU_SENSORS_MPU6000_MODULE
		.get_slave_descr = NULL,
#else
		.get_slave_descr = get_compass_slave_descr,
#endif
		.adapt_num   = 2,
		.bus         = EXT_SLAVE_BUS_SECONDARY,
		.address     = 0x0E,
		.orientation = {
			1, 0, 0,
			0, 1, 0,
			0, 0, 1
		},
	},
	/* pressure */
	.pressure = {
#ifdef CONFIG_MPU_SENSORS_MPU6000_MODULE
		.get_slave_descr = NULL,
#else
		.get_slave_descr = get_pressure_slave_descr,
#endif
		.adapt_num   = 2,
		.bus         = EXT_SLAVE_BUS_PRIMARY,
		.address     = 0x77,
		.orientation = {
			1, 0, 0,
			0, 1, 0,
			0, 0, 1
		},
	},

};
#endif

#ifdef CONFIG_GPIO_DUV060_EPORT

static void duv060_io_3v_power_set(int on)
{
#if 1
	/* EX_LV_ON : GPM3_7 */
	if (gpio_request(EXYNOS4212_GPM3(7), "IO_5V_ON"))
		printk("duv060_eport_register::EXYNOS4212_GPM3(7) failed!\n");
	else	
	{
		s3c_gpio_setpull(EXYNOS4212_GPM3(7), S3C_GPIO_PULL_NONE);
		s3c_gpio_cfgpin(EXYNOS4212_GPM3(7), S3C_GPIO_SFN(1));
		gpio_direction_output(EXYNOS4212_GPM3(7), 0);
		s5p_gpio_set_drvstr(EXYNOS4212_GPM3(7), S5P_GPIO_DRVSTR_LV4);
		gpio_free(EXYNOS4212_GPM3(7));
	}

	if (on)
	{
/*		printk("duv060_io_3v_power_set::on\n");		*/
		/* IO_3V_ON : GPA1_1 */
		if (gpio_request(EXYNOS4_GPA1(1), "IO_3V_ON"))
			printk("duv060_eport_register::EXYNOS4_GPA1(1) failed!\n");
		else	
		{
			s3c_gpio_setpull(EXYNOS4_GPA1(1), S3C_GPIO_PULL_NONE);
			s3c_gpio_cfgpin(EXYNOS4_GPA1(1), S3C_GPIO_SFN(1));
			gpio_direction_output(EXYNOS4_GPA1(1), 1);
			s5p_gpio_set_drvstr(EXYNOS4_GPA1(1), S5P_GPIO_DRVSTR_LV4);
			gpio_free(EXYNOS4_GPA1(1));
		}
	}
	else
	{
/*		printk("duv060_io_3v_power_set::off\n");	*/
		/* IO_3V_ON : GPA1_1 */
		if (gpio_request(EXYNOS4_GPA1(1), "IO_3V_ON"))
			printk("duv060_eport_register::EXYNOS4_GPA1(1) failed!\n");
		else	
		{
			s3c_gpio_setpull(EXYNOS4_GPA1(1), S3C_GPIO_PULL_NONE);
			s3c_gpio_cfgpin(EXYNOS4_GPA1(1), S3C_GPIO_SFN(1));
			gpio_direction_output(EXYNOS4_GPA1(1), 0);
			s5p_gpio_set_drvstr(EXYNOS4_GPA1(1), S5P_GPIO_DRVSTR_LV4);
			gpio_free(EXYNOS4_GPA1(1));
		}
	}
#else
	if (on)
	{
/*		printk("duv060_io_3v_power_set::on\n");		*/
		/* IO_3V_ON : GPJ1_0 */
		if (gpio_request(EXYNOS4212_GPJ1(0), "IO_3V_ON"))
			printk("duv060_eport_register::EXYNOS4212_GPJ1(0) failed!\n");
		else	
		{
			s3c_gpio_setpull(EXYNOS4212_GPJ1(0), S3C_GPIO_PULL_NONE);
			s3c_gpio_cfgpin(EXYNOS4212_GPJ1(0), S3C_GPIO_SFN(1));
			gpio_direction_output(EXYNOS4212_GPJ1(0), 1);
			s5p_gpio_set_drvstr(EXYNOS4212_GPJ1(0), S5P_GPIO_DRVSTR_LV4);
			gpio_free(EXYNOS4212_GPJ1(0));
		}
	}
	else
	{
/*		printk("duv060_io_3v_power_set::off\n");	*/
		/* IO_3V_ON : GPJ1_0 */
		if (gpio_request(EXYNOS4212_GPJ1(0), "IO_3V_ON"))
			printk("duv060_eport_register::EXYNOS4212_GPJ1(0) failed!\n");
		else	
		{
			s3c_gpio_setpull(EXYNOS4212_GPJ1(0), S3C_GPIO_PULL_NONE);
			s3c_gpio_cfgpin(EXYNOS4212_GPJ1(0), S3C_GPIO_SFN(1));
			gpio_direction_output(EXYNOS4212_GPJ1(0), 0);
			s5p_gpio_set_drvstr(EXYNOS4212_GPJ1(0), S5P_GPIO_DRVSTR_LV4);
			gpio_free(EXYNOS4212_GPJ1(0));
		}
	}
#endif
}

static int duv060_io_3v_power_get(void)
{
	int	ret = 0;
	
#if 1
	/* IO_3V_ON : GPA1_1 */
	if (gpio_request(EXYNOS4_GPA1(1), "IO_3V_ON"))
		printk("duv060_eport_register::EXYNOS4_GPA1(1) failed!\n");
	else	
	{
		ret = gpio_get_value(EXYNOS4_GPA1(1));
		gpio_free(EXYNOS4_GPA1(1));
	}
#else
	/* IO_3V_ON : GPJ1_0 */
	if (gpio_request(EXYNOS4212_GPJ1(0), "IO_3V_ON"))
		printk("duv060_eport_register::EXYNOS4212_GPJ1(0) failed!\n");
	else	
	{
		ret = gpio_get_value(EXYNOS4212_GPJ1(0));
		gpio_free(EXYNOS4212_GPJ1(0));
	}
#endif

	return ret;
}

static void duv060_io_5v_power_set(int on)
{
#if 1
	/* EX_LV_ON : GPM3_7 */
	if (gpio_request(EXYNOS4212_GPM3(7), "IO_5V_ON"))
		printk("duv060_eport_register::EXYNOS4212_GPM3(7) failed!\n");
	else	
	{
		s3c_gpio_setpull(EXYNOS4212_GPM3(7), S3C_GPIO_PULL_NONE);
		s3c_gpio_cfgpin(EXYNOS4212_GPM3(7), S3C_GPIO_SFN(1));
		gpio_direction_output(EXYNOS4212_GPM3(7), 0);
		s5p_gpio_set_drvstr(EXYNOS4212_GPM3(7), S5P_GPIO_DRVSTR_LV4);
		gpio_free(EXYNOS4212_GPM3(7));
	}

	if (on)
	{
/*		printk("duv060_io_5v_power_set::on\n");		*/
		/* IO_5V_ON : GPA1_3 */
		if (gpio_request(EXYNOS4_GPA1(3), "IO_5V_ON"))
			printk("duv060_eport_register::EXYNOS4_GPA1(3) failed!\n");
		else	
		{
			s3c_gpio_setpull(EXYNOS4_GPA1(3), S3C_GPIO_PULL_NONE);
			s3c_gpio_cfgpin(EXYNOS4_GPA1(3), S3C_GPIO_SFN(1));
			gpio_direction_output(EXYNOS4_GPA1(3), 1);
			s5p_gpio_set_drvstr(EXYNOS4_GPA1(3), S5P_GPIO_DRVSTR_LV4);
			gpio_free(EXYNOS4_GPA1(3));
		}
	}
	else
	{
/*		printk("duv060_io_5v_power_set::off\n");	*/
		/* IO_5V_ON : GPA1_3 */
		if (gpio_request(EXYNOS4_GPA1(3), "IO_5V_ON"))
			printk("duv060_eport_register::EXYNOS4_GPA1(3) failed!\n");
		else	
		{
			s3c_gpio_setpull(EXYNOS4_GPA1(3), S3C_GPIO_PULL_NONE);
			s3c_gpio_cfgpin(EXYNOS4_GPA1(3), S3C_GPIO_SFN(1));
			gpio_direction_output(EXYNOS4_GPA1(3), 0);
			s5p_gpio_set_drvstr(EXYNOS4_GPA1(3), S5P_GPIO_DRVSTR_LV4);
			gpio_free(EXYNOS4_GPA1(3));
		}
	}
#else
	if (on)
	{
/*		printk("duv060_io_5v_power_set::on\n");		*/
		/* IO_5V_ON : GPJ0_7 */
		if (gpio_request(EXYNOS4212_GPJ0(7), "IO_5V_ON"))
			printk("duv060_eport_register::EXYNOS4212_GPJ0(7) failed!\n");
		else	
		{
			s3c_gpio_setpull(EXYNOS4212_GPJ0(7), S3C_GPIO_PULL_NONE);
			s3c_gpio_cfgpin(EXYNOS4212_GPJ0(7), S3C_GPIO_SFN(1));
			gpio_direction_output(EXYNOS4212_GPJ0(7), 1);
			s5p_gpio_set_drvstr(EXYNOS4212_GPJ0(7), S5P_GPIO_DRVSTR_LV4);
			gpio_free(EXYNOS4212_GPJ0(7));
		}
	}
	else
	{
/*		printk("duv060_io_5v_power_set::off\n");	*/
		/* IO_5V_ON : GPJ0_7 */
		if (gpio_request(EXYNOS4212_GPJ0(7), "IO_5V_ON"))
			printk("duv060_eport_register::EXYNOS4212_GPJ0(7) failed!\n");
		else	
		{
			s3c_gpio_setpull(EXYNOS4212_GPJ0(7), S3C_GPIO_PULL_NONE);
			s3c_gpio_cfgpin(EXYNOS4212_GPJ0(7), S3C_GPIO_SFN(1));
			gpio_direction_output(EXYNOS4212_GPJ0(7), 0);
			s5p_gpio_set_drvstr(EXYNOS4212_GPJ0(7), S5P_GPIO_DRVSTR_LV4);
			gpio_free(EXYNOS4212_GPJ0(7));
		}
	}
#endif
}

static int duv060_io_5v_power_get(void)
{
	int	ret = 0;
	
#if 1
	/* IO_5V_ON : GPA1_3 */
	if (gpio_request(EXYNOS4_GPA1(3), "IO_5V_ON"))
		printk("duv060_eport_register::EXYNOS4_GPA1(3) failed!\n");
	else	
	{
		ret = gpio_get_value(EXYNOS4_GPA1(3));
		gpio_free(EXYNOS4_GPA1(3));
	}
#else
	/* IO_5V_ON : GPJ0_7 */
	if (gpio_request(EXYNOS4212_GPJ0(7), "IO_5V_ON"))
		printk("duv060_eport_register::EXYNOS4212_GPJ0(7) failed!\n");
	else	
	{
		ret = gpio_get_value(EXYNOS4212_GPJ0(7));
		gpio_free(EXYNOS4212_GPJ0(7));
	}
#endif

	return ret;
}

static void duv060_io_gpio_01_power_set(int on)
{
	if (on)
	{
/*		printk("duv060_io_gpio_01_power_set::on\n");		*/
		/* IO_GPIO1 : GPX1_3 */
		if (gpio_request(EXYNOS4_GPX1(3), "IO_GPIO1"))
			printk("duv060_eport_register::EXYNOS4_GPX1(3) failed!\n");
		else	
		{
			s3c_gpio_setpull(EXYNOS4_GPX1(3), S3C_GPIO_PULL_NONE);
			s3c_gpio_cfgpin(EXYNOS4_GPX1(3), S3C_GPIO_SFN(1));
			gpio_direction_output(EXYNOS4_GPX1(3), 1);
			s5p_gpio_set_drvstr(EXYNOS4_GPX1(3), S5P_GPIO_DRVSTR_LV4);
			gpio_free(EXYNOS4_GPX1(3));
		}
	}
	else
	{
/*		printk("duv060_io_gpio_01_power_set::off\n");	*/
		/* IO_GPIO1 : GPX1_3 */
		if (gpio_request(EXYNOS4_GPX1(3), "IO_GPIO1"))
			printk("duv060_eport_register::EXYNOS4_GPX1(3) failed!\n");
		else	
		{
			s3c_gpio_setpull(EXYNOS4_GPX1(3), S3C_GPIO_PULL_NONE);
			s3c_gpio_cfgpin(EXYNOS4_GPX1(3), S3C_GPIO_SFN(1));
			gpio_direction_output(EXYNOS4_GPX1(3), 0);
			s5p_gpio_set_drvstr(EXYNOS4_GPX1(3), S5P_GPIO_DRVSTR_LV4);
			gpio_free(EXYNOS4_GPX1(3));
		}
	}
}

static int duv060_io_gpio_01_power_get(void)
{
	int	ret = 0;
	
	/* IO_GPIO1 : GPX1_3 */
	if (gpio_request(EXYNOS4_GPX1(3), "IO_GPIO1"))
		printk("duv060_eport_register::EXYNOS4_GPX1(3) failed!\n");
	else	
	{
		ret = gpio_get_value(EXYNOS4_GPX1(3));
		gpio_free(EXYNOS4_GPX1(3));
	}

	return ret;
}

static void duv060_io_gpio_02_power_set(int on)
{
	if (on)
	{
/*		printk("duv060_io_gpio_02_power_set::on\n");		*/
		/* IO_GPIO2 : GPL1_0 */
		if (gpio_request(EXYNOS4_GPL1(0), "IO_GPIO2"))
			printk("duv060_eport_register::EXYNOS4_GPL1(0) failed!\n");
		else	
		{
			s3c_gpio_setpull(EXYNOS4_GPL1(0), S3C_GPIO_PULL_NONE);
			s3c_gpio_cfgpin(EXYNOS4_GPL1(0), S3C_GPIO_SFN(1));
			gpio_direction_output(EXYNOS4_GPL1(0), 1);
			s5p_gpio_set_drvstr(EXYNOS4_GPL1(0), S5P_GPIO_DRVSTR_LV4);
			gpio_free(EXYNOS4_GPL1(0));
		}
	}
	else
	{
/*		printk("duv060_io_gpio_02_power_set::off\n");	*/
		/* IO_GPIO2 : GPL1_0 */
		if (gpio_request(EXYNOS4_GPL1(0), "IO_GPIO2"))
			printk("duv060_eport_register::EXYNOS4_GPL1(0) failed!\n");
		else	
		{
			s3c_gpio_setpull(EXYNOS4_GPL1(0), S3C_GPIO_PULL_NONE);
			s3c_gpio_cfgpin(EXYNOS4_GPL1(0), S3C_GPIO_SFN(1));
			gpio_direction_output(EXYNOS4_GPL1(0), 0);
			s5p_gpio_set_drvstr(EXYNOS4_GPL1(0), S5P_GPIO_DRVSTR_LV4);
			gpio_free(EXYNOS4_GPL1(0));
		}
	}
}

static int duv060_io_gpio_02_power_get(void)
{
	int	ret = 0;
	
	/* IO_GPIO2 : GPL1_0 */
	if (gpio_request(EXYNOS4_GPL1(0), "IO_GPIO2"))
		printk("duv060_eport_register::EXYNOS4_GPL1(0) failed!\n");
	else	
	{
		ret = gpio_get_value(EXYNOS4_GPL1(0));
		gpio_free(EXYNOS4_GPL1(0));
	}

	return ret;
}

static void duv060_io_gpio_03_power_set(int on)
{
	if (on)
	{
/*		printk("duv060_io_gpio_03_power_set::on\n");		*/
		/* IO_GPIO3 : GPL1_1 */
		if (gpio_request(EXYNOS4_GPL1(1), "IO_GPIO3"))
			printk("duv060_eport_register::EXYNOS4_GPL1(1) failed!\n");
		else	
		{
			s3c_gpio_setpull(EXYNOS4_GPL1(1), S3C_GPIO_PULL_NONE);
			s3c_gpio_cfgpin(EXYNOS4_GPL1(1), S3C_GPIO_SFN(1));
			gpio_direction_output(EXYNOS4_GPL1(1), 1);
			s5p_gpio_set_drvstr(EXYNOS4_GPL1(1), S5P_GPIO_DRVSTR_LV4);
			gpio_free(EXYNOS4_GPL1(1));
		}
	}
	else
	{
/*		printk("duv060_io_gpio_03_power_set::off\n");	*/
		/* IO_GPIO3 : GPL1_1 */
		if (gpio_request(EXYNOS4_GPL1(1), "IO_GPIO3"))
			printk("duv060_eport_register::EXYNOS4_GPL1(1) failed!\n");
		else	
		{
			s3c_gpio_setpull(EXYNOS4_GPL1(1), S3C_GPIO_PULL_NONE);
			s3c_gpio_cfgpin(EXYNOS4_GPL1(1), S3C_GPIO_SFN(1));
			gpio_direction_output(EXYNOS4_GPL1(1), 0);
			s5p_gpio_set_drvstr(EXYNOS4_GPL1(1), S5P_GPIO_DRVSTR_LV4);
			gpio_free(EXYNOS4_GPL1(1));
		}
	}
}

static int duv060_io_gpio_03_power_get(void)
{
	int	ret = 0;
	
	/* IO_GPIO3 : GPL1_1 */
	if (gpio_request(EXYNOS4_GPL1(1), "IO_GPIO3"))
		printk("duv060_eport_register::EXYNOS4_GPL1(1) failed!\n");
	else	
	{
		ret = gpio_get_value(EXYNOS4_GPL1(1));
		gpio_free(EXYNOS4_GPL1(1));
	}

	return ret;
}

static void duv060_io_gpio_04_power_set(int on)
{
	if (on)
	{
/*		printk("duv060_io_gpio_04_power_set::on\n");		*/
		/* IO_GPIO4 : GPL2_0 */
		if (gpio_request(EXYNOS4_GPL2(0), "IO_GPIO4"))
			printk("duv060_eport_register::EXYNOS4_GPL2(0) failed!\n");
		else	
		{
			s3c_gpio_setpull(EXYNOS4_GPL2(0), S3C_GPIO_PULL_NONE);
			s3c_gpio_cfgpin(EXYNOS4_GPL2(0), S3C_GPIO_SFN(1));
			gpio_direction_output(EXYNOS4_GPL2(0), 1);
			s5p_gpio_set_drvstr(EXYNOS4_GPL2(0), S5P_GPIO_DRVSTR_LV4);
			gpio_free(EXYNOS4_GPL2(0));
		}
	}
	else
	{
/*		printk("duv060_io_gpio_04_power_set::off\n");	*/
		/* IO_GPIO4 : GPL2_0 */
		if (gpio_request(EXYNOS4_GPL2(0), "IO_GPIO4"))
			printk("duv060_eport_register::EXYNOS4_GPL2(0) failed!\n");
		else	
		{
			s3c_gpio_setpull(EXYNOS4_GPL2(0), S3C_GPIO_PULL_NONE);
			s3c_gpio_cfgpin(EXYNOS4_GPL2(0), S3C_GPIO_SFN(1));
			gpio_direction_output(EXYNOS4_GPL2(0), 0);
			s5p_gpio_set_drvstr(EXYNOS4_GPL2(0), S5P_GPIO_DRVSTR_LV4);
			gpio_free(EXYNOS4_GPL2(0));
		}
	}
}

static int duv060_io_gpio_04_power_get(void)
{
	int	ret = 0;
	
	/* IO_GPIO4 : GPL2_0 */
	if (gpio_request(EXYNOS4_GPL2(0), "IO_GPIO4"))
		printk("duv060_eport_register::EXYNOS4_GPL2(0) failed!\n");
	else	
	{
		ret = gpio_get_value(EXYNOS4_GPL2(0));
		gpio_free(EXYNOS4_GPL2(0));
	}

	return ret;
}

static void duv060_io_gpio_05_power_set(int on)
{
	if (on)
	{
/*		printk("duv060_io_gpio_05_power_set::on\n");		*/
		/* IO_GPIO5 : GPL2_1 */
		if (gpio_request(EXYNOS4_GPL2(1), "IO_GPIO5"))
			printk("duv060_eport_register::EXYNOS4_GPL2(1) failed!\n");
		else	
		{
			s3c_gpio_setpull(EXYNOS4_GPL2(1), S3C_GPIO_PULL_NONE);
			s3c_gpio_cfgpin(EXYNOS4_GPL2(1), S3C_GPIO_SFN(1));
			gpio_direction_output(EXYNOS4_GPL2(1), 1);
			s5p_gpio_set_drvstr(EXYNOS4_GPL2(1), S5P_GPIO_DRVSTR_LV4);
			gpio_free(EXYNOS4_GPL2(1));
		}
	}
	else
	{
/*		printk("duv060_io_gpio_05_power_set::off\n");	*/
		/* IO_GPIO5 : GPL2_1 */
		if (gpio_request(EXYNOS4_GPL2(1), "IO_GPIO5"))
			printk("duv060_eport_register::EXYNOS4_GPL2(1) failed!\n");
		else	
		{
			s3c_gpio_setpull(EXYNOS4_GPL2(1), S3C_GPIO_PULL_NONE);
			s3c_gpio_cfgpin(EXYNOS4_GPL2(1), S3C_GPIO_SFN(1));
			gpio_direction_output(EXYNOS4_GPL2(1), 0);
			s5p_gpio_set_drvstr(EXYNOS4_GPL2(1), S5P_GPIO_DRVSTR_LV4);
			gpio_free(EXYNOS4_GPL2(1));
		}
	}
}

static int duv060_io_gpio_05_power_get(void)
{
	int	ret = 0;
	
	/* IO_GPIO5 : GPL2_1 */
	if (gpio_request(EXYNOS4_GPL2(1), "IO_GPIO5"))
		printk("duv060_eport_register::EXYNOS4_GPL2(1) failed!\n");
	else	
	{
		ret = gpio_get_value(EXYNOS4_GPL2(1));
		gpio_free(EXYNOS4_GPL2(1));
	}

	return ret;
}

static void duv060_io_gpio_06_power_set(int on)
{
	if (on)
	{
/*		printk("duv060_io_gpio_06_power_set::on\n");		*/
		/* IO_GPIO6 : GPL2_2 */
		if (gpio_request(EXYNOS4_GPL2(2), "IO_GPIO6"))
			printk("duv060_eport_register::EXYNOS4_GPL2(2) failed!\n");
		else	
		{
			s3c_gpio_setpull(EXYNOS4_GPL2(2), S3C_GPIO_PULL_NONE);
			s3c_gpio_cfgpin(EXYNOS4_GPL2(2), S3C_GPIO_SFN(1));
			gpio_direction_output(EXYNOS4_GPL2(2), 1);
			s5p_gpio_set_drvstr(EXYNOS4_GPL2(2), S5P_GPIO_DRVSTR_LV4);
			gpio_free(EXYNOS4_GPL2(2));
		}
	}
	else
	{
/*		printk("duv060_io_gpio_06_power_set::off\n");	*/
		/* IO_GPIO6 : GPL2_2 */
		if (gpio_request(EXYNOS4_GPL2(2), "IO_GPIO6"))
			printk("duv060_eport_register::EXYNOS4_GPL2(2) failed!\n");
		else	
		{
			s3c_gpio_setpull(EXYNOS4_GPL2(2), S3C_GPIO_PULL_NONE);
			s3c_gpio_cfgpin(EXYNOS4_GPL2(2), S3C_GPIO_SFN(1));
			gpio_direction_output(EXYNOS4_GPL2(2), 0);
			s5p_gpio_set_drvstr(EXYNOS4_GPL2(2), S5P_GPIO_DRVSTR_LV4);
			gpio_free(EXYNOS4_GPL2(2));
		}
	}
}

static int duv060_io_gpio_06_power_get(void)
{
	int	ret = 0;
	
	/* IO_GPIO6 : GPL2_2 */
	if (gpio_request(EXYNOS4_GPL2(2), "IO_GPIO6"))
		printk("duv060_eport_register::EXYNOS4_GPL2(2) failed!\n");
	else	
	{
		ret = gpio_get_value(EXYNOS4_GPL2(2));
		gpio_free(EXYNOS4_GPL2(2));
	}

	return ret;
}

static void duv060_ir_tx1_on_set(int on)
{
	if (on)
	{
/*		printk("duv060_ir_tx1_on_set::on\n");		*/
		/* IR_TX1_ON : GPJ0_3 */
		if (gpio_request(EXYNOS4212_GPJ0(3), "IR_TX1_ON"))
			printk("duv060_eport_register::EXYNOS4212_GPJ0(3) failed!\n");
		else	
		{
			s3c_gpio_setpull(EXYNOS4212_GPJ0(3), S3C_GPIO_PULL_NONE);
			s3c_gpio_cfgpin(EXYNOS4212_GPJ0(3), S3C_GPIO_SFN(1));
			gpio_direction_output(EXYNOS4212_GPJ0(3), 1);
			s5p_gpio_set_drvstr(EXYNOS4212_GPJ0(3), S5P_GPIO_DRVSTR_LV4);
			gpio_free(EXYNOS4212_GPJ0(3));
		}
	}
	else
	{
/*		printk("duv060_ir_tx1_on_set::off\n");	*/
		/* IR_TX1_ON : GPJ0_3 */
		if (gpio_request(EXYNOS4212_GPJ0(3), "IR_TX1_ON"))
			printk("duv060_eport_register::EXYNOS4212_GPJ0(3) failed!\n");
		else	
		{
			s3c_gpio_setpull(EXYNOS4212_GPJ0(3), S3C_GPIO_PULL_NONE);
			s3c_gpio_cfgpin(EXYNOS4212_GPJ0(3), S3C_GPIO_SFN(1));
			gpio_direction_output(EXYNOS4212_GPJ0(3), 0);
			s5p_gpio_set_drvstr(EXYNOS4212_GPJ0(3), S5P_GPIO_DRVSTR_LV4);
			gpio_free(EXYNOS4212_GPJ0(3));
		}
	}
}

static int duv060_ir_tx1_on_get(void)
{
	int	ret = 0;
	
	/* IR_TX1_ON : GPJ0_3 */
	if (gpio_request(EXYNOS4212_GPJ0(3), "IR_TX1_ON"))
		printk("duv060_eport_register::EXYNOS4212_GPJ0(3) failed!\n");
	else	
	{
		ret = gpio_get_value(EXYNOS4212_GPJ0(3));
		gpio_free(EXYNOS4212_GPJ0(3));
	}

	return ret;
}

static void duv060_ir_tx2_on_set(int on)
{
	if (on)
	{
/*		printk("duv060_ir_tx2_on_set::on\n");		*/
		/* IR_TX2_ON : GPJ0_2 */
		if (gpio_request(EXYNOS4212_GPJ0(2), "IR_TX2_ON"))
			printk("duv060_eport_register::EXYNOS4212_GPJ0(2) failed!\n");
		else	
		{
			s3c_gpio_setpull(EXYNOS4212_GPJ0(2), S3C_GPIO_PULL_NONE);
			s3c_gpio_cfgpin(EXYNOS4212_GPJ0(2), S3C_GPIO_SFN(1));
			gpio_direction_output(EXYNOS4212_GPJ0(2), 1);
			s5p_gpio_set_drvstr(EXYNOS4212_GPJ0(2), S5P_GPIO_DRVSTR_LV4);
			gpio_free(EXYNOS4212_GPJ0(2));
		}
	}
	else
	{
/*		printk("duv060_ir_tx2_on_set::off\n");	*/
		/* IR_TX2_ON : GPJ0_2 */
		if (gpio_request(EXYNOS4212_GPJ0(2), "IR_TX2_ON"))
			printk("duv060_eport_register::EXYNOS4212_GPJ0(2) failed!\n");
		else	
		{
			s3c_gpio_setpull(EXYNOS4212_GPJ0(2), S3C_GPIO_PULL_NONE);
			s3c_gpio_cfgpin(EXYNOS4212_GPJ0(2), S3C_GPIO_SFN(1));
			gpio_direction_output(EXYNOS4212_GPJ0(2), 0);
			s5p_gpio_set_drvstr(EXYNOS4212_GPJ0(2), S5P_GPIO_DRVSTR_LV4);
			gpio_free(EXYNOS4212_GPJ0(2));
		}
	}
}

static int duv060_ir_tx2_on_get(void)
{
	int	ret = 0;
	
	/* IR_TX2_ON : GPJ0_2 */
	if (gpio_request(EXYNOS4212_GPJ0(2), "IR_TX2_ON"))
		printk("duv060_eport_register::EXYNOS4212_GPJ0(2) failed!\n");
	else	
	{
		ret = gpio_get_value(EXYNOS4212_GPJ0(2));
		gpio_free(EXYNOS4212_GPJ0(2));
	}

	return ret;
}

static void duv060_uart_sel_set(int on)
{
	if (on)
	{
/*		printk("duv060_uart_sel_set::on\n");		*/
		/* UART_SEL : GPZ5 */
		if (gpio_request(EXYNOS4_GPZ(5), "UART_SEL"))
			printk("duv060_eport_register::EXYNOS4_GPZ(5) failed!\n");
		else	
		{
			s3c_gpio_setpull(EXYNOS4_GPZ(5), S3C_GPIO_PULL_NONE);
			s3c_gpio_cfgpin(EXYNOS4_GPZ(5), S3C_GPIO_SFN(1));
			gpio_direction_output(EXYNOS4_GPZ(5), 1);
			s5p_gpio_set_drvstr(EXYNOS4_GPZ(5), S5P_GPIO_DRVSTR_LV4);
			gpio_free(EXYNOS4_GPZ(5));
		}
	}
	else
	{
/*		printk("duv060_uart_sel_set::off\n");	*/
		/* UART_SEL : GPZ5 */
		if (gpio_request(EXYNOS4_GPZ(5), "UART_SEL"))
			printk("duv060_eport_register::EXYNOS4_GPZ(5) failed!\n");
		else	
		{
			s3c_gpio_setpull(EXYNOS4_GPZ(5), S3C_GPIO_PULL_NONE);
			s3c_gpio_cfgpin(EXYNOS4_GPZ(5), S3C_GPIO_SFN(1));
			gpio_direction_output(EXYNOS4_GPZ(5), 0);
			s5p_gpio_set_drvstr(EXYNOS4_GPZ(5), S5P_GPIO_DRVSTR_LV4);
			gpio_free(EXYNOS4_GPZ(5));
		}
	}
}

static int duv060_uart_sel_get(void)
{
	int	ret = 0;
	
	/* UART_SEL : GPZ5 */
	if (gpio_request(EXYNOS4_GPZ(5), "UART_SEL"))
		printk("duv060_eport_register::EXYNOS4_GPZ(5) failed!\n");
	else	
	{
		ret = gpio_get_value(EXYNOS4_GPZ(5));
		gpio_free(EXYNOS4_GPZ(5));
	}

	return ret;
}

static struct duv060_eport_pdata duv060_eport= {
	.io_3v_power_set = duv060_io_3v_power_set, 
	.io_3v_power_get = duv060_io_3v_power_get, 
	.io_5v_power_set = duv060_io_5v_power_set, 
	.io_5v_power_get = duv060_io_5v_power_get, 
	.io_gpio_01_set = duv060_io_gpio_01_power_set, 
	.io_gpio_01_get = duv060_io_gpio_01_power_get, 
	.io_gpio_02_set = duv060_io_gpio_02_power_set, 
	.io_gpio_02_get = duv060_io_gpio_02_power_get, 
	.io_gpio_03_set = duv060_io_gpio_03_power_set, 
	.io_gpio_03_get = duv060_io_gpio_03_power_get, 
	.io_gpio_04_set = duv060_io_gpio_04_power_set, 
	.io_gpio_04_get = duv060_io_gpio_04_power_get, 
	.io_gpio_05_set = duv060_io_gpio_05_power_set, 
	.io_gpio_05_get = duv060_io_gpio_05_power_get, 
	.io_gpio_06_set = duv060_io_gpio_06_power_set, 
	.io_gpio_06_get = duv060_io_gpio_06_power_get, 
	.ir_tx1_on_set = duv060_ir_tx1_on_set, 
	.ir_tx1_on_get = duv060_ir_tx1_on_get, 
	.ir_tx2_on_set = duv060_ir_tx2_on_set, 
	.ir_tx2_on_get = duv060_ir_tx2_on_get, 
	.uart_sel_set = duv060_uart_sel_set, 
	.uart_sel_get = duv060_uart_sel_get, 
};

static struct platform_device duv060_eport_dev = {
	.name          = "duv060_eport",
	.id            = -1,
	.dev = {
		.platform_data = &duv060_eport,
	},
};

static void __init duv060_eport_register(void)
{
	duv060_io_3v_power_set(0);
	duv060_io_5v_power_set(0);
	duv060_io_gpio_01_power_set(0);
	duv060_io_gpio_02_power_set(0);
	duv060_io_gpio_03_power_set(0);
	duv060_io_gpio_04_power_set(0);
	duv060_io_gpio_05_power_set(0);
	duv060_io_gpio_06_power_set(0);
	duv060_ir_tx1_on_set(0);
	duv060_ir_tx2_on_set(0);
	duv060_uart_sel_set(0);
	
	platform_device_register(&duv060_eport_dev);
}

#endif

#ifdef CONFIG_LEDS_DUV060
/* GPL0_6: FLASH_PWR */
/* GPM2_3: LED_FLASH */
/* GPM2_4: LED_TORCH */
static struct gpio_led duv060_leds[] = {
	{
		.name = "led0",
		.gpio = EXYNOS4212_GPM2(4),
		.default_state = LEDS_GPIO_DEFSTATE_OFF,
	},
};

static struct gpio_led_platform_data duv060_led = {
	.num_leds = ARRAY_SIZE(duv060_leds),
	.leds = duv060_leds,
};

static struct platform_device duv060_led_dev = {
	.name          = "leds-gpio",
	.id            = -1,
	.dev = {
		.platform_data = &duv060_led,
	},
};

static void __init duv060_leds_register(void)
{
	int ret;

	/* GPL0_6: FLASH_PWR */
	if (gpio_request(EXYNOS4_GPL0(6), "FLASH_PWR"))
		printk(KERN_WARNING "FLASH_PWR(GPL0.6) Port request error!!!\n"); 
	else
	{
		s3c_gpio_setpull(EXYNOS4_GPL0(6), S3C_GPIO_PULL_NONE);
		s3c_gpio_cfgpin(EXYNOS4_GPL0(6), S3C_GPIO_SFN(1));
		gpio_direction_output(EXYNOS4_GPL0(6), 1);
		gpio_free(EXYNOS4_GPL0(6));
	}

	/* GPM2_3: LED_FLASH */
	if (gpio_request(EXYNOS4212_GPM2(3), "LED_FLASH"))
		printk(KERN_WARNING "LED_FLASH(GPM2.3) Port request error!!!\n"); 
	else
	{
		s3c_gpio_setpull(EXYNOS4212_GPM2(3), S3C_GPIO_PULL_NONE);
		s3c_gpio_cfgpin(EXYNOS4212_GPM2(3), S3C_GPIO_SFN(1));
		gpio_direction_output(EXYNOS4212_GPM2(3), 0);
		gpio_free(EXYNOS4212_GPM2(3));
	}

	/* GPM2_4: LED_TORCH */
	if (gpio_request(EXYNOS4212_GPM2(4), "LED_TORCH"))
		printk(KERN_WARNING "LED_TORCH(GPM2.4) Port request error!!!\n");
	else
	{
		s3c_gpio_setpull(EXYNOS4212_GPM2(4), S3C_GPIO_PULL_NONE);
		s3c_gpio_cfgpin(EXYNOS4212_GPM2(4), S3C_GPIO_SFN(1));
		gpio_direction_output(EXYNOS4212_GPM2(4), 0);
		gpio_free(EXYNOS4212_GPM2(4));
	}

	ret = platform_device_register(&duv060_led_dev);

	if (ret)
		printk(KERN_ERR "duv060_leds_register: failed to register leds device\n");
}
#endif

#ifdef CONFIG_VIBRATOR
#include "../../../drivers/staging/android/timed_gpio.h"

static struct timed_gpio timed_gpio_vibrator = {
	.name = "vibrator",
	.gpio = EXYNOS4_GPD0(0),
	.max_timeout	= 10000,
	.active_low = 0,
};

static struct timed_gpio_platform_data mldk_vibrator_pdata = {
	.num_gpios  = 1,
	.gpios	= &timed_gpio_vibrator,
};

static struct platform_device mldk_vibrator_device = {
	.name           = TIMED_GPIO_NAME,
	.id             = -1,
	.num_resources  = 0,
	.dev    = {
		.platform_data = &mldk_vibrator_pdata,
	},
};

static void __init mldk_vibrator_register(void)
{
	int ret;

	/* Vibrator GPIO : GPD0_0 */
	if (gpio_request(EXYNOS4_GPD0(0), "MOTOR_EN"))
		printk(KERN_WARNING "MOTOR_EN(GPD0.0) Port request error!!!\n");
	else	{
		s3c_gpio_setpull(EXYNOS4_GPD0(0), S3C_GPIO_PULL_NONE);
		s3c_gpio_cfgpin(EXYNOS4_GPD0(0), S3C_GPIO_SFN(1));
		gpio_direction_output(EXYNOS4_GPD0(0), 0);
		s5p_gpio_set_drvstr(EXYNOS4_GPD0(0), S5P_GPIO_DRVSTR_LV4);
		gpio_free(EXYNOS4_GPD0(0));
	}

	ret = platform_device_register(&mldk_vibrator_device);

	if (ret)
		printk(KERN_ERR "mldk: failed to register vibrator device\n");
}
#endif
#ifdef CONFIG_VIDEO_SAMSUNG_S5P_FIMC
#include <plat/fimc-core.h>
#include <media/s5p_fimc.h>
#endif

#ifdef CONFIG_VIDEO_JPEG_V2X
#include <plat/jpeg.h>
#endif

#ifdef CONFIG_REGULATOR_S5M8767
#include <linux/mfd/s5m87xx/s5m-core.h>
#include <linux/mfd/s5m87xx/s5m-pmic.h>
#endif

#if defined(CONFIG_EXYNOS_SETUP_THERMAL)
#include <plat/s5p-tmu.h>
#endif

#define REG_INFORM4            (S5P_INFORM4)

/* Following are default values for UCON, ULCON and UFCON UART registers */
#define SMDK4X12_UCON_DEFAULT	(S3C2410_UCON_TXILEVEL |	\
				 S3C2410_UCON_RXILEVEL |	\
				 S3C2410_UCON_TXIRQMODE |	\
				 S3C2410_UCON_RXIRQMODE |	\
				 S3C2410_UCON_RXFIFO_TOI |	\
				 S3C2443_UCON_RXERR_IRQEN)

#define SMDK4X12_ULCON_DEFAULT	S3C2410_LCON_CS8

#define SMDK4X12_UFCON_DEFAULT	(S3C2410_UFCON_FIFOMODE |	\
				 S5PV210_UFCON_TXTRIG4 |	\
				 S5PV210_UFCON_RXTRIG4)

static struct s3c2410_uartcfg smdk4x12_uartcfgs[] __initdata = {
	[0] = {
		.hwport		= 0,
		.flags		= 0,
		.ucon		= SMDK4X12_UCON_DEFAULT,
		.ulcon		= SMDK4X12_ULCON_DEFAULT,
		.ufcon		= SMDK4X12_UFCON_DEFAULT,
	},
	[1] = {
		.hwport		= 1,
		.flags		= 0,
		.ucon		= SMDK4X12_UCON_DEFAULT,
		.ulcon		= SMDK4X12_ULCON_DEFAULT,
		.ufcon		= SMDK4X12_UFCON_DEFAULT,
	},
	[2] = {
		.hwport		= 2,
		.flags		= 0,
		.ucon		= SMDK4X12_UCON_DEFAULT,
		.ulcon		= SMDK4X12_ULCON_DEFAULT,
		.ufcon		= SMDK4X12_UFCON_DEFAULT,
	},
	[3] = {
		.hwport		= 3,
		.flags		= 0,
		.ucon		= SMDK4X12_UCON_DEFAULT,
		.ulcon		= SMDK4X12_ULCON_DEFAULT,
		.ufcon		= SMDK4X12_UFCON_DEFAULT,
	},
};

#ifdef CONFIG_EXYNOS_MEDIA_DEVICE
struct platform_device exynos_device_md0 = {
	.name = "exynos-mdev",
	.id = -1,
};
#endif

/* #define WRITEBACK_ENABLED */

#if defined(CONFIG_VIDEO_FIMC) || defined(CONFIG_VIDEO_SAMSUNG_S5P_FIMC)
/*
 * External camera reset
 * Because the most of cameras take i2c bus signal, so that
 * you have to reset at the boot time for other i2c slave devices.
 * This function also called at fimc_init_camera()
 * Do optimization for cameras on your platform.
*/
#if defined(CONFIG_ITU_A) || defined(CONFIG_CSI_C) \
	|| defined(CONFIG_S5K3H2_CSI_C) || defined(CONFIG_S5K3H7_CSI_C) \
	|| defined(CONFIG_S5K4E5_CSI_C) || defined(CONFIG_S5K6A3_CSI_C)
static int smdk4x12_cam0_reset(int dummy)
{
	int err;
	/* Camera A */
	err = gpio_request(EXYNOS4_GPX1(2), "GPX1");
	if (err)
		printk(KERN_ERR "#### failed to request GPX1_2 ####\n");

	s3c_gpio_setpull(EXYNOS4_GPX1(2), S3C_GPIO_PULL_NONE);
	gpio_direction_output(EXYNOS4_GPX1(2), 0);
	gpio_direction_output(EXYNOS4_GPX1(2), 1);
	gpio_free(EXYNOS4_GPX1(2));

	return 0;
}
#endif
#if defined(CONFIG_ITU_B) || defined(CONFIG_CSI_D) \
	|| defined(CONFIG_S5K3H2_CSI_D) || defined(CONFIG_S5K3H7_CSI_D) \
	|| defined(CONFIG_S5K4E5_CSI_D) || defined(CONFIG_S5K6A3_CSI_D)

#define CAM_IIC5_SDA          EXYNOS4_GPB(2)
#define CAM_IIC5_SCL          EXYNOS4_GPB(3)

#define CAM_RESET	       EXYNOS4212_GPM1(2)
#define CAM_PWRDN	       EXYNOS4212_GPM1(3)
#define CAM_DPWEN	       EXYNOS4212_GPM3(1)
#define CAM_APWEN	       EXYNOS4212_GPM3(0)
#define CAM_VCM_PWEN	       EXYNOS4212_GPM3(2)

static int camera_powerdown_high()
{ 
	
        if (gpio_request(CAM_PWRDN, "CAM_PWRDN"))     {
                printk(KERN_ERR "%s :CAM_PWRDN request port error!\n", __func__);
        } else {
                s3c_gpio_setpull(CAM_PWRDN, S3C_GPIO_PULL_NONE);
                s3c_gpio_cfgpin(CAM_PWRDN, S3C_GPIO_SFN(1));
	
		gpio_set_value(CAM_PWRDN, 1);
	
                gpio_free(CAM_PWRDN);
                printk(KERN_DEBUG " CAM_PWRDN request port OK\n");
        }
	return 0;
}

static int  camera_reset_low()
{
	if (gpio_request(CAM_RESET, "CAM_RESET"))     {
                printk(KERN_ERR "%s :CAM_RESET request port error!\n", __func__);
        } else {
                s3c_gpio_setpull(CAM_RESET, S3C_GPIO_PULL_NONE);
                s3c_gpio_cfgpin(CAM_RESET, S3C_GPIO_SFN(1));

                gpio_set_value(CAM_RESET, 0);

                gpio_free(CAM_RESET);
                printk(KERN_DEBUG "CAM_RESET  request port OK\n");
        }
	return 0;
}
static int  camera_cm_dpwen_high()
{
	if (gpio_request(CAM_DPWEN, "CAM_DPWEN"))     {
                printk(KERN_ERR "%s :CAM_DPWEN request port error!\n", __func__);
        } else {
                s3c_gpio_setpull(CAM_DPWEN, S3C_GPIO_PULL_NONE);
                s3c_gpio_cfgpin(CAM_DPWEN, S3C_GPIO_SFN(1));

                gpio_set_value(CAM_DPWEN, 1);

                gpio_free(CAM_DPWEN);
                printk(KERN_DEBUG "CAM_DPWEN request port OK\n");
        }
	
	return 0;
}       
        
static int  camera_cm_apwen_high(){
	if (gpio_request(CAM_APWEN, "CAM_APWEN"))     {
                printk(KERN_ERR "%s :CAM_APWEN request port error!\n", __func__);
        } else {
                s3c_gpio_setpull(CAM_APWEN, S3C_GPIO_PULL_NONE);
                s3c_gpio_cfgpin(CAM_APWEN, S3C_GPIO_SFN(1));

                gpio_set_value(CAM_APWEN, 1);

                gpio_free(CAM_APWEN);
                printk(KERN_DEBUG "CAM_APWEN  request port OK\n");
        }
	return 0;
}
       
static int camera_cm_vcm_pwen_high(){
	if (gpio_request(CAM_VCM_PWEN, "CAM_VCM_PWEN"))     {
                printk(KERN_ERR "%s :CAM_RESET request port error!\n", __func__);
        } else {
                s3c_gpio_setpull(CAM_VCM_PWEN, S3C_GPIO_PULL_NONE);
                s3c_gpio_cfgpin(CAM_VCM_PWEN, S3C_GPIO_SFN(1));

                gpio_set_value(CAM_VCM_PWEN, 1);

                gpio_free(CAM_VCM_PWEN);
                printk(KERN_DEBUG "CAM_VCM_PWEN  request port OK\n");
        }
	return 0;
}

static int camera_device_init_mclk(int freq){
	return 0;
}
 
static int  camera_powerdown_low(){
	 if (gpio_request(CAM_PWRDN, "CAM_PWRDN"))     {
                printk(KERN_ERR "%s :CAM_PWRDN request port error!\n", __func__);
        } else {
                s3c_gpio_setpull(CAM_PWRDN, S3C_GPIO_PULL_NONE);
                s3c_gpio_cfgpin(CAM_PWRDN, S3C_GPIO_SFN(1));

                gpio_set_value(CAM_PWRDN, 0);

                gpio_free(CAM_PWRDN);
                printk(KERN_DEBUG " CAM_PWRDN request port OK\n");
        }
        

	return 0;
}
static int camera_device_mclk_enable(){
	return 0;
}
static int camera_device_interface_init(){
	return 0;
}
 
static int camera_reset_high(){
	if (gpio_request(CAM_RESET, "CAM_RESET"))     {
                printk(KERN_ERR "%s :CAM_RESET request port error!\n", __func__);
        } else {
                s3c_gpio_setpull(CAM_RESET, S3C_GPIO_PULL_NONE);
                s3c_gpio_cfgpin(CAM_RESET, S3C_GPIO_SFN(1));

                gpio_set_value(CAM_RESET, 1);

                gpio_free(CAM_RESET);
                printk(KERN_DEBUG "CAM_RESET  request port OK\n");
        }
	return 0;
}

static int duv060_i2c5_reset(){

	/* config for camera I2C- 5 pins   */
        if (gpio_request(CAM_IIC5_SDA, "CAMERA_IIC5_SDA"))     {
                printk(KERN_ERR "%s :CAM_IIC5_SDA request port error!\n", __func__);
        } else {
                s3c_gpio_setpull(CAM_IIC5_SDA, S3C_GPIO_PULL_NONE);
                s3c_gpio_cfgpin(CAM_IIC5_SDA, S3C_GPIO_SFN(4));
                gpio_free(CAM_IIC5_SDA);
                printk(KERN_DEBUG "CAM_IIC5 request port OK\n");
        }
        if (gpio_request(CAM_IIC5_SCL, "CAMERA_IIC5_SCL"))     {
                printk(KERN_ERR "%s :CAM_IIC5_SCL request port error!\n", __func__);
        } else {
                s3c_gpio_setpull(CAM_IIC5_SCL, S3C_GPIO_PULL_NONE);
                s3c_gpio_cfgpin(CAM_IIC5_SCL, S3C_GPIO_SFN(4));
                gpio_free(CAM_IIC5_SCL);
                printk(KERN_DEBUG "CAM_IIC5_SCL request port OK\n");
        }
        return 0;

}
 
static int smdk4x12_cam1_reset(int dummy)
{
	int err;
	/* Camera B */ 
	printk("<<<<<<In Cam1 reset dummy = %d>>>>>>>>>>>>> ",dummy);
	/* CAM A port(b0010) : PCLK, VSYNC, HREF, DATA[0-4] */
        //s3c_gpio_cfgrange_nopull(EXYNOS4212_GPJ0(0), 8, S3C_GPIO_SFN(2));
        /* CAM A port(b0010) : DATA[5-7], CLKOUT(MIPI CAM also), FIELD */
        //s3c_gpio_cfgrange_nopull(EXYNOS4212_GPJ1(0), 5, S3C_GPIO_SFN(2));
        /* CAM B port(b0011) : PCLK, DATA[0-6] */
        s3c_gpio_cfgrange_nopull(EXYNOS4212_GPM0(0), 8, S3C_GPIO_SFN(3));
        /* CAM B port(b0011) : FIELD, DATA[7]*/
        s3c_gpio_cfgrange_nopull(EXYNOS4212_GPM1(0), 2, S3C_GPIO_SFN(3));
        /* CAM B port(b0011) : VSYNC, HREF, CLKOUT*/
        s3c_gpio_cfgrange_nopull(EXYNOS4212_GPM2(0), 3, S3C_GPIO_SFN(3));


	err = gpio_request(EXYNOS4212_GPM1(2), "GPM1_2");
        if (err)
                printk(KERN_ERR "#### failed to request GPM1_2 ####\n");

        s3c_gpio_setpull(EXYNOS4212_GPM1(2), S3C_GPIO_PULL_NONE); //nCMOS_RESET
        gpio_direction_output(EXYNOS4212_GPM1(2), 0);
        gpio_direction_output(EXYNOS4212_GPM1(2), 1);
        gpio_free(EXYNOS4212_GPM1(2));
	/*Camera sensor ov5640  power up sequence*/
	camera_powerdown_high();
	camera_reset_low();
	camera_cm_dpwen_high();
	mdelay(1);
	camera_cm_apwen_high();
	camera_cm_vcm_pwen_high();
	camera_device_init_mclk(0);//dummy function not required.
	camera_powerdown_low();
	camera_device_mclk_enable();
	camera_device_interface_init();
	mdelay(1);
	camera_reset_high();
	mdelay(1); 
	/* sensor via i2c reg  can be read now*/
	/* camera sensor power up end */

	return 0;
}
#endif
#endif

#ifdef CONFIG_VIDEO_FIMC
#ifdef CONFIG_VIDEO_S5K4BA
static struct s5k4ba_platform_data s5k4ba_plat = {
	.default_width = 640,
	.default_height = 480,
	.pixelformat = V4L2_PIX_FMT_RGB565,//V4L2_PIX_FMT_NV21,
	.freq = 24000000,
	.is_mipi = 0,
};

static struct i2c_board_info s5k4ba_i2c_info = {
	I2C_BOARD_INFO("OV5640", 0x3c),
	.platform_data = &s5k4ba_plat,
};

static struct s3c_platform_camera s5k4ba = {
#ifdef CONFIG_ITU_A
	.id		= CAMERA_PAR_A,
	.clk_name	= "sclk_cam0",
	.i2c_busnum	= 5,
	.cam_power	= smdk4x12_cam0_reset,
#endif
#ifdef CONFIG_ITU_B
	.id		= CAMERA_PAR_B,
	.clk_name	= "sclk_cam1",
	.i2c_busnum	= 5,
	.cam_power	= smdk4x12_cam1_reset,
#endif
	.type		= CAM_TYPE_ITU,
	.fmt		= ITU_601_YCBCR422_8BIT,
	.order422	= CAM_ORDER422_8BIT_YCBYCR,//CAM_ORDER422_8BIT_CBYCRY,
	.info		= &s5k4ba_i2c_info,
	.pixelformat	= V4L2_PIX_FMT_RGB565,//V4L2_PIX_FMT_NV21,//V4L2_PIX_FMT_YUYV,
	.srclk_name	= "xusbxti",
	.clk_rate	= 24000000,
	.line_length	= 1920,
	.width		= 640,
	.height		= 480,
	.window		= {
		.left	= 0,
		.top	= 0,
		.width	= 640,
		.height	= 480,
	},

	/* Polarity */
	.inv_pclk	= 0,
	.inv_vsync	= 0,
	.inv_href	= 0,
	.inv_hsync	= 0,
	.reset_camera	= 1,
	.initialized	= 0,
};
#endif

#ifdef CONFIG_VIDEO_OV5640
static struct ov5640_platform_data ov5640_plat = {
	.default_width = 800,
	.default_height = 600,
	.pixelformat = V4L2_PIX_FMT_YUYV,
	.freq = 24000000,
	.is_mipi = 0,
};

static struct i2c_board_info ov5640_i2c_info = {
	I2C_BOARD_INFO("OV5640", 0x2d),
	.platform_data = &ov5640_plat,
};

static struct s3c_platform_camera ov5640 = {
#ifdef CONFIG_ITU_A
	.id		= CAMERA_PAR_A,
	.clk_name	= "sclk_cam0",
	.i2c_busnum	= 5,
	.cam_power	= smdk4x12_cam0_reset,
#endif
#ifdef CONFIG_ITU_B
	.id		= CAMERA_PAR_B,
	.clk_name	= "sclk_cam1",
	.i2c_busnum	= 5,
	.cam_power	= smdk4x12_cam1_reset,
#endif
	.type		= CAM_TYPE_ITU,
	.fmt		= ITU_601_YCBCR422_8BIT,
	.order422	= CAM_ORDER422_8BIT_CBYCRY,
	.info		= &ov5640_i2c_info,
	.pixelformat	= V4L2_PIX_FMT_YUYV,
	.srclk_name	= "xusbxti",
	.clk_rate	= 24000000,
	.line_length	= 1920,
	.width		= 1600,
	.height		= 1200,
	.window		= {
		.left	= 0,
		.top	= 0,
		.width	= 1600,
		.height	= 1200,
	},

	/* Polarity */
	.inv_pclk	= 0,
	.inv_vsync	= 1,
	.inv_href	= 0,
	.inv_hsync	= 0,
	.reset_camera	= 1,
	.initialized	= 0,
};
#endif

#ifdef WRITEBACK_ENABLED
static struct i2c_board_info writeback_i2c_info = {
	I2C_BOARD_INFO("WriteBack", 0x0),
};

static struct s3c_platform_camera writeback = {
	.id		= CAMERA_WB,
	.fmt		= ITU_601_YCBCR422_8BIT,
	.order422	= CAM_ORDER422_8BIT_CBYCRY,
	.i2c_busnum	= 0,
	.info		= &writeback_i2c_info,
	.pixelformat	= V4L2_PIX_FMT_YUV444,
	.line_length	= 800,
	.width		= 480,
	.height		= 800,
	.window		= {
		.left	= 0,
		.top	= 0,
		.width	= 480,
		.height	= 800,
	},

	.initialized	= 0,
};
#endif

#ifdef CONFIG_VIDEO_EXYNOS_FIMC_IS
#ifdef CONFIG_VIDEO_S5K3H7
static struct i2c_board_info s5k3h7_sensor_info = {
	.type = "S5K3H7",
};

static struct s3c_platform_camera s5k3h7 = {
#ifdef CONFIG_S5K3H7_CSI_C
	.id		= CAMERA_CSI_C,
	.clk_name	= "sclk_cam0",
	.cam_power	= smdk4x12_cam0_reset,
#endif
#ifdef CONFIG_S5K3H7_CSI_D
	.id		= CAMERA_CSI_D,
	.clk_name	= "sclk_cam1",
	.cam_power	= smdk4x12_cam1_reset,
#endif
	.type		= CAM_TYPE_MIPI,
	.fmt		= MIPI_CSI_RAW10,
	.info		= &s5k3h7_sensor_info,
	.order422	= CAM_ORDER422_8BIT_YCBYCR,
	.pixelformat	= V4L2_PIX_FMT_UYVY,
	.line_length	= 1920,
	.width		= 1920,
	.height		= 1080,
	.window		= {
		.left	= 0,
		.top	= 0,
		.width	= 1920,
		.height	= 1080,
	},
	.srclk_name	= "xusbxti",
	.clk_rate	= 24000000,
	.mipi_lanes	= 2,
	.mipi_settle	= 12,
	.mipi_align	= 24,

	.initialized	= 0,
#ifdef CONFIG_S5K3H7_CSI_C
	.flite_id	= FLITE_IDX_A,
#endif
#ifdef CONFIG_S5K3H7_CSI_D
	.flite_id	= FLITE_IDX_B,
#endif
	.use_isp	= true,
#ifdef CONFIG_S5K3H7_CSI_C
	.sensor_index	= 4,
#endif
#ifdef CONFIG_S5K3H7_CSI_D
	.sensor_index	= 104,
#endif
};
#endif

#ifdef CONFIG_VIDEO_S5K4E5
static struct i2c_board_info s5k4e5_sensor_info = {
	.type = "S5K4E5",
};

static struct s3c_platform_camera s5k4e5 = {
#ifdef CONFIG_S5K4E5_CSI_C
	.id		= CAMERA_CSI_C,
	.clk_name	= "sclk_cam0",
	.cam_power	= smdk4x12_cam0_reset,
#endif
#ifdef CONFIG_S5K4E5_CSI_D
	.id		= CAMERA_CSI_D,
	.clk_name	= "sclk_cam1",
	.cam_power	= smdk4x12_cam1_reset,
#endif
	.type		= CAM_TYPE_MIPI,
	.fmt		= MIPI_CSI_RAW10,
	.info		= &s5k4e5_sensor_info,
	.order422	= CAM_ORDER422_8BIT_YCBYCR,
	.pixelformat	= V4L2_PIX_FMT_UYVY,
	.line_length	= 1920,
	.width		= 1920,
	.height		= 1080,
	.window		= {
		.left	= 0,
		.top	= 0,
		.width	= 1920,
		.height	= 1080,
	},
	.srclk_name	= "xusbxti",
	.clk_rate	= 24000000,
	.mipi_lanes	= 2,
	.mipi_settle	= 12,
	.mipi_align	= 24,

	.initialized	= 0,
#ifdef CONFIG_S5K4E5_CSI_C
	.flite_id	= FLITE_IDX_A,
#endif
#ifdef CONFIG_S5K4E5_CSI_D
	.flite_id	= FLITE_IDX_B,
#endif
	.use_isp	= true,
#ifdef CONFIG_S5K4E5_CSI_C
	.sensor_index	= 3,
#endif
#ifdef CONFIG_S5K4E5_CSI_D
	.sensor_index	= 103,
#endif
};
#endif


#ifdef CONFIG_VIDEO_S5K6A3
static struct i2c_board_info s5k6a3_sensor_info = {
	.type = "S5K6A3",
};

static struct s3c_platform_camera s5k6a3 = {
#ifdef CONFIG_S5K6A3_CSI_C
	.id		= CAMERA_CSI_C,
	.clk_name	= "sclk_cam0",
	.cam_power	= smdk4x12_cam0_reset,
#endif
#ifdef CONFIG_S5K6A3_CSI_D
	.id		= CAMERA_CSI_D,
	.clk_name	= "sclk_cam1",
	.cam_power	= smdk4x12_cam1_reset,
#endif
	.type		= CAM_TYPE_MIPI,
	.fmt		= MIPI_CSI_RAW10,
	.info		= &s5k6a3_sensor_info,
	.order422	= CAM_ORDER422_8BIT_YCBYCR,
	.pixelformat	= V4L2_PIX_FMT_UYVY,
	.line_length	= 1920,
	.width		= 1920,
	.height		= 1080,
	.window		= {
		.left	= 0,
		.top	= 0,
		.width	= 1920,
		.height	= 1080,
	},
	.srclk_name	= "xusbxti",
	.clk_rate	= 24000000,
	.mipi_lanes	= 1,
	.mipi_settle	= 18,
	.mipi_align	= 24,

	.initialized	= 0,
#ifdef CONFIG_S5K6A3_CSI_C
	.flite_id	= FLITE_IDX_A,
#endif
#ifdef CONFIG_S5K6A3_CSI_D
	.flite_id	= FLITE_IDX_B,
#endif
	.use_isp	= true,
#ifdef CONFIG_S5K6A3_CSI_C
	.sensor_index	= 2,
#endif
#ifdef CONFIG_S5K6A3_CSI_D
	.sensor_index	= 102,
#endif
};
#endif

#endif

/* legacy M5MOLS Camera driver configuration */
#ifdef CONFIG_VIDEO_M5MO
#define CAM_CHECK_ERR_RET(x, msg)	\
	if (unlikely((x) < 0)) { \
		printk(KERN_ERR "\nfail to %s: err = %d\n", msg, x); \
		return x; \
	}
#define CAM_CHECK_ERR(x, msg)	\
		if (unlikely((x) < 0)) { \
			printk(KERN_ERR "\nfail to %s: err = %d\n", msg, x); \
		}

static int m5mo_config_isp_irq(void)
{
	s3c_gpio_cfgpin(EXYNOS4_GPX3(3), S3C_GPIO_SFN(0xF));
	s3c_gpio_setpull(EXYNOS4_GPX3(3), S3C_GPIO_PULL_NONE);
	return 0;
}

static struct m5mo_platform_data m5mo_plat = {
	.default_width = 640, /* 1920 */
	.default_height = 480, /* 1080 */
	.pixelformat = V4L2_PIX_FMT_UYVY,
	.freq = 24000000,
	.is_mipi = 1,
	.config_isp_irq = m5mo_config_isp_irq,
	.irq = IRQ_EINT(27),
};

static struct i2c_board_info m5mo_i2c_info = {
	I2C_BOARD_INFO("M5MO", 0x1F),
	.platform_data = &m5mo_plat,
	.irq = IRQ_EINT(27),
};

static struct s3c_platform_camera m5mo = {
#ifdef CONFIG_CSI_C
	.id		= CAMERA_CSI_C,
	.clk_name	= "sclk_cam0",
	.i2c_busnum	= 4,
	.cam_power	= smdk4x12_cam0_reset,
#endif
#ifdef CONFIG_CSI_D
	.id		= CAMERA_CSI_D,
	.clk_name	= "sclk_cam1",
	.i2c_busnum	= 5,
	.cam_power	= smdk4x12_cam1_reset,
#endif
	.type		= CAM_TYPE_MIPI,
	.fmt		= MIPI_CSI_YCBCR422_8BIT,
	.order422	= CAM_ORDER422_8BIT_YCBYCR,
	.info		= &m5mo_i2c_info,
	.pixelformat	= V4L2_PIX_FMT_UYVY,
	.srclk_name	= "xusbxti", /* "mout_mpll" */
	.clk_rate	= 24000000, /* 48000000 */
	.line_length	= 1920,
	.width		= 640,
	.height		= 480,
	.window		= {
		.left	= 0,
		.top	= 0,
		.width	= 640,
		.height	= 480,
	},

	.mipi_lanes	= 2,
	.mipi_settle	= 12,
	.mipi_align	= 32,

	/* Polarity */
	.inv_pclk	= 1,
	.inv_vsync	= 1,
	.inv_href	= 0,
	.inv_hsync	= 0,
	.reset_camera	= 0,
	.initialized	= 0,
};
#endif

/* Interface setting */
static struct s3c_platform_fimc fimc_plat = {
#ifdef CONFIG_ITU_A
	.default_cam	= CAMERA_PAR_A,
#endif
#ifdef CONFIG_ITU_B
	.default_cam	= CAMERA_PAR_B,
#endif
#ifdef CONFIG_CSI_C
	.default_cam	= CAMERA_CSI_C,
#endif
#ifdef CONFIG_CSI_D
	.default_cam	= CAMERA_CSI_D,
#endif
#ifdef WRITEBACK_ENABLED
	.default_cam	= CAMERA_WB,
#endif
	.camera		= {
#ifdef CONFIG_VIDEO_S5K4BA
		&s5k4ba,
#endif
#ifdef CONFIG_VIDEO_OV5640
		&ov5640,
#endif
#ifdef CONFIG_VIDEO_M5MO
		&m5mo,
#endif
#ifdef CONFIG_VIDEO_S5K3H2
		&s5k3h2,
#endif
#ifdef CONFIG_VIDEO_S5K3H7
		&s5k3h7,
#endif
#ifdef CONFIG_VIDEO_S5K4E5
		&s5k4e5,
#endif
#ifdef CONFIG_VIDEO_S5K6A3
		&s5k6a3,
#endif
#ifdef CONFIG_VIDEO_S5K6AA
		&s5k6aa,
#endif
#ifdef CONFIG_VIDEO_S5K4ECGX
		&s5k4ecgx,
#endif
#ifdef WRITEBACK_ENABLED
		&writeback,
#endif
	},
	.hw_ver		= 0x51,
};
#endif /* CONFIG_VIDEO_FIMC */

#define PMIC_SET1 EXYNOS4_GPX0(0)		/* PMIC_SET1 */
#define PMIC_SET2 EXYNOS4_GPX0(1)		/* PMIC_SET2 */
#define PMIC_SET3 EXYNOS4_GPX0(2)		/* PMIC_SET3 */

#define PMIC_BUCK2_DVS_nEN EXYNOS4212_GPM1(4)	/* PMIC_BUCK2_DVS_nEN */
#define PMIC_BUCK3_DVS_nEN EXYNOS4212_GPM1(5)	/* PMIC_BUCK3_DVS_nEN */
#define PMIC_BUCK4_DVS_nEN EXYNOS4212_GPM1(6)	/* PMIC_BUCK4_DVS_nEN */

static void __init duv060_pmic_hw_init(void)
{
#ifdef CONFIG_REGULATOR_MAX77686
	/* PMIC DVS config for Buck2, Buck3, Buck4 */
	if (gpio_request(PMIC_SET1, "PMIC_SET1"))
		printk(KERN_WARNING "Set PMIC_SET1 Port request error!!!\n");
	else	{
		s3c_gpio_setpull(PMIC_SET1, S3C_GPIO_PULL_NONE);
		s3c_gpio_cfgpin(PMIC_SET1, S3C_GPIO_SFN(1));
		gpio_direction_output(PMIC_SET1, 0);
		gpio_free(PMIC_SET1);
	}

	if (gpio_request(PMIC_SET2, "PMIC_SET2"))
		printk(KERN_WARNING "Set PMIC_SET2 Port request error!!!\n");
	else	{
		s3c_gpio_setpull(PMIC_SET2, S3C_GPIO_PULL_NONE);
		s3c_gpio_cfgpin(PMIC_SET2, S3C_GPIO_SFN(1));
		gpio_direction_output(PMIC_SET2, 0);
		gpio_free(PMIC_SET2);
	}

	if (gpio_request(PMIC_SET3, "PMIC_SET3"))
		printk(KERN_WARNING "Set PMIC_SET3 Port request error!!!\n");
	else	{
		s3c_gpio_setpull(PMIC_SET3, S3C_GPIO_PULL_NONE);
		s3c_gpio_cfgpin(PMIC_SET3, S3C_GPIO_SFN(1));
		gpio_direction_output(PMIC_SET3, 0);
		gpio_free(PMIC_SET3);
	}

	/* PMIC DVS Enable/Disable for Buck2, Buck3, Buck4 */
	if (gpio_request(PMIC_BUCK2_DVS_nEN, "PMIC_BUCK2_DVS_nEN"))
		printk(KERN_WARNING "Set PMIC_BUCK2_DVS_nEN Port request error!!!\n");
	else	{
		s3c_gpio_setpull(PMIC_BUCK2_DVS_nEN, S3C_GPIO_PULL_NONE);
		s3c_gpio_cfgpin(PMIC_BUCK2_DVS_nEN, S3C_GPIO_SFN(1));
		gpio_direction_output(PMIC_BUCK2_DVS_nEN, 1);
		gpio_free(PMIC_BUCK2_DVS_nEN);
	}

	if (gpio_request(PMIC_BUCK3_DVS_nEN, "PMIC_BUCK3_DVS_nEN"))
		printk(KERN_WARNING "Set PMIC_BUCK3_DVS_nEN Port request error!!!\n");
	else	{
		s3c_gpio_setpull(PMIC_BUCK3_DVS_nEN, S3C_GPIO_PULL_NONE);
		s3c_gpio_cfgpin(PMIC_BUCK3_DVS_nEN, S3C_GPIO_SFN(1));
		gpio_direction_output(PMIC_BUCK3_DVS_nEN, 1);
		gpio_free(PMIC_BUCK3_DVS_nEN);
	}

	if (gpio_request(PMIC_BUCK4_DVS_nEN, "PMIC_BUCK4_DVS_nEN"))
		printk(KERN_WARNING "Set PMIC_BUCK4_DVS_nEN Port request error!!!\n");
	else	{
		s3c_gpio_setpull(PMIC_BUCK4_DVS_nEN, S3C_GPIO_PULL_NONE);
		s3c_gpio_cfgpin(PMIC_BUCK4_DVS_nEN, S3C_GPIO_SFN(1));
		gpio_direction_output(PMIC_BUCK4_DVS_nEN, 1);
		gpio_free(PMIC_BUCK4_DVS_nEN);
	}
#endif
}


static void duv060_sensor_hw_poweron(void)
{
#ifdef CONFIG_MPU_SENSORS_MPU6050B1
	printk(KERN_ERR "duv060_sensor_hw_poweron\n");

	if (gpio_request(EXYNOS4212_GPM4(3), "GR18V_ON"))
		printk("duv060_sensor_hw_poweron::GR18V_ON failed!\n");
	else	
	{
		s3c_gpio_setpull(EXYNOS4212_GPM4(3), S3C_GPIO_PULL_NONE);
		s3c_gpio_cfgpin(EXYNOS4212_GPM4(3), S3C_GPIO_SFN(1));
		gpio_direction_output(EXYNOS4212_GPM4(3), 1);
		gpio_free(EXYNOS4212_GPM4(3));
	}
	if (gpio_request(EXYNOS4212_GPM4(2), "GR30V_ON"))
		printk("duv060_sensor_hw_poweron::GR30V_ON failed!\n");
	else	
	{
		s3c_gpio_setpull(EXYNOS4212_GPM4(2), S3C_GPIO_PULL_NONE);
		s3c_gpio_cfgpin(EXYNOS4212_GPM4(2), S3C_GPIO_SFN(1));
		gpio_direction_output(EXYNOS4212_GPM4(2), 1);
		gpio_free(EXYNOS4212_GPM4(2));
	}

	if (gpio_request(EXYNOS4212_GPM4(1), "ECOM_18ON"))
		printk("duv060_sensor_hw_poweron::GR18V_ON failed!\n");
	else	
	{
		s3c_gpio_setpull(EXYNOS4212_GPM4(1), S3C_GPIO_PULL_NONE);
		s3c_gpio_cfgpin(EXYNOS4212_GPM4(1), S3C_GPIO_SFN(1));
		gpio_direction_output(EXYNOS4212_GPM4(1), 1);
		gpio_free(EXYNOS4212_GPM4(1));
	}
	if (gpio_request(EXYNOS4212_GPM4(0), "ECOM_3ON"))
		printk("duv060_sensor_hw_poweron::GR30V_ON failed!\n");
	else	
	{
		s3c_gpio_setpull(EXYNOS4212_GPM4(0), S3C_GPIO_PULL_NONE);
		s3c_gpio_cfgpin(EXYNOS4212_GPM4(0), S3C_GPIO_SFN(1));
		gpio_direction_output(EXYNOS4212_GPM4(0), 1);
		gpio_free(EXYNOS4212_GPM4(0));
	}

#endif
}

static void duv060_sensor_hw_poweroff(void)
{
#ifdef CONFIG_MPU_SENSORS_MPU6050B1
	printk(KERN_ERR "duv060_sensor_hw_poweroff\n");

	if (gpio_request(EXYNOS4212_GPM4(3), "GR18V_ON"))
		printk("duv060_sensor_hw_poweroff::GR18V_ON failed!\n");
	else	
	{
		s3c_gpio_setpull(EXYNOS4212_GPM4(3), S3C_GPIO_PULL_NONE);
		s3c_gpio_cfgpin(EXYNOS4212_GPM4(3), S3C_GPIO_SFN(1));
		gpio_direction_output(EXYNOS4212_GPM4(3), 0);
		gpio_free(EXYNOS4212_GPM4(3));
	}
	if (gpio_request(EXYNOS4212_GPM4(2), "GR30V_ON"))
		printk("duv060_sensor_hw_poweroff::GR30V_ON failed!\n");
	else	
	{
		s3c_gpio_setpull(EXYNOS4212_GPM4(2), S3C_GPIO_PULL_NONE);
		s3c_gpio_cfgpin(EXYNOS4212_GPM4(2), S3C_GPIO_SFN(1));
		gpio_direction_output(EXYNOS4212_GPM4(2), 0);
		gpio_free(EXYNOS4212_GPM4(2));
	}

	if (gpio_request(EXYNOS4212_GPM4(1), "ECOM_18ON"))
		printk("duv060_sensor_hw_poweroff::GR18V_ON failed!\n");
	else	
	{
		s3c_gpio_setpull(EXYNOS4212_GPM4(1), S3C_GPIO_PULL_NONE);
		s3c_gpio_cfgpin(EXYNOS4212_GPM4(1), S3C_GPIO_SFN(1));
		gpio_direction_output(EXYNOS4212_GPM4(1), 0);
		gpio_free(EXYNOS4212_GPM4(1));
	}
	if (gpio_request(EXYNOS4212_GPM4(0), "ECOM_3ON"))
		printk("duv060_sensor_hw_poweroff::GR30V_ON failed!\n");
	else	
	{
		s3c_gpio_setpull(EXYNOS4212_GPM4(0), S3C_GPIO_PULL_NONE);
		s3c_gpio_cfgpin(EXYNOS4212_GPM4(0), S3C_GPIO_SFN(1));
		gpio_direction_output(EXYNOS4212_GPM4(0), 0);
		gpio_free(EXYNOS4212_GPM4(0));
	}

#endif
}

static void __init duv060_sensor_hw_init(void)
{
#ifdef CONFIG_MPU_SENSORS_MPU6050B1
	printk(KERN_ERR "duv060_sensor_hw_init\n");
	duv060_sensor_hw_poweron();

	/* Sensor MPU6050 INT */
	if (gpio_request(EXYNOS4_GPX1(5), "MPU6050 INT"))
		printk(KERN_WARNING "MPU6050 INT(GPX1.5) Port request error!!!\n");
	else	{
		s3c_gpio_setpull(EXYNOS4_GPX1(5), S3C_GPIO_PULL_NONE);
		s3c_gpio_cfgpin(EXYNOS4_GPX1(5), S3C_GPIO_SFN(0xf));
		irq_set_irq_type(gpio_to_irq(EXYNOS4_GPX1(5)), IRQ_TYPE_LEVEL_HIGH);
		gpio_free(EXYNOS4_GPX1(5));
	}

	/* Sensor AK8975 DRDY */
	if (gpio_request(EXYNOS4_GPX1(4), "AK8975 RDY"))
		printk(KERN_WARNING "AK8975 RDY(GPX1.4) Port request error!!!\n");
	else	{
		s3c_gpio_setpull(EXYNOS4_GPX1(4), S3C_GPIO_PULL_NONE);
		s3c_gpio_cfgpin(EXYNOS4_GPX1(4), S3C_GPIO_SFN(0xf));
		irq_set_irq_type(gpio_to_irq(EXYNOS4_GPX1(4)), IRQ_TYPE_LEVEL_HIGH);
		gpio_free(EXYNOS4_GPX1(4));
	}
#endif
}

#ifdef CONFIG_MPU_SENSORS_MPU6050B1
static struct mpu_platform_ops inv_mpu_power_ops = {
	.poweron = duv060_sensor_hw_poweron,
	.poweroff = duv060_sensor_hw_poweroff,
};

static struct mpu_platform_data_package inv_mpu_data = {
	.data = {
		.int_config = 0x10,
		.orientation = {
			-1, 0, 0,
			0, -1, 0,
			0, 0,  1},
		.level_shifter = 0,
	},
	.ops = &inv_mpu_power_ops,
};

/* compass */
static struct ext_slave_platform_data inv_mpu_compass_data = {
	.bus = EXT_SLAVE_BUS_PRIMARY,
	.orientation = {
		1, 0, 0,
		0, 1, 0,
		0, 0, 1},
};
#endif

#if defined(CONFIG_TOUCHSCREEN_HIMAX)
#define TS_IRQ		EXYNOS4212_GPM3(4)
#define TS_SDA		EXYNOS4_GPC1(3)
#define TS_SCL		EXYNOS4_GPC1(4)
#define TS_RST		EXYNOS4_GPL2(3)

static void __init duv060_touch_hw_init(void)
{
	printk(KERN_DEBUG "HIMAX:duv060_touch_hw_init+\n");

	/* config for Reset pin */
	if (gpio_request(TS_RST, "TS_RST"))	{
		printk(KERN_ERR "%s :TS RST request port error!\n", __func__);
	} else {
		s3c_gpio_setpull(TS_RST, S3C_GPIO_PULL_NONE);
		s3c_gpio_cfgpin(TS_RST, S3C_GPIO_SFN(1));
		gpio_direction_output(TS_RST, 0);
		gpio_free(TS_RST);
		printk(KERN_DEBUG "HIMAX:TS RST request port OK\n");
	}

	/* config for interrupt pin */
	if (gpio_request(TS_IRQ, "TS_IRQ"))	{
		printk(KERN_ERR "%s :TS IRQ request port error!\n", __func__);
	} else {
		s3c_gpio_setpull(TS_IRQ, S3C_GPIO_PULL_NONE);
		gpio_direction_input(TS_IRQ);
		gpio_free(TS_IRQ);
		printk(KERN_DEBUG "HIMAX:TS IRQ request port OK\n");
	}

	/* config for I2C pins */
	if (gpio_request(TS_SDA, "TS_SDA"))	{
		printk(KERN_ERR "%s :TS_SDA request port error!\n", __func__);
	} else {
		s3c_gpio_setpull(TS_SDA, S3C_GPIO_PULL_NONE);
		s3c_gpio_cfgpin(TS_SDA, S3C_GPIO_SFN(4));
		gpio_free(TS_SDA);
		printk(KERN_DEBUG "HIMAX:TS_SDA request port OK\n");
	}
}
#endif


static bool duv060_as3643_pm_flag = 0;


static struct as3643_pin_state duv060_as3643_pinstate = {
        .mask           = 0x0008, /*VGP3*/
        .values         = 0x0008,
};

static int duv060_as3643_pm(int pwr)
{
                //return -1;
                return 0;
}

static int as3643_gpio_envm(int gpio){
        switch (gpio) {
        case 1:
		/* GPM2_3: LED_FLASH */
	        if (gpio_request(EXYNOS4212_GPM2(3), "LED_FLASH"))
         	       printk(KERN_WARNING "LED_FLASH(GPM2.3) Port request error!!!\n");
        	else
        	{	
                	s3c_gpio_setpull(EXYNOS4212_GPM2(3), S3C_GPIO_PULL_NONE);
                	s3c_gpio_cfgpin(EXYNOS4212_GPM2(3), S3C_GPIO_SFN(1));
                	gpio_direction_output(EXYNOS4212_GPM2(3), 0); 
			gpio_set_value(EXYNOS4212_GPM2(3), 1);	
                	gpio_free(EXYNOS4212_GPM2(3));
        	}
		return 0;
        case 2:
        	/* GPM2_4: LED_TORCH */
        	if (gpio_request(EXYNOS4212_GPM2(4), "LED_TORCH"))
                	printk(KERN_WARNING "LED_TORCH(GPM2.4) Port request error!!!\n");
        	else
        	{
                	s3c_gpio_setpull(EXYNOS4212_GPM2(4), S3C_GPIO_PULL_NONE);
                	s3c_gpio_cfgpin(EXYNOS4212_GPM2(4), S3C_GPIO_SFN(1));
                	gpio_direction_output(EXYNOS4212_GPM2(4), 0);
			gpio_set_value(EXYNOS4212_GPM2(4), 1);
                	gpio_free(EXYNOS4212_GPM2(4));
        	}
		return 0;
	case 3:
		/* GPM2_3: LED_FLASH */
                if (gpio_request(EXYNOS4212_GPM2(3), "LED_FLASH"))
                       printk(KERN_WARNING "LED_FLASH(GPM2.3) Port request error!!!\n");
                else
                {
                        s3c_gpio_setpull(EXYNOS4212_GPM2(3), S3C_GPIO_PULL_NONE);
                        s3c_gpio_cfgpin(EXYNOS4212_GPM2(3), S3C_GPIO_SFN(1));
                        gpio_direction_output(EXYNOS4212_GPM2(3), 0);
                        gpio_set_value(EXYNOS4212_GPM2(3), 0);
                        gpio_free(EXYNOS4212_GPM2(3));
                }

		/* GPM2_4: LED_TORCH */
                if (gpio_request(EXYNOS4212_GPM2(4), "LED_TORCH"))
                        printk(KERN_WARNING "LED_TORCH(GPM2.4) Port request error!!!\n");
                else
                {
                        s3c_gpio_setpull(EXYNOS4212_GPM2(4), S3C_GPIO_PULL_NONE);
                        s3c_gpio_cfgpin(EXYNOS4212_GPM2(4), S3C_GPIO_SFN(1));
                        gpio_direction_output(EXYNOS4212_GPM2(4), 0);
                        gpio_set_value(EXYNOS4212_GPM2(4), 0);
                        gpio_free(EXYNOS4212_GPM2(4));
                }
		return 0;		
	
        default:
                return 0;
        }
 
}

static struct as3643_platform_data duv060_as3643_data = {
        .cfg            = 0,
        .num            = 1,
        .max_amp_torch  = CAMERA_FLASH_MAX_TORCH_AMP,
        .max_amp_flash  = CAMERA_FLASH_MAX_FLASH_AMP,
        .pinstate       = &duv060_as3643_pinstate,
        .init           = NULL,
        .exit           = NULL,
        .pm             = duv060_as3643_pm,
        .gpio_envm      = as3643_gpio_envm,
        .gpio_sync      = NULL,
};



#define AS3643_SCL     EXYNOS4_GPB(1)
#define AS3643_SDA     EXYNOS4_GPB(0)

#define AS3643_FLASH_PWR  
#define AS3643_LED_TORCH
#define AS3643_LED_FLASH
#define AS3643_CM_STROBE 


static void __init as3643_hw_init(void){

	/* GPL0_6: FLASH_PWR */
        if (gpio_request(EXYNOS4_GPL0(6), "FLASH_PWR"))
                printk(KERN_WARNING "FLASH_PWR(GPL0.6) Port request error!!!\n");
        else
        {
                s3c_gpio_setpull(EXYNOS4_GPL0(6), S3C_GPIO_PULL_NONE);
                s3c_gpio_cfgpin(EXYNOS4_GPL0(6), S3C_GPIO_SFN(1));
                gpio_direction_output(EXYNOS4_GPL0(6), 1);
		 gpio_set_value(EXYNOS4_GPL0(6), 1);

                gpio_free(EXYNOS4_GPL0(6));
        }
	//mdelay(1);
	

	 /* config for I2C pins */
        if (gpio_request(AS3643_SDA, "AS3643_SDA"))     {
                printk(KERN_ERR "%s :AS3643_SDA request port error!\n", __func__);
        } else {
                s3c_gpio_setpull(AS3643_SDA, S3C_GPIO_PULL_NONE);
                s3c_gpio_cfgpin(AS3643_SDA, S3C_GPIO_SFN(4));
                gpio_free(AS3643_SDA);
                printk(KERN_DEBUG "as3643: AS3643_SDA request port OK\n");
        }

        if (gpio_request(AS3643_SCL, "AS3643_SCL"))     {
                printk(KERN_ERR "%s :AS3643_SCL request port error!\n", __func__);
        } else {
                s3c_gpio_setpull(AS3643_SCL, S3C_GPIO_PULL_NONE);
                s3c_gpio_cfgpin(AS3643_SCL, S3C_GPIO_SFN(4));
                gpio_free(AS3643_SCL);
                printk(KERN_DEBUG "as3643:AS3643_SCL request port OK\n");
        }


}


#if defined(CONFIG_TOUCHSCREEN_FT5x06)
/*
#define TS_IRQ		EXYNOS4_GPX1(1)
#define TS_SDA		EXYNOS4_GPC1(3)
#define TS_SCL		EXYNOS4_GPC1(4)
#define TS_RST		EXYNOS4_GPL2(3)
*/
#define TS_IRQ		EXYNOS4_GPB(5)
#define TS_SDA		EXYNOS4_GPA0(6)
#define TS_SCL		EXYNOS4_GPA0(7)
#define TS_RST		EXYNOS4_GPB(4)

static void __init duv060_touch_hw_init(void)
{
	printk(KERN_DEBUG "FT5x06:duv060_touch_hw_init+\n");

	/* config for Reset pin */
	if (gpio_request(TS_RST, "TS_RST"))	{
		printk(KERN_ERR "%s :TS RST request port error!\n", __func__);
	} else {
		s3c_gpio_setpull(TS_RST, S3C_GPIO_PULL_NONE);
		s3c_gpio_cfgpin(TS_RST, S3C_GPIO_SFN(1));
		gpio_direction_output(TS_RST, 0);
		gpio_free(TS_RST);
		printk(KERN_DEBUG "FT5x06:TS RST request port OK\n");
	}

	/* config for interrupt pin */
	if (gpio_request(TS_IRQ, "TS_IRQ"))	{
		printk(KERN_ERR "%s :TS IRQ request port error!\n", __func__);
	} else {
		s3c_gpio_setpull(TS_IRQ, S3C_GPIO_PULL_NONE);
		gpio_direction_input(TS_IRQ);
		gpio_free(TS_IRQ);
		printk(KERN_DEBUG "FT5x06:TS IRQ request port OK\n");
	}

	/* config for I2C pins */
	if (gpio_request(TS_SDA, "TS_SDA"))	{
		printk(KERN_ERR "%s :TS_SDA request port error!\n", __func__);
	} else {
		s3c_gpio_setpull(TS_SDA, S3C_GPIO_PULL_NONE);
		s3c_gpio_cfgpin(TS_SDA, S3C_GPIO_SFN(4));
		gpio_free(TS_SDA);
		printk(KERN_DEBUG "FT5x06:TS_SDA request port OK\n");
	}

	if (gpio_request(TS_SCL, "TS_SCL"))	{
		printk(KERN_ERR "%s :TS_SCL request port error!\n", __func__);
	} else {
		s3c_gpio_setpull(TS_SCL, S3C_GPIO_PULL_NONE);
		s3c_gpio_cfgpin(TS_SCL, S3C_GPIO_SFN(4));
		gpio_free(TS_SCL);
		printk(KERN_DEBUG "FT5x06:TS_SCL request port OK\n");
	}

	printk(KERN_DEBUG "FT5x06:duv060_touch_hw_init-\n");
}
#endif

#if defined (CONFIG_SENSORS_AS3039S07)
static void __init duv060_as3039s07_hw_init(void)
{
	/* LIGHT_ON : GPM4_4 */
	if (gpio_request(EXYNOS4212_GPM4(4), "LIGHT_ON"))
		printk("duv060_as3039s07_hw_init::EXYNOS4212_GPM4(4) failed!\n");
	else
	{
		s3c_gpio_setpull(EXYNOS4212_GPM4(4), S3C_GPIO_PULL_NONE);
		s3c_gpio_cfgpin(EXYNOS4212_GPM4(4), S3C_GPIO_SFN(1));
		gpio_direction_output(EXYNOS4212_GPM4(4), 0);
		s5p_gpio_set_drvstr(EXYNOS4212_GPM4(4), S5P_GPIO_DRVSTR_LV4);
		gpio_free(EXYNOS4212_GPM4(4));
	}
}
#endif

#if defined (CONFIG_SENSORS_BH1750FVI)
static void __init duv060_bh170fvi_hw_init(void)
{
	/* LIGHT_ON : GPM4_4 */
	if (gpio_request(EXYNOS4212_GPM4(4), "LIGHT_ON"))
		printk("duv060_bh170fvi_hw_init::EXYNOS4212_GPM4(4) failed!\n");
	else
	{
		s3c_gpio_setpull(EXYNOS4212_GPM4(4), S3C_GPIO_PULL_NONE);
		s3c_gpio_cfgpin(EXYNOS4212_GPM4(4), S3C_GPIO_SFN(1));
		gpio_direction_output(EXYNOS4212_GPM4(4), 0);
		s5p_gpio_set_drvstr(EXYNOS4212_GPM4(4), S5P_GPIO_DRVSTR_LV4);
		gpio_free(EXYNOS4212_GPM4(4));
	}
}
#endif

#if defined (CONFIG_I2C_SI4704)
static void duv060_si4704_hw_init(void)
{
	/* FM_VA_ON : GPC0_2 */
	if (gpio_request(EXYNOS4_GPC0(2), "SI4704_VA"))
		printk("duv060_si4704_hw_init::EXYNOS4_GPC0(2) failed!\n");
	else
	{
		s3c_gpio_setpull(EXYNOS4_GPC0(2), S3C_GPIO_PULL_NONE);
		s3c_gpio_cfgpin(EXYNOS4_GPC0(2), S3C_GPIO_SFN(1));
		gpio_direction_output(EXYNOS4_GPC0(2), 0);
		s5p_gpio_set_drvstr(EXYNOS4_GPC0(2), S5P_GPIO_DRVSTR_LV4);
		gpio_free(EXYNOS4_GPC0(2));
	}

	/* FM_VD_ON : GPC0_3 */
	if (gpio_request(EXYNOS4_GPC0(3), "SI4704_VD"))
		printk("duv060_si4704_hw_init::EXYNOS4_GPC0(3) failed!\n");
	else
	{
		s3c_gpio_setpull(EXYNOS4_GPC0(3), S3C_GPIO_PULL_NONE);
		s3c_gpio_cfgpin(EXYNOS4_GPC0(3), S3C_GPIO_SFN(1));
		gpio_direction_output(EXYNOS4_GPC0(3), 0);
		s5p_gpio_set_drvstr(EXYNOS4_GPC0(3), S5P_GPIO_DRVSTR_LV4);
		gpio_free(EXYNOS4_GPC0(3));
	}

	/* FM_INT : GPJ0_5 */
	if (gpio_request(EXYNOS4212_GPJ0(5), "SI4704_INTR"))
		printk("duv060_si4704_hw_init::EXYNOS4212_GPJ0(5) failed!\n");
	else	
	{
		s3c_gpio_setpull(EXYNOS4212_GPJ0(5), S3C_GPIO_PULL_NONE);
		s3c_gpio_cfgpin(EXYNOS4212_GPJ0(5), S3C_GPIO_SFN(0));
		s5p_gpio_set_drvstr(EXYNOS4212_GPJ0(5), S5P_GPIO_DRVSTR_LV4);
		gpio_free(EXYNOS4212_GPJ0(5));
	}

	/* FM_nRST : GPC0_4 */
	if (gpio_request(EXYNOS4_GPC0(4), "SI4704_nRST"))
		printk("duv060_si4704_hw_init::EXYNOS4_GPC0(4) failed!\n");
	else
	{
		s3c_gpio_setpull(EXYNOS4_GPC0(4), S3C_GPIO_PULL_NONE);
		s3c_gpio_cfgpin(EXYNOS4_GPC0(4), S3C_GPIO_SFN(1));
		gpio_direction_output(EXYNOS4_GPC0(4), 0);
		s5p_gpio_set_drvstr(EXYNOS4_GPC0(4), S5P_GPIO_DRVSTR_LV4);
		gpio_free(EXYNOS4_GPC0(4));
	}

	/* FM_INT : GPJ0_5 */
/*	
	if (gpio_request(EXYNOS4212_GPJ0(5), "SI4704_INTR"))
		printk("duv060_si4704_hw_init::EXYNOS4212_GPJ0(5) failed!\n");
	else	
	{
		s3c_gpio_setpull(EXYNOS4212_GPJ0(5), S3C_GPIO_PULL_NONE);
		s3c_gpio_cfgpin(EXYNOS4212_GPJ0(5), S3C_GPIO_SFN(0xf));
		irq_set_irq_type(gpio_to_irq(EXYNOS4212_GPJ0(5)), IRQ_TYPE_EDGE_FALLING);
		gpio_free(EXYNOS4212_GPJ0(5));
	}
*/
}
#endif

/* for mainline fimc interface */
#ifdef CONFIG_VIDEO_SAMSUNG_S5P_FIMC
#ifdef WRITEBACK_ENABLED
struct writeback_mbus_platform_data {
	int id;
	struct v4l2_mbus_framefmt fmt;
};

static struct i2c_board_info __initdata writeback_info = {
	I2C_BOARD_INFO("writeback", 0x0),
};
#endif

#ifdef CONFIG_VIDEO_S5K4BA
static struct s5k4ba_mbus_platform_data s5k4ba_mbus_plat = {
	.id		= 0,
	.fmt = {
		.width	= 1600,
		.height	= 1200,
		/*.code	= V4L2_MBUS_FMT_UYVY8_2X8, */
		.code	= V4L2_MBUS_FMT_VYUY8_2X8,
	},
	.clk_rate	= 24000000UL,
#ifdef CONFIG_ITU_A
	.set_power	= smdk4x12_cam0_reset,
#endif
#ifdef CONFIG_ITU_B
	.set_power	= smdk4x12_cam1_reset,
#endif
};

static struct i2c_board_info s5k4ba_info = {
	I2C_BOARD_INFO("S5K4BA", 0x2d),
	.platform_data = &s5k4ba_mbus_plat,
};
#endif

#ifdef CONFIG_VIDEO_EXYNOS_FIMC_IS
#ifdef CONFIG_VIDEO_S5K3H7
static struct i2c_board_info s5k3h7_sensor_info = {
	.type = "S5K3H7",
};
#endif
#ifdef CONFIG_VIDEO_S5K4E5
static struct i2c_board_info s5k4e5_sensor_info = {
	.type = "S5K4E5",
};
#endif
#ifdef CONFIG_VIDEO_S5K6A3
static struct i2c_board_info s5k6a3_sensor_info = {
	.type = "S5K6A3",
};
#endif
#endif
#ifdef CONFIG_VIDEO_EXYNOS_FIMC_LITE
/* This is for platdata of fimc-lite */

#ifdef CONFIG_VIDEO_S5K3H7
static struct s3c_platform_camera s5k3h7 = {
	.type  = CAM_TYPE_MIPI,
	.use_isp = true,
	.inv_pclk = 0,
	.inv_vsync = 0,
	.inv_href = 0,
	.inv_hsync = 0,
};
#endif

#ifdef CONFIG_VIDEO_S5K4E5
static struct s3c_platform_camera s5k4e5 = {
	.type  = CAM_TYPE_MIPI,
	.use_isp = true,
	.inv_pclk = 0,
	.inv_vsync = 0,
	.inv_href = 0,
	.inv_hsync = 0,
};
#endif


#ifdef CONFIG_VIDEO_S5K6A3
static struct s3c_platform_camera s5k6a3 = {
	.type  = CAM_TYPE_MIPI,
	.use_isp = true,
	.inv_pclk = 0,
	.inv_vsync = 0,
	.inv_href = 0,
	.inv_hsync = 0,
};
#endif
#endif
#endif /* CONFIG_VIDEO_SAMSUNG_S5P_FIMC */

#ifdef CONFIG_S3C64XX_DEV_SPI
static struct s3c64xx_spi_csinfo spi0_csi[] = {
	[0] = {
		.line = EXYNOS4_GPB(1),
		.set_level = gpio_set_value,
		.fb_delay = 0x2,
	},
};

static struct spi_board_info spi0_board_info[] __initdata = {
	{
		.modalias = "spidev",
		.platform_data = NULL,
		.max_speed_hz = 10*1000*1000,
		.bus_num = 0,
		.chip_select = 0,
		.mode = SPI_MODE_0,
		.controller_data = &spi0_csi[0],
	}
};

#ifndef CONFIG_FB_S5P_LMS501KF03
static struct s3c64xx_spi_csinfo spi1_csi[] = {
	[0] = {
		.line = EXYNOS4_GPB(5),
		.set_level = gpio_set_value,
		.fb_delay = 0x2,
	},
};

static struct spi_board_info spi1_board_info[] __initdata = {
	{
		.modalias = "spidev",
		.platform_data = NULL,
		.max_speed_hz = 10*1000*1000,
		.bus_num = 1,
		.chip_select = 0,
		.mode = SPI_MODE_3,
		.controller_data = &spi1_csi[0],
	}
};
#endif

static struct s3c64xx_spi_csinfo spi2_csi[] = {
	[0] = {
		.line = EXYNOS4_GPC1(2),
		.set_level = gpio_set_value,
		.fb_delay = 0x2,
	},
};

static struct spi_board_info spi2_board_info[] __initdata = {
	{
		.modalias = "spidev",
		.platform_data = NULL,
		.max_speed_hz = 10*1000*1000,
		.bus_num = 2,
		.chip_select = 0,
		.mode = SPI_MODE_0,
		.controller_data = &spi2_csi[0],
	}
};
#endif

static int exynos4_notifier_call(struct notifier_block *this,
					unsigned long code, void *_cmd)
{
	int mode = 0;

	if ((code == SYS_RESTART) && _cmd){
		if (!strcmp((char *)_cmd, "clean")){
			mode = 0xC;
		}
		if (!strcmp((char *)_cmd, "recovery")){
			mode = 0xf;
		}
		if (!strcmp((char *)_cmd, "update")){
			mode = 0xA;
		}
    }
	__raw_writel(mode, REG_INFORM4);

	return NOTIFY_DONE;
}

static struct notifier_block exynos4_reboot_notifier = {
	.notifier_call = exynos4_notifier_call,
};

#ifdef CONFIG_EXYNOS4_DEV_DWMCI
static void exynos_dwmci_cfg_gpio(int width)
{
	unsigned int gpio;

	for (gpio = EXYNOS4_GPK0(0); gpio < EXYNOS4_GPK0(2); gpio++) {
		s3c_gpio_cfgpin(gpio, S3C_GPIO_SFN(3));
		s3c_gpio_setpull(gpio, S3C_GPIO_PULL_NONE);
		s5p_gpio_set_drvstr(gpio, S5P_GPIO_DRVSTR_LV2);
	}

	switch (width) {
	case 8:
		for (gpio = EXYNOS4_GPK1(3); gpio <= EXYNOS4_GPK1(6); gpio++) {
			s3c_gpio_cfgpin(gpio, S3C_GPIO_SFN(4));
			s3c_gpio_setpull(gpio, S3C_GPIO_PULL_UP);
			s5p_gpio_set_drvstr(gpio, S5P_GPIO_DRVSTR_LV2);
		}
	case 4:
		for (gpio = EXYNOS4_GPK0(3); gpio <= EXYNOS4_GPK0(6); gpio++) {
			s3c_gpio_cfgpin(gpio, S3C_GPIO_SFN(3));
			s3c_gpio_setpull(gpio, S3C_GPIO_PULL_UP);
			s5p_gpio_set_drvstr(gpio, S5P_GPIO_DRVSTR_LV2);
		}
		break;
	case 1:
		gpio = EXYNOS4_GPK0(3);
		s3c_gpio_cfgpin(gpio, S3C_GPIO_SFN(3));
		s3c_gpio_setpull(gpio, S3C_GPIO_PULL_UP);
		s5p_gpio_set_drvstr(gpio, S5P_GPIO_DRVSTR_LV2);
	default:
		break;
	}
}

static struct dw_mci_board exynos_dwmci_pdata __initdata = {
	.num_slots		= 1,
	.quirks			= DW_MCI_QUIRK_BROKEN_CARD_DETECTION | DW_MCI_QUIRK_HIGHSPEED,
	.bus_hz			= 100 * 1000 * 1000,
	.caps			= MMC_CAP_UHS_DDR50 | MMC_CAP_1_8V_DDR |
				MMC_CAP_8_BIT_DATA | MMC_CAP_CMD23,
	.fifo_depth		= 0x80,
	.detect_delay_ms	= 200,
	.hclk_name		= "dwmci",
	.cclk_name		= "sclk_dwmci",
	.cfg_gpio		= exynos_dwmci_cfg_gpio,
};
#endif

#ifdef CONFIG_S3C_DEV_HSMMC
static struct s3c_sdhci_platdata smdk4x12_hsmmc0_pdata __initdata = {
	.cd_type		= S3C_SDHCI_CD_INTERNAL,
	.clk_type		= S3C_SDHCI_CLK_DIV_EXTERNAL,
#ifdef CONFIG_EXYNOS4_SDHCI_CH0_8BIT
	.max_width		= 8,
	.host_caps		= MMC_CAP_8_BIT_DATA,
#endif
};
#endif

#ifdef CONFIG_S3C_DEV_HSMMC1
static struct s3c_sdhci_platdata smdk4x12_hsmmc1_pdata __initdata = {
	.cd_type		= S3C_SDHCI_CD_INTERNAL,
	.clk_type		= S3C_SDHCI_CLK_DIV_EXTERNAL,
};
#endif

#ifdef CONFIG_S3C_DEV_HSMMC2
static struct s3c_sdhci_platdata smdk4x12_hsmmc2_pdata __initdata = {
	.cd_type		= S3C_SDHCI_CD_PERMANENT,
	.clk_type		= S3C_SDHCI_CLK_DIV_EXTERNAL,
#ifdef CONFIG_EXYNOS4_SDHCI_CH2_8BIT
	.max_width		= 8,
	.host_caps		= MMC_CAP_8_BIT_DATA,
#endif
};
#endif

#ifdef CONFIG_S3C_DEV_HSMMC3
static struct s3c_sdhci_platdata smdk4x12_hsmmc3_pdata __initdata = {
	.cd_type		= S3C_SDHCI_CD_EXTERNAL,
	.clk_type		= S3C_SDHCI_CLK_DIV_EXTERNAL,
};
#endif

#ifdef CONFIG_S5P_DEV_MSHC
static struct s3c_mshci_platdata exynos4_mshc_pdata __initdata = {
	.cd_type		= S3C_MSHCI_CD_PERMANENT,
	.has_wp_gpio		= true,
	.wp_gpio		= 0xffffffff,
#if defined(CONFIG_EXYNOS4_MSHC_8BIT) && \
	defined(CONFIG_EXYNOS4_MSHC_DDR)
	.max_width		= 8,
	.host_caps		= MMC_CAP_8_BIT_DATA | MMC_CAP_1_8V_DDR |
				  MMC_CAP_UHS_DDR50,
#elif defined(CONFIG_EXYNOS4_MSHC_8BIT)
	.max_width		= 8,
	.host_caps		= MMC_CAP_8_BIT_DATA,
#elif defined(CONFIG_EXYNOS4_MSHC_DDR)
	.host_caps		= MMC_CAP_1_8V_DDR | MMC_CAP_UHS_DDR50,
#endif
};
#endif

#ifdef CONFIG_USB_EHCI_S5P
static struct s5p_ehci_platdata smdk4x12_ehci_pdata;

static void __init smdk4x12_ehci_init(void)
{
	struct s5p_ehci_platdata *pdata = &smdk4x12_ehci_pdata;

	s5p_ehci_set_platdata(pdata);
}
#endif

#if defined(CONFIG_USB_S5P_HSIC0) || defined(CONFIG_USB_S5P_HSIC1)
/* USB3503A, HSIC1 -> USB Host */
#define HSIC_RESET EXYNOS4_GPL0(2)
#define HSIC_HUB_INT EXYNOS4_GPX3(1)
void hsic_hub_init()
{
	if (gpio_request(HSIC_HUB_INT, "HSIC_HUB_INT"))     {
		printk("%s :HSIC_HUB_INT request port error!\n", __FUNCTION__);
	} else {
		s3c_gpio_cfgpin(HSIC_HUB_INT, S3C_GPIO_SFN(2));
		s3c_gpio_setpull(HSIC_HUB_INT, S3C_GPIO_PULL_NONE);
		gpio_free(HSIC_HUB_INT);
		printk("%s :HSIC_HUB_INT request port OK!\n", __FUNCTION__);
	}

	if (gpio_request(HSIC_RESET, "HSIC_RESET"))     {
		printk("%s :HSIC_RESET request port error!\n", __FUNCTION__);
	} else {
		s3c_gpio_cfgpin(HSIC_RESET, S3C_GPIO_OUTPUT);
		s3c_gpio_setpull(HSIC_RESET, S3C_GPIO_PULL_NONE);
		gpio_direction_output(HSIC_RESET, 1);
		mdelay(2);
		gpio_direction_output(HSIC_RESET, 0);
		mdelay(2);
		gpio_direction_output(HSIC_RESET, 1);
		gpio_free(HSIC_RESET);
		printk("%s :HSIC_RESET request port OK!\n", __FUNCTION__);
	}

	// Follow SMSC's suggestion delay 100ms to let HUB stable
	// mdelay(100);
}

void hsic_hub_exit()
{
	// Suspend HSIC
	if (gpio_request(HSIC_RESET, "HSIC_RESET"))     {
		printk("%s :HSIC_RESET request port error!\n", __FUNCTION__);
	} else {
		s3c_gpio_cfgpin(HSIC_RESET, S3C_GPIO_OUTPUT);
		s3c_gpio_setpull(HSIC_RESET, S3C_GPIO_PULL_NONE);
		gpio_direction_output(HSIC_RESET, 0);
		gpio_free(HSIC_RESET);
		printk("%s :HSIC_RESET request port OK!\n", __FUNCTION__);
	}

	if (gpio_request(HSIC_HUB_INT, "HSIC_HUB_INT"))     {
		printk("%s :HSIC_HUB_INT request port error!\n", __FUNCTION__);
	} else {
		s3c_gpio_cfgpin(HSIC_HUB_INT, S3C_GPIO_SFN(0));
		s3c_gpio_setpull(HSIC_HUB_INT, S3C_GPIO_PULL_NONE);
		gpio_free(HSIC_HUB_INT);
		printk("%s :HSIC_HUB_INT request port OK!\n", __FUNCTION__);
	}
}
#endif

#ifdef CONFIG_USB_OHCI_S5P
static struct s5p_ohci_platdata smdk4x12_ohci_pdata;

static void __init smdk4x12_ohci_init(void)
{
	struct s5p_ohci_platdata *pdata = &smdk4x12_ohci_pdata;

	s5p_ohci_set_platdata(pdata);
}
#endif

/* USB GADGET */
#ifdef CONFIG_USB_GADGET
static struct s5p_usbgadget_platdata smdk4x12_usbgadget_pdata;

static void __init smdk4x12_usbgadget_init(void)
{
	struct s5p_usbgadget_platdata *pdata = &smdk4x12_usbgadget_pdata;

	s5p_usbgadget_set_platdata(pdata);
}
#endif

#ifdef CONFIG_SND_SOC_WM8994

static struct regulator_consumer_supply wm8994_fixed_voltage0_supplies[] = {
	REGULATOR_SUPPLY("AVDD2", NULL),
	REGULATOR_SUPPLY("CPVDD", NULL),
	REGULATOR_SUPPLY("DBVDD", NULL),
};

static struct regulator_consumer_supply wm8994_fixed_voltage1_supplies[] = {
	REGULATOR_SUPPLY("LDO1VDD", NULL),
	REGULATOR_SUPPLY("SPKVDD1", NULL),
	REGULATOR_SUPPLY("SPKVDD2", NULL),
};

static struct regulator_init_data wm8994_fixed_voltage0_init_data = {
	.constraints = {
		.always_on = 1,
	},
	.num_consumer_supplies	= ARRAY_SIZE(wm8994_fixed_voltage0_supplies),
	.consumer_supplies	= wm8994_fixed_voltage0_supplies,
};

static struct regulator_init_data wm8994_fixed_voltage1_init_data = {
	.constraints = {
		.always_on = 1,
	},
	.num_consumer_supplies	= ARRAY_SIZE(wm8994_fixed_voltage1_supplies),
	.consumer_supplies	= wm8994_fixed_voltage1_supplies,
};


static struct fixed_voltage_config wm8994_fixed_voltage0_config = {
	.supply_name	= "VDD_1.8V",
	.microvolts	= 1800000,
	.gpio		= -EINVAL,
	.init_data	= &wm8994_fixed_voltage0_init_data,
};

static struct fixed_voltage_config wm8994_fixed_voltage1_config = {
	.supply_name	= "VINC",
	.microvolts	= 5000000,
	.gpio		= EXYNOS4_GPK1(1),
	.enable_high = 1,
	.enabled_at_boot = 1,
	.init_data	= &wm8994_fixed_voltage1_init_data,
};

static struct platform_device wm8994_fixed_voltage0 = {
	.name		= "reg-fixed-voltage",
	.id		= 0,
	.dev		= {
		.platform_data	= &wm8994_fixed_voltage0_config,
	},
};

static struct platform_device wm8994_fixed_voltage1 = {
	.name		= "reg-fixed-voltage",
	.id		= 1,
	.dev		= {
		.platform_data	= &wm8994_fixed_voltage1_config,
	},
};

static struct regulator_consumer_supply wm8994_avdd1_supply =
	REGULATOR_SUPPLY("AVDD1", NULL);

static struct regulator_consumer_supply wm8994_dcvdd_supply =
	REGULATOR_SUPPLY("DCVDD", NULL);

static struct regulator_init_data wm8994_ldo1_data = {
	.constraints	= {
		.name		= "AVDD1",
		.valid_ops_mask = REGULATOR_CHANGE_STATUS,
	},
	.num_consumer_supplies	= 1,
	.consumer_supplies	= &wm8994_avdd1_supply,
};

static struct regulator_init_data wm8994_ldo2_data = {
	.constraints	= {
		.name		= "DCVDD",
	},
	.num_consumer_supplies	= 1,
	.consumer_supplies	= &wm8994_dcvdd_supply,
};

static struct wm8994_pdata wm8994_platform_data = {
	/* The enable is shared but assign it to LDO1 for software */
	.ldo[0] = { EXYNOS4_GPK1(2), NULL, &wm8994_ldo1_data },
	.ldo[1] = { 0, NULL, &wm8994_ldo2_data },
};

#endif

/* I2C devs mapping: for duv060
  * i2c0 : MAX77686
  * i2c1 : WM8994, HSIC HUB
  * i2c2 : pixtree secure ic
  * i2c3 : non-use
  * i2c4 : sensors
  * i2c5 : camera, LED driver
  * i2c6 : pixcur touch ic
  * i2c7 : HDMI
*/
//static struct s3c2410_platform_i2c i2c0_data __initdata = {
//	.flags		= 0,
//	.slave_addr	= 0x10,
//	.frequency	= 100*1000,
//	.sda_delay	= 100,
//};

static struct i2c_board_info i2c_devs0[] __initdata = {
	{
		I2C_BOARD_INFO("max77686", (0x12 >> 1)),
		.platform_data	= &max77686_pdata,
	},
};

/* I2C1 */
static struct i2c_board_info i2c_devs1[] __initdata = {
#ifdef CONFIG_SND_SOC_WM8994
	{
		I2C_BOARD_INFO("wm8994", (0x34 >> 1)),
		.platform_data	= &wm8994_platform_data,
	},
#endif
};

/* I2C2 */
static struct i2c_board_info i2c_devs2[] __initdata = {
#ifdef CONFIG_BATTERY_DLI
	{
		I2C_BOARD_INFO("dli-battery", (0x58 >> 1)),
	}
#endif
};

#ifdef CONFIG_S3C_DEV_I2C3
/* I2C3 */
static struct i2c_board_info i2c_devs3[] __initdata = {

};
#endif

#ifdef CONFIG_SENSORS_AS3039S07
static void as3039s07_hw_power(int on)
{
	if (on)
	{
		/* LIGHT_ON : GPM4_4 */
		if (gpio_request(EXYNOS4212_GPM4(4), "LIGHT_ON"))
			printk("as3039s07_hw_power::EXYNOS4212_GPM4(4) failed!\n");
		else
		{
			s3c_gpio_setpull(EXYNOS4212_GPM4(4), S3C_GPIO_PULL_NONE);
			s3c_gpio_cfgpin(EXYNOS4212_GPM4(4), S3C_GPIO_SFN(1));
			gpio_direction_output(EXYNOS4212_GPM4(4), 1);
			s5p_gpio_set_drvstr(EXYNOS4212_GPM4(4), S5P_GPIO_DRVSTR_LV4);
			gpio_free(EXYNOS4212_GPM4(4));
		}
	}
	else
	{
		/* LIGHT_ON : GPM4_4 */
		if (gpio_request(EXYNOS4212_GPM4(4), "LIGHT_ON"))
			printk("as3039s07_hw_power::EXYNOS4212_GPM4(4) failed!\n");
		else
		{
			s3c_gpio_setpull(EXYNOS4212_GPM4(4), S3C_GPIO_PULL_NONE);
			s3c_gpio_cfgpin(EXYNOS4212_GPM4(4), S3C_GPIO_SFN(1));
			gpio_direction_output(EXYNOS4212_GPM4(4), 0);
			s5p_gpio_set_drvstr(EXYNOS4212_GPM4(4), S5P_GPIO_DRVSTR_LV4);
			gpio_free(EXYNOS4212_GPM4(4));
		}
	}		
}

static struct as3039s07_pdata as3039s07_pdata = {
	.timeout = 1000, 
	.power = as3039s07_hw_power, 
};
#endif

#ifdef CONFIG_SENSORS_BH1750FVI
static void bh1750fvi_hw_power(int on)
{
	if (on)
	{
		/* LIGHT_ON : GPM4_4 */
		if (gpio_request(EXYNOS4212_GPM4(4), "LIGHT_ON"))
			printk("bh1750fvi_hw_power::EXYNOS4212_GPM4(4) failed!\n");
		else
		{
			s3c_gpio_setpull(EXYNOS4212_GPM4(4), S3C_GPIO_PULL_NONE);
			s3c_gpio_cfgpin(EXYNOS4212_GPM4(4), S3C_GPIO_SFN(1));
			gpio_direction_output(EXYNOS4212_GPM4(4), 1);
			s5p_gpio_set_drvstr(EXYNOS4212_GPM4(4), S5P_GPIO_DRVSTR_LV4);
			gpio_free(EXYNOS4212_GPM4(4));
		}
	}
	else
	{
		/* LIGHT_ON : GPM4_4 */
		if (gpio_request(EXYNOS4212_GPM4(4), "LIGHT_ON"))
			printk("bh1750fvi_hw_power::EXYNOS4212_GPM4(4) failed!\n");
		else
		{
			s3c_gpio_setpull(EXYNOS4212_GPM4(4), S3C_GPIO_PULL_NONE);
			s3c_gpio_cfgpin(EXYNOS4212_GPM4(4), S3C_GPIO_SFN(1));
			gpio_direction_output(EXYNOS4212_GPM4(4), 0);
			s5p_gpio_set_drvstr(EXYNOS4212_GPM4(4), S5P_GPIO_DRVSTR_LV4);
			gpio_free(EXYNOS4212_GPM4(4));
		}
	}		
}

static struct bh1750fvi_pdata bh1750fvi_pdata = {
	.timeout = 1000, 
	.power = bh1750fvi_hw_power, 
};
#endif

/* I2C4 */
static struct i2c_board_info i2c_devs4[] __initdata = {
#ifdef CONFIG_SENSORS_BH1750FVI
	{
/*		I2C_BOARD_INFO("bh1750_i2c", 0xB8 >> 1), */	/* ADDR = H */
		I2C_BOARD_INFO("bh1750_i2c", 0x46 >> 1),	/* ADDR = L */
		.platform_data = &bh1750fvi_pdata, 
	}, 
#endif
#ifdef CONFIG_SENSORS_AS3039S07
	{
/*		I2C_BOARD_INFO("as3039s07_i2c", 0x88 >> 1), */	/* ADDR = H */
		I2C_BOARD_INFO("as3039s07_i2c", 0x52 >> 1),		/* ADDR = L */
		.platform_data = &as3039s07_pdata, 
	}, 
#endif
#ifdef CONFIG_MPU_SENSORS_MPU6050B1
	{
		I2C_BOARD_INFO("mpu6050", 0x68),
		.platform_data = &inv_mpu_data,
		.irq = EXYNOS4_GPX1(5),
	},
	{
		I2C_BOARD_INFO("ak8975", 0x0F),
		.platform_data = &inv_mpu_compass_data,
		.irq = EXYNOS4_GPX1(4),
	},
#endif 
	/*{
                I2C_BOARD_INFO("as3643", 0x30),
                .platform_data = &as3643_plat,
        },*/
	{
                I2C_BOARD_INFO("as3643", 0x30),
                .platform_data = &duv060_as3643_data,
        },
};

/* I2C5 */
static struct i2c_board_info i2c_devs5[] __initdata = {
};

/* I2C6 */
static struct i2c_board_info i2c_devs6[] __initdata = {
#ifdef CONFIG_TOUCHSCREEN_FT5x06
	{
		I2C_BOARD_INFO("ft5x06_ts", 0x38),
		.irq = TS_IRQ,
	}
#endif
#ifdef CONFIG_TOUCHSCREEN_HIMAX
	{
		I2C_BOARD_INFO("himax_ts", (0x94 >> 1)),
		.irq = TS_IRQ,
	}
#endif
};

#if defined (CONFIG_I2C_SI4704)

static void si4704_hw_init(void)
{
	duv060_si4704_hw_init();
}

static void si4704_hw_reset(int enable)
{
	if (enable)
	{
		/* FM_nRST : GPC0_4 */
		if (gpio_request(EXYNOS4_GPC0(4), "SI4704_nRST"))
			printk("si4704_hw_reset::EXYNOS4_GPC0(4) failed!\n");
		else
		{
			s3c_gpio_setpull(EXYNOS4_GPC0(4), S3C_GPIO_PULL_NONE);
			s3c_gpio_cfgpin(EXYNOS4_GPC0(4), S3C_GPIO_SFN(1));
			gpio_direction_output(EXYNOS4_GPC0(4), 1);
			s5p_gpio_set_drvstr(EXYNOS4_GPC0(4), S5P_GPIO_DRVSTR_LV4);
			gpio_free(EXYNOS4_GPC0(4));
		}
	}
	else
	{
		/* FM_nRST : GPC0_4 */
		if (gpio_request(EXYNOS4_GPC0(4), "SI4704_nRST"))
			printk("si4704_hw_reset::EXYNOS4_GPC0(4) failed!\n");
		else
		{
			s3c_gpio_setpull(EXYNOS4_GPC0(4), S3C_GPIO_PULL_NONE);
			s3c_gpio_cfgpin(EXYNOS4_GPC0(4), S3C_GPIO_SFN(1));
			gpio_direction_output(EXYNOS4_GPC0(4), 0);
			s5p_gpio_set_drvstr(EXYNOS4_GPC0(4), S5P_GPIO_DRVSTR_LV4);
			gpio_free(EXYNOS4_GPC0(4));
		}
	}
}

static void si4704_hw_power(int on)
{
	if (on)
	{
		/* FM_VA_ON : GPC0_2 */
		if (gpio_request(EXYNOS4_GPC0(2), "SI4704_VA"))
			printk("si4704_hw_power::EXYNOS4_GPC0(2) failed!\n");
		else
		{
			s3c_gpio_setpull(EXYNOS4_GPC0(2), S3C_GPIO_PULL_NONE);
			s3c_gpio_cfgpin(EXYNOS4_GPC0(2), S3C_GPIO_SFN(1));
			gpio_direction_output(EXYNOS4_GPC0(2), 1);
			s5p_gpio_set_drvstr(EXYNOS4_GPC0(2), S5P_GPIO_DRVSTR_LV4);
			gpio_free(EXYNOS4_GPC0(2));
		}

		/* FM_VD_ON : GPC0_3 */
		if (gpio_request(EXYNOS4_GPC0(3), "SI4704_VD"))
			printk("si4704_hw_power::EXYNOS4_GPC0(3) failed!\n");
		else
		{
			s3c_gpio_setpull(EXYNOS4_GPC0(3), S3C_GPIO_PULL_NONE);
			s3c_gpio_cfgpin(EXYNOS4_GPC0(3), S3C_GPIO_SFN(1));
			gpio_direction_output(EXYNOS4_GPC0(3), 1);
			s5p_gpio_set_drvstr(EXYNOS4_GPC0(3), S5P_GPIO_DRVSTR_LV4);
			gpio_free(EXYNOS4_GPC0(3));
		}
	}
	else
	{
		/* FM_VA_ON : GPC0_2 */
		if (gpio_request(EXYNOS4_GPC0(2), "SI4704_VA"))
			printk("si4704_hw_power::EXYNOS4_GPC0(2) failed!\n");
		else
		{
			s3c_gpio_setpull(EXYNOS4_GPC0(2), S3C_GPIO_PULL_NONE);
			s3c_gpio_cfgpin(EXYNOS4_GPC0(2), S3C_GPIO_SFN(1));
			gpio_direction_output(EXYNOS4_GPC0(2), 0);
			s5p_gpio_set_drvstr(EXYNOS4_GPC0(2), S5P_GPIO_DRVSTR_LV4);
			gpio_free(EXYNOS4_GPC0(2));
		}

		/* FM_VD_ON : GPC0_3 */
		if (gpio_request(EXYNOS4_GPC0(3), "SI4704_VD"))
			printk("si4704_hw_power::EXYNOS4_GPC0(3) failed!\n");
		else
		{
			s3c_gpio_setpull(EXYNOS4_GPC0(3), S3C_GPIO_PULL_NONE);
			s3c_gpio_cfgpin(EXYNOS4_GPC0(3), S3C_GPIO_SFN(1));
			gpio_direction_output(EXYNOS4_GPC0(3), 0);
			s5p_gpio_set_drvstr(EXYNOS4_GPC0(3), S5P_GPIO_DRVSTR_LV4);
			gpio_free(EXYNOS4_GPC0(3));
		}
	}
}

static void si4704_hw_intr(int enable)
{
	if (enable)
	{
		/* FM_INT : GPJ0_5 */
		if (gpio_request(EXYNOS4212_GPJ0(5), "SI4704_INTR"))
			printk("si4704_hw_intr::EXYNOS4212_GPJ0(5) failed!\n");
		else	
		{
			s3c_gpio_setpull(EXYNOS4212_GPJ0(5), S3C_GPIO_PULL_NONE);
			s3c_gpio_cfgpin(EXYNOS4212_GPJ0(5), S3C_GPIO_SFN(0xf));
			irq_set_irq_type(gpio_to_irq(EXYNOS4212_GPJ0(5)), IRQ_TYPE_EDGE_FALLING);
			gpio_free(EXYNOS4212_GPJ0(5));
		}
	}
	else
	{
		/* FM_INT : GPJ0_5 */
		if (gpio_request(EXYNOS4212_GPJ0(5), "SI4704_INTR"))
			printk("si4704_hw_intr::EXYNOS4212_GPJ0(5) failed!\n");
		else	
		{
			s3c_gpio_setpull(EXYNOS4212_GPJ0(5), S3C_GPIO_PULL_NONE);
			s3c_gpio_cfgpin(EXYNOS4212_GPJ0(5), S3C_GPIO_SFN(0));
			gpio_free(EXYNOS4212_GPJ0(5));
		}
	}
}

static struct si4704_pdata si4704_pdata = {
	.init = si4704_hw_init, 
	.reset = si4704_hw_reset, 
	.power = si4704_hw_power, 
	.intr = si4704_hw_intr, 
	.pdata_values = (1 << 0) | (1 << 1) | (1 << 2) | (1 << 3) | (1 << 4) | (1 << 5) | (1 << 6) | (1 << 7) | (1 << 8) | (1 << 9), 
	.rx_vol_steps = 11, 
	.rx_vol_table = {0x0000, 0x0007, 0x000E, 0x0015, 0x001B, 0x0021, 0x0027, 0x002F, 0x0033, 0x0039, 0x003F}, 
#ifdef CONFIG_I2C_SI4704_TESMODE
	.freq_min = 7200, 
	.freq_max = 10800, 
#else
	.freq_min = 7200, 
	.freq_max = 7600, 
#endif
	.de = 2,								/* 1, 50us, Europe, Australia, Japan; 2, 75 us, South Korea, USA, Taiwan */
/*	.rx_seek_tune_rssi_threshold = 0x20, */	/* 0x00~0x7f */
/*	.rx_seek_tune_snr_threshold = 0x10, */	/* 0x00~0x7f */
	.rx_seek_tune_rssi_threshold = 0x14, 	/* 0x00~0x7f */
	.rx_seek_tune_snr_threshold = 0x03, 	/* 0x00~0x7f */
	.blend_rssi_stereo_threshold = 0x8, 	/* 0x00~0x7f */
	.blend_rssi_mono_threshold = 0x5, 		/* 0x00~0x7f */
	.blend_snr_stereo_threshold = 0x5, 	/* 0x00~0x7f */
	.blend_snr_mono_threshold = 0x3, 		/* 0x00~0x7f */
	.blend_multi_stereo_threshold = 0xa, 	/* 0x00~0x64 */
	.blend_multi_mono_threshold = 0x5A, 	/* 0x00~0x64 */
};
#endif

static struct i2c_board_info i2c_devs7[] __initdata = {
#if defined (CONFIG_I2C_SI4704)
	{
		I2C_BOARD_INFO("si4704", 0x22 >> 1),
		.irq = EXYNOS4212_GPJ0(5), 				/* FM_INT : GPJ0_5 */
		.platform_data = &si4704_pdata, 
	}, 
#endif
#ifdef CONFIG_VIDEO_TVOUT
	{
		I2C_BOARD_INFO("s5p_ddc", (0x74 >> 1)),
	},
#endif
};

#ifdef CONFIG_BATTERY_SAMSUNG
static struct platform_device samsung_device_battery = {
	.name	= "samsung-fake-battery",
	.id	= -1,
};
#endif

extern void __init duv060_gpio_key_init(void);
extern struct platform_device duv060_gpio_key_device;

#ifdef CONFIG_WAKEUP_ASSIST
static struct platform_device wakeup_assist_device = {
	.name   = "wakeup_assist",
};
#endif

#ifdef CONFIG_VIDEO_FIMG2D
static struct fimg2d_platdata fimg2d_data __initdata = {
	.hw_ver = 0x41,
	.parent_clkname = "mout_g2d0",
	.clkname = "sclk_fimg2d",
	.gate_clkname = "fimg2d",
	.clkrate = 201 * 1000000,	/* 200 Mhz */
};
#endif

#ifdef CONFIG_EXYNOS_C2C
struct exynos_c2c_platdata smdk4x12_c2c_pdata = {
	.setup_gpio	= NULL,
	.shdmem_addr	= C2C_SHAREDMEM_BASE,
	.shdmem_size	= C2C_MEMSIZE_64,
	.ap_sscm_addr	= NULL,
	.cp_sscm_addr	= NULL,
	.rx_width	= C2C_BUSWIDTH_16,
	.tx_width	= C2C_BUSWIDTH_16,
	.clk_opp100	= 400,
	.clk_opp50	= 266,
	.clk_opp25	= 0,
	.default_opp_mode	= C2C_OPP50,
	.get_c2c_state	= NULL,
	.c2c_sysreg	= S5P_VA_CMU + 0x12000,
};
#endif

#ifdef CONFIG_USB_EXYNOS_SWITCH
static struct s5p_usbswitch_platdata smdk4x12_usbswitch_pdata;

static void __init smdk4x12_usbswitch_init(void)
{
	struct s5p_usbswitch_platdata *pdata = &smdk4x12_usbswitch_pdata;
	int err;

	pdata->gpio_host_detect = EXYNOS4_GPX3(7); /* low active */
	err = gpio_request_one(pdata->gpio_host_detect, GPIOF_IN, "HOST_DETECT");
	if (err) {
		printk(KERN_ERR "failed to request gpio_host_detect\n");
		return;
	}

	s3c_gpio_cfgpin(pdata->gpio_host_detect, S3C_GPIO_SFN(0xF));
	s3c_gpio_setpull(pdata->gpio_host_detect, S3C_GPIO_PULL_UP);
	gpio_free(pdata->gpio_host_detect);

	pdata->gpio_device_detect = EXYNOS4_GPX3(6); /* low active */
	err = gpio_request_one(pdata->gpio_device_detect, GPIOF_IN, "DEVICE_DETECT");
	if (err) {
		printk(KERN_ERR "failed to request gpio_host_detect for\n");
		return;
	}

	s3c_gpio_cfgpin(pdata->gpio_device_detect, S3C_GPIO_SFN(0xF));
	s3c_gpio_setpull(pdata->gpio_device_detect, S3C_GPIO_PULL_UP);
	gpio_free(pdata->gpio_device_detect);

#ifndef CONFIG_BOARD_DUV060
	if (samsung_board_rev_is_0_0())
		pdata->gpio_host_vbus = 0;
	else {
		pdata->gpio_host_vbus = EXYNOS4_GPL2(0);
		err = gpio_request_one(pdata->gpio_host_vbus, GPIOF_OUT_INIT_LOW, "HOST_VBUS_CONTROL");
		if (err) {
			printk(KERN_ERR "failed to request gpio_host_vbus\n");
			return;
		}

		s3c_gpio_setpull(pdata->gpio_host_vbus, S3C_GPIO_PULL_NONE);
		gpio_free(pdata->gpio_host_vbus);
	}
#endif

	s5p_usbswitch_set_platdata(pdata);
}
#endif

#ifdef CONFIG_BUSFREQ_OPP
/* BUSFREQ to control memory/bus*/
static struct device_domain busfreq;
#endif

static struct platform_device exynos4_busfreq = {
	.id = -1,
	.name = "exynos-busfreq",
};

#ifdef CONFIG_DUV060_UBLOX_GPS_POWER

struct platform_device ublox_gps_power = {
	.name             = "ublox-gps-power",
	.id               = -1,
};

#endif

#ifdef CONFIG_DUV060_CINTERION_PH8_POWER

struct platform_device ph8_power = {
	.name             = "ph8-power",
	.id               = -1,
};

#endif

#ifdef CONFIG_DUV060_MTK_COMBO_WIFI_BT_POWER

struct platform_device mtk_combo_wb_power = {
	.name             = "mtk-combo-wb-power",
	.id               = -1,
};

#endif

#ifdef CONFIG_BATTERY_DLI

struct platform_device dli_battery_leds = {
	.name             = "dli-battery-leds",
	.id               = -1,
};

#endif
#if defined CONFIG_AH397_WB_MODULE
static void __init duv060_aw_ah397_init(void)
{
	printk(KERN_WARNING "duv060_aw_ah397_init!!!\n");
	
	/* WB_3V3USB_ON : GPJ0_6 */
	if (gpio_request(EXYNOS4212_GPJ0(6), "WB_3V3USB_ON"))
		printk("duv060_aw_ah397_init::EXYNOS4212_GPJ0(6) failed!\n");
	else	
	{
		s3c_gpio_setpull(EXYNOS4212_GPJ0(6), S3C_GPIO_PULL_NONE);
		s3c_gpio_cfgpin(EXYNOS4212_GPJ0(6), S3C_GPIO_SFN(1));
		gpio_direction_output(EXYNOS4212_GPJ0(6), 1);
		gpio_free(EXYNOS4212_GPJ0(6));
	}
	
	/* WB_VIO_ON : GPM3_4 */
	if (gpio_request(EXYNOS4212_GPM3(4), "WB_VIO_ON"))
		printk("duv060_aw_ah397_init::EXYNOS4212_GPM3(4) failed!\n");
	else	
	{
		s3c_gpio_setpull(EXYNOS4212_GPM3(4), S3C_GPIO_PULL_NONE);
		s3c_gpio_cfgpin(EXYNOS4212_GPM3(4), S3C_GPIO_SFN(1));
		gpio_direction_output(EXYNOS4212_GPM3(4), 1);
		gpio_free(EXYNOS4212_GPM3(4));
	}
	
	/* WB_RF_ON : GPM3_5 */
	if (gpio_request(EXYNOS4212_GPM3(5), "WB_RF_ON"))
		printk("duv060_aw_ah397_init::EXYNOS4212_GPM3(5) failed!\n");
	else	
	{
		s3c_gpio_setpull(EXYNOS4212_GPM3(5), S3C_GPIO_PULL_NONE);
		s3c_gpio_cfgpin(EXYNOS4212_GPM3(5), S3C_GPIO_SFN(1));
		gpio_direction_output(EXYNOS4212_GPM3(5), 1);
		gpio_free(EXYNOS4212_GPM3(5));
	}
	
	/* WB_1V8_ON : GPM3_6 */
	if (gpio_request(EXYNOS4212_GPM3(6), "WB_1V8_ON"))
		printk("duv060_aw_ah397_init::EXYNOS4212_GPM3(6) failed!\n");
	else	
	{
		s3c_gpio_setpull(EXYNOS4212_GPM3(6), S3C_GPIO_PULL_NONE);
		s3c_gpio_cfgpin(EXYNOS4212_GPM3(6), S3C_GPIO_SFN(1));
		gpio_direction_output(EXYNOS4212_GPM3(6), 1);
		gpio_free(EXYNOS4212_GPM3(6));
	}
	
	/* AH_nRST : GPM4_5 */
	if (gpio_request(EXYNOS4212_GPM4(5), "AH_nRST"))
		printk("duv060_aw_ah397_init::EXYNOS4212_GPM4(5) failed!\n");
	else	
	{
		s3c_gpio_setpull(EXYNOS4212_GPM4(5), S3C_GPIO_PULL_NONE);
		s3c_gpio_cfgpin(EXYNOS4212_GPM4(5), S3C_GPIO_SFN(1));
		gpio_direction_output(EXYNOS4212_GPM4(5), 0);
		mdelay(50);
		gpio_direction_output(EXYNOS4212_GPM4(5), 1);
		gpio_free(EXYNOS4212_GPM4(5));
	}
	
	/* AH_nPD : GPM4_6 */
	if (gpio_request(EXYNOS4212_GPM4(6), "AH_nPD"))
		printk("duv060_aw_ah397_init::EXYNOS4212_GPM4(6) failed!\n");
	else	
	{
		s3c_gpio_setpull(EXYNOS4212_GPM4(6), S3C_GPIO_PULL_NONE);
		s3c_gpio_cfgpin(EXYNOS4212_GPM4(6), S3C_GPIO_SFN(1));
		gpio_direction_output(EXYNOS4212_GPM4(6), 1);
		gpio_free(EXYNOS4212_GPM4(6));
	}
}
#endif

static struct platform_device *smdk4412_devices[] __initdata = {
	&s3c_device_adc,
};

static struct platform_device *smdk4x12_devices[] __initdata = {
	/* Samsung Power Domain */
	&exynos4_device_pd[PD_MFC],
	&exynos4_device_pd[PD_G3D],
	&exynos4_device_pd[PD_LCD0],
	&exynos4_device_pd[PD_CAM],
	&exynos4_device_pd[PD_TV],
	&exynos4_device_pd[PD_GPS],
	&exynos4_device_pd[PD_GPS_ALIVE],
#ifdef CONFIG_VIDEO_EXYNOS_FIMC_IS
	&exynos4_device_pd[PD_ISP],
#endif
#ifdef CONFIG_FB_MIPI_DSIM
	&s5p_device_mipi_dsim,
#endif
/* mainline fimd */
	/* legacy fimd */
#ifdef CONFIG_FB_S5P
	&s3c_device_fb,
#endif
	&s3c_device_wdt,
	&s3c_device_rtc,
	&s3c_device_i2c0,
#ifdef CONFIG_S3C_DEV_I2C1
	&s3c_device_i2c1,
#endif
#ifdef CONFIG_S3C_DEV_I2C2
	&s3c_device_i2c2,
#endif
#ifdef CONFIG_S3C_DEV_I2C3
	&s3c_device_i2c3,
#endif
#ifdef CONFIG_S3C_DEV_I2C4
	&s3c_device_i2c4,
#endif
#ifdef CONFIG_S3C_DEV_I2C5
	&s3c_device_i2c5,
#endif
#ifdef CONFIG_S3C_DEV_I2C6
	&s3c_device_i2c6,
#endif
#ifdef CONFIG_S3C_DEV_I2C7
	&s3c_device_i2c7,
#endif
#ifdef CONFIG_USB_EHCI_S5P
	&s5p_device_ehci,
#endif
#ifdef CONFIG_USB_OHCI_S5P
	&s5p_device_ohci,
#endif
#ifdef CONFIG_USB_GADGET
	&s3c_device_usbgadget,
#endif
#ifdef CONFIG_USB_ANDROID_RNDIS
	&s3c_device_rndis,
#endif
#ifdef CONFIG_USB_ANDROID
	&s3c_device_android_usb,
	&s3c_device_usb_mass_storage,
#endif
#ifdef CONFIG_S3C_DEV_HSMMC
	&s3c_device_hsmmc0,
#endif
#ifdef CONFIG_S3C_DEV_HSMMC1
	&s3c_device_hsmmc1,
#endif
#ifdef CONFIG_S3C_DEV_HSMMC2
	&s3c_device_hsmmc2,
#endif
#ifdef CONFIG_S3C_DEV_HSMMC3
	&s3c_device_hsmmc3,
#endif
#ifdef CONFIG_S5P_DEV_MSHC
	&s3c_device_mshci,
#endif
#ifdef CONFIG_EXYNOS4_DEV_DWMCI
	&exynos_device_dwmci,
#endif
#ifdef CONFIG_SND_SAMSUNG_AC97
	&exynos_device_ac97,
#endif
#ifdef CONFIG_SND_SAMSUNG_I2S
	&exynos_device_i2s0,
#endif
#ifdef CONFIG_SND_SAMSUNG_PCM
	&exynos_device_pcm0,
#endif
#ifdef CONFIG_SND_SAMSUNG_SPDIF
	&exynos_device_spdif,
#endif
#if defined(CONFIG_SND_SAMSUNG_RP) || defined(CONFIG_SND_SAMSUNG_ALP)
	&exynos_device_srp,
#endif
#ifdef CONFIG_VIDEO_EXYNOS_FIMC_IS
	&exynos4_device_fimc_is,
#endif
#if defined(CONFIG_VIDEO_FIMC)
	&s3c_device_fimc0,
	&s3c_device_fimc1,
	&s3c_device_fimc2,
	&s3c_device_fimc3,
/* CONFIG_VIDEO_SAMSUNG_S5P_FIMC is the feature for mainline */
#elif defined(CONFIG_VIDEO_SAMSUNG_S5P_FIMC)
	&s5p_device_fimc0,
	&s5p_device_fimc1,
	&s5p_device_fimc2,
	&s5p_device_fimc3,
#endif
#if defined(CONFIG_VIDEO_FIMC_MIPI)
	&s3c_device_csis0,
	&s3c_device_csis1,
#endif

#if defined(CONFIG_VIDEO_MFC5X) || defined(CONFIG_VIDEO_SAMSUNG_S5P_MFC)
	&s5p_device_mfc,
#endif
#ifdef CONFIG_S5P_SYSTEM_MMU
	&SYSMMU_PLATDEV(g2d_acp),
	&SYSMMU_PLATDEV(fimc0),
	&SYSMMU_PLATDEV(fimc1),
	&SYSMMU_PLATDEV(fimc2),
	&SYSMMU_PLATDEV(fimc3),
	&SYSMMU_PLATDEV(jpeg),
	&SYSMMU_PLATDEV(mfc_l),
	&SYSMMU_PLATDEV(mfc_r),
	&SYSMMU_PLATDEV(tv),
#ifdef CONFIG_VIDEO_EXYNOS_FIMC_IS
	&SYSMMU_PLATDEV(is_isp),
	&SYSMMU_PLATDEV(is_drc),
	&SYSMMU_PLATDEV(is_fd),
	&SYSMMU_PLATDEV(is_cpu),
#endif
#endif /* CONFIG_S5P_SYSTEM_MMU */
#ifdef CONFIG_ION_EXYNOS
	&exynos_device_ion,
#endif
#ifdef CONFIG_VIDEO_EXYNOS_FIMC_LITE
	&exynos_device_flite0,
	&exynos_device_flite1,
#endif
#ifdef CONFIG_VIDEO_FIMG2D
	&s5p_device_fimg2d,
#endif
#ifdef CONFIG_EXYNOS_MEDIA_DEVICE
	&exynos_device_md0,
#endif
#ifdef CONFIG_VIDEO_JPEG_V2X
	&s5p_device_jpeg,
#endif
#ifdef CONFIG_SND_SOC_WM8994
	&wm8994_fixed_voltage0,
	&wm8994_fixed_voltage1,	
#endif
	&samsung_asoc_dma,
	&samsung_asoc_idma,
#ifdef CONFIG_BATTERY_SAMSUNG
	&samsung_device_battery,
#endif
#ifdef CONFIG_WAKEUP_ASSIST
	&wakeup_assist_device,
#endif
#ifdef CONFIG_EXYNOS_C2C
	&exynos_device_c2c,
#endif
	&duv060_gpio_key_device,
#ifdef CONFIG_S3C64XX_DEV_SPI
	&exynos_device_spi0,
	&exynos_device_spi1,
	&exynos_device_spi2,
#endif
#ifdef CONFIG_EXYNOS_SETUP_THERMAL
	&exynos_device_tmu,
#endif
#ifdef CONFIG_S5P_DEV_ACE
	&s5p_device_ace,
#endif
	&exynos4_busfreq,
#ifdef CONFIG_DUV060_UBLOX_GPS_POWER
	&ublox_gps_power,
#endif
#ifdef CONFIG_DUV060_CINTERION_PH8_POWER
	&ph8_power,
#endif
#ifdef CONFIG_DUV060_MTK_COMBO_WIFI_BT_POWER
     &mtk_combo_wb_power,
#endif
#ifdef CONFIG_BATTERY_DLI
	&dli_battery_leds,
#endif
};

#ifdef CONFIG_EXYNOS_SETUP_THERMAL
/* below temperature base on the celcius degree */
struct tmu_data exynos_tmu_data __initdata = {
	.ts = {
		.stop_throttle  = 82,
		.start_throttle = 85,
		.stop_warning  = 95,
		.start_warning = 103,
		.start_tripping = 110, /* temp to do tripping */
	},
	.efuse_value = 55,
	.slope = 0x10008802,
	.mode = 0,
};
#endif

#ifdef CONFIG_VIDEO_EXYNOS_HDMI_CEC
static struct s5p_platform_cec hdmi_cec_data __initdata = {

};
#endif

#ifdef CONFIG_VIDEO_SAMSUNG_S5P_FIMC
static struct s5p_fimc_isp_info isp_info[] = {
#if defined(CONFIG_VIDEO_S5K4BA)
	{
		.board_info	= &s5k4ba_info,
		.clk_frequency  = 24000000UL,
		.bus_type	= FIMC_ITU_601,
#ifdef CONFIG_ITU_A
		.i2c_bus_num	= 0,
		.mux_id		= 0, /* A-Port : 0, B-Port : 1 */
#endif
#ifdef CONFIG_ITU_B
		.i2c_bus_num	= 1,
		.mux_id		= 1, /* A-Port : 0, B-Port : 1 */
#endif
		.flags		= FIMC_CLK_INV_VSYNC,
	},
#endif
#if defined(CONFIG_VIDEO_M5MOLS)
	{
		.board_info	= &m5mols_board_info,
		.clk_frequency  = 24000000UL,
		.bus_type	= FIMC_MIPI_CSI2,
#ifdef CONFIG_CSI_C
		.i2c_bus_num	= 4,
		.mux_id		= 0, /* A-Port : 0, B-Port : 1 */
#endif
#ifdef CONFIG_CSI_D
		.i2c_bus_num	= 5,
		.mux_id		= 1, /* A-Port : 0, B-Port : 1 */
#endif
		.flags		= FIMC_CLK_INV_PCLK | FIMC_CLK_INV_VSYNC,
		.csi_data_align = 32,
	},
#endif
#ifdef CONFIG_VIDEO_EXYNOS_FIMC_IS
#if defined(CONFIG_VIDEO_S5K3H2)
	{
		.board_info	= &s5k3h2_sensor_info,
		.clk_frequency  = 24000000UL,
		.bus_type	= FIMC_MIPI_CSI2,
#ifdef CONFIG_S5K3H2_CSI_C
		.i2c_bus_num	= 0,
		.mux_id		= 0, /* A-Port : 0, B-Port : 1 */
		.flite_id	= FLITE_IDX_A,
		.cam_power	= smdk4x12_cam0_reset,
#endif
#ifdef CONFIG_S5K3H2_CSI_D
		.i2c_bus_num	= 1,
		.mux_id		= 1, /* A-Port : 0, B-Port : 1 */
		.flite_id	= FLITE_IDX_B,
		.cam_power	= smdk4x12_cam1_reset,
#endif
		.flags		= 0,
		.csi_data_align = 24,
		.use_isp	= true,
	},
#endif
#if defined(CONFIG_VIDEO_S5K3H7)
	{
		.board_info	= &s5k3h7_sensor_info,
		.clk_frequency  = 24000000UL,
		.bus_type	= FIMC_MIPI_CSI2,
#ifdef CONFIG_S5K3H7_CSI_C
		.i2c_bus_num	= 0,
		.mux_id		= 0, /* A-Port : 0, B-Port : 1 */
		.flite_id	= FLITE_IDX_A,
		.cam_power	= smdk4x12_cam0_reset,
#endif
#ifdef CONFIG_S5K3H7_CSI_D
		.i2c_bus_num	= 1,
		.mux_id		= 1, /* A-Port : 0, B-Port : 1 */
		.flite_id	= FLITE_IDX_B,
		.cam_power	= smdk4x12_cam1_reset,
#endif
		.csi_data_align = 24,
		.use_isp	= true,
	},
#endif
#if defined(CONFIG_VIDEO_S5K4E5)
	{
		.board_info	= &s5k4e5_sensor_info,
		.clk_frequency  = 24000000UL,
		.bus_type	= FIMC_MIPI_CSI2,
#ifdef CONFIG_S5K4E5_CSI_C
		.i2c_bus_num	= 0,
		.mux_id		= 0, /* A-Port : 0, B-Port : 1 */
		.flite_id	= FLITE_IDX_A,
		.cam_power	= smdk4x12_cam0_reset,
#endif
#ifdef CONFIG_S5K4E5_CSI_D
		.i2c_bus_num	= 1,
		.mux_id		= 1, /* A-Port : 0, B-Port : 1 */
		.flite_id	= FLITE_IDX_B,
		.cam_power	= smdk4x12_cam1_reset,
#endif
		.csi_data_align = 24,
		.use_isp	= true,
	},
#endif
#if defined(CONFIG_VIDEO_S5K6A3)
	{
		.board_info	= &s5k6a3_sensor_info,
		.clk_frequency  = 12000000UL,
		.bus_type	= FIMC_MIPI_CSI2,
#ifdef CONFIG_S5K6A3_CSI_C
		.i2c_bus_num	= 0,
		.mux_id		= 0, /* A-Port : 0, B-Port : 1 */
		.flite_id	= FLITE_IDX_A,
		.cam_power	= smdk4x12_cam0_reset,
#endif
#ifdef CONFIG_S5K6A3_CSI_D
		.i2c_bus_num	= 1,
		.mux_id		= 1, /* A-Port : 0, B-Port : 1 */
		.flite_id	= FLITE_IDX_B,
		.cam_power	= smdk4x12_cam1_reset,
#endif
		.flags		= 0,
		.csi_data_align = 12,
		.use_isp	= true,
	},
#endif
#endif
#if defined(WRITEBACK_ENABLED)
	{
		.board_info	= &writeback_info,
		.bus_type	= FIMC_LCD_WB,
		.i2c_bus_num	= 0,
		.mux_id		= 0, /* A-Port : 0, B-Port : 1 */
		.flags		= FIMC_CLK_INV_VSYNC,
	},
#endif
};

static void __init smdk4x12_subdev_config(void)
{
	s3c_fimc0_default_data.isp_info[0] = &isp_info[0];
	s3c_fimc0_default_data.isp_info[0]->use_cam = true;
	s3c_fimc0_default_data.isp_info[1] = &isp_info[1];
	s3c_fimc0_default_data.isp_info[1]->use_cam = true;
	/* support using two fimc as one sensore */
	{
		static struct s5p_fimc_isp_info camcording1;
		static struct s5p_fimc_isp_info camcording2;
		memcpy(&camcording1, &isp_info[0], sizeof(struct s5p_fimc_isp_info));
		memcpy(&camcording2, &isp_info[1], sizeof(struct s5p_fimc_isp_info));
		s3c_fimc1_default_data.isp_info[0] = &camcording1;
		s3c_fimc1_default_data.isp_info[0]->use_cam = false;
		s3c_fimc1_default_data.isp_info[1] = &camcording2;
		s3c_fimc1_default_data.isp_info[1]->use_cam = false;
	}
#ifdef CONFIG_VIDEO_EXYNOS_FIMC_IS
#ifdef CONFIG_VIDEO_S5K3H2
#ifdef CONFIG_S5K3H2_CSI_C
	s5p_mipi_csis0_default_data.clk_rate	= 160000000;
	s5p_mipi_csis0_default_data.lanes	= 2;
	s5p_mipi_csis0_default_data.alignment	= 24;
	s5p_mipi_csis0_default_data.hs_settle	= 12;
#endif
#ifdef CONFIG_S5K3H2_CSI_D
	s5p_mipi_csis1_default_data.clk_rate	= 160000000;
	s5p_mipi_csis1_default_data.lanes	= 2;
	s5p_mipi_csis1_default_data.alignment	= 24;
	s5p_mipi_csis1_default_data.hs_settle	= 12;
#endif
#endif
#ifdef CONFIG_VIDEO_S5K3H7
#ifdef CONFIG_S5K3H7_CSI_C
	s5p_mipi_csis0_default_data.clk_rate	= 160000000;
	s5p_mipi_csis0_default_data.lanes	= 2;
	s5p_mipi_csis0_default_data.alignment	= 24;
	s5p_mipi_csis0_default_data.hs_settle	= 12;
#endif
#ifdef CONFIG_S5K3H7_CSI_D
	s5p_mipi_csis1_default_data.clk_rate	= 160000000;
	s5p_mipi_csis1_default_data.lanes	= 2;
	s5p_mipi_csis1_default_data.alignment	= 24;
	s5p_mipi_csis1_default_data.hs_settle	= 12;
#endif
#endif
#ifdef CONFIG_VIDEO_S5K4E5
#ifdef CONFIG_S5K4E5_CSI_C
	s5p_mipi_csis0_default_data.clk_rate	= 160000000;
	s5p_mipi_csis0_default_data.lanes	= 2;
	s5p_mipi_csis0_default_data.alignment	= 24;
	s5p_mipi_csis0_default_data.hs_settle	= 12;
#endif
#ifdef CONFIG_S5K4E5_CSI_D
	s5p_mipi_csis1_default_data.clk_rate	= 160000000;
	s5p_mipi_csis1_default_data.lanes	= 2;
	s5p_mipi_csis1_default_data.alignment	= 24;
	s5p_mipi_csis1_default_data.hs_settle	= 12;
#endif
#endif
#ifdef CONFIG_VIDEO_S5K6A3
#ifdef CONFIG_S5K6A3_CSI_C
	s5p_mipi_csis0_default_data.clk_rate	= 160000000;
	s5p_mipi_csis0_default_data.lanes 	= 1;
	s5p_mipi_csis0_default_data.alignment	= 24;
	s5p_mipi_csis0_default_data.hs_settle	= 12;
#endif
#ifdef CONFIG_S5K6A3_CSI_D
	s5p_mipi_csis1_default_data.clk_rate	= 160000000;
	s5p_mipi_csis1_default_data.lanes 	= 1;
	s5p_mipi_csis1_default_data.alignment	= 24;
	s5p_mipi_csis1_default_data.hs_settle	= 12;
#endif
#endif
#endif
}
static void __init smdk4x12_camera_config(void)
{
	/* CAM A port(b0010) : PCLK, VSYNC, HREF, DATA[0-4] */
	//s3c_gpio_cfgrange_nopull(EXYNOS4212_GPJ0(0), 8, S3C_GPIO_SFN(2));
	/* CAM A port(b0010) : DATA[5-7], CLKOUT(MIPI CAM also), FIELD */
	//s3c_gpio_cfgrange_nopull(EXYNOS4212_GPJ1(0), 5, S3C_GPIO_SFN(2));
	/* CAM B port(b0011) : PCLK, DATA[0-6] */
	s3c_gpio_cfgrange_nopull(EXYNOS4212_GPM0(0), 8, S3C_GPIO_SFN(3));
	/* CAM B port(b0011) : FIELD, DATA[7]*/
	s3c_gpio_cfgrange_nopull(EXYNOS4212_GPM1(0), 2, S3C_GPIO_SFN(3));
	/* CAM B port(b0011) : VSYNC, HREF, CLKOUT*/
	s3c_gpio_cfgrange_nopull(EXYNOS4212_GPM2(0), 3, S3C_GPIO_SFN(3));

	/* note : driver strength to max is unnecessary */
#ifdef CONFIG_VIDEO_M5MOLS
	s3c_gpio_cfgpin(EXYNOS4_GPX2(6), S3C_GPIO_SFN(0xF));
	s3c_gpio_setpull(EXYNOS4_GPX2(6), S3C_GPIO_PULL_NONE);
#endif
}
#endif /* CONFIG_VIDEO_SAMSUNG_S5P_FIMC */

#ifdef CONFIG_VIDEO_EXYNOS_FIMC_LITE
static void __set_flite_camera_config(struct exynos_platform_flite *data,
					u32 active_index, u32 max_cam)
{
	data->active_cam_index = active_index;
	data->num_clients = max_cam;
}

static void __init smdk4x12_set_camera_flite_platdata(void)
{
	int flite0_cam_index = 0;
	int flite1_cam_index = 0;
#ifdef CONFIG_VIDEO_S5K3H2
#ifdef CONFIG_S5K3H2_CSI_C
	exynos_flite0_default_data.cam[flite0_cam_index++] = &s5k3h2;
#endif
#ifdef CONFIG_S5K3H2_CSI_D
	exynos_flite1_default_data.cam[flite1_cam_index++] = &s5k3h2;
#endif
#endif
#ifdef CONFIG_VIDEO_S5K3H7
#ifdef CONFIG_S5K3H7_CSI_C
	exynos_flite0_default_data.cam[flite0_cam_index++] = &s5k3h7;
#endif
#ifdef CONFIG_S5K3H7_CSI_D
	exynos_flite1_default_data.cam[flite1_cam_index++] = &s5k3h7;
#endif
#endif
#ifdef CONFIG_VIDEO_S5K4E5
#ifdef CONFIG_S5K4E5_CSI_C
	exynos_flite0_default_data.cam[flite0_cam_index++] = &s5k4e5;
#endif
#ifdef CONFIG_S5K4E5_CSI_D
	exynos_flite1_default_data.cam[flite1_cam_index++] = &s5k4e5;
#endif
#endif

#ifdef CONFIG_VIDEO_S5K6A3
#ifdef CONFIG_S5K6A3_CSI_C
	exynos_flite0_default_data.cam[flite0_cam_index++] = &s5k6a3;
#endif
#ifdef CONFIG_S5K6A3_CSI_D
	exynos_flite1_default_data.cam[flite1_cam_index++] = &s5k6a3;
#endif
#endif
	__set_flite_camera_config(&exynos_flite0_default_data, 0, flite0_cam_index);
	__set_flite_camera_config(&exynos_flite1_default_data, 0, flite1_cam_index);
}
#endif

#if defined(CONFIG_CMA)
static void __init exynos4_reserve_mem(void)
{
	static struct cma_region regions[] = {
#ifndef CONFIG_VIDEOBUF2_ION
#ifdef CONFIG_VIDEO_SAMSUNG_MEMSIZE_TV
		{
			.name = "tv",
			.size = CONFIG_VIDEO_SAMSUNG_MEMSIZE_TV * SZ_1K,
			.start = 0
		},
#endif
#ifdef CONFIG_VIDEO_SAMSUNG_MEMSIZE_JPEG
		{
			.name = "jpeg",
			.size = CONFIG_VIDEO_SAMSUNG_MEMSIZE_JPEG * SZ_1K,
			.start = 0
		},
#endif
#ifdef CONFIG_AUDIO_SAMSUNG_MEMSIZE_SRP
		{
			.name = "srp",
			.size = CONFIG_AUDIO_SAMSUNG_MEMSIZE_SRP * SZ_1K,
			.start = 0,
		},
#endif
#ifdef CONFIG_VIDEO_SAMSUNG_MEMSIZE_FIMG2D
		{
			.name = "fimg2d",
			.size = CONFIG_VIDEO_SAMSUNG_MEMSIZE_FIMG2D * SZ_1K,
			.start = 0
		},
#endif
#ifdef CONFIG_VIDEO_SAMSUNG_MEMSIZE_FIMD
		{
			.name = "fimd",
			.size = CONFIG_VIDEO_SAMSUNG_MEMSIZE_FIMD * SZ_1K,
			.start = 0
		},
#endif
#ifdef CONFIG_VIDEO_SAMSUNG_MEMSIZE_FIMC0
		{
			.name = "fimc0",
			.size = CONFIG_VIDEO_SAMSUNG_MEMSIZE_FIMC0 * SZ_1K,
			.start = 0
		},
#endif
#ifdef CONFIG_VIDEO_SAMSUNG_MEMSIZE_FIMC2
		{
			.name = "fimc2",
			.size = CONFIG_VIDEO_SAMSUNG_MEMSIZE_FIMC2 * SZ_1K,
			.start = 0
		},
#endif
#if !defined(CONFIG_EXYNOS4_CONTENT_PATH_PROTECTION) && \
	defined(CONFIG_VIDEO_SAMSUNG_MEMSIZE_FIMC3)
		{
			.name = "fimc3",
			.size = CONFIG_VIDEO_SAMSUNG_MEMSIZE_FIMC3 * SZ_1K,
		},
#endif
#ifdef CONFIG_VIDEO_SAMSUNG_MEMSIZE_FIMC1
		{
			.name = "fimc1",
			.size = CONFIG_VIDEO_SAMSUNG_MEMSIZE_FIMC1 * SZ_1K,
			.start = 0
		},
#endif
#ifdef CONFIG_VIDEO_SAMSUNG_MEMSIZE_MFC_NORMAL
		{
			.name = "mfc-normal",
			.size = CONFIG_VIDEO_SAMSUNG_MEMSIZE_MFC_NORMAL * SZ_1K,
			{ .alignment = 1 << 17 },
		},
#endif
#ifdef CONFIG_VIDEO_SAMSUNG_MEMSIZE_MFC1
		{
			.name = "mfc1",
			.size = CONFIG_VIDEO_SAMSUNG_MEMSIZE_MFC1 * SZ_1K,
			{ .alignment = 1 << 17 },
		},
#endif
#ifdef CONFIG_VIDEO_SAMSUNG_MEMSIZE_MFC0
		{
			.name = "mfc0",
			.size = CONFIG_VIDEO_SAMSUNG_MEMSIZE_MFC0 * SZ_1K,
			{ .alignment = 1 << 17 },
		},
#endif
#ifdef CONFIG_VIDEO_SAMSUNG_MEMSIZE_MFC
		{
			.name = "mfc",
			.size = CONFIG_VIDEO_SAMSUNG_MEMSIZE_MFC * SZ_1K,
			{ .alignment = 1 << 17 },
		},
#endif
#ifdef CONFIG_VIDEO_EXYNOS_FIMC_IS
		{
			.name = "fimc_is",
			.size = CONFIG_VIDEO_EXYNOS_MEMSIZE_FIMC_IS * SZ_1K,
			{
				.alignment = 1 << 26,
			},
			.start = 0
		},
#ifdef CONFIG_VIDEO_EXYNOS_FIMC_IS_BAYER
		{
			.name = "fimc_is_isp",
			.size = CONFIG_VIDEO_EXYNOS_MEMSIZE_FIMC_IS_ISP * SZ_1K,
			.start = 0
		},
#endif
#endif
#if !defined(CONFIG_EXYNOS4_CONTENT_PATH_PROTECTION) && \
	defined(CONFIG_VIDEO_SAMSUNG_S5P_MFC)
		{
			.name		= "b2",
			.size		= 32 << 20,
			{ .alignment	= 128 << 10 },
		},
		{
			.name		= "b1",
			.size		= 32 << 20,
			{ .alignment	= 128 << 10 },
		},
		{
			.name		= "fw",
			.size		= 1 << 20,
			{ .alignment	= 128 << 10 },
		},
#endif
#else /* !CONFIG_VIDEOBUF2_ION */
#ifdef CONFIG_FB_S5P
#error CONFIG_FB_S5P is defined. Select CONFIG_FB_S3C, instead
#endif
		{
			.name	= "ion",
			.size	= CONFIG_ION_EXYNOS_CONTIGHEAP_SIZE * SZ_1K,
		},
#endif /* !CONFIG_VIDEOBUF2_ION */
		{
			.size = 0
		},
	};
#ifdef CONFIG_EXYNOS4_CONTENT_PATH_PROTECTION
	static struct cma_region regions_secure[] = {
#ifdef CONFIG_VIDEO_SAMSUNG_MEMSIZE_FIMC3
		{
			.name = "fimc3",
			.size = CONFIG_VIDEO_SAMSUNG_MEMSIZE_FIMC3 * SZ_1K,
		},
#endif
#ifdef CONFIG_VIDEO_SAMSUNG_MEMSIZE_FIMD_VIDEO
		{
			.name = "video",
			.size = CONFIG_VIDEO_SAMSUNG_MEMSIZE_FIMD_VIDEO * SZ_1K,
		},
#endif
#ifdef CONFIG_VIDEO_SAMSUNG_MEMSIZE_MFC_SECURE
		{
			.name = "mfc-secure",
			.size = CONFIG_VIDEO_SAMSUNG_MEMSIZE_MFC_SECURE * SZ_1K,
		},
#endif
		{
			.name = "sectbl",
			.size = SZ_1M,
		},
		{
			.size = 0
		},
	};
#else /* !CONFIG_EXYNOS4_CONTENT_PATH_PROTECTION */
	struct cma_region *regions_secure = NULL;
#endif
	static const char map[] __initconst =
#ifdef CONFIG_EXYNOS_C2C
		"samsung-c2c=c2c_shdmem;"
#endif
		"s3cfb.0/fimd=fimd;exynos4-fb.0/fimd=fimd;"
#ifdef CONFIG_EXYNOS4_CONTENT_PATH_PROTECTION
		"s3cfb.0/video=video;exynos4-fb.0/video=video;"
#endif
		"s3c-fimc.0=fimc0;s3c-fimc.1=fimc1;s3c-fimc.2=fimc2;s3c-fimc.3=fimc3;"
		"exynos4210-fimc.0=fimc0;exynos4210-fimc.1=fimc1;exynos4210-fimc.2=fimc2;exynos4210-fimc.3=fimc3;"
#ifdef CONFIG_VIDEO_MFC5X
		"s3c-mfc/A=mfc0,mfc-secure;"
		"s3c-mfc/B=mfc1,mfc-normal;"
		"s3c-mfc/AB=mfc;"
#endif
#ifdef CONFIG_VIDEO_SAMSUNG_S5P_MFC
		"s5p-mfc/f=fw;"
		"s5p-mfc/a=b1;"
		"s5p-mfc/b=b2;"
#endif
		"samsung-rp=srp;"
		"s5p-jpeg=jpeg;"
		"exynos4-fimc-is/f=fimc_is;"
#ifdef CONFIG_VIDEO_EXYNOS_FIMC_IS_BAYER
		"exynos4-fimc-is/i=fimc_is_isp;"
#endif
		"s5p-mixer=tv;"
		"s5p-fimg2d=fimg2d;"
		"ion-exynos=ion,fimd,fimc0,fimc1,fimc2,fimc3,fw,b1,b2;"
#ifdef CONFIG_EXYNOS4_CONTENT_PATH_PROTECTION
		"s5p-smem/video=video;"
		"s5p-smem/sectbl=sectbl;"
#endif
		"s5p-smem/mfc=mfc0,mfc-secure;"
		"s5p-smem/fimc=fimc3;"
		"s5p-smem/mfc-shm=mfc1,mfc-normal;"
		"s5p-smem/fimd=fimd;";

	s5p_cma_region_reserve(regions, regions_secure, 0, map);
}
#else
static inline void exynos4_reserve_mem(void)
{
}
#endif /* CONFIG_CMA */

/* LCD Backlight data */
static struct samsung_bl_gpio_info smdk4x12_bl_gpio_info = {
	.no = 0, //EXYNOS4_GPD0(1),
	.func = S3C_GPIO_SFN(2),
};

static struct platform_pwm_backlight_data smdk4x12_bl_data = {
	.pwm_id = 1,
};

static void __init smdk4x12_map_io(void)
{
	clk_xusbxti.rate = 24000000;
	s5p_init_io(NULL, 0, S5P_VA_CHIPID);
	s3c24xx_init_clocks(24000000);
	s3c24xx_init_uarts(smdk4x12_uartcfgs, ARRAY_SIZE(smdk4x12_uartcfgs));

	exynos4_reserve_mem();
}

static void __init exynos_sysmmu_init(void)
{
	ASSIGN_SYSMMU_POWERDOMAIN(fimc0, &exynos4_device_pd[PD_CAM].dev);
	ASSIGN_SYSMMU_POWERDOMAIN(fimc1, &exynos4_device_pd[PD_CAM].dev);
	ASSIGN_SYSMMU_POWERDOMAIN(fimc2, &exynos4_device_pd[PD_CAM].dev);
	ASSIGN_SYSMMU_POWERDOMAIN(fimc3, &exynos4_device_pd[PD_CAM].dev);
	ASSIGN_SYSMMU_POWERDOMAIN(jpeg, &exynos4_device_pd[PD_CAM].dev);
	ASSIGN_SYSMMU_POWERDOMAIN(mfc_l, &exynos4_device_pd[PD_MFC].dev);
	ASSIGN_SYSMMU_POWERDOMAIN(mfc_r, &exynos4_device_pd[PD_MFC].dev);
	ASSIGN_SYSMMU_POWERDOMAIN(tv, &exynos4_device_pd[PD_TV].dev);
#ifdef CONFIG_VIDEO_FIMG2D
	sysmmu_set_owner(&SYSMMU_PLATDEV(g2d_acp).dev, &s5p_device_fimg2d.dev);
#endif
#if defined(CONFIG_VIDEO_SAMSUNG_S5P_MFC) || defined(CONFIG_VIDEO_MFC5X)
	sysmmu_set_owner(&SYSMMU_PLATDEV(mfc_l).dev, &s5p_device_mfc.dev);
	sysmmu_set_owner(&SYSMMU_PLATDEV(mfc_r).dev, &s5p_device_mfc.dev);
#endif
#if defined(CONFIG_VIDEO_FIMC)
	sysmmu_set_owner(&SYSMMU_PLATDEV(fimc0).dev, &s3c_device_fimc0.dev);
	sysmmu_set_owner(&SYSMMU_PLATDEV(fimc1).dev, &s3c_device_fimc1.dev);
	sysmmu_set_owner(&SYSMMU_PLATDEV(fimc2).dev, &s3c_device_fimc2.dev);
	sysmmu_set_owner(&SYSMMU_PLATDEV(fimc3).dev, &s3c_device_fimc3.dev);
#elif defined(CONFIG_VIDEO_SAMSUNG_S5P_FIMC)
	sysmmu_set_owner(&SYSMMU_PLATDEV(fimc0).dev, &s5p_device_fimc0.dev);
	sysmmu_set_owner(&SYSMMU_PLATDEV(fimc1).dev, &s5p_device_fimc1.dev);
	sysmmu_set_owner(&SYSMMU_PLATDEV(fimc2).dev, &s5p_device_fimc2.dev);
	sysmmu_set_owner(&SYSMMU_PLATDEV(fimc3).dev, &s5p_device_fimc3.dev);
#endif
#ifdef CONFIG_VIDEO_JPEG_V2X
	sysmmu_set_owner(&SYSMMU_PLATDEV(jpeg).dev, &s5p_device_jpeg.dev);
#endif
#ifdef CONFIG_VIDEO_EXYNOS_FIMC_IS
	ASSIGN_SYSMMU_POWERDOMAIN(is_isp, &exynos4_device_pd[PD_ISP].dev);
	ASSIGN_SYSMMU_POWERDOMAIN(is_drc, &exynos4_device_pd[PD_ISP].dev);
	ASSIGN_SYSMMU_POWERDOMAIN(is_fd, &exynos4_device_pd[PD_ISP].dev);
	ASSIGN_SYSMMU_POWERDOMAIN(is_cpu, &exynos4_device_pd[PD_ISP].dev);

	sysmmu_set_owner(&SYSMMU_PLATDEV(is_isp).dev,
						&exynos4_device_fimc_is.dev);
	sysmmu_set_owner(&SYSMMU_PLATDEV(is_drc).dev,
						&exynos4_device_fimc_is.dev);
	sysmmu_set_owner(&SYSMMU_PLATDEV(is_fd).dev,
						&exynos4_device_fimc_is.dev);
	sysmmu_set_owner(&SYSMMU_PLATDEV(is_cpu).dev,
						&exynos4_device_fimc_is.dev);
#endif
}


#define SMDK4412_REV_0_0_ADC_VALUE 0
#define SMDK4412_REV_0_1_ADC_VALUE 443
int samsung_board_rev;

static int get_samsung_board_rev(void)
{
	int		ret = 0;
#ifdef CONFIG_BOARD_DUV060
	return ret;
#else /* It's only for smdk */
	int 		adc_val = 0;
	struct clk	*adc_clk;
	struct resource	*res;
	void __iomem	*adc_regs;
	unsigned int	con;
	int		ret;

	if ((soc_is_exynos4412() && samsung_rev() < EXYNOS4412_REV_1_0) ||
		(soc_is_exynos4212() && samsung_rev() < EXYNOS4212_REV_1_0))
		return SAMSUNG_BOARD_REV_0_0;

	adc_clk = clk_get(NULL, "adc");
	if (unlikely(IS_ERR(adc_clk)))
		return SAMSUNG_BOARD_REV_0_0;

	clk_enable(adc_clk);

	res = platform_get_resource(&s3c_device_adc, IORESOURCE_MEM, 0);
	if (unlikely(!res))
		goto err_clk;

	adc_regs = ioremap(res->start, resource_size(res));
	if (unlikely(!adc_regs))
		goto err_clk;

	writel(S5PV210_ADCCON_SELMUX(3), adc_regs + S5PV210_ADCMUX);

	con = readl(adc_regs + S3C2410_ADCCON);
	con &= ~S3C2410_ADCCON_MUXMASK;
	con &= ~S3C2410_ADCCON_STDBM;
	con &= ~S3C2410_ADCCON_STARTMASK;
	con |=  S3C2410_ADCCON_PRSCEN;

	con |= S3C2410_ADCCON_ENABLE_START;
	writel(con, adc_regs + S3C2410_ADCCON);

	udelay (50);

	adc_val = readl(adc_regs + S3C2410_ADCDAT0) & 0xFFF;
	writel(0, adc_regs + S3C64XX_ADCCLRINT);

	iounmap(adc_regs);
err_clk:
	clk_disable(adc_clk);
	clk_put(adc_clk);

	ret = (adc_val < SMDK4412_REV_0_1_ADC_VALUE/2) ?
			SAMSUNG_BOARD_REV_0_0 : SAMSUNG_BOARD_REV_0_1;

	pr_info ("SMDK MAIN Board Rev 0.%d (ADC value:%d)\n", ret, adc_val);
	return ret;
#endif

}


static void __init smdk4x12_machine_init(void)
{
#ifdef CONFIG_S3C64XX_DEV_SPI
	struct clk *sclk = NULL;
	struct clk *prnt = NULL;
	struct device *spi0_dev = &exynos_device_spi0.dev;
	struct device *spi1_dev = &exynos_device_spi1.dev;
	struct device *spi2_dev = &exynos_device_spi2.dev;
#endif

	samsung_board_rev = get_samsung_board_rev();
#if defined(CONFIG_EXYNOS_DEV_PD) && defined(CONFIG_PM_RUNTIME)
	exynos_pd_disable(&exynos4_device_pd[PD_MFC].dev);
	exynos_pd_disable(&exynos4_device_pd[PD_G3D].dev);
	exynos_pd_disable(&exynos4_device_pd[PD_LCD0].dev);
	exynos_pd_disable(&exynos4_device_pd[PD_CAM].dev);
	exynos_pd_disable(&exynos4_device_pd[PD_TV].dev);
	exynos_pd_disable(&exynos4_device_pd[PD_GPS].dev);
	exynos_pd_disable(&exynos4_device_pd[PD_GPS_ALIVE].dev);
	exynos_pd_disable(&exynos4_device_pd[PD_ISP].dev);
#elif defined(CONFIG_EXYNOS_DEV_PD)
	/*
	 * These power domains should be always on
	 * without runtime pm support.
	 */
	exynos_pd_enable(&exynos4_device_pd[PD_MFC].dev);
	exynos_pd_enable(&exynos4_device_pd[PD_G3D].dev);
	exynos_pd_enable(&exynos4_device_pd[PD_LCD0].dev);
	exynos_pd_enable(&exynos4_device_pd[PD_CAM].dev);
	exynos_pd_enable(&exynos4_device_pd[PD_TV].dev);
	exynos_pd_enable(&exynos4_device_pd[PD_GPS].dev);
	exynos_pd_enable(&exynos4_device_pd[PD_GPS_ALIVE].dev);
	exynos_pd_enable(&exynos4_device_pd[PD_ISP].dev);
#endif
	duv060_pmic_hw_init();
	s3c_i2c0_set_platdata(NULL);
	i2c_register_board_info(0, i2c_devs0, ARRAY_SIZE(i2c_devs0));
#ifdef CONFIG_S3C_DEV_I2C1
	s3c_i2c1_set_platdata(NULL);
	i2c_register_board_info(1, i2c_devs1, ARRAY_SIZE(i2c_devs1));
#endif
#ifdef CONFIG_S3C_DEV_I2C2
	s3c_i2c2_set_platdata(NULL);
	i2c_register_board_info(2, i2c_devs2, ARRAY_SIZE(i2c_devs2));
#endif
#ifdef CONFIG_S3C_DEV_I2C3
	s3c_i2c3_set_platdata(NULL);
	i2c_register_board_info(3, i2c_devs3, ARRAY_SIZE(i2c_devs3));
#endif
#ifdef CONFIG_S3C_DEV_I2C4
	duv060_sensor_hw_init();
//	as3643_hw_init();

#ifdef CONFIG_SENSORS_AS3039S07
	duv060_as3039s07_hw_init();
#endif

#ifdef CONFIG_SENSORS_BH1750FVI
	duv060_bh170fvi_hw_init();
#endif

	s3c_i2c4_set_platdata(NULL);
	i2c_register_board_info(4, i2c_devs4, ARRAY_SIZE(i2c_devs4));
#endif
#ifdef CONFIG_S3C_DEV_I2C5
	s3c_i2c5_set_platdata(NULL);
	i2c_register_board_info(5, i2c_devs5, ARRAY_SIZE(i2c_devs5));
#endif
#ifdef CONFIG_S3C_DEV_I2C6
#if defined(CONFIG_TOUCHSCREEN_HIMAX) || defined(CONFIG_TOUCHSCREEN_FT5x06)
	duv060_touch_hw_init();
#endif
	s3c_i2c6_set_platdata(NULL);
	i2c_register_board_info(6, i2c_devs6, ARRAY_SIZE(i2c_devs6));
#endif
#ifdef CONFIG_S3C_DEV_I2C7
	s3c_i2c7_set_platdata(NULL);
#if defined (CONFIG_I2C_SI4704)
	duv060_si4704_hw_init();
#endif
#ifndef CONFIG_BOARD_DUV060
	i2c_devs7[0].irq = samsung_board_rev_is_0_0() ? IRQ_EINT(15) : IRQ_EINT(22);
#endif
	i2c_register_board_info(7, i2c_devs7, ARRAY_SIZE(i2c_devs7));

#endif

#ifdef CONFIG_FB_S5P
	s3cfb_set_platdata(NULL);
#ifdef CONFIG_EXYNOS_DEV_PD
	s3c_device_fb.dev.parent = &exynos4_device_pd[PD_LCD0].dev;
#endif
#endif
#ifdef CONFIG_USB_EHCI_S5P
	smdk4x12_ehci_init();
#endif
#ifdef CONFIG_USB_OHCI_S5P
	smdk4x12_ohci_init();
#endif
#ifdef CONFIG_USB_GADGET
	smdk4x12_usbgadget_init();
#endif
#ifdef CONFIG_USB_EXYNOS_SWITCH
	smdk4x12_usbswitch_init();
#endif

	samsung_bl_set(&smdk4x12_bl_gpio_info, &smdk4x12_bl_data);

#ifdef CONFIG_EXYNOS4_DEV_DWMCI
	exynos_dwmci_set_platdata(&exynos_dwmci_pdata);
#endif

#ifdef CONFIG_VIDEO_EXYNOS_FIMC_IS
	exynos4_fimc_is_set_platdata(NULL);
#ifdef CONFIG_EXYNOS_DEV_PD
	exynos4_device_fimc_is.dev.parent = &exynos4_device_pd[PD_ISP].dev;
#endif
#endif
#ifdef CONFIG_S3C_DEV_HSMMC
	s3c_sdhci0_set_platdata(&smdk4x12_hsmmc0_pdata);
#endif
#ifdef CONFIG_S3C_DEV_HSMMC1
	s3c_sdhci1_set_platdata(&smdk4x12_hsmmc1_pdata);
#endif
#ifdef CONFIG_S3C_DEV_HSMMC2
	s3c_sdhci2_set_platdata(&smdk4x12_hsmmc2_pdata);
#endif
#ifdef CONFIG_S3C_DEV_HSMMC3
	s3c_sdhci3_set_platdata(&smdk4x12_hsmmc3_pdata);
#endif
#ifdef CONFIG_S5P_DEV_MSHC
	s3c_mshci_set_platdata(&exynos4_mshc_pdata);
#endif
#ifdef CONFIG_VIDEO_EXYNOS_FIMC_LITE
	smdk4x12_set_camera_flite_platdata();
	s3c_set_platdata(&exynos_flite0_default_data,
			sizeof(exynos_flite0_default_data), &exynos_device_flite0);
	s3c_set_platdata(&exynos_flite1_default_data,
			sizeof(exynos_flite1_default_data), &exynos_device_flite1);
#ifdef CONFIG_EXYNOS_DEV_PD
	exynos_device_flite0.dev.parent = &exynos4_device_pd[PD_ISP].dev;
	exynos_device_flite1.dev.parent = &exynos4_device_pd[PD_ISP].dev;
#endif
#endif
#ifdef CONFIG_EXYNOS_SETUP_THERMAL
	s5p_tmu_set_platdata(&exynos_tmu_data);
#endif
#ifdef CONFIG_VIDEO_FIMC
	s3c_fimc0_set_platdata(&fimc_plat);
	s3c_fimc1_set_platdata(&fimc_plat);
	s3c_fimc2_set_platdata(&fimc_plat);
	s3c_fimc3_set_platdata(NULL);
#ifdef CONFIG_EXYNOS_DEV_PD
	s3c_device_fimc0.dev.parent = &exynos4_device_pd[PD_CAM].dev;
	s3c_device_fimc1.dev.parent = &exynos4_device_pd[PD_CAM].dev;
	s3c_device_fimc2.dev.parent = &exynos4_device_pd[PD_CAM].dev;
	s3c_device_fimc3.dev.parent = &exynos4_device_pd[PD_CAM].dev;
#ifdef CONFIG_EXYNOS4_CONTENT_PATH_PROTECTION
	secmem.parent = &exynos4_device_pd[PD_CAM].dev;
#endif
#endif
#ifdef CONFIG_VIDEO_FIMC_MIPI
	s3c_csis0_set_platdata(NULL);
	s3c_csis1_set_platdata(NULL);
#ifdef CONFIG_EXYNOS_DEV_PD
	s3c_device_csis0.dev.parent = &exynos4_device_pd[PD_CAM].dev;
	s3c_device_csis1.dev.parent = &exynos4_device_pd[PD_CAM].dev;
#endif
#endif

#if defined(CONFIG_ITU_A) || defined(CONFIG_CSI_C) \
	|| defined(CONFIG_S5K3H1_CSI_C) || defined(CONFIG_S5K3H2_CSI_C) \
	|| defined(CONFIG_S5K6A3_CSI_C)
	smdk4x12_cam0_reset(1);
#endif
#if defined(CONFIG_ITU_B) || defined(CONFIG_CSI_D) \
	|| defined(CONFIG_S5K3H1_CSI_D) || defined(CONFIG_S5K3H2_CSI_D) \
	|| defined(CONFIG_S5K6A3_CSI_D)
	smdk4x12_cam1_reset(1);
#endif
#endif /* CONFIG_VIDEO_FIMC */

#ifdef CONFIG_VIDEO_SAMSUNG_S5P_FIMC
	smdk4x12_camera_config();
	smdk4x12_subdev_config();

	dev_set_name(&s5p_device_fimc0.dev, "s3c-fimc.0");
	dev_set_name(&s5p_device_fimc1.dev, "s3c-fimc.1");
	dev_set_name(&s5p_device_fimc2.dev, "s3c-fimc.2");
	dev_set_name(&s5p_device_fimc3.dev, "s3c-fimc.3");

	clk_add_alias("fimc", "exynos4210-fimc.0", "fimc", &s5p_device_fimc0.dev);
	clk_add_alias("sclk_fimc", "exynos4210-fimc.0", "sclk_fimc",
			&s5p_device_fimc0.dev);
	clk_add_alias("fimc", "exynos4210-fimc.1", "fimc", &s5p_device_fimc1.dev);
	clk_add_alias("sclk_fimc", "exynos4210-fimc.1", "sclk_fimc",
			&s5p_device_fimc1.dev);
	clk_add_alias("fimc", "exynos4210-fimc.2", "fimc", &s5p_device_fimc2.dev);
	clk_add_alias("sclk_fimc", "exynos4210-fimc.2", "sclk_fimc",
			&s5p_device_fimc2.dev);
	clk_add_alias("fimc", "exynos4210-fimc.3", "fimc", &s5p_device_fimc3.dev);
	clk_add_alias("sclk_fimc", "exynos4210-fimc.3", "sclk_fimc",
			&s5p_device_fimc3.dev);

	s3c_fimc_setname(0, "exynos4210-fimc");
	s3c_fimc_setname(1, "exynos4210-fimc");
	s3c_fimc_setname(2, "exynos4210-fimc");
	s3c_fimc_setname(3, "exynos4210-fimc");
	/* FIMC */
	s3c_set_platdata(&s3c_fimc0_default_data,
			 sizeof(s3c_fimc0_default_data), &s5p_device_fimc0);
	s3c_set_platdata(&s3c_fimc1_default_data,
			 sizeof(s3c_fimc1_default_data), &s5p_device_fimc1);
	s3c_set_platdata(&s3c_fimc2_default_data,
			 sizeof(s3c_fimc2_default_data), &s5p_device_fimc2);
	s3c_set_platdata(&s3c_fimc3_default_data,
			 sizeof(s3c_fimc3_default_data), &s5p_device_fimc3);
#ifdef CONFIG_EXYNOS_DEV_PD
	s5p_device_fimc0.dev.parent = &exynos4_device_pd[PD_CAM].dev;
	s5p_device_fimc1.dev.parent = &exynos4_device_pd[PD_CAM].dev;
	s5p_device_fimc2.dev.parent = &exynos4_device_pd[PD_CAM].dev;
	s5p_device_fimc3.dev.parent = &exynos4_device_pd[PD_CAM].dev;
#endif
#if defined(CONFIG_ITU_A) || defined(CONFIG_CSI_C) \
	|| defined(CONFIG_S5K3H1_CSI_C) || defined(CONFIG_S5K3H2_CSI_C) \
	|| defined(CONFIG_S5K6A3_CSI_C)
	smdk4x12_cam0_reset(1);
#endif
#if defined(CONFIG_ITU_B) || defined(CONFIG_CSI_D) \
	|| defined(CONFIG_S5K3H1_CSI_D) || defined(CONFIG_S5K3H2_CSI_D) \
	|| defined(CONFIG_S5K6A3_CSI_D)
	smdk4x12_cam1_reset(1);
#endif
#endif

#ifdef CONFIG_VIDEO_JPEG_V2X
#ifdef CONFIG_EXYNOS_DEV_PD
	s5p_device_jpeg.dev.parent = &exynos4_device_pd[PD_CAM].dev;
	exynos4_jpeg_setup_clock(&s5p_device_jpeg.dev, 160000000);
#endif
#endif

#ifdef CONFIG_ION_EXYNOS
	exynos_ion_set_platdata();
#endif

#if defined(CONFIG_VIDEO_MFC5X) || defined(CONFIG_VIDEO_SAMSUNG_S5P_MFC)
#ifdef CONFIG_EXYNOS_DEV_PD
	s5p_device_mfc.dev.parent = &exynos4_device_pd[PD_MFC].dev;
#endif
	if (soc_is_exynos4412() && samsung_rev() >= EXYNOS4412_REV_1_0)
		exynos4_mfc_setup_clock(&s5p_device_mfc.dev, 200 * MHZ);
	else
		exynos4_mfc_setup_clock(&s5p_device_mfc.dev, 267 * MHZ);
#endif

#if defined(CONFIG_VIDEO_SAMSUNG_S5P_MFC)
	dev_set_name(&s5p_device_mfc.dev, "s3c-mfc");
	clk_add_alias("mfc", "s5p-mfc", "mfc", &s5p_device_mfc.dev);
	s5p_mfc_setname(&s5p_device_mfc, "s5p-mfc");
#endif

#ifdef CONFIG_VIDEO_FIMG2D
	s5p_fimg2d_set_platdata(&fimg2d_data);
#endif

#ifdef CONFIG_EXYNOS_C2C
	exynos_c2c_set_platdata(&smdk4x12_c2c_pdata);
#endif

	exynos_sysmmu_init();

	duv060_gpio_key_init();

	platform_add_devices(smdk4x12_devices, ARRAY_SIZE(smdk4x12_devices));
	if (soc_is_exynos4412())
		platform_add_devices(smdk4412_devices, ARRAY_SIZE(smdk4412_devices));

#ifdef CONFIG_S3C64XX_DEV_SPI
	sclk = clk_get(spi0_dev, "dout_spi0");
	if (IS_ERR(sclk))
		dev_err(spi0_dev, "failed to get sclk for SPI-0\n");
	prnt = clk_get(spi0_dev, "mout_mpll_user");
	if (IS_ERR(prnt))
		dev_err(spi0_dev, "failed to get prnt\n");
	if (clk_set_parent(sclk, prnt))
		printk(KERN_ERR "Unable to set parent %s of clock %s.\n",
				prnt->name, sclk->name);

	clk_set_rate(sclk, 800 * 1000 * 1000);
	clk_put(sclk);
	clk_put(prnt);

	if (!gpio_request(EXYNOS4_GPB(1), "SPI_CS0")) {
		gpio_direction_output(EXYNOS4_GPB(1), 1);
		s3c_gpio_cfgpin(EXYNOS4_GPB(1), S3C_GPIO_SFN(1));
		s3c_gpio_setpull(EXYNOS4_GPB(1), S3C_GPIO_PULL_UP);
		exynos_spi_set_info(0, EXYNOS_SPI_SRCCLK_SCLK,
			ARRAY_SIZE(spi0_csi));
	}

	spi_register_board_info(spi0_board_info, ARRAY_SIZE(spi0_board_info));

#ifndef CONFIG_FB_S5P_LMS501KF03
	sclk = clk_get(spi1_dev, "dout_spi1");
	if (IS_ERR(sclk))
		dev_err(spi1_dev, "failed to get sclk for SPI-1\n");
	prnt = clk_get(spi1_dev, "mout_mpll_user");
	if (IS_ERR(prnt))
		dev_err(spi1_dev, "failed to get prnt\n");
	if (clk_set_parent(sclk, prnt))
		printk(KERN_ERR "Unable to set parent %s of clock %s.\n",
				prnt->name, sclk->name);

	clk_set_rate(sclk, 800 * 1000 * 1000);
	clk_put(sclk);
	clk_put(prnt);

	if (!gpio_request(EXYNOS4_GPB(5), "SPI_CS1")) {
		gpio_direction_output(EXYNOS4_GPB(5), 1);
		s3c_gpio_cfgpin(EXYNOS4_GPB(5), S3C_GPIO_SFN(1));
		s3c_gpio_setpull(EXYNOS4_GPB(5), S3C_GPIO_PULL_UP);
		exynos_spi_set_info(1, EXYNOS_SPI_SRCCLK_SCLK,
			ARRAY_SIZE(spi1_csi));
	}

	spi_register_board_info(spi1_board_info, ARRAY_SIZE(spi1_board_info));
#endif

	sclk = clk_get(spi2_dev, "dout_spi2");
	if (IS_ERR(sclk))
		dev_err(spi2_dev, "failed to get sclk for SPI-2\n");
	prnt = clk_get(spi2_dev, "mout_mpll_user");
	if (IS_ERR(prnt))
		dev_err(spi2_dev, "failed to get prnt\n");
	if (clk_set_parent(sclk, prnt))
		printk(KERN_ERR "Unable to set parent %s of clock %s.\n",
				prnt->name, sclk->name);

	clk_set_rate(sclk, 800 * 1000 * 1000);
	clk_put(sclk);
	clk_put(prnt);

	if (!gpio_request(EXYNOS4_GPC1(2), "SPI_CS2")) {
		gpio_direction_output(EXYNOS4_GPC1(2), 1);
		s3c_gpio_cfgpin(EXYNOS4_GPC1(2), S3C_GPIO_SFN(1));
		s3c_gpio_setpull(EXYNOS4_GPC1(2), S3C_GPIO_PULL_UP);
		exynos_spi_set_info(2, EXYNOS_SPI_SRCCLK_SCLK,
			ARRAY_SIZE(spi2_csi));
	}

	spi_register_board_info(spi2_board_info, ARRAY_SIZE(spi2_board_info));
#endif
#ifdef CONFIG_VIBRATOR
	mldk_vibrator_register();
#endif
#ifdef CONFIG_GPIO_DUV060_EPORT
	duv060_eport_register();
#endif
#ifdef CONFIG_LEDS_DUV060
	duv060_leds_register();
#endif
#ifdef CONFIG_BUSFREQ_OPP
	dev_add(&busfreq, &exynos4_busfreq.dev);
	ppmu_init(&exynos_ppmu[PPMU_DMC0], &exynos4_busfreq.dev);
	ppmu_init(&exynos_ppmu[PPMU_DMC1], &exynos4_busfreq.dev);
	ppmu_init(&exynos_ppmu[PPMU_CPU], &exynos4_busfreq.dev);
#endif

	register_reboot_notifier(&exynos4_reboot_notifier);

#ifdef CONFIG_DUV060_UBLOX_GPS_POWER
	/* Set UART2 (ttySAC2) gpio pull */
	if (gpio_request(EXYNOS4_GPA1(4), "UART_3_RXD"))
		printk(KERN_WARNING "UART_3_RXD Port request error!!!\n");
	else	{
		s3c_gpio_setpull(EXYNOS4_GPA1(4), S3C_GPIO_PULL_NONE);
	}
	if (gpio_request(EXYNOS4_GPA1(5), "UART_3_TXD"))
		printk(KERN_WARNING "UART_3_TXD Port request error!!!\n");
	else	{
		s3c_gpio_setpull(EXYNOS4_GPA1(5), S3C_GPIO_PULL_NONE);
	}
#endif
#if defined (CONFIG_SERIAL_8250_EXAR_XR16M265x)  || \
	defined (CONFIG_SERIAL_8250_EXAR_XR16M275x)
/*	duv060_xr16m2x5x_init();	*/
#endif
#ifdef CONFIG_AH397_WB_MODULE
	duv060_aw_ah397_init();
#endif
#ifdef CONFIG_MTK_COMBO
    MTK_COMBO_WB_POWER_INIT();
#endif

}

#ifdef CONFIG_EXYNOS_C2C
static void __init exynos_c2c_reserve(void)
{
	static struct cma_region region[] = {
		{
			.name = "c2c_shdmem",
			.size = 64 * SZ_1M,
			{ .alignment    = 64 * SZ_1M },
			.start = C2C_SHAREDMEM_BASE
		}, {
		.size = 0,
		}
	};

	s5p_cma_region_reserve(region, NULL, 0, NULL);
}
#endif

MACHINE_START(SMDK4412, "duv060")
	.boot_params	= S5P_PA_SDRAM + 0x100,
	.init_irq	= exynos4_init_irq,
	.map_io		= smdk4x12_map_io,
	.init_machine	= smdk4x12_machine_init,
	.timer		= &exynos4_timer,
#ifdef CONFIG_EXYNOS_C2C
	.reserve	= &exynos_c2c_reserve,
#endif
MACHINE_END
