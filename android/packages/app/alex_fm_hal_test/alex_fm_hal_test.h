
#ifndef	__ALEX_FM_HAL_TEST_H__
#define	__ALEX_FM_HAL_TEST_H__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <libfm.h>

struct alex_mixer
{
	const char	*ctl;
	const int	val;
};

alex_mixer device_input_FM_ON[] = {
	{"Input Clamp", 1},	
	{"FM In Switch", 1},	
	{"IN2R Switch", 1},
	{"Input Clamp", 0},
	{"Right Output Mixer Right Input Switch", 1},
	{"Left Output Mixer Right Input Switch", 1},
    {NULL, NULL}
};

alex_mixer device_input_FM_OFF[] = {	
	{"Right Output Mixer Right Input Switch", 0},
	{"Left Output Mixer Right Input Switch", 0},
	{"IN2R Switch", 0},
	{"FM In Switch", 0},
    {NULL, NULL}
};

int		alex_get_tuner(fm_si4704_device_t *p_dev);
int		alex_set_tuner(fm_si4704_device_t *p_dev);
int		alex_set_freq(fm_si4704_device_t *p_dev, unsigned int freq);
int		alex_get_freq(fm_si4704_device_t *p_dev);
int		alex_seek_freq(fm_si4704_device_t *p_dev);
int		alex_get_vol(fm_si4704_device_t *p_dev);
int		alex_set_vol(fm_si4704_device_t *p_dev, unsigned int vol);
int		alex_get_mute(fm_si4704_device_t *p_dev);
int		alex_set_mute(fm_si4704_device_t *p_dev, unsigned int mute);
void	alex_print_status(fm_si4704_device_t *p_dev);
void	alex_set_fm_onoff(bool on);
int		alex_get_fm_force_power(fm_si4704_device_t *p_dev);
void	alex_set_fm_force_power(fm_si4704_device_t *p_dev, int on);

#endif	// __ALEX_FM_HAL_TEST_H__
