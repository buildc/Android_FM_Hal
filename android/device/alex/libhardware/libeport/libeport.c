
#include <libeport.h>

static void duv060_io_3v_set(int on)
{
	int	fd = open(DUV060_EPORT_IO_3V, O_RDWR);
	if (fd >= 0)
	{
		if (on != 0)
			on = 1;
		write(fd, on ? "1" : "0", 2);
		close(fd);
	}
}

static int duv060_io_3v_get()
{
	int		on = 0;
	char	buf[16] = {0};
	int		fd = open(DUV060_EPORT_IO_3V, O_RDWR);
	if (fd >= 0)
	{
		read(fd, buf, sizeof(buf));
		if (strcmp(buf, "0\n") == 0)
			on = 0;
		else
			on = 1;
		close(fd);
	}

	return on;
}

static void duv060_io_5v_set(int on)
{
	int	fd = open(DUV060_EPORT_IO_5V, O_RDWR);
	if (fd >= 0)
	{
		if (on != 0)
			on = 1;
		write(fd, on ? "1" : "0", 2);
		close(fd);
	}
}

static int duv060_io_5v_get()
{
	int		on = 0;
	char	buf[16] = {0};
	int		fd = open(DUV060_EPORT_IO_5V, O_RDWR);
	if (fd >= 0)
	{
		read(fd, buf, sizeof(buf));
		if (strcmp(buf, "0\n") == 0)
			on = 0;
		else
			on = 1;
		close(fd);
	}

	return on;
}

static void duv060_io_gpio_01_set(int on)
{
	int	fd = open(DUV060_EPORT_IO_GPIO_01, O_RDWR);
	if (fd >= 0)
	{
		if (on != 0)
			on = 1;
		write(fd, on ? "1" : "0", 2);
		close(fd);
	}
}

static int duv060_io_gpio_01_get()
{
	int		on = 0;
	char	buf[16] = {0};
	int		fd = open(DUV060_EPORT_IO_GPIO_01, O_RDWR);
	if (fd >= 0)
	{
		read(fd, buf, sizeof(buf));
		if (strcmp(buf, "0\n") == 0)
			on = 0;
		else
			on = 1;
		close(fd);
	}

	return on;
}

static void duv060_io_gpio_02_set(int on)
{
	int	fd = open(DUV060_EPORT_IO_GPIO_02, O_RDWR);
	if (fd >= 0)
	{
		if (on != 0)
			on = 1;
		write(fd, on ? "1" : "0", 2);
		close(fd);
	}
}

static int duv060_io_gpio_02_get()
{
	int		on = 0;
	char	buf[16] = {0};
	int		fd = open(DUV060_EPORT_IO_GPIO_02, O_RDWR);
	if (fd >= 0)
	{
		read(fd, buf, sizeof(buf));
		if (strcmp(buf, "0\n") == 0)
			on = 0;
		else
			on = 1;
		close(fd);
	}

	return on;
}

static void duv060_io_gpio_03_set(int on)
{
	int	fd = open(DUV060_EPORT_IO_GPIO_03, O_RDWR);
	if (fd >= 0)
	{
		if (on != 0)
			on = 1;
		write(fd, on ? "1" : "0", 2);
		close(fd);
	}
}

static int duv060_io_gpio_03_get()
{
	int		on = 0;
	char	buf[16] = {0};
	int		fd = open(DUV060_EPORT_IO_GPIO_03, O_RDWR);
	if (fd >= 0)
	{
		read(fd, buf, sizeof(buf));
		if (strcmp(buf, "0\n") == 0)
			on = 0;
		else
			on = 1;
		close(fd);
	}

	return on;
}

static void duv060_io_gpio_04_set(int on)
{
	int	fd = open(DUV060_EPORT_IO_GPIO_04, O_RDWR);
	if (fd >= 0)
	{
		if (on != 0)
			on = 1;
		write(fd, on ? "1" : "0", 2);
		close(fd);
	}
}

static int duv060_io_gpio_04_get()
{
	int		on = 0;
	char	buf[16] = {0};
	int		fd = open(DUV060_EPORT_IO_GPIO_04, O_RDWR);
	if (fd >= 0)
	{
		read(fd, buf, sizeof(buf));
		if (strcmp(buf, "0\n") == 0)
			on = 0;
		else
			on = 1;
		close(fd);
	}

	return on;
}

static void duv060_io_gpio_05_set(int on)
{
	int	fd = open(DUV060_EPORT_IO_GPIO_05, O_RDWR);
	if (fd >= 0)
	{
		if (on != 0)
			on = 1;
		write(fd, on ? "1" : "0", 2);
		close(fd);
	}
}

static int duv060_io_gpio_05_get()
{
	int		on = 0;
	char	buf[16] = {0};
	int		fd = open(DUV060_EPORT_IO_GPIO_05, O_RDWR);
	if (fd >= 0)
	{
		read(fd, buf, sizeof(buf));
		if (strcmp(buf, "0\n") == 0)
			on = 0;
		else
			on = 1;
		close(fd);
	}

	return on;
}

static void duv060_io_gpio_06_set(int on)
{
	int	fd = open(DUV060_EPORT_IO_GPIO_06, O_RDWR);
	if (fd >= 0)
	{
		if (on != 0)
			on = 1;
		write(fd, on ? "1" : "0", 2);
		close(fd);
	}
}

