/*
 * Copyright (C) 2011 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ANDROID_FM_HAL_INTERFACE_H
#define	ANDROID_FM_HAL_INTERFACE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <strings.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/cdefs.h>
#include <sys/types.h>
#include <linux/videodev2.h>
#include <hardware/hardware.h>

__BEGIN_DECLS

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

#define	FM_RADIO_MODULE_DEVICE		"/dev/radio0"
#define FM_HARDWARE_MODULE_ID		"libfm"
#define FM_SI4704_CONTROLLER		"si4704"
#define	FM_SI4704_FORCE_POWER		"/sys/kernel/duv060_fm_radio/force_power_on"

struct si4704_defs
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
	unsigned int 				blend_rssi_stereo_threshold;
	unsigned int 				blend_rssi_mono_threshold;
	unsigned int 				blend_snr_stereo_threshold;
	unsigned int 				blend_snr_mono_threshold;
	unsigned int 				blend_multi_stereo_threshold;
	unsigned int 				blend_multi_mono_threshold;
	unsigned int				stblend;
	unsigned int				rssi;
	unsigned int				snr;
	unsigned int				multi;
};

struct fm_module_t
{
	struct hw_module_t	common;
};

struct fm_si4704_device_t
{
	struct hw_device_t	common;
	struct si4704_defs	defs;
	
	int		(* open_radio)(struct fm_si4704_device_t *dev);
	void	(* close_radio)(struct fm_si4704_device_t *dev);
	int		(* get_tuner)(struct fm_si4704_device_t *dev);
	int		(* set_tuner)(struct fm_si4704_device_t *dev);
	int		(* set_freq)(struct fm_si4704_device_t *dev, unsigned int freq);
	int		(* get_freq)(struct fm_si4704_device_t *dev);
	int		(* seek_freq)(struct fm_si4704_device_t *dev);
	int		(* get_vol)(struct fm_si4704_device_t *dev);
	int		(* set_vol)(struct fm_si4704_device_t *dev, unsigned int vol);
	int		(* get_mute)(struct fm_si4704_device_t *dev);
	int		(* set_mute)(struct fm_si4704_device_t *dev, unsigned int mute);
	void	(* get_status)(struct fm_si4704_device_t *dev);
	int		(* get_force_power)(struct fm_si4704_device_t *dev);
	void	(* set_force_power)(struct fm_si4704_device_t *dev, int on);
};

/** convenience API for opening and closing a device */

static inline int libfm_open(const struct hw_module_t *module, struct fm_si4704_device_t **dev)
{
	return module->methods->open(module, FM_SI4704_CONTROLLER, (struct hw_device_t **)dev);
}

static inline int libfm_close(struct fm_si4704_device_t *dev)
{
	return dev->common.close(&dev->common);
}

/* API */
static inline int libfm_open_radio(struct fm_si4704_device_t *dev)
{
	return dev->open_radio(dev);
}

static inline void libfm_close_radio(struct fm_si4704_device_t *dev)
{
	dev->close_radio(dev);
}

static inline int libfm_get_tuner(struct fm_si4704_device_t *dev)
{
	return dev->get_tuner(dev);
}

static inline int libfm_set_tuner(struct fm_si4704_device_t *dev)
{
	return dev->set_tuner(dev);
}

static inline int libfm_set_freq(struct fm_si4704_device_t *dev, unsigned int freq)
{
	return dev->set_freq(dev, freq);
}

static inline int libfm_get_freq(struct fm_si4704_device_t *dev)
{
	return dev->get_freq(dev);
}

static inline int libfm_seek_freq(struct fm_si4704_device_t *dev)
{
	return dev->seek_freq(dev);
}

static inline int libfm_get_vol(struct fm_si4704_device_t *dev)
{
	return dev->get_vol(dev);
}

static inline int libfm_set_vol(struct fm_si4704_device_t *dev, unsigned int vol)
{
	return dev->set_vol(dev, vol);
}

static inline int libfm_get_mute(struct fm_si4704_device_t *dev)
{
	return dev->get_mute(dev);
}

static inline int libfm_set_mute(struct fm_si4704_device_t *dev, unsigned int mute)
{
	return dev->set_mute(dev, mute);
}

static inline void libfm_get_status(struct fm_si4704_device_t *dev)
{
	dev->get_status(dev);
}

static inline int libfm_get_force_power(struct fm_si4704_device_t *dev)
{
	return dev->get_force_power(dev);
}

static inline void libfm_set_force_power(struct fm_si4704_device_t *dev, int on)
{
	dev->set_force_power(dev, on);
}

__END_DECLS

#endif	/* ANDROID_FM_HAL_INTERFACE_H */
