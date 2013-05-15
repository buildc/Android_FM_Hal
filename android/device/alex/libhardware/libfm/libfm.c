
#include <libfm.h>

int	g_radio_handle = -1;
int	g_freq = 0;

static int si4704_open_radio(struct fm_si4704_device_t *dev)
{
	int		ret = 0;

	if (g_radio_handle >= 0)
	{
		printf("si4704_open_radio::Already opened!\n");
		ret = -EINVAL;
		goto finish;
	}

	if ((dev->defs.radio = open(FM_RADIO_MODULE_DEVICE, O_RDONLY)) < 0)
	{
		printf("si4704_open_radio::Open %s failed!\n", FM_RADIO_MODULE_DEVICE);
		ret = -EINVAL;
		goto finish;
	}

	if (ioctl(dev->defs.radio, VIDIOC_QUERYCAP, &(dev->defs.vc)) < 0)
	{
		printf("si4704_open_radio::Error: VIDIOC_QUERYCAP\n");
		ret = -EINVAL;
		goto finish;
	}

	g_radio_handle = dev->defs.radio;

finish:

	return ret;
}

static void si4704_close_radio(struct fm_si4704_device_t *dev)
{
	if ( (dev->defs.radio >= 0) && (g_radio_handle >= 0) )
	{
		close(dev->defs.radio);
		dev->defs.radio = -1;
		g_radio_handle = -1;
	}
}

static int si4704_get_tuner(struct fm_si4704_device_t *dev)
{
	int	ret = 0;

	if (dev->defs.radio < 0)
		return -1;

	if (ioctl(dev->defs.radio, VIDIOC_G_TUNER, &(dev->defs.vt)) < 0)
	{
		printf("si4704_get_tuner::Error: VIDIOC_G_TUNER\n");
		ret = -1;
	}

	dev->defs.freq_min = FREQ_V4L2_TO_DEV(dev->defs.vt.rangelow);
	dev->defs.freq_max = FREQ_V4L2_TO_DEV(dev->defs.vt.rangehigh);
	dev->defs.signal = dev->defs.vt.signal;
	dev->defs.pilot = dev->defs.vt.pilot;
	dev->defs.seek_snr = dev->defs.vt.seek_snr;
	dev->defs.seek_rssi = dev->defs.vt.seek_rssi;
	dev->defs.blend_rssi_stereo_threshold = dev->defs.vt.blend_rssi_stereo_threshold;
	dev->defs.blend_rssi_mono_threshold = dev->defs.vt.blend_rssi_mono_threshold;
	dev->defs.blend_snr_stereo_threshold = dev->defs.vt.blend_snr_stereo_threshold;
	dev->defs.blend_snr_mono_threshold = dev->defs.vt.blend_snr_mono_threshold;
	dev->defs.blend_multi_stereo_threshold = dev->defs.vt.blend_multi_stereo_threshold;
	dev->defs.blend_multi_mono_threshold = dev->defs.vt.blend_multi_mono_threshold;
	dev->defs.stblend = dev->defs.vt.stblend;
	dev->defs.rssi = dev->defs.vt.rssi;
	dev->defs.snr = dev->defs.vt.snr;
	dev->defs.multi = dev->defs.vt.multi;

	return ret;
}

static int si4704_set_tuner(struct fm_si4704_device_t *dev)
{
	int	ret = -1;

	if (dev->defs.radio < 0)
		return -1;

	dev->defs.vt.rangelow = FREQ_DEV_TO_V4L2(dev->defs.freq_min);
	dev->defs.vt.rangehigh = FREQ_DEV_TO_V4L2(dev->defs.freq_max);
	dev->defs.vt.seek_snr = dev->defs.seek_snr;
	dev->defs.vt.seek_rssi = dev->defs.seek_rssi;
	dev->defs.vt.blend_rssi_stereo_threshold = dev->defs.blend_rssi_stereo_threshold;
	dev->defs.vt.blend_rssi_mono_threshold = dev->defs.blend_rssi_mono_threshold;
	dev->defs.vt.blend_snr_stereo_threshold = dev->defs.blend_snr_stereo_threshold;
	dev->defs.vt.blend_snr_mono_threshold = dev->defs.blend_snr_mono_threshold;
	dev->defs.vt.blend_multi_stereo_threshold = dev->defs.blend_multi_stereo_threshold;
	dev->defs.vt.blend_multi_mono_threshold = dev->defs.blend_multi_mono_threshold;

	if (ioctl(dev->defs.radio, VIDIOC_S_TUNER, &(dev->defs.vt)) < 0)
	{
		printf("si4704_set_tuner::Error: VIDIOC_G_TUNER\n");
		ret = -1;
	}

	return ret;
}

