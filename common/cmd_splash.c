/*
 * Copyright (C) 2016 Next Thing Co.
 * Jose Angel Torres <jose@nextthing.co>
 *
 * SPDX-License-Identifier:     GPL-2.0+
 */

#include <common.h>
#include <command.h>
#include <malloc.h>
#include <video.h>
#include <watchdog.h>
#include <bmp_layout.h>

#include <splash_info.h>

static int do_num(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	printf("Number of splash images: %d\n", NUM_SPLASHES);

	return 0;
}

static int do_display(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	int splash_num = simple_strtoul(argv[1], NULL, 10);

	if (splash_num < 0 || splash_num > NUM_SPLASHES-1)
	{
		splash_num = 0;
	}

	video_display_splash(splash_num);
	
	return 0;
}

static cmd_tbl_t cmd_splash_sub[] = {
		U_BOOT_CMD_MKENT(num, 0, 1, do_num, "", ""),
		U_BOOT_CMD_MKENT(display, 1, 1, do_display, "", ""),
};

/*
 * Subroutine:  do_splash
 *
 * Description: Handler for 'splash' command..
 *
 * Inputs:	argv[1] contains the subcommand
 *
 * Return:      None
 *
 */
static int do_splash(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	cmd_tbl_t *c;

	if (argc < 1)
		return CMD_RET_USAGE;

	/* Strip off leading 'sound' command argument */
	argc--;
	argv++;

	c = find_cmd_tbl(argv[0], &cmd_splash_sub[0],
						ARRAY_SIZE(cmd_splash_sub));

	if (c)
		return c->cmd(cmdtp, flag, argc, argv);
	else
		return CMD_RET_USAGE;
}

U_BOOT_CMD(
		splash, 4, 1, do_splash,
		"splash sub-system",
		"num - lists the number of splash images\n"
		"display  - display a splash image\n"
);

