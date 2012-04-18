#ifndef __ACER_AUDIO_CONTROL_T20_H__
#define __ACER_AUDIO_CONTROL_T20_H__

#include "acer_audio_common.h"

#define ACOUSTIC_DEVICE_MIC_RECORDING_TABLE             0x02
#define ACOUSTIC_HEADSET_MIC_RECORDING_TABLE            0x04
#define ACOUSTIC_CAMCORDER_RECORDER_TABLE               0x06

extern void mic_switch(struct tegra_wm8903_platform_data *pdata);
extern void set_int_mic_state(bool state);
extern void set_ext_mic_state(bool state);
extern int switch_audio_table(int control_mode, bool fromAP);

enum mic_mode{
	SINGLE_MIC = 1,
	DUAL_MIC,
};

#define SOC_DAPM_SET_PARAM(xname) \
{	.iface = SNDRV_CTL_ELEM_IFACE_MIXER, .name = xname, \
	.info = snd_soc_dapm_info_iconia_param, \
	.get = snd_soc_dapm_get_iconia_param, \
	.put = snd_soc_dapm_put_iconia_param, \
	.private_value = (unsigned long)xname }

int snd_soc_dapm_info_iconia_param(struct snd_kcontrol *kcontrol,
	struct snd_ctl_elem_info *uinfo);
int snd_soc_dapm_get_iconia_param(struct snd_kcontrol *kcontrol,
	struct snd_ctl_elem_value *uncontrol);
int snd_soc_dapm_put_iconia_param(struct snd_kcontrol *kcontrol,
	struct snd_ctl_elem_value *uncontrol);

#endif
