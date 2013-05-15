
/*
 *  Written by Kevin Chen< k.chen "at" dataltd "dot" com >
 *
 *	
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

/* kernel includes */
#include <linux/wakelock.h>
#include <linux/si4704_pdata.h>
#include "radio-si4704.h"

extern struct si4704_device	*g_radio;

/**************************************************************************
 * Module Parameters
 **************************************************************************/

/* Spacing (kHz) */
/* 0: 200 kHz (USA, Australia) */
/* 1: 100 kHz (Europe, Japan) */
/* 2:  50 kHz */
static unsigned short space = 2;
module_param(space, ushort, 0444);
MODULE_PARM_DESC(space, "Spacing: 0=200kHz 1=100kHz *2=50kHz*");

/* Bottom of Band (MHz) */
/* 0: 87.5 - 108 MHz (USA, Europe)*/
/* 1: 76   - 108 MHz (Japan wide band) */
/* 2: 76   -  90 MHz (Japan) */
static unsigned short band = 0;
module_param(band, ushort, 0444);
MODULE_PARM_DESC(band, "Band: 0=87.5..108MHz *1=76..108MHz* 2=76..90MHz");

/* De-emphasis */
/* 0: V4L2_DEEMPHASIS_DISABLED */
/* 1: V4L2_DEEMPHASIS_50_uS (Europe, Australia, Japan) */
/* 2: V4L2_DEEMPHASIS_75_uS (South Korea, USA, Taiwan) */
/*
static unsigned short de = 1;
module_param(de, ushort, 0444);
MODULE_PARM_DESC(de, "De-emphasis: 0=Disabled *1=50us* 2=75us");
*/
/* Tune timeout */
/*static unsigned int tune_timeout = 4000;	*/
static unsigned int tune_timeout = 500;
module_param(tune_timeout, uint, 0644);
MODULE_PARM_DESC(tune_timeout, "Tune timeout: *4000*");

/* Seek timeout */
/*static unsigned int seek_timeout = 6000;	*/
static unsigned int seek_timeout = 2000;
module_param(seek_timeout, uint, 0644);
MODULE_PARM_DESC(seek_timeout, "Seek timeout: *5000*");

/**************************************************************************
 * Defines
 **************************************************************************/

/* si4704 uses 10kHz unit */
#define FREQ_DEV_TO_V4L2(x)	(x * FREQ_MUL / 100)
#define FREQ_V4L2_TO_DEV(x)	(x * 100 / FREQ_MUL)

/**************************************************************************
 * Generic Functions
 **************************************************************************/
/*
 * si4704_set_deemphasis
 */
static int si4704_set_deemphasis(struct si4704_device *radio, unsigned int deemphasis)
{
	int retval;
	unsigned short chip_de;

	switch (deemphasis) {
	case V4L2_DEEMPHASIS_DISABLED:
		return 0;
	case V4L2_DEEMPHASIS_50_uS: /* 1, Europe, Australia, Japan */
		chip_de = 0x01;
		break;
	case V4L2_DEEMPHASIS_75_uS: /* 2, South Korea, USA */
		chip_de = 0x02;
		break;
	default:
		retval = -EINVAL;
		goto done;
	}

	retval = si4704_set_property(g_radio, FM_DEEMPHASIS, chip_de);

done:
	if (retval < 0)
		pr_err("%s: set de-emphasis failed with %d", __func__, retval);

	return retval;
}

/*
 * si4704_set_chan - set the channel
 */
static int si4704_set_chan(struct si4704_device *radio, unsigned short chan)
{
	int retval;
	unsigned long timeout;
	bool timed_out = 0;
	u8 stcint = 0;

/*	pr_info("%s: chan = %d\n", __func__, chan);	*/

	/* start tuning */
	retval = si4704_fm_tune_freq(g_radio, chan, 0x00);
	if (retval < 0)
		goto done;

	/* currently I2C driver only uses interrupt way to tune */
	if (g_radio->stci_enabled) {
		INIT_COMPLETION(g_radio->completion);

		/* wait till tune operation has completed */
		retval = wait_for_completion_timeout(&g_radio->completion, msecs_to_jiffies(tune_timeout));
		if (!retval)
		{
			timed_out = true;
		}

		stcint = (g_radio->int_status & GET_INT_STATUS_STCINT);
/*		pr_err("%s: stcint = 0x%02x", __func__, stcint);	*/
	} else {
		/* wait till tune operation has completed */
		timeout = jiffies + msecs_to_jiffies(tune_timeout);
		do {
			retval = si4704_get_int_status(g_radio, NULL, NULL, NULL, NULL, &stcint);
			if (retval < 0)
				goto stop;
			timed_out = time_after(jiffies, timeout);
		} while ((stcint == 0) && (!timed_out));
	}

	if (stcint == 0)
		pr_err("%s: tune does not complete. stcint = %d\n", __func__, stcint);
	if (timed_out)
		pr_err("%s: tune timed out after %u ms\n", __func__, tune_timeout);

stop:
	/* stop tuning */
	retval = si4704_fm_tune_status(g_radio, 0x01, 0x01, NULL, NULL, NULL, NULL, NULL);

done:
	return retval;
}