static int duv060_io_gpio_06_get()
{
	int		on = 0;
	char	buf[16] = {0};
	int		fd = open(DUV060_EPORT_IO_GPIO_06, O_RDWR);
	if (fd >= 0)
	{
		read(fd, buf, sizeof(buf));
		if (strcmp(buf, "0\n") == 0)
			on = 0;
		else
			on = 1;
		close(fd);
	}

	return on;
}

static void duv060_ir_tx1_on_set(int on)
{
	int	fd = open(DUV060_EPORT_IR_TX1_ON, O_RDWR);
	if (fd >= 0)
	{
		if (on != 0)
			on = 1;
		write(fd, on ? "1" : "0", 2);
		close(fd);
	}
}

static int duv060_ir_tx1_on_get()
{
	int		on = 0;
	char	buf[16] = {0};
	int		fd = open(DUV060_EPORT_IR_TX1_ON, O_RDWR);
	if (fd >= 0)
	{
		read(fd, buf, sizeof(buf));
		if (strcmp(buf, "0\n") == 0)
			on = 0;
		else
			on = 1;
		close(fd);
	}

	return on;
}

static void duv060_ir_tx2_on_set(int on)
{
	int	fd = open(DUV060_EPORT_IR_TX2_ON, O_RDWR);
	if (fd >= 0)
	{
		if (on != 0)
			on = 1;
		write(fd, on ? "1" : "0", 2);
		close(fd);
	}
}

static int duv060_ir_tx2_on_get()
{
	int		on = 0;
	char	buf[16] = {0};
	int		fd = open(DUV060_EPORT_IR_TX2_ON, O_RDWR);
	if (fd >= 0)
	{
		read(fd, buf, sizeof(buf));
		if (strcmp(buf, "0\n") == 0)
			on = 0;
		else
			on = 1;
		close(fd);
	}

	return on;
}

static void duv060_uart_sel_set(int on)
{
	int	fd = open(DUV060_EPORT_UART_SEL, O_RDWR);
	if (fd >= 0)
	{
		if (on != 0)
			on = 1;
		write(fd, on ? "1" : "0", 2);
		close(fd);
	}
}

static int duv060_uart_sel_get()
{
	int		on = 0;
	char	buf[16] = {0};
	int		fd = open(DUV060_EPORT_UART_SEL, O_RDWR);
	if (fd >= 0)
	{
		read(fd, buf, sizeof(buf));
		if (strcmp(buf, "0\n") == 0)
			on = 0;
		else
			on = 1;
		close(fd);
	}

	return on;
}

static int duv060_eport_close(hw_device_t *dev)
{
	free(dev);
	return 0;
}

/*
 * Generic device handling
 */
static int duv060_eport_open(const hw_module_t* module, const char* name, hw_device_t** device)
{
	struct duv060_eport_device_t	*dev = malloc(sizeof(struct duv060_eport_device_t));
	memset(dev, 0, sizeof(*dev));

	dev->common.tag = HARDWARE_DEVICE_TAG;
	dev->common.version = 0;
	dev->common.module = (struct hw_module_t*)module;
	dev->common.close = duv060_eport_close;
	dev->io_3v_set = duv060_io_3v_set;
	dev->io_3v_get = duv060_io_3v_get;
	dev->io_5v_set = duv060_io_5v_set;
	dev->io_5v_get = duv060_io_5v_get;
	dev->io_gpio_01_set = duv060_io_gpio_01_set;
	dev->io_gpio_01_get = duv060_io_gpio_01_get;
	dev->io_gpio_02_set = duv060_io_gpio_02_set;
	dev->io_gpio_02_get = duv060_io_gpio_02_get;
	dev->io_gpio_03_set = duv060_io_gpio_03_set;
	dev->io_gpio_03_get = duv060_io_gpio_03_get;
	dev->io_gpio_04_set = duv060_io_gpio_04_set;
	dev->io_gpio_04_get = duv060_io_gpio_04_get;
	dev->io_gpio_05_set = duv060_io_gpio_05_set;
	dev->io_gpio_05_get = duv060_io_gpio_05_get;
	dev->io_gpio_06_set = duv060_io_gpio_06_set;
	dev->io_gpio_06_get = duv060_io_gpio_06_get;
	dev->ir_tx1_on_set = duv060_ir_tx1_on_set;
	dev->ir_tx1_on_get = duv060_ir_tx1_on_get;
	dev->ir_tx2_on_set = duv060_ir_tx2_on_set;
	dev->ir_tx2_on_get = duv060_ir_tx2_on_get;
	dev->uart_sel_set = duv060_uart_sel_set;
	dev->uart_sel_get = duv060_uart_sel_get;

	*device = (hw_device_t*)dev;

	return 0;
}

static struct hw_module_methods_t duv060_eport_module_methods = 
{
    .open = duv060_eport_open,
};

struct duv060_eport_module_t HAL_MODULE_INFO_SYM = 
{
	.common = 
	{
		.tag = HARDWARE_MODULE_TAG, 
		.version_major = 1, 
		.version_minor = 0, 
		.id = DUV060_EPORT_MODULE_ID, 
		.name = "DUV060 EPORT HW HAL", 
		.author = "DLI", 
		.methods = &duv060_eport_module_methods, 
	},
};
