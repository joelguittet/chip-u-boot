/*
 * (C) Copyright 2016 NextThing Co
 * (C) Copyright 2016 Free Electrons
 *
 * Maxime Ripard <maxime.ripard@free-electrons.com>
 *
 * CHIP's DIP spec implementation in U-Boot
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <eeprom.h>
#include <w1.h>

#include <asm/arch/gpio.h>

#include <dm/device-internal.h>


#define dip_convert(field)						\
	(								\
		(sizeof(field) == 1) ? field :				\
		(sizeof(field) == 2) ? be16_to_cpu(field) :		\
		(sizeof(field) == 4) ? be32_to_cpu(field) :		\
		-1							\
	)

#define DIP_MAGIC	0x50494843	/* CHIP */

#define DIP_VID_NTC		0x9d011a
#define DIP_PID_NTC_POCKET	0x1
#define DIP_PID_NTC_VGA		0x2
#define DIP_PID_NTC_HDMI	0x3

struct dip_header {
	u32     magic;                  /* CHIP */
	u8      version;                /* spec version */
	u32     vendor_id;
	u16     product_id;
	u8      product_version;
	char    vendor_name[32];
	char    product_name[32];
	u8      rsvd[36];               /* rsvd for futre spec versions */
	u8      data[16];               /* user data, per-dip specific */
} __packed;

enum disp_output {
	DISPLAY_COMPOSITE,
	DISPLAY_RGB_HDMI_BRIDGE,
	DISPLAY_RGB_VGA_BRIDGE,
	DISPLAY_RGB_POCKET,
};

static char dip_name[64];

static void dip_setup_pocket_display(enum disp_output display)
{
	char *env_var;

	switch (display) {
	case DISPLAY_RGB_HDMI_BRIDGE:
		env_var = "sunxi:1024x768-24@60,monitor=hdmi";
		break;

	case DISPLAY_RGB_VGA_BRIDGE:
		env_var = "sunxi:1024x768-24@60,monitor=vga";
		break;

	case DISPLAY_RGB_POCKET:
		env_var = "sunxi:480x272-16@60,monitor=lcd";
		break;

	default:
		env_var = "sunxi:720x480-24@60,monitor=composite-ntsc,overscan_x=40,overscan_y=20";
		break;
	}

	setenv("video-mode", env_var);
}

static void dip_detect(void)
{
	struct udevice *bus, *dev;
	u8 display = DISPLAY_COMPOSITE;
	u32 vid;
	u16 pid;
	int ret;

	sunxi_gpio_set_pull(SUNXI_GPD(2), SUNXI_GPIO_PULL_UP);

	w1_get_bus(0, &bus);

	for (device_find_first_child(bus, &dev); dev;
	     device_find_next_child(&dev)) {
		struct dip_header header;

		if (w1_get_device_family(dev) != W1_FAMILY_DS2431)
			continue;

		ret = device_probe(dev);
		if (ret) {
			printf("Couldn't probe device %s: error %d",
			       dev->name, ret);
			continue;
		}

		eeprom_read_buf(dev, 0, (u8 *)&header, sizeof(header));

		if (header.magic != DIP_MAGIC)
			continue;

		vid = dip_convert(header.vendor_id);
		pid = dip_convert(header.product_id);

		printf("DIP: Found %s (0x%x) from %s (0x%x) detected\n",
		       header.product_name, pid,
		       header.vendor_name, vid);

		snprintf(dip_name, 64, "dip-%x-%x.dtbo", vid, pid);

		if (vid == DIP_VID_NTC) {
			switch (pid) {
			case DIP_PID_NTC_POCKET:
				display = DISPLAY_RGB_POCKET;
				break;

			case DIP_PID_NTC_HDMI:
				setenv("kernelarg_video", "video=HDMI-A-1:1024x768@60");
				display = DISPLAY_RGB_HDMI_BRIDGE;
				break;

			case DIP_PID_NTC_VGA:
				setenv("kernelarg_video", "video=VGA-1:1024x768@60");
				display = DISPLAY_RGB_VGA_BRIDGE;
				break;
			}
		}
	}

	dip_setup_pocket_display(display);
}

int board_video_pre_init(void)
{
	dip_detect();

	return 0;
}

int chip_dip_dt_setup(void)
{
	int ret;
	char *cmd;

	cmd = getenv("dip_overlay_cmd");
	if (!cmd)
		return 0;

	setenv("dip_overlay_name", dip_name);
	ret = run_command(cmd, 0);
	if (ret)
		return 0;

	return run_command("fdt apply $dip_addr_r", 0);
}
