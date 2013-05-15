
/*
 *  Written by Kevin Chen< k.chen "at" dataltd "dot" com >
 *
 *	
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/interrupt.h>
#include <linux/input.h>
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
#include "bh1750fvi.h"

static struct class *lightsensor_class;

static int bh1750fvi_get_register(struct bh1750_data *light_sensor, u8 cmd_size, u8 *cmd)
{
	struct i2c_client *i2c = light_sensor->client;
	int ret = 0;

	ret = i2c_master_recv(i2c, (char *)cmd, cmd_size);
	if (ret != cmd_size)
		return -EIO;

	return 0;
}

static int bh1750fvi_set_register(struct bh1750_data *light_sensor, u8 cmd_size, u8 *cmd)
{
	struct i2c_client *i2c = light_sensor->client;
	int ret = 0;

/*	printk("bh1750fvi_set_register::i2c->addr = 0x%x\n", i2c->addr << 1);	*/

	ret = i2c_master_send(i2c, (const char *)cmd, cmd_size);
	if (ret != cmd_size)
		return -EIO;

	return 0;
}

static void bh1750fvi_power(struct bh1750_data *light_sensor, int on)
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

static u16 bh1750fvi_get_one_hres_mode(struct bh1750_data *light_sensor)
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

static void bh1750fvi_light_enable(struct bh1750_data *light_sensor)
{
	light_sensor->pdata->power(1);
	light_sensor->power_state = 1;

	hrtimer_start(&light_sensor->light_timer, light_sensor->light_poll_delay, HRTIMER_MODE_REL);
}

static void bh1750fvi_light_disable(struct bh1750_data *light_sensor)
{
	hrtimer_cancel(&light_sensor->light_timer);
	cancel_work_sync(&light_sensor->work_light);

	light_sensor->pdata->power(0);
	light_sensor->power_state = 0;
	light_sensor->lx = 0;
}

static ssize_t bh1750fvi_poll_delay_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct bh1750_data *light_sensor = dev_get_drvdata(dev);
	return sprintf(buf, "%lld\n", ktime_to_ns(light_sensor->light_poll_delay));
}

static ssize_t bh1750fvi_poll_delay_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t size)
{
	struct bh1750_data *light_sensor = dev_get_drvdata(dev);
	int64_t new_delay;
	int err;

	err = strict_strtoll(buf, 10, &new_delay);
	if (err < 0)
		return err;

	if (new_delay != ktime_to_ns(light_sensor->light_poll_delay))
	{
/*		light_sensor->light_poll_delay = ns_to_ktime(new_delay);	*/
		light_sensor->light_poll_delay = ns_to_ktime(new_delay*2);
		
		if (light_sensor->power_state == 1)
		{
			hrtimer_cancel(&light_sensor->light_timer);
			cancel_work_sync(&light_sensor->work_light);

			/* Restart timer */
			hrtimer_start(&light_sensor->light_timer, light_sensor->light_poll_delay, HRTIMER_MODE_REL);
		}
	}

	return size;
}

static ssize_t light_enable_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct bh1750_data *light_sensor = dev_get_drvdata(dev);
	return sprintf(buf, "%d\n", light_sensor->power_state);
}

static ssize_t light_enable_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t size)
{
	struct bh1750_data *light_sensor = dev_get_drvdata(dev);
	int	power_state = 0;

	if (sysfs_streq(buf, "1"))
		power_state = 1;
	else if (sysfs_streq(buf, "0"))
		power_state = 0;
	else
	{
		printk("invalid value %d\n", *buf);
		return -EINVAL;
	}

	mutex_lock(&light_sensor->lock);

	if (power_state == 0)
	{
		bh1750fvi_light_disable(light_sensor);
	}
	else
	{
		bh1750fvi_light_enable(light_sensor);
	}

	mutex_unlock(&light_sensor->lock);

	return size;
}

static ssize_t light_lux_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct bh1750_data *light_sensor = dev_get_drvdata(dev);
	return sprintf(buf, "%d\n", light_sensor->lx);
}

static ssize_t light_data_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct bh1750_data *light_sensor = dev_get_drvdata(dev);
	return sprintf(buf, "%d\n", light_sensor->lx);
}

/* sysfs for vendor & name */
static ssize_t bh1750fvi_vendor_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "%s\n", BH1750FVI_VENDOR);
}

static ssize_t bh1750fvi_name_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "%s\n", BH1750FVI_CHIP_ID);
}

static DEVICE_ATTR(poll_delay, S_IRUGO | S_IWUSR | S_IWGRP, bh1750fvi_poll_delay_show, bh1750fvi_poll_delay_store);