/*
 * si4704_get_freq - get the frequency
 */
static int si4704_get_freq(struct si4704_device *radio, unsigned int *freq)
{
	int retval;
	u16 chan = 0;

	retval = si4704_fm_tune_status(g_radio, 0x00, 0x00, NULL, &chan, NULL, NULL, NULL);

	*freq = FREQ_DEV_TO_V4L2(chan);

	pr_info("%s: frequency = %d\n", __func__, chan);

	return retval;
}

/*
 * si4704_set_freq - set the frequency
 */
int si4704_set_freq(struct si4704_device *radio, unsigned int freq)
{
	u16 chan = 0;

	chan = FREQ_V4L2_TO_DEV(freq);

	return si4704_set_chan(g_radio, chan);
}

/*
 * si4704_set_seek - set seek
 */
static int si4704_set_seek(struct si4704_device *radio, unsigned int wrap_around, unsigned int seek_upward)
{
	int retval = 0;
	unsigned long timeout;
	bool timed_out = 0;
	u8 stcint = 0;
	u8 bltf = 0;

	/* start seeking */
	retval = si4704_fm_seek_start(g_radio, seek_upward, wrap_around);
	if (retval < 0)
		goto done;

	/* currently I2C driver only uses interrupt way to seek */
	if (g_radio->stci_enabled) {
		INIT_COMPLETION(g_radio->completion);

		/* wait till seek operation has completed */
		retval = wait_for_completion_timeout(&g_radio->completion, msecs_to_jiffies(seek_timeout));
		if (!retval)
			timed_out = true;

		stcint = (g_radio->int_status & GET_INT_STATUS_STCINT);
	} else {
		/* wait till seek operation has completed */
		timeout = jiffies + msecs_to_jiffies(seek_timeout);
		do {
			retval = si4704_get_int_status(g_radio, NULL, NULL, NULL, NULL, &stcint);
			if (retval < 0)
				goto stop;
			timed_out = time_after(jiffies, timeout);
		} while ((stcint == 0) && (!timed_out));
	}

	if (stcint == 0)
		pr_err("%s: seek does not complete\n", __func__);
	if (bltf)
		pr_err("%s: seek failed / band limit reached\n", __func__);
	if (timed_out)
		pr_err("%s: seek timed out after %u ms\n", __func__, seek_timeout);

stop:
	/* stop seeking */
	retval = si4704_fm_tune_status(g_radio, 0x01, 0x01, NULL, NULL, NULL, NULL, NULL);

done:
	/* try again, if timed out */
	if ((retval == 0) && timed_out)
		retval = -EAGAIN;

	return retval;
}

/*
 * si4704_start - switch on radio
 */