static int si4704_set_freq(struct fm_si4704_device_t *dev, unsigned int freq)
{
	int	res = -1;
	
	if (dev->defs.radio < 0)
		return -1;

	dev->defs.vf.tuner = 0;
	dev->defs.vf.frequency = FREQ_DEV_TO_V4L2(freq);
 
	res = ioctl(dev->defs.radio, VIDIOC_S_FREQUENCY, &(dev->defs.vf));

	if (res > 0)
		dev->defs.freq = freq;

	return res;
}

static int si4704_get_freq(struct fm_si4704_device_t *dev)
{
	int				res = -1;
	unsigned long	freq;
	
	if (dev->defs.radio < 0)
		return -1;
	
	res = ioctl(dev->defs.radio, VIDIOC_G_FREQUENCY, &(dev->defs.vf));
	
	if(res < 0)
		return -1;
	
	freq = FREQ_V4L2_TO_DEV(dev->defs.vf.frequency);
	
	dev->defs.freq = freq;
	
	return freq;
}

static int si4704_seek_freq(struct fm_si4704_device_t *dev)
{
	if (dev->defs.radio < 0)
		return -1;

	if (ioctl(dev->defs.radio, VIDIOC_S_HW_FREQ_SEEK, &(dev->defs.vfs)) < 0)
		return -1;

	return 0;
}

static int si4704_get_vol(struct fm_si4704_device_t *dev)
{
	if (dev->defs.radio < 0)
		return -1;
		
	dev->defs.vcl.id = V4L2_CID_AUDIO_VOLUME;
	if (ioctl(dev->defs.radio, VIDIOC_G_CTRL, &(dev->defs.vcl)) < 0)
		return -1;
		
	dev->defs.vol = dev->defs.vcl.value;
		
	return dev->defs.vcl.value;
}

static int si4704_set_vol(struct fm_si4704_device_t *dev, unsigned int vol)
{
	if (dev->defs.radio < 0)
		return -1;
	
	dev->defs.vcl.id = V4L2_CID_AUDIO_VOLUME;
	dev->defs.vcl.value = vol;

	if (ioctl(dev->defs.radio, VIDIOC_S_CTRL, &(dev->defs.vcl)) < 0)
		return -1;
	
	return 0;
}

static int si4704_get_mute(struct fm_si4704_device_t *dev)
{
	if (dev->defs.radio < 0)
		return -1;

	dev->defs.vcl.id = V4L2_CID_AUDIO_MUTE;
	if (ioctl(dev->defs.radio, VIDIOC_G_CTRL, &(dev->defs.vcl)) < 0)
		return -1;
		
	return dev->defs.vcl.value;
}

static int si4704_set_mute(struct fm_si4704_device_t *dev, unsigned int mute)
{
	if (dev->defs.radio < 0)
		return -1;

	dev->defs.vcl.id = V4L2_CID_AUDIO_MUTE;
	dev->defs.vcl.value = mute;

	if (ioctl(dev->defs.radio, VIDIOC_S_CTRL, &(dev->defs.vcl)) < 0)
		return -1;
	
	return 0;
}

