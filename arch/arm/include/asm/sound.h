/*
 * Copyright (C) 2016 Next Thing Co.
 * Jose Angel Torres <software@nextthing.co>
 *
 * SPDX-License-Identifier:     GPL-2.0+
 */

#ifndef __SOUND_H
#define __SOUND_H

int sound_play(unsigned int msec, unsigned int frequency);

int sound_init(const void *blob);

#endif
