
#ifndef __BH1750FVI_H__
#define	__BH1750FVI_H__

#define	BH1750FVI_VENDOR		"rohm"
#define	BH1750FVI_CHIP_ID		"bh1750fvi"
#define	BH1750FVI_NAME			"light_sensor"

/************ definition to dependent on sensor IC ************/
#define BH1750_I2C_NAME				"bh1750_i2c"
#define BH1750_I2C_ADDRESS_H		(0xB8 >> 1)				/*	7 bits slave address 1011100b, where ADDR=H	*/
#define BH1750_I2C_ADDRESS_L		(0x46 >> 1)				/*	7 bits slave address 0100011b, where ADDR=L	*/
#define	BH1750_I2C_ADDRESS			BH1750_I2C_ADDRESS_L

#define	BH1750_CMD_POWER_DOWN		0x00		/*	0000_0000	*/
#define	BH1750_CMD_POWER_ON			0x01		/*	0000_0001	*/
#define	BH1750_CMD_RESET			0x07		/*	0000_0111	*/
#define	BH1750_CMD_CON_H_RES		0x10		/*	0001_0000	*/
#define	BH1750_CMD_CON_H_RES_2		0x11		/*	0001_0001	*/
#define	BH1750_CMD_CON_L_RES		0x13		/*	0001_0011	*/
#define	BH1750_CMD_ONE_H_RES		0x20		/*	0010_0000	*/
#define	BH1750_CMD_ONE_H_RES_2		0x21		/*	0010_0001	*/
#define	BH1750_CMD_ONE_L_RES		0x23		/*	0010_0011	*/

struct bh1750_data
{
	struct mutex	lock;
	int				power_state;
	int				lx;

	ktime_t					light_poll_delay;
	struct hrtimer			light_timer;
	struct workqueue_struct *light_wq;
	struct work_struct		work_light;
	struct i2c_client		*client;
	struct input_dev		*dev;
	struct device			*light_dev;
	struct bh1750fvi_pdata	*pdata;
};

#endif	// __BH1750FVI_H__
