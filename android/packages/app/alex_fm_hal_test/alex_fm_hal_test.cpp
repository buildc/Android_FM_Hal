
#include <string.h>
#include <stdint.h>
#include <string.h>
#include <sys/cdefs.h>
#include <sys/types.h>
#include <cutils/log.h>
#include <utils/Timers.h>
#include <tinyalsa/asoundlib.h>
#include "alex_fm_hal_test.h"

int main(int argc, char** argv)
{
	int		ret = 0;
	int		err = 0;
	int		force_power = 0;
	bool	exit = false;
	char	buf;
	char	tmp_freq[20];

	struct fm_module_t			*module;
	struct fm_si4704_device_t	*device;
	
	printf("alex_fm_hal_test::start!\r\n");

	err = hw_get_module(FM_HARDWARE_MODULE_ID, (hw_module_t const**)&module);
	if (err != 0)
	{
		printf("alex_fm_hal_test::hw_get_module(%s) failed (%s)\n", FM_HARDWARE_MODULE_ID, strerror(-err));
		ret = -1;
		goto finish;
	}

	err = libfm_open(&module->common, &device);
	if (err != 0)
	{
		printf("alex_fm_hal_test::libfm_open() failed (%s)\n", strerror(-err));
		ret = -1;
		goto finish;
	}

	err = libfm_open_radio(device);
	if (err != 0)
	{
		printf("alex_fm_hal_test::libfm_open_radio() failed (%s)\n", strerror(-err));
		ret = -1;
		goto finish;
	}

	alex_set_fm_onoff(1);

	while (!exit)
	{
		alex_get_tuner(device);
		alex_get_freq(device);
		alex_get_vol(device);

		force_power = alex_get_fm_force_power(device);

		printf("\n+++ alex_fm_hal_test +++\n");
		
		printf("\n");
		printf("Max Freq: %d\n", device->defs.freq_max);
		printf("Min Freq: %d\n", device->defs.freq_min);
		printf("Current Freq: %d\n", device->defs.freq);
		if (device->defs.pilot)
			printf("Signal: %d dB stereo\n", device->defs.signal);
		else
			printf("Signal: %d dB mono\n", device->defs.signal);

		printf("Stereo: %d %%\n", device->defs.stblend);
		printf("RSSI: %d\n", device->defs.rssi);
		printf("SNR: %d\n", device->defs.snr);
		printf("Multipath: %d\n", device->defs.multi);
		
		printf("Volume: %d\n", device->defs.vol);
		printf("Seek RSSI: %d\n", device->defs.seek_rssi);
		printf("Seek SNR: %d\n", device->defs.seek_snr);

		printf("Blend RSSI Stereo Threshold: %d\n", device->defs.blend_rssi_stereo_threshold);
		printf("Blend RSSI Mono Threshold: %d\n", device->defs.blend_rssi_mono_threshold);
		printf("Blend SNR Stereo Threshold: %d\n", device->defs.blend_snr_stereo_threshold);
		printf("Blend SNR Mono Threshold: %d\n", device->defs.blend_snr_mono_threshold);
		printf("Blend Multipath Stereo Threshold: %d\n", device->defs.blend_multi_stereo_threshold);
		printf("Blend Multipath Mono Threshold: %d\n", device->defs.blend_multi_mono_threshold);

		printf("\n");
		printf("[Z] Set to 10770\n");
		printf("[U] Up Freq\n");
		printf("[W] Down Freq\n");
		printf("[S] Seek Freq Up\n");
		printf("[T] Seek Freq Down\n");
		printf("[V] Volume Up\n");
		printf("[X] Volume Down\n");
		printf("[1] Seek RSSI +\n");
		printf("[2] Seek RSSI -\n");
		printf("[3] Seek SNR +\n");
		printf("[4] Seek SNR -\n");
		printf("[5] Blend RSSI Stereo Threshold +\n");
		printf("[6] Blend RSSI Stereo Threshold -\n");
		printf("[7] Blend RSSI MONO Threshold +\n");
		printf("[8] Blend RSSI MONO Threshold -\n");
		printf("[9] Blend SNR Stereo Threshold +\n");
		printf("[A] Blend SNR Stereo Threshold -\n");
		printf("[B] Blend SNR Mono Threshold +\n");
		printf("[C] Blend SNR Mono Threshold -\n");
		printf("[D] Blend Multi Stereo Threshold +\n");
		printf("[E] Blend Multi Stereo Threshold -\n");
		printf("[F] Blend Multi Mono Threshold +\n");
		printf("[G] Blend Multi Mono Threshold -\n");
		printf("[P] Print Status\n");
		printf("[L] Force Power: %d\n", force_power);
		printf("[Q] Quit!\n\n");

		printf("--- alex_fm_hal_test ---\n\n");

		buf = getchar();

		switch (buf)
		{
		case 'z':
		case 'Z':
			alex_set_freq(device, 10770);
			break;
			
		case 'u':
		case 'U':
			if (device->defs.freq < device->defs.freq_max)
				alex_set_freq(device, device->defs.freq + 10);
			break;
			
		case 'w':
		case 'W':
			if (device->defs.freq > device->defs.freq_min)
				alex_set_freq(device, device->defs.freq - 10);
			break;

		case 's':
		case 'S':
			device->defs.vfs.tuner = 0;
			device->defs.vfs.type = device->defs.vt.type;
			device->defs.vfs.seek_upward = 1;
			device->defs.vfs.wrap_around = 1;
			alex_seek_freq(device);
			break;

		case 't':
		case 'T':
			device->defs.vfs.tuner = 0;
			device->defs.vfs.type = device->defs.vt.type;
			device->defs.vfs.seek_upward = 0;
			device->defs.vfs.wrap_around = 1;
			alex_seek_freq(device);
			break;

		case 'v':
		case 'V':
			alex_set_vol(device, device->defs.vol + 1);
			break;

		case 'x':
		case 'X':
			alex_set_vol(device, device->defs.vol - 1);
			break;
			
		case '1':
			if (device->defs.seek_rssi < 0x7F)
			{
				device->defs.seek_rssi ++;
				alex_set_tuner(device);
			}
			break;

		case '2':
			if (device->defs.seek_rssi > 0)
			{
				device->defs.seek_rssi --;
				alex_set_tuner(device);
			}
			break;

		case '3':
			if (device->defs.seek_snr < 0x7F)
			{
				device->defs.seek_snr ++;
				alex_set_tuner(device);
			}
			break;
			
		case '4':
			if (device->defs.seek_snr > 0)
			{
				device->defs.seek_snr --;
				alex_set_tuner(device);
			}
			break;

		case '5':
			if (device->defs.blend_rssi_stereo_threshold < 0x7F)
			{
				device->defs.blend_rssi_stereo_threshold ++;
				alex_set_tuner(device);
			}
			break;
			
		case '6':
			if (device->defs.blend_rssi_stereo_threshold > 0)
			{
				device->defs.blend_rssi_stereo_threshold --;
				alex_set_tuner(device);
			}
			break;

		case '7':
			if (device->defs.blend_rssi_mono_threshold < 0x7F)
			{
				device->defs.blend_rssi_mono_threshold ++;
				alex_set_tuner(device);
			}
			break;
			
		case '8':
			if (device->defs.blend_rssi_mono_threshold > 0)
			{
				device->defs.blend_rssi_mono_threshold --;
				alex_set_tuner(device);
			}
			break;

		case '9':
			if (device->defs.blend_snr_stereo_threshold < 0x7F)
			{
				device->defs.blend_snr_stereo_threshold ++;
				alex_set_tuner(device);
			}
			break;
			
		case 'a':
		case 'A':
			if (device->defs.blend_snr_stereo_threshold > 0)
			{
				device->defs.blend_snr_stereo_threshold --;
				alex_set_tuner(device);
			}
			break;

		case 'b':
		case 'B':
			if (device->defs.blend_snr_mono_threshold < 0x7F)
			{
				device->defs.blend_snr_mono_threshold ++;
				alex_set_tuner(device);
			}
			break;
			
		case 'c':
		case 'C':
			if (device->defs.blend_snr_mono_threshold > 0)
			{
				device->defs.blend_snr_mono_threshold --;
				alex_set_tuner(device);
			}
			break;

		case 'd':
		case 'D':
			if (device->defs.blend_multi_stereo_threshold < 0x64)
			{
				device->defs.blend_multi_stereo_threshold ++;
				alex_set_tuner(device);
			}
			break;
			
		case 'e':
		case 'E':
			if (device->defs.blend_multi_stereo_threshold > 0)
			{
				device->defs.blend_multi_stereo_threshold --;
				alex_set_tuner(device);
			}
			break;

		case 'f':
		case 'F':
			if (device->defs.blend_multi_mono_threshold < 0x64)
			{
				device->defs.blend_multi_mono_threshold ++;
				alex_set_tuner(device);
			}
			break;
			
		case 'g':
		case 'G':
			if (device->defs.blend_multi_mono_threshold > 0)
			{
				device->defs.blend_multi_mono_threshold --;
				alex_set_tuner(device);
			}
			break;

		case 'p':
		case 'P':
			alex_print_status(device);
			break;
			
		case 'l':
		case 'L':
			if (force_power == 1)
				alex_set_fm_force_power(device, 0);
			else if (force_power == 0)
				alex_set_fm_force_power(device, 1);
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
		printf("alex_fm_hal_test::failed!\r\n");

	alex_set_fm_onoff(0);

	libfm_close_radio(device);

	err = libfm_close(device);
	if (err != 0)
		printf("alex_fm_hal_test::libfm_close() failed (%s)\n", strerror(-err));

	printf("alex_fm_hal_test::leave!\r\n");

	return ret;
}

int alex_get_tuner(fm_si4704_device_t *p_dev)
{
	int	ret = 0;

	ret = libfm_get_tuner(p_dev);

	return ret;
}

int alex_set_tuner(fm_si4704_device_t *p_dev)
{
	int	ret = 0;

	ret = libfm_set_tuner(p_dev);

	return ret;
}

int alex_set_freq(fm_si4704_device_t *p_dev, unsigned int freq)
{
	int ret = 0;
	
	ret = libfm_set_freq(p_dev, freq);
	
	return ret;
}

int alex_get_freq(fm_si4704_device_t *p_dev)
{
	int ret = 0;
	
	ret = libfm_get_freq(p_dev);
	
	return ret;
}

int alex_seek_freq(fm_si4704_device_t *p_dev)
{
	int ret = 0;
	
	ret = libfm_seek_freq(p_dev);
	
	return ret;
}

int alex_get_vol(fm_si4704_device_t *p_dev)
{
	int	ret = 0;
	
	ret = libfm_get_vol(p_dev);
	
	return ret;
}

int alex_set_vol(fm_si4704_device_t *p_dev, unsigned int vol)
{
	int ret = 0;
	
	ret = libfm_set_vol(p_dev, vol);
	
	return ret;
}

int alex_get_mute(fm_si4704_device_t *p_dev)
{
	int ret = 0;
	
	ret = libfm_get_mute(p_dev);
	
	return ret;
}

int alex_set_mute(fm_si4704_device_t *p_dev, unsigned int mute)
{
	int	ret = 0;
	
	ret = libfm_set_mute(p_dev, mute);
	
	return ret;
}

void alex_print_status(fm_si4704_device_t *p_dev)
{
	libfm_get_status(p_dev);
}

void alex_set_fm_onoff(bool on)
{
	struct mixer		*mMixer;
	struct mixer_ctl	*mRouteCtl;
	struct alex_mixer	*mixer;

	if (on)
        mixer = device_input_FM_ON;
	else
        mixer = device_input_FM_OFF;

	mMixer = mixer_open(0);

/*	printf("alex_set_fm_onoff::%d\n", on);	*/

	for(int cnt = 0; NULL != mixer[cnt].ctl; ++cnt)
	{
/*		printf("alex_set_fm_onoff::mixer[%d].ctl = %s\n", cnt, mixer[cnt].ctl);	*/
		mRouteCtl = mixer_get_ctl_by_name(mMixer, mixer[cnt].ctl);

		if (NULL != mRouteCtl)
		{
			for (unsigned int idx = 0; mixer_ctl_get_num_values(mRouteCtl) > idx; ++idx)
			{
/*				printf("alex_set_fm_onoff::mixer[%d].val = %d\n", cnt, mixer[cnt].val);	*/
				mixer_ctl_set_value(mRouteCtl, idx, mixer[cnt].val);
/*				printf("alex_set_fm_onoff::idx = %d\n", idx);	*/
			}
		}
	}

	mixer_close(mMixer);
}

int alex_get_fm_force_power(fm_si4704_device_t *p_dev)
{
	return libfm_get_force_power(p_dev);
}

void alex_set_fm_force_power(fm_si4704_device_t *p_dev, int on)
{
	libfm_set_force_power(p_dev, on);
}