int si4704_start(struct si4704_device *radio)
{
	int retval;
	u16 threshold;

	retval = si4704_power_up(g_radio);
	if (retval < 0)
		goto done;

	retval = si4704_set_property(g_radio, GPO_IEN, GPO_IEN_STCIEN_MASK);
	if (retval < 0)
		goto done;

	retval = si4704_set_property(g_radio, REFCLK_FREQ, 0x8000);		/*	32768	*/
	if (retval < 0)
		goto done;

	retval = si4704_set_property(g_radio, REFCLK_PRESCALE, 0x0001);	/*	1	*/
	if (retval < 0)
		goto done;

	retval = si4704_set_property(g_radio, FM_ANTENNA_INPUT, 0x0000);
	if (retval < 0)
		goto done;

	if (g_radio->vol)
		retval = si4704_set_property(g_radio, RX_VOLUME, g_radio->vol);
	else
	{
		retval = si4704_set_property(g_radio, RX_VOLUME, RX_VOLUME_MED);
		g_radio->vol = RX_VOLUME_MED;
	}
	if (retval < 0)
		goto done;

/*	retval = si4704_set_deemphasis(g_radio, de);	*/
	retval = si4704_set_deemphasis(g_radio, g_radio->pdata->de);
	if (retval < 0)
		goto done;

/*	retval = si4704_set_property(g_radio, RX_HARD_MUTE, 0x03);	*/	/* Mute first	*/
	retval = si4704_set_property(g_radio, RX_HARD_MUTE, 0x00);		/* Unmute first	*/
	if (retval < 0)
		goto done;

/*	retval = si4704_set_property(g_radio, FM_MAX_TUNE_ERROR, 0x0028);	*/
	retval = si4704_set_property(g_radio, FM_MAX_TUNE_ERROR, 0x0014);
	if (retval < 0)
		goto done;

	retval = si4704_set_property(g_radio, FM_RSQ_INT_SOURCE, 0x008F);
	if (retval < 0)
		goto done;

	retval = si4704_set_property(g_radio, FM_RSQ_SNR_HI_THRESHOLD, 0x001E);
	if (retval < 0)
		goto done;

	retval = si4704_set_property(g_radio, FM_RSQ_SNR_LO_THRESHOLD, 0x0006);
	if (retval < 0)
		goto done;

	retval = si4704_set_property(g_radio, FM_RSQ_RSSI_HI_THRESHOLD, 0x0032);
	if (retval < 0)
		goto done;

	retval = si4704_set_property(g_radio, FM_RSQ_RSSI_LO_THRESHOLD, 0x0018);
	if (retval < 0)
		goto done;

	retval = si4704_set_property(g_radio, FM_RSQ_BLEND_THRESHOLD, 0x00B2);
	if (retval < 0)
		goto done;

	retval = si4704_set_property(g_radio, FM_SOFT_MUTE_MAX_ATTENUATION, 0x000A);
	if (retval < 0)
		goto done;

	retval = si4704_set_property(g_radio, FM_SOFT_MUTE_SNR_THRESHOLD, 0x0006);
	if (retval < 0)
		goto done;
		
	/* RSSI, SNR, Multipath */
	if (!g_radio->blend_rssi_stereo_threshold)
		g_radio->blend_rssi_stereo_threshold = g_radio->pdata->blend_rssi_stereo_threshold;
	if (!g_radio->blend_rssi_mono_threshold)
		g_radio->blend_rssi_mono_threshold = g_radio->pdata->blend_rssi_mono_threshold;
	if (!g_radio->blend_snr_stereo_threshold)
		g_radio->blend_snr_stereo_threshold = g_radio->pdata->blend_snr_stereo_threshold;
	if (!g_radio->blend_snr_mono_threshold)
		g_radio->blend_snr_mono_threshold = g_radio->pdata->blend_snr_mono_threshold;
	if (!g_radio->blend_multi_stereo_threshold)
		g_radio->blend_multi_stereo_threshold = g_radio->pdata->blend_multi_stereo_threshold;
	if (!g_radio->blend_multi_mono_threshold)
		g_radio->blend_multi_mono_threshold = g_radio->pdata->blend_multi_mono_threshold;
	
	retval = si4704_set_property(g_radio, FM_BLEND_RSSI_STEREO_THRESHOLD, g_radio->blend_rssi_stereo_threshold);
	if (retval < 0)
		goto done;

	retval = si4704_set_property(g_radio, FM_BLEND_RSSI_MONO_THRESHOLD, g_radio->blend_rssi_mono_threshold);
	if (retval < 0)
		goto done;

	retval = si4704_set_property(g_radio, FM_BLEND_SNR_STEREO_THRESHOLD, g_radio->blend_snr_stereo_threshold);
	if (retval < 0)
		goto done;

	retval = si4704_set_property(g_radio, FM_BLEND_SNR_MONO_THRESHOLD, g_radio->blend_snr_mono_threshold);
	if (retval < 0)
		goto done;

	retval = si4704_set_property(g_radio, FM_BLEND_MULTIPATH_STEREO_THRESHOLD, g_radio->blend_multi_stereo_threshold);
	if (retval < 0)
		goto done;

	retval = si4704_set_property(g_radio, FM_BLEND_MULTIPATH_MONO_THRESHOLD, g_radio->blend_multi_mono_threshold);
	if (retval < 0)
		goto done;

	retval = si4704_set_property(g_radio, FM_SEEK_BAND_BOTTOM, g_radio->pdata->freq_min);
	if (retval < 0)
		goto done;

	retval = si4704_set_property(g_radio, FM_SEEK_BAND_TOP, g_radio->pdata->freq_max);
	if (retval < 0)
		goto done;

	retval = si4704_set_property(g_radio, FM_SEEK_FREQ_SPACING, 0x000A);
	if (retval < 0)
		goto done;

	if (g_radio->rx_seek_tune_snr_threshold)
		threshold = g_radio->rx_seek_tune_snr_threshold;
	else
	{
		threshold = si4704_get_seek_tune_snr_threshold_value(g_radio);
		g_radio->rx_seek_tune_snr_threshold = threshold;
	}

	retval = si4704_set_property(g_radio, FM_SEEK_TUNE_SNR_THRESHOLD, threshold);
	if (retval < 0)
		goto done;

	if (g_radio->rx_seek_tune_rssi_threshold)
		threshold = g_radio->rx_seek_tune_rssi_threshold;
	else
	{
		threshold = si4704_get_seek_tune_rssi_threshold_value(g_radio);
		g_radio->rx_seek_tune_rssi_threshold = threshold;
	}

	retval = si4704_set_property(g_radio, FM_SEEK_TUNE_RSSI_THRESHOLD, threshold);
	if (retval < 0)
		goto done;

done:
	return retval;
}

/*
 * si4704_stop - switch off radio
 */