static struct device_attribute dev_attr_light_enable =
__ATTR(enable, S_IRUGO | S_IWUSR | S_IWGRP,
	light_enable_show, light_enable_store);

static struct attribute *light_sysfs_attrs[] = {
	&dev_attr_light_enable.attr,
	&dev_attr_poll_delay.attr,
	NULL
};

static struct attribute_group light_attribute_group = {
	.attrs = light_sysfs_attrs,
};

static DEVICE_ATTR(lux, 0644, light_lux_show, NULL);
static DEVICE_ATTR(raw_data, 0644, light_data_show, NULL);
static DEVICE_ATTR(vendor, 0644, bh1750fvi_vendor_show, NULL);
static DEVICE_ATTR(name, 0644, bh1750fvi_name_show, NULL);

static enum hrtimer_restart bh1750fvi_light_timer_func(struct hrtimer *timer)
{
	struct bh1750_data *light_sensor = container_of(timer, struct bh1750_data, light_timer);
	queue_work(light_sensor->light_wq, &light_sensor->work_light);
	hrtimer_forward_now(&light_sensor->light_timer, light_sensor->light_poll_delay);
	return HRTIMER_RESTART;
}

static void bh1750fvi_work_func_light(struct work_struct *work)
{
	struct bh1750_data *light_sensor = container_of(work, struct bh1750_data, work_light);
	u16	lx = 0;
	int	i = 0;

	mutex_lock(&light_sensor->lock);

	for (i = 0; i < 2; i ++)
	{
		/* Power on	*/
		bh1750fvi_power(light_sensor, 1);

		lx = bh1750fvi_get_one_hres_mode(light_sensor);

		if (lx > 0)
			break;
		else
			msleep(500);
	}

	mutex_unlock(&light_sensor->lock);

	light_sensor->lx = (int)((lx*10)/12);
	light_sensor->lx = light_sensor->lx*50;

	input_report_abs(light_sensor->dev, ABS_MISC, light_sensor->lx);
	input_sync(light_sensor->dev);
/*	printk("bh1750fvi_work_func_light::%d\n", light_sensor->lx);	*/
}

/******************************************************************************
 * NAME       : bh1750fvi_probe
 * FUNCTION   : initialize system
 * REMARKS    :
 *****************************************************************************/
static int bh1750fvi_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
    int      result = -1;
	struct bh1750_data	*light_sensor = NULL;

	/* private data allocation and initialization */
	light_sensor = kzalloc(sizeof(struct bh1750_data), GFP_KERNEL);
	if (!light_sensor)
	{
		result = -ENOMEM;
		goto err_initial;
	}
	
	light_sensor->dev = input_allocate_device();
	if (!light_sensor->dev)
	{
		result = -ENOMEM;
		goto err_op_failed;
	}

	light_sensor->dev->name = BH1750FVI_NAME;
	set_bit(EV_SYN, light_sensor->dev->evbit);
	set_bit(EV_ABS, light_sensor->dev->evbit);
	input_set_abs_params(light_sensor->dev, ABS_MISC, 0, 65535, 0, 0);

	input_set_drvdata(light_sensor->dev, light_sensor);

	light_sensor->pdata = client->dev.platform_data;

	result = input_register_device(light_sensor->dev);
	if (result < 0)
	{
		goto err_op_failed;
	}

	result = sysfs_create_group(&light_sensor->dev->dev.kobj, &light_attribute_group);
	if (result)
	{
		goto err_op_failed;
	}

	/* light_timer settings. we poll for light values using a timer. */
	hrtimer_init(&light_sensor->light_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
	light_sensor->light_poll_delay = ns_to_ktime(light_sensor->pdata->timeout*NSEC_PER_MSEC);
	light_sensor->light_timer.function = bh1750fvi_light_timer_func;

	/* the timer just fires off a work queue request.  we need a thread
	   to read the i2c (can be slow and blocking). */
	light_sensor->light_wq = create_singlethread_workqueue("bh1750fvi_light_wq");
	if (!light_sensor->light_wq)
	{
		goto err_op_failed;
	}

	/* this is the thread function we run on the work queue */
	INIT_WORK(&light_sensor->work_light, bh1750fvi_work_func_light);

	/* set sysfs for light sensor */
	light_sensor->light_dev = device_create(lightsensor_class, NULL, 0, NULL, "%s", BH1750FVI_NAME);

	if (IS_ERR(light_sensor->light_dev))
	{
		goto err_op_failed;
	}

	if (device_create_file(light_sensor->light_dev, &dev_attr_lux) < 0)
	{
		goto err_op_failed;
	}

	if (device_create_file(light_sensor->light_dev, &dev_attr_raw_data) < 0)
	{
		goto err_op_failed;
	}

	if (device_create_file(light_sensor->light_dev, &dev_attr_vendor) < 0)
	{
		goto err_op_failed;
	}

	if (device_create_file(light_sensor->light_dev, &dev_attr_name) < 0)
	{
		goto err_op_failed;
	}

	mutex_init(&light_sensor->lock);
	light_sensor->client = client;
	i2c_set_clientdata(client, light_sensor);
	dev_set_drvdata(light_sensor->light_dev, light_sensor);

	bh1750fvi_light_enable(light_sensor);

	return 0;

err_op_failed:
	destroy_workqueue(light_sensor->light_wq);
	device_remove_file(light_sensor->light_dev, &dev_attr_name);
	device_remove_file(light_sensor->light_dev, &dev_attr_vendor);
	device_remove_file(light_sensor->light_dev, &dev_attr_raw_data);
	device_remove_file(light_sensor->light_dev, &dev_attr_lux);
	sysfs_remove_group(&light_sensor->dev->dev.kobj, &light_attribute_group);
	device_unregister(light_sensor->light_dev);
	input_free_device(light_sensor->dev);
	kfree(light_sensor);
err_initial:

	return result;
}

