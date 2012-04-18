/*
 * drivers/video/tegra/dc/edid.h
 *
 * Copyright (C) 2010 Google, Inc.
 * Author: Erik Gilling <konkers@android.com>
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

#ifndef __DRIVERS_VIDEO_TEGRA_DC_EDID_H
#define __DRIVERS_VIDEO_TEGRA_DC_EDID_H

#include <linux/i2c.h>
#include <linux/wait.h>
#include <mach/dc.h>

#define ELD_MAX_MNL	16
#define ELD_MAX_SAD	16
struct tegra_edid;

/*
 * ELD: EDID Like Data
 */
struct tegra_edid_hdmi_eld {
	u8	baseline_len;
	u8	eld_ver;
	u8	cea_edid_ver;
	char	monitor_name[ELD_MAX_MNL + 1];
	u8	mnl;
	u8	manufacture_id[2];
	u8	product_id[2];
	u8	port_id[8];
	u8	support_hdcp;
	u8	support_ai;
	u8	conn_type;
	u8	aud_synch_delay;
	u8	spk_alloc;
	u8	sad_count;
#if defined(CONFIG_ARCH_ACER_T30) || defined(CONFIG_ARCH_ACER_T20)
	u8	vsdb;
#endif
	u8	sad[ELD_MAX_SAD];
};

#if defined(CONFIG_ARCH_ACER_T30) || defined(CONFIG_ARCH_ACER_T20)
// The struct "tegra_edid_pvt" come from file "edid.c".
struct tegra_edid_pvt {
	struct kref			refcnt;
	struct tegra_edid_hdmi_eld	eld;
	bool				support_stereo;
	bool                            support_underscan;
	/* Note: dc_edid must remain the last member */
	struct tegra_dc_edid		dc_edid;
};

// The struct "tegra_edid" come from file "edid.c".
struct tegra_edid {
	struct i2c_client	*client;
	struct i2c_board_info	info;
	int			bus;

	struct tegra_edid_pvt	*data;

	struct mutex		lock;
	u8			vsdb;
};
#endif

struct tegra_edid *tegra_edid_create(int bus);
void tegra_edid_destroy(struct tegra_edid *edid);

int tegra_edid_get_monspecs_test(struct tegra_edid *edid,
				struct fb_monspecs *specs, u8 *edid_ptr);
int tegra_edid_get_monspecs(struct tegra_edid *edid, struct fb_monspecs *specs);
int tegra_edid_get_eld(struct tegra_edid *edid, struct tegra_edid_hdmi_eld *elddata);

struct tegra_dc_edid *tegra_edid_get_data(struct tegra_edid *edid);
void tegra_edid_put_data(struct tegra_dc_edid *data);

int tegra_edid_underscan_supported(struct tegra_edid *edid);
#endif
