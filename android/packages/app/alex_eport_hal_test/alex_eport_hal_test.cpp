
#include <string.h>
#include <stdint.h>
#include <string.h>
#include <sys/cdefs.h>
#include <sys/types.h>
#include <cutils/log.h>
#include <utils/Timers.h>
#include "alex_eport_hal_test.h"

static int		io_3v_on = 0;
static int		io_5v_on = 0;
static int		io_gpio01_on = 0;
static int		io_gpio02_on = 0;
static int		io_gpio03_on = 0;
static int		io_gpio04_on = 0;
static int		io_gpio05_on = 0;
static int		io_gpio06_on = 0;
static int		ir_tx1_on = 0;
static int		ir_tx2_on = 0;

int main(int argc, char** argv)
{
	int		ret = 0;
	int		err = 0;
	bool	exit = false;
	char	buf;
	char	tmp_freq[20];

	struct duv060_eport_module_t	*module;
	struct duv060_eport_device_t	*device;
	
	printf("alex_eport_hal_test::start!\r\n");

	err = hw_get_module(DUV060_EPORT_MODULE_ID, (hw_module_t const**)&module);
	if (err != 0)
	{
		printf("alex_eport_hal_test::hw_get_module(%s) failed (%s)\n", DUV060_EPORT_MODULE_ID, strerror(-err));
		ret = -1;
		goto finish;
	}

	err = libeport_open(&module->common, &device);
	if (err != 0)
	{
		printf("alex_eport_hal_test::libfm_open() failed (%s)\n", strerror(-err));
		ret = -1;
		goto finish;
	}

	while (!exit)
	{
		io_3v_on = device->io_3v_get();
		io_5v_on = device->io_5v_get();
		io_gpio01_on = device->io_gpio_01_get();
		io_gpio02_on = device->io_gpio_02_get();
		io_gpio03_on = device->io_gpio_03_get();
		io_gpio04_on = device->io_gpio_04_get();
		io_gpio05_on = device->io_gpio_05_get();
		io_gpio06_on = device->io_gpio_06_get();
		ir_tx1_on = device->ir_tx1_on_get();
		ir_tx2_on = device->ir_tx2_on_get();

		printf("\n+++ alex_eport_hal_test +++\n");
		
		printf("\n");
		printf("[A] io_3v: %d\n", io_3v_on);
		printf("[B] io_5v: %d\n", io_5v_on);
		printf("[1] io_gpio01: %d\n", io_gpio01_on);
		printf("[2] io_gpio02: %d\n", io_gpio02_on);
		printf("[3] io_gpio03: %d\n", io_gpio03_on);
		printf("[4] io_gpio04: %d\n", io_gpio04_on);
		printf("[5] io_gpio05: %d\n", io_gpio05_on);
		printf("[6] io_gpio06: %d\n", io_gpio06_on);
		printf("[7] ir_tx1: %d\n", ir_tx1_on);
		printf("[8] ir_tx2: %d\n", ir_tx2_on);
		printf("[Q] Quit\n");

		printf("--- alex_eport_hal_test ---\n\n");

		buf = getchar();

		switch (buf)
		{
		case '1':
			io_gpio01_on = (io_gpio01_on == 1) ? 0 : 1;
			device->io_gpio_01_set(io_gpio01_on);
			break;
			
		case '2':
			io_gpio02_on = (io_gpio02_on == 1) ? 0 : 1;
			device->io_gpio_02_set(io_gpio02_on);
			break;

		case '3':
			io_gpio03_on = (io_gpio03_on == 1) ? 0 : 1;
			device->io_gpio_03_set(io_gpio03_on);
			break;

		case '4':
			io_gpio04_on = (io_gpio04_on == 1) ? 0 : 1;
			device->io_gpio_04_set(io_gpio04_on);
			break;

		case '5':
			io_gpio05_on = (io_gpio05_on == 1) ? 0 : 1;
			device->io_gpio_05_set(io_gpio05_on);
			break;

		case '6':
			io_gpio06_on = (io_gpio06_on == 1) ? 0 : 1;
			device->io_gpio_06_set(io_gpio06_on);
			break;

		case '7':
			ir_tx1_on = (ir_tx1_on == 1) ? 0 : 1;
			device->ir_tx1_on_set(ir_tx1_on);
			break;

		case '8':
			ir_tx2_on = (ir_tx2_on == 1) ? 0 : 1;
			device->ir_tx2_on_set(ir_tx2_on);
			break;

		case 'A':
		case 'a':
			io_3v_on = (io_3v_on == 1) ? 0 : 1;
			device->io_3v_set(io_3v_on);
			break;
			
		case 'B':
		case 'b':
			io_5v_on = (io_5v_on == 1) ? 0 : 1;
			device->io_5v_set(io_5v_on);
			break;
			
		case 'q':
		case 'Q':
			exit = true;
			goto finish;
			break;
		}
	}

finish:

	if (ret != 0)
		printf("alex_eport_hal_test::failed!\r\n");

	err = libeport_close(device);
	if (err != 0)
		printf("alex_eport_hal_test::libfm_close() failed (%s)\n", strerror(-err));

	printf("alex_eport_hal_test::leave!\r\n");

	return ret;
}

