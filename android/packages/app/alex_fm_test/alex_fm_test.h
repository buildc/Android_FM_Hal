
#ifndef	__ALEX_FM_TEST_H__
#define	__ALEX_FM_TEST_H__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/videodev2.h>

/*
 * The frequency is set in units of 62.5 Hz when using V4L2_TUNER_CAP_LOW,
 * 62.5 kHz otherwise.
 * The tuner is able to have a channel spacing of 50, 100 or 200 kHz.
 * tuner->capability is therefore set to V4L2_TUNER_CAP_LOW
 * The FREQ_MUL is then: 1 MHz / 62.5 Hz = 16000
 */
#define FREQ_MUL 			(10000000 / 625)
#define FREQ_DEV_TO_V4L2(x)	(x * FREQ_MUL / 100)
#define FREQ_V4L2_TO_DEV(x)	(x * 100 / FREQ_MUL)

#define	RADIO_DEF_DEVICE	"/dev/radio0"

typedef struct
{
	int							radio;
	struct v4l2_capability		vc;
	struct v4l2_tuner			vt;
	struct v4l2_frequency		vf;
	struct v4l2_hw_freq_seek	vfs;
	struct v4l2_control			vcl;
	
	unsigned int				freq_max;
	unsigned int				freq_min;
	unsigned int				freq;
	unsigned int				signal;
	unsigned int				vol;
	unsigned int				pilot;
	unsigned int				seek_snr;
	unsigned int				seek_rssi;
} alex_defs, *p_alex_defs;

bool	alex_open_radio(alex_defs *p_dev);
int		alex_get_tuner(alex_defs *p_dev);
int		alex_set_tuner(alex_defs *p_dev);
void	alex_close_radio(alex_defs *p_dev);
int		alex_set_freq(alex_defs *p_dev, unsigned int freq);
int		alex_get_freq(alex_defs *p_dev);
int		alex_seek_freq(alex_defs *p_dev);
int		alex_get_vol(alex_defs *p_dev);
int		alex_set_vol(alex_defs *p_dev, unsigned int vol);
int		alex_get_mute(alex_defs *p_dev);
int		alex_set_mute(alex_defs *p_dev, unsigned int mute);

void	alex_print_status(alex_defs *p_dev);

#endif	// __ALEX_FM_TEST_H__
