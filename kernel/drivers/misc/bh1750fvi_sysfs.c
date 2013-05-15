
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/interrupt.h>
#include <linux/mutex.h>
#include <linux/regulator/consumer.h>
#include <linux/pm_runtime.h>
#include <linux/workqueue.h>
#include <linux/delay.h>
#include <linux/wait.h>
#include <linux/slab.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/bh1750fvi_pdata.h>
#include "bh1750fvi_sysfs.h"

static struct workqueue_struct *rohm_workqueue;

struct bh1750_data		*light_sensor;

static int bh1750fvi_get_register(struct bh1750_data *sensor, u8 cmd_size, u8 *cmd)
{
	struct i2c_client *i2c = sensor->client;
	int ret = 0;

	ret = i2c_master_recv(i2c, (char *)cmd, cmd_size);
	if (ret != cmd_size)
		return -EIO;

	return 0;
}

static int bh1750fvi_set_register(struct bh1750_data *sensor, u8 cmd_size, u8 *cmd)
{
	struct i2c_client *i2c = sensor->client;
	int ret = 0;

/*	printk("bh1750fvi_set_register::i2c->addr = 0x%x\n", i2c->addr << 1);	*/

	ret = i2c_master_send(i2c, (const char *)cmd, cmd_size);
	if (ret != cmd_size)
		return -EIO;

	return 0;
}

static void bh1750fvi_power(int on)
{
	u8	cmd = 0;

	if (on)
	{
		cmd = BH1750_CMD_POWER_ON;
		bh1750fvi_set_register(light_sensor, 1, &cmd);
	}
	else
	{
		cmd = BH1750_CMD_POWER_DOWN;
		bh1750fvi_set_register(light_sensor, 1, &cmd);
	}
}

static u16 bh1750fvi_get_one_hres_mode(void)
{
	u8	cmd = 0;
	u8	buffer[2];
	u16	lx = 0;
	
	/* Get value from 0010_0000 One tim H-Resolution Mode	*/
	cmd = BH1750_CMD_ONE_H_RES;
	bh1750fvi_set_register(light_sensor, 1, &cmd);
	bh1750fvi_get_register(light_sensor, sizeof(buffer), buffer);

	lx = (buffer[0] << 8) | buffer[1];
/*
	printk("bh1750fvi_get_one_hres_mode::0x%02x%02x\n", buffer[0], buffer[1]);
	printk("bh1750fvi_get_one_hres_mode::lx = 0x%04x\n", lx);
*/
	return lx;
}

/*
 * /sys/kernel/lightsensor  In that directory, 1 files are created: "lx"
 * The "lx" file where a static variable is read from and written to.
 */
static ssize_t lx_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	u16	lx = 0;
	int	i = 0;
/*
	light_sensor->pdata->power(1);
	msleep(50);
*/
	for (i = 0; i < 2; i ++)
	{
		/* Power on	*/
		bh1750fvi_power(1);

		lx = bh1750fvi_get_one_hres_mode();

		if (lx > 0)
			break;
		else
			msleep(500);
	}

	light_sensor->lx = (int)((lx*10)/12);

/*	printk("%d, 0x%x\n", light_sensor->lx, light_sensor->lx);	*/

	sprintf(buf, "%d\n", light_sensor->lx);
/*	printk("buf = %s\n", buf);	*/

/*	light_sensor->pdata->power(0);	*/

	return strlen(buf);
}

static struct kobj_attribute lx_attribute = 
	__ATTR(lx, 0666, lx_show, NULL);

/*
 * Create a group of attributes so that we can create and destroy them all
 * at once.
 */
static struct attribute *attrs[] = {
	&lx_attribute.attr,
	NULL,				/* need to NULL terminate the list of attributes */
};

/*
 * An unnamed attribute group will put all of the attributes directly in
 * the kobject directory.  If we specify a name, a subdirectory will be
 * created for the attributes with the directory being the name of the
 * attribute group.
 */
static struct attribute_group attr_group = {
	.attrs = attrs,
};

/******************************************************************************
 * NAME       : als_probe
 * FUNCTION   : initialize system
 * REMARKS    :
 *****************************************************************************/
static int als_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
    int      result = -1;

	/* private data allocation and initialization */
	light_sensor = kzalloc(sizeof(struct bh1750_data), GFP_KERNEL);
	if (!light_sensor)
	{
		result = -ENOMEM;
		goto err_initial;
	}

	light_sensor->lx_kobj = kobject_create_and_add(BH1750FVI_NAME, kernel_kobj);
	if (!light_sensor->lx_kobj)
	{
		result = -ENOMEM;
		goto err_op_failed;
	}

	result = sysfs_create_group(light_sensor->lx_kobj, &attr_group);
	if (result)
	{
		result = -ENOMEM;
		goto err_op_failed;
	}

	light_sensor->pdata = client->dev.platform_data;
	mutex_init(&light_sensor->lock);
	light_sensor->client = client;
	i2c_set_clientdata(client, light_sensor);

	light_sensor->pdata->power(1);

	return 0;

err_op_failed:
	kobject_put(light_sensor->lx_kobj);
	kfree(light_sensor);
err_initial:

	return result;
}

/******************************************************************************
 * NAME       : als_remove
 * FUNCTION   : close system
 * REMARKS    :
 *****************************************************************************/
static int als_remove(struct i2c_client *client)
{
	light_sensor->pdata->power(0);

	sysfs_remove_group(light_sensor->lx_kobj, &attr_group);
	kobject_put(light_sensor->lx_kobj);
	kfree(light_sensor);

	return 0;
}

/**************************** structure declaration ****************************/
/* I2C device IDs supported by this driver */
static const struct i2c_device_id als_id[] = {
    { BH1750_I2C_NAME, 0 }, /* rohm bh1750fvi driver */
    { }
};

/* represent an I2C device driver */
static struct i2c_driver bh1750_driver = {
    .driver = {                   /* device driver model driver */
        .name = BH1750_I2C_NAME,
    },
    .probe    = als_probe,        /* callback for device binding */
    .remove   = als_remove,       /* callback for device unbinding */
    .id_table = als_id,           /* list of I2C devices supported by this driver */
};

/******************************************************************************
 * NAME       : als_init
 * FUNCTION   : register driver to kernel
 * REMARKS    :
 *****************************************************************************/

static int __devinit als_init(void)
{
	rohm_workqueue = create_singlethread_workqueue("rohm_workqueue");
	if (!rohm_workqueue) {
		return (-ENOMEM);
	}

	return (i2c_add_driver(&bh1750_driver));
}

/******************************************************************************
 * NAME       : als_exit
 * FUNCTION   : remove driver from kernel
 * REMARKS    :
 *****************************************************************************/
static void __exit als_exit(void)
{
	i2c_del_driver(&bh1750_driver);
	if (rohm_workqueue) {
		destroy_workqueue(rohm_workqueue);
	}

	return;
}

MODULE_DESCRIPTION("ROHM Ambient Light Sensor Driver");
MODULE_LICENSE("GPL");

module_init(als_init);
module_exit(als_exit);