int si4704_stop(struct si4704_device *radio)
{
	int retval;

	retval = si4704_set_property(g_radio, RDS_CONFIG, 0x00);
	if (retval < 0)
		goto done;

	retval = si4704_set_property(g_radio, RX_HARD_MUTE, 0x03);
	if (retval < 0)
		goto done;

	retval = si4704_power_down(g_radio);

done:
	return retval;
}

/*
 * si4704_rds_on - switch on rds reception
 */
/*
static int si4704_rds_on(struct si4704_device *radio)
{
	int retval;

	retval = si4704_set_property(radio, GPO_IEN, GPO_IEN_STCIEN_MASK
		| GPO_IEN_RDSIEN_MASK | GPO_IEN_STCREP_MASK);
	if (retval < 0)
		goto done;

	retval = si4704_set_property(radio, RDS_INT_SOURCE,
				RDS_INT_SOURCE_RECV_MASK);
	if (retval < 0)
		goto done;

	retval = si4704_set_property(radio, RDS_CONFIG, RDS_CONFIG_RDSEN_MASK |
				(3 << RDS_CONFIG_BLETHA_SHFT) |
				(3 << RDS_CONFIG_BLETHB_SHFT) |
				(3 << RDS_CONFIG_BLETHC_SHFT) |
				(3 << RDS_CONFIG_BLETHD_SHFT));

done:
	return retval;
}
*/
/**************************************************************************
 * File Operations Interface
 **************************************************************************/

/*
 * si4704_fops_open - file open
 */
int si4704_fops_open(struct file *file)
{
/*	struct si4704_device *radio = video_drvdata(file);	*/
	int retval = 0;

	mutex_lock(&g_radio->lock);

	if (g_radio->force_power_on != 1)
	{
		retval = -1;
		goto done;
	}

	g_radio->users++;

	if (g_radio->users == 1) {
		/* start radio */
		retval = si4704_start(g_radio);
		if (retval < 0)
			goto done;
		wake_lock(&g_radio->wlock);
	}

done:
	mutex_unlock(&g_radio->lock);
	return retval;
}

/*
 * si4704_fops_release - file release
 */
int si4704_fops_release(struct file *file)
{
/*	struct si4704_device *radio = video_drvdata(file);	*/
	int retval = 0;

	/* safety check */
	if (!g_radio)
		return -ENODEV;

	mutex_lock(&g_radio->lock);
	g_radio->users--;
	if (g_radio->users == 0)
	{
		/* stop radio */
		retval = si4704_stop(g_radio);
		wake_unlock(&g_radio->wlock);
	}

	mutex_unlock(&g_radio->lock);

	return retval;
}

/*
 * si4704_fops_read - read RDS data
 */
static ssize_t si4704_fops_read(struct file *file, char __user *buf, size_t count, loff_t *ppos)
{
/*
	struct si4704_device *radio = video_drvdata(file);
	int retval = 0;
	unsigned int block_count = 0;
	u16 rdsen = 0;

	mutex_lock(&radio->lock);

	retval = si4704_get_property(radio, RDS_CONFIG, &rdsen);
	if (retval < 0)
		goto done;

	rdsen &= RDS_CONFIG_RDSEN_MASK;

	if (rdsen == 0)
		si4704_rds_on(radio);

	while (radio->wr_index == radio->rd_index) {
		if (file->f_flags & O_NONBLOCK) {
			retval = -EWOULDBLOCK;
			goto done;
		}
		if (wait_event_interruptible(radio->read_queue,
			radio->wr_index != radio->rd_index) < 0) {
			retval = -EINTR;
			goto done;
		}
	}

	count /= 3;

	while (block_count < count) {
		if (radio->rd_index == radio->wr_index)
			break;

		if (copy_to_user(buf, &radio->buffer[radio->rd_index], 3))
			break;

		radio->rd_index += 3;
		if (radio->rd_index >= radio->buf_size)
			radio->rd_index = 0;

		block_count++;
		buf += 3;
		retval += 3;
	}

done:
	mutex_unlock(&radio->lock);
	return retval;
*/
	return 0;
}

/*
 * si4704_fops_poll - poll RDS data
 */
static unsigned int si4704_fops_poll(struct file *file, struct poll_table_struct *pts)
{
/*	
	struct si4704_device *radio = video_drvdata(file);
	int retval = 0;
	u16 rdsen = 0;

	mutex_lock(&radio->lock);

	retval = si4704_get_property(radio, RDS_CONFIG, &rdsen);
	if (retval < 0)
		goto done;

	rdsen &= RDS_CONFIG_RDSEN_MASK;

	if (rdsen == 0)
		si4704_rds_on(radio);
	mutex_unlock(&radio->lock);

	poll_wait(file, &radio->read_queue, pts);

	if (radio->rd_index != radio->wr_index)
		retval = POLLIN | POLLRDNORM;

	return retval;

done:
	mutex_unlock(&radio->lock);
	return retval;
*/
	return 0;
}

/*
 * si4704_fops - file operations interface
 */
