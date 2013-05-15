
#ifndef ANDROID_EPORT_HAL_INTERFACE_H
#define	ANDROID_EPORT_HAL_INTERFACE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <strings.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/cdefs.h>
#include <sys/types.h>
#include <hardware/hardware.h>

__BEGIN_DECLS

#define	DUV060_EPORT_IO_3V			"/sys/kernel/duv060_eport/io_3v"
#define	DUV060_EPORT_IO_5V			"/sys/kernel/duv060_eport/io_5v"
#define	DUV060_EPORT_IO_GPIO_01		"/sys/kernel/duv060_eport/io_gpio_01"
#define	DUV060_EPORT_IO_GPIO_02		"/sys/kernel/duv060_eport/io_gpio_02"
#define	DUV060_EPORT_IO_GPIO_03		"/sys/kernel/duv060_eport/io_gpio_03"
#define	DUV060_EPORT_IO_GPIO_04		"/sys/kernel/duv060_eport/io_gpio_04"
#define	DUV060_EPORT_IO_GPIO_05		"/sys/kernel/duv060_eport/io_gpio_05"
#define	DUV060_EPORT_IO_GPIO_06		"/sys/kernel/duv060_eport/io_gpio_06"
#define	DUV060_EPORT_IR_TX1_ON		"/sys/kernel/duv060_eport/ir_tx1_on"
#define	DUV060_EPORT_IR_TX2_ON		"/sys/kernel/duv060_eport/ir_tx2_on"
#define	DUV060_EPORT_UART_SEL		"/sys/kernel/duv060_eport/uart_sel"
#define DUV060_EPORT_MODULE_ID		"libeport"
#define	DUV060_EPORT_MODULE_NAME	"duv060_eport"

struct duv060_eport_module_t
{
	struct hw_module_t	common;
};

struct duv060_eport_device_t
{
	struct hw_device_t	common;

	void		(* io_3v_set)(int on);
	int			(* io_3v_get)(void);
	void		(* io_5v_set)(int on);
	int			(* io_5v_get)(void);
	void		(* io_gpio_01_set)(int on);
	int			(* io_gpio_01_get)(void);
	void		(* io_gpio_02_set)(int on);
	int			(* io_gpio_02_get)(void);
	void		(* io_gpio_03_set)(int on);
	int			(* io_gpio_03_get)(void);
	void		(* io_gpio_04_set)(int on);
	int			(* io_gpio_04_get)(void);
	void		(* io_gpio_05_set)(int on);
	int			(* io_gpio_05_get)(void);
	void		(* io_gpio_06_set)(int on);
	int			(* io_gpio_06_get)(void);
	void		(*ir_tx1_on_set)(int on);
	int			(*ir_tx1_on_get)(void);
	void		(*ir_tx2_on_set)(int on);
	int			(*ir_tx2_on_get)(void);
	void		(*uart_sel_set)(int on);
	int			(*uart_sel_get)(void);
};

/** convenience API for opening and closing a device */

static inline int libeport_open(const struct hw_module_t *module, struct duv060_eport_device_t **dev)
{
	return module->methods->open(module, NULL, (struct hw_device_t **)dev);
}

static inline int libeport_close(struct duv060_eport_device_t *dev)
{
	return dev->common.close(&dev->common);
}

/* API */
static inline void libeport_io_3v_set(struct duv060_eport_device_t *dev, int on)
{
	dev->io_3v_set(on);
}

static inline int libeport_io_3v_get(struct duv060_eport_device_t *dev)
{
	return dev->io_3v_get();
}

static inline void libeport_io_5v_set(struct duv060_eport_device_t *dev, int on)
{
	dev->io_5v_set(on);
}

static inline int libeport_io_5v_get(struct duv060_eport_device_t *dev)
{
	return dev->io_5v_get();
}

static inline void libeport_io_gpio_01_set(struct duv060_eport_device_t *dev, int on)
{
	dev->io_gpio_01_set(on);
}

static inline int libeport_io_gpio_01_get(struct duv060_eport_device_t *dev)
{
	return dev->io_gpio_01_get();
}

static inline void libeport_io_gpio_02_set(struct duv060_eport_device_t *dev, int on)
{
	dev->io_gpio_02_set(on);
}

static inline int libeport_io_gpio_02_get(struct duv060_eport_device_t *dev)
{
	return dev->io_gpio_02_get();
}

static inline void libeport_io_gpio_03_set(struct duv060_eport_device_t *dev, int on)
{
	dev->io_gpio_03_set(on);
}

static inline int libeport_io_gpio_03_get(struct duv060_eport_device_t *dev)
{
	return dev->io_gpio_03_get();
}

static inline void libeport_io_gpio_04_set(struct duv060_eport_device_t *dev, int on)
{
	dev->io_gpio_04_set(on);
}

static inline int libeport_io_gpio_04_get(struct duv060_eport_device_t *dev)
{
	return dev->io_gpio_04_get();
}

static inline void libeport_io_gpio_05_set(struct duv060_eport_device_t *dev, int on)
{
	dev->io_gpio_05_set(on);
}

static inline int libeport_io_gpio_05_get(struct duv060_eport_device_t *dev)
{
	return dev->io_gpio_05_get();
}

static inline void libeport_io_gpio_06_set(struct duv060_eport_device_t *dev, int on)
{
	dev->io_gpio_06_set(on);
}

static inline int libeport_io_gpio_06_get(struct duv060_eport_device_t *dev)
{
	return dev->io_gpio_06_get();
}

static inline void libeport_ir_tx1_on_set(struct duv060_eport_device_t *dev, int on)
{
	dev->ir_tx1_on_set(on);
}

static inline int libeport_ir_tx1_on_get(struct duv060_eport_device_t *dev)
{
	return dev->ir_tx1_on_get();
}

static inline void libeport_ir_tx2_on_set(struct duv060_eport_device_t *dev, int on)
{
	dev->ir_tx2_on_set(on);
}

static inline int libeport_ir_tx2_on_get(struct duv060_eport_device_t *dev)
{
	return dev->ir_tx2_on_get();
}

static inline void libeport_uart_sel_set(struct duv060_eport_device_t *dev, int on)
{
	dev->uart_sel_set(on);
}

static inline int libeport_uart_sel_get(struct duv060_eport_device_t *dev)
{
	return dev->uart_sel_get();
}

__END_DECLS

#endif	/* ANDROID_EPORT_HAL_INTERFACE_H */
