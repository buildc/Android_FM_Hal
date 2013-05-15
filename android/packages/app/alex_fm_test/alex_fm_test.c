#include <time.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/ioctl.h>
#include <sys/limits.h>
#include <errno.h>

#include "alex_fm_test.h"

int main(int argc, char **argv)
{
	int				ret = 0;
	bool			exit = false;
	char			buf;
	unsigned int 	freq = 0;
	
	alex_defs	*p_alex = (alex_defs *)malloc(sizeof(alex_defs));

	if (!alex_open_radio(p_alex))
	{
		printf("alex_open_radio failed!\n");
		ret = -1;
		goto finish;
	}
	
	if (argc == 2)
	{
		freq = atoi(argv[1]);
		printf("freq = %d\n", freq);
	}	

	while (!exit)
	{
		alex_get_tuner(p_alex);
		alex_get_freq(p_alex);
		alex_get_vol(p_alex);
		
		printf("\n+++ alex_fm_test +++\n");
		
		printf("\n");
		printf("Max Freq: %d\n", p_alex->freq_max);
		printf("Min Freq: %d\n", p_alex->freq_min);
		printf("Current Freq: %d\n", p_alex->freq);
		if (p_alex->pilot)
			printf("Signal: %d dB stereo\n", p_alex->signal);
		else
			printf("Signal: %d dB mono\n", p_alex->signal);
		printf("Volume: %d\n", p_alex->vol);
		printf("Seek SNR: %d\n", p_alex->seek_snr);
		printf("Seek RSSI: %d\n", p_alex->seek_rssi);

		printf("\n");
		if (freq == 0)
			freq = 10770;
		printf("[F] Set to %d\n", freq);
		printf("[U] Up Freq\n");
		printf("[D] Down Freq\n");
		printf("[S] Seek Freq Up\n");
		printf("[T] Seek Freq Down\n");
		printf("[V] Volume Up\n");
		printf("[X] Volume Down\n");
		printf("[A] Seek SNR +\n");
		printf("[B] Seek SNR -\n");
		printf("[M] Seek RSSI +\n");
		printf("[N] Seek RSSI -\n");
		printf("[P] Print Status\n");
		printf("[Q] Quit!\n\n");

		printf("--- alex_fm_test ---\n\n");

		buf = getchar();

		switch (buf)
		{
		case 'f':
		case 'F':
			alex_set_freq(p_alex, freq);
			break;
			
		case 'u':
		case 'U':
			if (p_alex->freq < p_alex->freq_max)
				alex_set_freq(p_alex, p_alex->freq + 10);
			break;
			
		case 'd':
		case 'D':
			if (p_alex->freq > p_alex->freq_min)
				alex_set_freq(p_alex, p_alex->freq - 10);
			break;

		case 's':
		case 'S':
			p_alex->vfs.tuner = 0;
			p_alex->vfs.type = p_alex->vt.type;
			p_alex->vfs.seek_upward = 1;
			p_alex->vfs.wrap_around = 1;
			alex_seek_freq(p_alex);
			break;

		case 't':
		case 'T':
			p_alex->vfs.tuner = 0;
			p_alex->vfs.type = p_alex->vt.type;
			p_alex->vfs.seek_upward = 0;
			p_alex->vfs.wrap_around = 1;
			alex_seek_freq(p_alex);
			break;

		case 'v':
		case 'V':
			alex_set_vol(p_alex, p_alex->vol + 1);
			break;

		case 'x':
		case 'X':
			alex_set_vol(p_alex, p_alex->vol - 1);
			break;
			
		case 'a':
		case 'A':
			if (p_alex->seek_snr < 0x7F)
			{
				p_alex->seek_snr ++;
				alex_set_tuner(p_alex);
			}
			break;
			
		case 'b':
		case 'B':
			if (p_alex->seek_snr > 0)
			{
				p_alex->seek_snr --;
				alex_set_tuner(p_alex);
			}
			break;
			
		case 'm':
		case 'M':
			if (p_alex->seek_rssi < 0x7F)
			{
				p_alex->seek_rssi ++;
				alex_set_tuner(p_alex);
			}
			break;

		case 'n':
		case 'N':
			if (p_alex->seek_rssi > 0)
			{
				p_alex->seek_rssi --;
				alex_set_tuner(p_alex);
			}
			break;

		case 'p':
		case 'P':
			alex_print_status(p_alex);
			break;
			
		case 'q':
		case 'Q':
			exit = true;
			goto finish;
			break;
		}
	}

finish:

	alex_close_radio(p_alex);
	
	if (p_alex)
		free(p_alex);
	
	if (ret != 0)
		printf("Error:: %d\n", ret);
		
	printf("alex_fm_test::leave!\r\n");
	
	return ret;
}

bool alex_open_radio(alex_defs *p_dev)
{
	bool	ret = true;
	
	if ((p_dev->radio = open(RADIO_DEF_DEVICE, O_RDONLY)) < 0)
	{
		printf("alex_open_radio::Open %s failed!\n", RADIO_DEF_DEVICE);
		ret = false;
		goto finish;
	}
	
	if (ioctl(p_dev->radio, VIDIOC_QUERYCAP, &(p_dev->vc)) < 0)
	{
		printf("alex_open_radio::Error: VIDIOC_QUERYCAP\n");
		ret = false;
		goto finish;
	}

finish:

	return ret;
}