static const struct v4l2_file_operations si4704_fops = {
	.owner			= THIS_MODULE,
	.read			= si4704_fops_read,
	.poll			= si4704_fops_poll,
	.unlocked_ioctl		= video_ioctl2,
	.open			= si4704_fops_open,
	.release		= si4704_fops_release,
};


/**************************************************************************
 * Video4Linux Interface
 **************************************************************************/

/*
 * si4704_vidioc_querycap - query device capabilities
 */
int si4704_vidioc_querycap(struct file *file, void *priv, struct v4l2_capability *capability)
{
	strlcpy(capability->driver, DRIVER_NAME, sizeof(capability->driver));
	strlcpy(capability->card, DRIVER_CARD, sizeof(capability->card));
	capability->version = DRIVER_KERNEL_VERSION;
	capability->capabilities = V4L2_CAP_HW_FREQ_SEEK | V4L2_CAP_TUNER | V4L2_CAP_RADIO;

	return 0;
}

/*
 * si4704_vidioc_queryctrl - enumerate control items
 */
static int si4704_vidioc_queryctrl(struct file *file, void *priv, struct v4l2_queryctrl *qc)
{
/*	struct si4704_device *radio = video_drvdata(file);	*/
	int retval = -EINVAL;

	/* abort if qc->id is below V4L2_CID_BASE */
	if (qc->id < V4L2_CID_BASE)
		goto done;

	/* search video control */
	switch (qc->id) {
	case V4L2_CID_AUDIO_VOLUME:
		return v4l2_ctrl_query_fill(qc, 0, 15, 1, 15);
	case V4L2_CID_AUDIO_MUTE:
		return v4l2_ctrl_query_fill(qc, 0, 1, 1, 1);
	}

	/* disable unsupported base controls */
	/* to satisfy kradio and such apps */
	if ((retval == -EINVAL) && (qc->id < V4L2_CID_LASTP1)) {
		qc->flags = V4L2_CTRL_FLAG_DISABLED;
		retval = 0;
	}

done:
	if (retval < 0)
		dev_warn(&g_radio->videodev->dev, "query controls failed with %d\n", retval);
	return retval;
}

/*
 * si4704_vidioc_g_ctrl - get the value of a control
 */
static int si4704_vidioc_g_ctrl(struct file *file, void *priv, struct v4l2_control *ctrl)
{
/*	struct si4704_device *radio = video_drvdata(file);	*/
	int retval = 0;
	u16 vol_mute = 0;

	mutex_lock(&g_radio->lock);
	/* safety checks */
	retval = si4704_disconnect_check(g_radio);
	if (retval)
		goto done;

	switch (ctrl->id) {
	case V4L2_CID_AUDIO_VOLUME:
		retval = si4704_get_property(g_radio, RX_VOLUME, &vol_mute);
		if (retval)
			goto done;

		vol_mute &= RX_VOLUME_MASK;
		ctrl->value = si4704_vol_conv_value_to_index(g_radio, vol_mute);
		break;
	case V4L2_CID_AUDIO_MUTE:

		retval = si4704_get_property(g_radio, RX_HARD_MUTE, &vol_mute);
		if (retval)
			goto done;
		ctrl->value = (vol_mute == 0x03) ? 1 : 0;
		break;
	/*
	 * TODO : need to support op_mode change lp_mode <-> rich_mode
	 */
	default:
		retval = -EINVAL;
	}

done:
	if (retval < 0)
		dev_warn(&g_radio->videodev->dev, "get control failed with %d\n", retval);

	mutex_unlock(&g_radio->lock);
	return retval;
}

/*
 * si4704_vidioc_s_ctrl - set the value of a control
 */
static int si4704_vidioc_s_ctrl(struct file *file, void *priv, struct v4l2_control *ctrl)
{
/*	struct si4704_device *radio = video_drvdata(file);	*/
	int retval = 0;
	u16 vol;

	mutex_lock(&g_radio->lock);
	/* safety checks */
	retval = si4704_disconnect_check(g_radio);
	if (retval)
		goto done;

	switch (ctrl->id) {
	case V4L2_CID_AUDIO_VOLUME:
		vol = si4704_vol_conv_index_to_value(g_radio, ctrl->value);
		g_radio->vol = vol;
		retval = si4704_set_property(g_radio, RX_VOLUME, vol);
		break;
	case V4L2_CID_AUDIO_MUTE:
		if (ctrl->value == 1)
			retval = si4704_set_property(g_radio, RX_HARD_MUTE, 0x03);
		else
			retval = si4704_set_property(g_radio, RX_HARD_MUTE, 0x00);
		break;
	case V4L2_CID_TUNE_DEEMPHASIS:
		retval = si4704_set_deemphasis(g_radio, ctrl->value);
		break;

	/*
	 * TODO : need to support op_mode change lp_mode <-> rich_mode
	 */
	default:
		retval = -EINVAL;
	}

done:
	if (retval < 0)
		dev_warn(&g_radio->videodev->dev, "set control failed with %d\n", retval);
	mutex_unlock(&g_radio->lock);
	return retval;
}