static void si4704_get_status(struct fm_si4704_device_t *dev)
{
	printf("dev->defs.vt.name = %s\n", dev->defs.vt.name);
	printf("dev->defs.vt.type = %d\n", dev->defs.vt.type);
	printf("dev->defs.vt.type = 0x%x\n", dev->defs.vt.capability);
	printf("dev->defs.vt.rangelow = %d, %d\n", dev->defs.vt.rangelow, FREQ_V4L2_TO_DEV(dev->defs.vt.rangelow));
	printf("dev->defs.vt.rangehigh = %d, %d\n", dev->defs.vt.rangehigh, FREQ_V4L2_TO_DEV(dev->defs.vt.rangehigh));
	printf("dev->defs.vt.rxsubchans = 0x%x\n", dev->defs.vt.rxsubchans);
	printf("dev->defs.vt.audmode = 0x%x\n", dev->defs.vt.audmode);
	printf("dev->defs.vt.signal = 0x%x\n", dev->defs.vt.signal);
	printf("dev->defs.vt.afc = 0x%x\n", dev->defs.vt.afc);
	printf("dev->defs.vt.pilot = 0x%x\n", dev->defs.vt.pilot);
	printf("dev->defs.vt.seek_snr = 0x%x\n", dev->defs.vt.seek_snr);
	printf("dev->defs.vt.seek_rssi = 0x%x\n", dev->defs.vt.seek_rssi);
	printf("dev->defs.vt.blend_rssi_stereo_threshold = 0x%x\n", dev->defs.vt.blend_rssi_stereo_threshold);
	printf("dev->defs.vt.blend_rssi_mono_threshold = 0x%x\n", dev->defs.vt.blend_rssi_mono_threshold);
	printf("dev->defs.vt.blend_snr_stereo_threshold = 0x%x\n", dev->defs.vt.blend_snr_stereo_threshold);
	printf("dev->defs.vt.blend_snr_mono_threshold = 0x%x\n", dev->defs.vt.blend_snr_mono_threshold);
	printf("dev->defs.vt.blend_multi_stereo_threshold = 0x%x\n", dev->defs.vt.blend_multi_stereo_threshold);
	printf("dev->defs.vt.blend_multi_mono_threshold = 0x%x\n", dev->defs.vt.blend_multi_mono_threshold);
	printf("dev->defs.vt.stblend = 0x%x\n", dev->defs.vt.stblend);
	printf("dev->defs.vt.rssi = 0x%x\n", dev->defs.vt.rssi);
	printf("dev->defs.vt.snr = 0x%x\n", dev->defs.vt.snr);
	printf("dev->defs.vt.multi = 0x%x\n", dev->defs.vt.multi);
}

static int si4704_get_force_power(struct fm_si4704_device_t *dev)
{
	int		on = 0;
	char	buf[16] = {0};
	int		fd = open(FM_SI4704_FORCE_POWER, O_RDWR);
	if (fd >= 0)
	{
		read(fd, buf, sizeof(buf));
		if (strcmp(buf, "0\n") == 0)
			on = 0;
		else
			on = 1;
	}

	return on;
}

static void si4704_set_force_power(struct fm_si4704_device_t *dev, int on)
{
	int	fd = open(FM_SI4704_FORCE_POWER, O_RDWR);
	int current = si4704_get_force_power(dev);

	if (fd >= 0)
	{
		if ( (on == 1) && (current == 0) )
		{
			write(fd, on ? "1" : "0", 2);
			sleep(1);
			si4704_set_freq(dev, g_freq);
		}
		else if ( (on == 0) && (current == 1) )
		{
			g_freq = si4704_get_freq(dev);
			sleep(1);
			write(fd, on ? "1" : "0", 2);
		}

		close(fd);
	}
}

/* Close an opened si4704 device instance */
static int si4704_close(hw_device_t *dev)
{
	free(dev);
	return 0;
}

/*
 * Generic device handling
 */
static int fm_open(const hw_module_t* module, const char* name, hw_device_t** device)
{
/*	printf("fm_open::name = %s\n", name);	*/

	if (strcmp(name, FM_SI4704_CONTROLLER) == 0)
	{
		struct fm_si4704_device_t	*dev = malloc(sizeof(struct fm_si4704_device_t));
		memset(dev, 0, sizeof(*dev));

		dev->common.tag = HARDWARE_DEVICE_TAG;
		dev->common.version = 0;
		dev->common.module = (struct hw_module_t*)module;
		dev->common.close = si4704_close;
		dev->open_radio = si4704_open_radio;
		dev->close_radio = si4704_close_radio;
		dev->get_tuner = si4704_get_tuner;
		dev->set_tuner = si4704_set_tuner;
		dev->set_freq = si4704_set_freq;
		dev->get_freq = si4704_get_freq;
		dev->seek_freq = si4704_seek_freq;
		dev->get_vol = si4704_get_vol;
		dev->set_vol = si4704_set_vol;
		dev->get_mute = si4704_get_mute;
		dev->set_mute = si4704_set_mute;
		dev->get_status = si4704_get_status;
		dev->get_force_power = si4704_get_force_power;
		dev->set_force_power = si4704_set_force_power;

		*device = (hw_device_t*)dev;

		return 0;
	}
	else
	{
		return -EINVAL;
	}

	return 0;
}

static struct hw_module_methods_t fm_module_methods = 
{
    .open = fm_open,
};

struct fm_module_t HAL_MODULE_INFO_SYM = 
{
	.common = 
	{
		.tag = HARDWARE_MODULE_TAG, 
		.version_major = 1, 
		.version_minor = 0, 
		.id = FM_HARDWARE_MODULE_ID, 
		.name = "DUV060 FM HW HAL", 
		.author = "DLI", 
		.methods = &fm_module_methods, 
	},
};
