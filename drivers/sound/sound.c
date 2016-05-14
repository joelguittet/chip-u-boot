/*
 * Copyright (C) 2016 Next Thing Co.
 * Jose Angel Torres <software@nextthing.co>
 *
 * Copyright (C) 2012 Samsung Electronics
 * R. Chandrasekar <rcsekar@samsung.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <asm/sound.h>
#ifdef CONFIG_SOUND_SUNXI
#include "sunxi-codec.h"
#endif

int sound_play(unsigned int msec, unsigned int frequency)
{
	return -1;
}

int sound_init(const void *blob)
{
	#ifdef CONFIG_SOUND_SUNXI
	return sunxi_codec_init(blob);
	#endif
	return 0;
}

void sound_create_square_wave(unsigned short *data, int size, uint32_t freq)
{
	const int sample = 48000;
	const unsigned short amplitude = 16000; /* between 1 and 32767 */
	const int period = freq ? sample / freq : 0;
	const int half = period / 2;

	assert(freq);

	/* Make sure we don't overflow our buffer */
	if (size % 2)
		size--;

	while (size) {
		int i;
		for (i = 0; size && i < half; i++) {
			size -= 2;
			*data++ = amplitude;
			*data++ = amplitude;
		}
		for (i = 0; size && i < period - half; i++) {
			size -= 2;
			*data++ = -amplitude;
			*data++ = -amplitude;
		}
	}
}
