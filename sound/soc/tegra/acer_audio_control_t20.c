/*
 * acer_audio_control.c - for WM8903 codec and fm2018 voice processor.
 *
 * Copyright (C) 2011 Acer, Inc.
 * Author: Andyl Liu <Andyl_Liu@acer.com.tw>
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/clk.h>
#include <linux/jiffies.h>
#include <linux/io.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/dma-mapping.h>
#include <linux/kthread.h>
#include <linux/moduleparam.h>
#include <linux/timer.h>
#include <linux/interrupt.h>
#include <linux/i2c.h>
#include <linux/regulator/consumer.h>
#include <linux/gpio.h>
#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/initval.h>
#include <sound/soc.h>
#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/soc-dapm.h>
#include <sound/soc-dai.h>
#include <sound/control.h>
#include <sound/tlv.h>
#include <asm/io.h>
#include <asm/mach-types.h>
#include <asm/hardware/scoop.h>

#include "acer_audio_control_t20.h"
#include "../codecs/wm8903.h"

#define AUDIO_CONTROL_DRIVER_NAME "acer-audio-control"

/* Module function */
static int acer_audio_control_probe(struct platform_device *pdev);
static int acer_audio_control_remove(struct platform_device *pdev);
static int switch_audio_table_single(int control_mode, bool fromAP);

/* extern */
extern int getAudioTable(void);
extern void setAudioTable(int table_value);
extern int get_headset_state(void);

extern struct acer_audio_data audio_data;

enum headset_state {
	BIT_NO_HEADSET = 0,
	BIT_HEADSET = (1 << 0),
	BIT_HEADSET_NO_MIC = (1 << 1),
};

int snd_soc_dapm_get_iconia_param(struct snd_kcontrol *kcontrol,
				struct snd_ctl_elem_value *ucontrol)
{
	struct snd_soc_codec *codec = snd_kcontrol_chip(kcontrol);
	const char *pin = (const char *)kcontrol->private_value;

	mutex_lock(&codec->mutex);

	/* TODO: get iconia param. */
	audio_data.pin = pin;

	mutex_unlock(&codec->mutex);

	return 0;
}
EXPORT_SYMBOL_GPL(snd_soc_dapm_get_iconia_param);

int snd_soc_dapm_put_iconia_param(struct snd_kcontrol *kcontrol,
				struct snd_ctl_elem_value *ucontrol)
{
	struct snd_soc_codec *codec = snd_kcontrol_chip(kcontrol);
	const char *pin = (const char *)kcontrol->private_value;
	int is_mode_new = ucontrol->value.integer.value[0];

	mutex_lock(&codec->mutex);

	audio_data.pin = pin;

	switch_audio_table(is_mode_new, false);

	mutex_unlock(&codec->mutex);

	return 0;
}
EXPORT_SYMBOL_GPL(snd_soc_dapm_put_iconia_param);

int snd_soc_dapm_info_iconia_param(struct snd_kcontrol *kcontrol,
				 struct snd_ctl_elem_info *uinfo)
{
	uinfo->type = SNDRV_CTL_ELEM_TYPE_INTEGER;
	uinfo->count = 1;
	uinfo->value.integer.min = 0;
	uinfo->value.integer.max = 1;

	return 0;
}
EXPORT_SYMBOL_GPL(snd_soc_dapm_info_iconia_param);

void set_int_mic_state(bool state)
{
	audio_data.state.int_mic = state;
}

void set_ext_mic_state(bool state)
{
	audio_data.state.ext_mic = state;
}

static void fm2018_switch(struct tegra_wm8903_platform_data *pdata)
{
	if (!audio_data.state.int_mic && !audio_data.state.ext_mic)
		gpio_set_value_cansleep(pdata->gpio_int_mic_en, 0);
	else
		gpio_set_value_cansleep(pdata->gpio_int_mic_en, 1);
}

void mic_switch(struct tegra_wm8903_platform_data *pdata)
{
	switch_audio_table(audio_data.mode.control, false);
	fm2018_switch(pdata);
}