/*
 * si4704_vidioc_g_audio - get audio attributes
 */
static int si4704_vidioc_g_audio(struct file *file, void *priv, struct v4l2_audio *audio)
{
	/* driver constants */
	audio->index = 0;
	strcpy(audio->name, "Radio");
	audio->capability = V4L2_AUDCAP_STEREO;
	audio->mode = 0;

	return 0;
}

/*
 * si4704_vidioc_g_tuner - get tuner attributes
 */
static int si4704_vidioc_g_tuner(struct file *file, void *priv, struct v4l2_tuner *tuner)
{
/*	struct si4704_device *radio = video_drvdata(file);	*/
	int retval = 0;
	u8 afcrl = 0;
	u8 stblend = 0;
	u8 rssi = 0;
	u8 snr = 0;
	u8 multi = 0;
	u16 bandLow = 0;
	u16 bandHigh = 0;
	u16 seek_snr = 0;
	u16 seek_rssi = 0;
	u16 blend_rssi_stereo_threshold = 0;
	u16 blend_rssi_mono_threshold = 0;
	u16 blend_snr_stereo_threshold = 0;
	u16 blend_snr_mono_threshold = 0;
	u16 blend_multi_stereo_threshold = 0;
	u16 blend_multi_mono_threshold = 0;

	mutex_lock(&g_radio->lock);
	/* safety checks */
	retval = si4704_disconnect_check(g_radio);
	if (retval)
		goto done;

	if (tuner->index != 0) {
		retval = -EINVAL;
		goto done;
	}

	retval = si4704_fm_rsq_status(g_radio, 0x00, NULL, &afcrl, &stblend, &rssi, &snr, &multi, NULL);
	if (retval < 0)
		goto done;
/*
	pr_err("%s: afcrl = 0x%02x, stblend = 0x%02x, rssi = 0x%02x\n", __func__, afcrl, stblend, rssi);
*/
	/* driver constants */
	strcpy(tuner->name, "FM");
	tuner->type = V4L2_TUNER_RADIO;
	tuner->capability = V4L2_TUNER_CAP_LOW | V4L2_TUNER_CAP_STEREO;

	/* range limits */
	retval = si4704_get_property(g_radio, FM_SEEK_BAND_BOTTOM, &bandLow);
	if (retval)
		goto done;

	retval = si4704_get_property(g_radio, FM_SEEK_BAND_TOP, &bandHigh);
	if (retval)
		goto done;

	tuner->rangelow  = FREQ_DEV_TO_V4L2(bandLow);
	tuner->rangehigh = FREQ_DEV_TO_V4L2(bandHigh);

	/* pilot, bit7 */
	if ((stblend & FM_RSQ_STATUS_OUT_PILOT))
		tuner->pilot = 1;
	else
		tuner->pilot = 0;

	/* stereo indicator == stereo (instead of mono) */
	if ((stblend & FM_RSQ_STATUS_OUT_STBLEND) == 0)
		tuner->rxsubchans = V4L2_TUNER_SUB_MONO;
	else
		tuner->rxsubchans = V4L2_TUNER_SUB_MONO | V4L2_TUNER_SUB_STEREO;

	/* stdblend */
	tuner->stblend = stblend & FM_RSQ_STATUS_OUT_STBLEND;

	/* get RSSI, SNR, Multipath threshold */
	retval = si4704_get_property(g_radio, FM_BLEND_RSSI_STEREO_THRESHOLD, &blend_rssi_stereo_threshold);
	if (retval < 0)
		goto done;

	retval = si4704_get_property(g_radio, FM_BLEND_RSSI_MONO_THRESHOLD, &blend_rssi_mono_threshold);
	if (retval < 0)
		goto done;

	retval = si4704_get_property(g_radio, FM_BLEND_SNR_STEREO_THRESHOLD, &blend_snr_stereo_threshold);
	if (retval < 0)
		goto done;

	retval = si4704_get_property(g_radio, FM_BLEND_SNR_MONO_THRESHOLD, &blend_snr_mono_threshold);
	if (retval < 0)
		goto done;

	retval = si4704_get_property(g_radio, FM_BLEND_MULTIPATH_STEREO_THRESHOLD, &blend_multi_stereo_threshold);
	if (retval < 0)
		goto done;

	retval = si4704_get_property(g_radio, FM_BLEND_MULTIPATH_MONO_THRESHOLD, &blend_multi_mono_threshold);
	if (retval < 0)
		goto done;

	/* mono/stereo selector */
	if ( (blend_rssi_stereo_threshold == 127) || (blend_rssi_mono_threshold == 127) || 
		(blend_snr_stereo_threshold == 127) || (blend_snr_mono_threshold == 127) || 
		(blend_multi_stereo_threshold == 0) || (blend_multi_mono_threshold == 0) )
		tuner->audmode = V4L2_TUNER_MODE_MONO;
	else
		tuner->audmode = V4L2_TUNER_MODE_STEREO;

	tuner->blend_rssi_stereo_threshold = blend_rssi_stereo_threshold;
	tuner->blend_rssi_mono_threshold = blend_rssi_mono_threshold;
	tuner->blend_snr_stereo_threshold = blend_snr_stereo_threshold;
	tuner->blend_snr_mono_threshold = blend_snr_mono_threshold;
	tuner->blend_multi_stereo_threshold = blend_multi_stereo_threshold;
	tuner->blend_multi_mono_threshold = blend_multi_mono_threshold;

	/* seek snr & rssi */
	retval = si4704_get_property(g_radio, FM_SEEK_TUNE_SNR_THRESHOLD, &seek_snr);
	if (retval < 0)
		goto done;
	
	retval = si4704_get_property(g_radio, FM_SEEK_TUNE_RSSI_THRESHOLD, &seek_rssi);
	if (retval < 0)
		goto done;

	tuner->seek_snr = seek_snr;
	tuner->seek_rssi = seek_rssi;

	/* V4L2 does not specify how to use signal strength field. */
	/* it just descripbes higher is a better signal */
	/* update rssi value as real dbµV unit */
	tuner->signal = rssi;
	tuner->rssi = rssi;
	tuner->snr = snr;
	tuner->multi = multi;

	/* automatic frequency control: -1: freq to low, 1 freq to high */
	/* AFCRL does only indicate that freq. differs, not if too low/high */
	tuner->afc = (afcrl & FM_RSQ_STATUS_OUT_AFCRL) ? 1 : 0;

done:
	if (retval < 0)
		pr_err("%s: get tuner failed with %d\n", __func__, retval);
	mutex_unlock(&g_radio->lock);
	return retval;
}