/******************************************************************************
 * NAME       : bh1750fvi_remove
 * FUNCTION   : close system
 * REMARKS    :
 *****************************************************************************/
static int bh1750fvi_remove(struct i2c_client *client)
{
	struct bh1750_data *light_sensor = i2c_get_clientdata(client);;

	bh1750fvi_light_disable(light_sensor);

	destroy_workqueue(light_sensor->light_wq);
	device_remove_file(light_sensor->light_dev, &dev_attr_name);
	device_remove_file(light_sensor->light_dev, &dev_attr_vendor);
	device_remove_file(light_sensor->light_dev, &dev_attr_raw_data);
	device_remove_file(light_sensor->light_dev, &dev_attr_lux);
	sysfs_remove_group(&light_sensor->dev->dev.kobj, &light_attribute_group);
	device_unregister(light_sensor->light_dev);
	input_free_device(light_sensor->dev);
	kfree(light_sensor);

	return 0;
}

static int bh1750fvi_suspend(struct device *dev)
{
	struct bh1750_data *light_sensor = dev_get_drvdata(dev);;

	bh1750fvi_light_disable(light_sensor);

	return 0;
}

static int bh1750fvi_resume(struct device *dev)
{
	struct bh1750_data *light_sensor = dev_get_drvdata(dev);;
	
	bh1750fvi_light_enable(light_sensor);

	return 0;
}

/**************************** structure declaration ****************************/
/* I2C device IDs supported by this driver */
static const struct i2c_device_id bh1750fvi_id[] = {
    { BH1750_I2C_NAME, 0 }, /* rohm bh1750fvi driver */
    { }
};

static const struct dev_pm_ops bh1750fvi_pm_ops = {
	.suspend = bh1750fvi_suspend,
	.resume = bh1750fvi_resume
};

/* represent an I2C device driver */
static struct i2c_driver bh1750_driver = {
    .driver = {                   /* device driver model driver */
        .name = BH1750_I2C_NAME, 
        .owner = THIS_MODULE, 
        .pm = &bh1750fvi_pm_ops, 
    },
    .probe    = bh1750fvi_probe,        /* callback for device binding */
    .remove   = bh1750fvi_remove,       /* callback for device unbinding */
    .id_table = bh1750fvi_id,           /* list of I2C devices supported by this driver */
};

/******************************************************************************
 * NAME       : bh1750fvi_init
 * FUNCTION   : register driver to kernel
 * REMARKS    :
 *****************************************************************************/
static int __devinit bh1750fvi_init(void)
{
	lightsensor_class = class_create(THIS_MODULE, "sensors");

	return (i2c_add_driver(&bh1750_driver));
}

/******************************************************************************
 * NAME       : bh1750fvi_exit
 * FUNCTION   : remove driver from kernel
 * REMARKS    :
 *****************************************************************************/
static void __exit bh1750fvi_exit(void)
{
	i2c_del_driver(&bh1750_driver);
	class_destroy(lightsensor_class);

	return;
}

MODULE_DESCRIPTION("ROHM Ambient Light Sensor Driver");
MODULE_LICENSE("GPL");

module_init(bh1750fvi_init);
module_exit(bh1750fvi_exit);