static int switch_audio_table_single(int control_mode, bool fromAP)
{
	int state = get_headset_state();
	bool state_change = false;
	audio_data.mode.control = control_mode;

	if ((audio_data.state.old != state) && audio_data.AP_Lock) {
		state_change = true;
		audio_data.state.old = state;
		if(audio_data.mode.ap_control != ACOUSTIC_DEVICE_MIC_RECORDING_TABLE)
			control_mode = audio_data.mode.ap_control;
	}

	if (!audio_data.AP_Lock && !fromAP) {
		switch (audio_data.mode.control) {
			case CAMCORDER:
				if (state == BIT_HEADSET)
					audio_data.table.input = ACOUSTIC_HEADSET_MIC_RECORDING_TABLE;
				else
					audio_data.table.input = ACOUSTIC_CAMCORDER_RECORDER_TABLE;
				break;

			case MIC: /* For RECORD */
			case DEFAULT:
			default:
				if (state == BIT_HEADSET)
					audio_data.table.input = ACOUSTIC_HEADSET_MIC_RECORDING_TABLE;
				else
					audio_data.table.input = ACOUSTIC_DEVICE_MIC_RECORDING_TABLE;
		}
	}

	if (fromAP || state_change) {
		audio_data.mode.ap_control = control_mode;

		switch (control_mode) {
			case ACOUSTIC_CAMCORDER_RECORDER_TABLE: /* For Recorder */
				if (state == BIT_HEADSET)
					audio_data.table.input = ACOUSTIC_HEADSET_MIC_RECORDING_TABLE;
				else
					audio_data.table.input = control_mode;
				break;

			default:
				audio_data.table.input = control_mode;
				break;
		}

		if (audio_data.mode.ap_control == ACOUSTIC_DEVICE_MIC_RECORDING_TABLE)
			audio_data.AP_Lock = false;
		else
			audio_data.AP_Lock = true;
	}

	if (audio_data.state.int_mic || audio_data.state.ext_mic) {
		setAudioTable(audio_data.table.input);
	}

	return 1;
}

int switch_audio_table(int control_mode, bool fromAP)
{
	if (!fromAP) {
		audio_data.mode.input_source = control_mode;
	}

	return switch_audio_table_single(control_mode, fromAP);
}

/* platform driver register */
static struct platform_driver acer_audio_control_driver = {
	.probe  = acer_audio_control_probe,
	.remove = acer_audio_control_remove,
	.driver = {
		.name = AUDIO_CONTROL_DRIVER_NAME
	},
};

/* platform device register */
static struct platform_device acer_audio_control_device = {
	.name = AUDIO_CONTROL_DRIVER_NAME,
};

static int acer_audio_control_probe(struct platform_device *pdev)
{
	audio_data.table.input = ACOUSTIC_DEVICE_MIC_RECORDING_TABLE;
	audio_data.mode.control = DEFAULT;

	audio_data.state.int_mic = false;
	audio_data.state.ext_mic = false;
	audio_data.state.old = BIT_NO_HEADSET;
	audio_data.mode.ap_control = ACOUSTIC_DEVICE_MIC_RECORDING_TABLE;
	audio_data.AP_Lock = false;
	return 0;
}

static int acer_audio_control_remove(struct platform_device *pdev)
{
	return 0;
}

static int __init acer_audio_control_init(void)
{
	int ret;
	ret = platform_driver_register(&acer_audio_control_driver);
	if (ret) {
		pr_err("[acer_audio_control_driver] failed to register!!\n");
		return ret;
	}
	return platform_device_register(&acer_audio_control_device);
}

static void __exit acer_audio_control_exit(void)
{
	platform_device_unregister(&acer_audio_control_device);
	platform_driver_unregister(&acer_audio_control_driver);
}

module_init(acer_audio_control_init);
module_exit(acer_audio_control_exit);

MODULE_DESCRIPTION("ACER AUDIO CONTROL DRIVER");
MODULE_LICENSE("GPL");