/*
 * si4704_vidioc_s_tuner - set tuner attributes
 */
static int si4704_vidioc_s_tuner(struct file *file, void *priv, struct v4l2_tuner *tuner)
{
/*	struct si4704_device *radio = video_drvdata(file);	*/
	int retval = 0;
	u16 bandLow = 0;
	u16 bandHigh = 0;
	u16 seek_snr = 0;
	u16 seek_rssi = 0;
	u16 blend_rssi_stereo_threshold = 0;
	u16 blend_rssi_mono_threshold = 0;
	u16 blend_snr_stereo_threshold = 0;
	u16 blend_snr_mono_threshold = 0;
	u16 blend_multi_stereo_threshold = 0;
	u16 blend_multi_mono_threshold = 0;

	mutex_lock(&g_radio->lock);
	/* safety checks */
	retval = si4704_disconnect_check(g_radio);
	if (retval)
		goto done;

	if (tuner->index != 0)
		goto done;

	/* get RSSI, SNR, Multipath threshold */
	blend_rssi_stereo_threshold = tuner->blend_rssi_stereo_threshold;
	blend_rssi_mono_threshold = tuner->blend_rssi_mono_threshold;
	blend_snr_stereo_threshold = tuner->blend_snr_stereo_threshold;
	blend_snr_mono_threshold = tuner->blend_snr_mono_threshold;
	blend_multi_stereo_threshold = tuner->blend_multi_stereo_threshold;
	blend_multi_mono_threshold = tuner->blend_multi_mono_threshold;

	retval = si4704_set_property(g_radio, FM_BLEND_RSSI_STEREO_THRESHOLD, blend_rssi_stereo_threshold);
	if (retval < 0)
		goto done;

	retval = si4704_set_property(g_radio, FM_BLEND_RSSI_MONO_THRESHOLD, blend_rssi_mono_threshold);
	if (retval < 0)
		goto done;

	retval = si4704_set_property(g_radio, FM_BLEND_SNR_STEREO_THRESHOLD, blend_snr_stereo_threshold);
	if (retval < 0)
		goto done;

	retval = si4704_set_property(g_radio, FM_BLEND_SNR_MONO_THRESHOLD, blend_snr_mono_threshold);
	if (retval < 0)
		goto done;

	retval = si4704_set_property(g_radio, FM_BLEND_MULTIPATH_STEREO_THRESHOLD, blend_multi_stereo_threshold);
	if (retval < 0)
		goto done;

	retval = si4704_set_property(g_radio, FM_BLEND_MULTIPATH_MONO_THRESHOLD, blend_multi_mono_threshold);
	if (retval < 0)
		goto done;

	g_radio->blend_rssi_stereo_threshold = blend_rssi_stereo_threshold;
	g_radio->blend_rssi_mono_threshold = blend_rssi_mono_threshold;
	g_radio->blend_snr_stereo_threshold = blend_snr_stereo_threshold;
	g_radio->blend_snr_mono_threshold = blend_snr_mono_threshold;
	g_radio->blend_multi_stereo_threshold = blend_multi_stereo_threshold;
	g_radio->blend_multi_mono_threshold = blend_multi_mono_threshold;

	/* seek snr & rssi */
	seek_snr = tuner->seek_snr;
	g_radio->rx_seek_tune_snr_threshold = seek_snr;
	retval = si4704_set_property(g_radio, FM_SEEK_TUNE_SNR_THRESHOLD, seek_snr);
	if (retval < 0)
		goto done;

	seek_rssi = tuner->seek_rssi;
	g_radio->rx_seek_tune_rssi_threshold = seek_rssi;
	retval = si4704_set_property(g_radio, FM_SEEK_TUNE_RSSI_THRESHOLD, seek_rssi);
	if (retval < 0)
		goto done;

	/* range limit */
	if (tuner->rangelow) {
		bandLow = FREQ_V4L2_TO_DEV(tuner->rangelow);
		retval = si4704_set_property(g_radio, FM_SEEK_BAND_BOTTOM, bandLow);
		if (retval < 0)
			goto done;
	}

	if (tuner->rangehigh) {
		bandHigh = FREQ_V4L2_TO_DEV(tuner->rangehigh);
		retval = si4704_set_property(g_radio, FM_SEEK_BAND_TOP, bandHigh);
		if (retval < 0)
			goto done;
	}

done:
	if (retval < 0)
		pr_err("%s: set tuner failed with %d\n", __func__, retval);
	mutex_unlock(&g_radio->lock);
	return retval;
}