int alex_get_tuner(alex_defs *p_dev)
{
	int	ret = 0;

	if (p_dev->radio < 0)
		return -1;

	if (ioctl(p_dev->radio, VIDIOC_G_TUNER, &(p_dev->vt)) < 0)
	{
		printf("alex_get_tuner::Error: VIDIOC_G_TUNER\n");
		ret = -1;
	}

	p_dev->freq_min = FREQ_V4L2_TO_DEV(p_dev->vt.rangelow);
	p_dev->freq_max = FREQ_V4L2_TO_DEV(p_dev->vt.rangehigh);
	p_dev->signal = p_dev->vt.signal;
	p_dev->pilot = p_dev->vt.pilot;
	p_dev->seek_snr = p_dev->vt.seek_snr;
	p_dev->seek_rssi = p_dev->vt.seek_rssi;

	return ret;
}

int alex_set_tuner(alex_defs *p_dev)
{
	int	ret = 0;

	if (p_dev->radio < 0)
		return -1;
		
	p_dev->vt.seek_snr = p_dev->seek_snr;
	p_dev->vt.seek_rssi = p_dev->seek_rssi;

	if (ioctl(p_dev->radio, VIDIOC_S_TUNER, &(p_dev->vt)) < 0)
	{
		printf("alex_get_tuner::Error: VIDIOC_S_TUNER\n");
		ret = -1;
	}

	return ret;
}

void alex_close_radio(alex_defs *p_dev)
{
	if (p_dev->radio >= 0)
	{
		close(p_dev->radio);
		p_dev->radio = -1;
	}
}

int alex_set_freq(alex_defs *p_dev, unsigned int freq)
{
	int	res = -1;
	
	if (p_dev->radio < 0)
		return -1;

	p_dev->vf.tuner = 0;
	p_dev->vf.frequency = FREQ_DEV_TO_V4L2(freq);
 
	res = ioctl(p_dev->radio, VIDIOC_S_FREQUENCY, &(p_dev->vf));
	
	if (res > 0)
		p_dev->freq = freq;

	return res;
}	

int alex_get_freq(alex_defs *p_dev)
{
	int				res = -1;
	unsigned long	freq;
	
	if (p_dev->radio < 0)
		return -1;
	
	res = ioctl(p_dev->radio, VIDIOC_G_FREQUENCY, &(p_dev->vf));
	
	if(res < 0)
		return -1;
	
	freq = FREQ_V4L2_TO_DEV(p_dev->vf.frequency);
	
	p_dev->freq = freq;
	
	return freq;
}

int alex_seek_freq(alex_defs *p_dev)
{
	if (p_dev->radio < 0)
		return -1;

	if (ioctl(p_dev->radio, VIDIOC_S_HW_FREQ_SEEK, &(p_dev->vfs)) < 0)
		return -1;

	return 0;
}

int alex_get_vol(alex_defs *p_dev)
{
	if (p_dev->radio < 0)
		return -1;
		
	p_dev->vcl.id = V4L2_CID_AUDIO_VOLUME;
	if (ioctl(p_dev->radio, VIDIOC_G_CTRL, &(p_dev->vcl)) < 0)
		return -1;
		
	p_dev->vol = p_dev->vcl.value;
		
	return p_dev->vcl.value;
}

int alex_set_vol(alex_defs *p_dev, unsigned int vol)
{
	if (p_dev->radio < 0)
		return -1;
	
	p_dev->vcl.id = V4L2_CID_AUDIO_VOLUME;
	p_dev->vcl.value = vol;

	if (ioctl(p_dev->radio, VIDIOC_S_CTRL, &(p_dev->vcl)) < 0)
		return -1;
	
	return 0;
}

int	alex_get_mute(alex_defs *p_dev)
{
	if (p_dev->radio < 0)
		return -1;

	p_dev->vcl.id = V4L2_CID_AUDIO_MUTE;
	if (ioctl(p_dev->radio, VIDIOC_G_CTRL, &(p_dev->vcl)) < 0)
		return -1;
		
	return p_dev->vcl.value;
}

int alex_set_mute(alex_defs *p_dev, unsigned int mute)
{
	if (p_dev->radio < 0)
		return -1;

	p_dev->vcl.id = V4L2_CID_AUDIO_MUTE;
	p_dev->vcl.value = mute;

	if (ioctl(p_dev->radio, VIDIOC_S_CTRL, &(p_dev->vcl)) < 0)
		return -1;
	
	return 0;
}

void alex_print_status(alex_defs *p_dev)
{
	printf("p_dev->vt.name = %s\n", p_dev->vt.name);
	printf("p_dev->vt.type = %d\n", p_dev->vt.type);
	printf("p_dev->vt.type = 0x%x\n", p_dev->vt.capability);
	printf("p_dev->vt.rangelow = %d, %d\n", p_dev->vt.rangelow, FREQ_V4L2_TO_DEV(p_dev->vt.rangelow));
	printf("p_dev->vt.rangehigh = %d, %d\n", p_dev->vt.rangehigh, FREQ_V4L2_TO_DEV(p_dev->vt.rangehigh));
	printf("p_dev->vt.rxsubchans = 0x%x\n", p_dev->vt.rxsubchans);
	printf("p_dev->vt.audmode = 0x%x\n", p_dev->vt.audmode);
	printf("p_dev->vt.signal = 0x%x\n", p_dev->vt.signal);
	printf("p_dev->vt.afc = 0x%x\n", p_dev->vt.afc);
	printf("p_dev->vt.pilot = 0x%x\n", p_dev->vt.pilot);
	printf("p_dev->vt.seek_snr = 0x%x\n", p_dev->vt.seek_snr);
	printf("p_dev->vt.seek_rssi = 0x%x\n", p_dev->vt.seek_rssi);
}