/*
 * si4704_vidioc_g_frequency - get tuner or modulator radio frequency
 */
static int si4704_vidioc_g_frequency(struct file *file, void *priv, struct v4l2_frequency *freq)
{
/*	struct si4704_device *radio = video_drvdata(file);	*/
	int retval = 0;

	/* safety checks */
	mutex_lock(&g_radio->lock);
	retval = si4704_disconnect_check(g_radio);
	if (retval)
		goto done;

	if (freq->tuner != 0) {
		retval = -EINVAL;
		goto done;
	}

	freq->type = V4L2_TUNER_RADIO;
	retval = si4704_get_freq(g_radio, &freq->frequency);

done:
	if (retval < 0)
		pr_err("%s: set frequency failed with %d\n", __func__, retval);
	mutex_unlock(&g_radio->lock);
	return retval;
}

/*
 * si4704_vidioc_s_frequency - set tuner or modulator radio frequency
 */
static int si4704_vidioc_s_frequency(struct file *file, void *priv, struct v4l2_frequency *freq)
{
/*	struct si4704_device *radio = video_drvdata(file);	*/
	int retval = 0;

	mutex_lock(&g_radio->lock);
	/* safety checks */
	retval = si4704_disconnect_check(g_radio);
	if (retval)
		goto done;

	if (freq->tuner != 0) {
		retval = -EINVAL;
		goto done;
	}

	retval = si4704_set_freq(g_radio, freq->frequency);

done:
	if (retval < 0)
		pr_err("%s: set frequency failed with %d\n", __func__, retval);
	mutex_unlock(&g_radio->lock);
	return retval;
}

/*
 * si4704_vidioc_s_hw_freq_seek - set hardware frequency seek
 */
static int si4704_vidioc_s_hw_freq_seek(struct file *file, void *priv, struct v4l2_hw_freq_seek *seek)
{
/*	struct si4704_device *radio = video_drvdata(file);	*/
	int retval = 0;

	mutex_lock(&g_radio->lock);
	/* safety checks */
	retval = si4704_disconnect_check(g_radio);
	if (retval)
		goto done;

	if (seek->tuner != 0) {
		retval = -EINVAL;
		goto done;
	}

	retval = si4704_set_seek(g_radio, seek->wrap_around, seek->seek_upward);

done:
	if (retval < 0)
		pr_err("%s : set hardware frequency seek failed with %d\n", __func__, retval);
	mutex_unlock(&g_radio->lock);
	return retval;
}

/*
 * si4704_ioctl_ops - video device ioctl operations
 */
static const struct v4l2_ioctl_ops si4704_ioctl_ops = {
	.vidioc_querycap	= si4704_vidioc_querycap,
	.vidioc_queryctrl	= si4704_vidioc_queryctrl,
	.vidioc_g_ctrl		= si4704_vidioc_g_ctrl,
	.vidioc_s_ctrl		= si4704_vidioc_s_ctrl,
	.vidioc_g_audio		= si4704_vidioc_g_audio,
	.vidioc_g_tuner		= si4704_vidioc_g_tuner,
	.vidioc_s_tuner		= si4704_vidioc_s_tuner,
	.vidioc_g_frequency	= si4704_vidioc_g_frequency,
	.vidioc_s_frequency	= si4704_vidioc_s_frequency,
	.vidioc_s_hw_freq_seek	= si4704_vidioc_s_hw_freq_seek,
};

/*
 * si4704_viddev_template - video device interface
 */
struct video_device si4704_viddev_template = {
	.fops			= &si4704_fops,
	.name			= DRIVER_NAME,
	.release		= video_device_release,
	.ioctl_ops		= &si4704_ioctl_ops,
};
