/*
 * Copyright (C) 2016 Next Thing Co.
 * Jose Angel Torres <software@nextthing.co>
 *
 * SPDX-License-Identifier:     GPL-2.0+
 */
 
#include <common.h>
#include <libfdt.h>
#include <fdtdec.h>
#include "sunxi-codec.h"
 
int sunxi_codec_init(const void *blob)
{
        int ret = 0;
        ret = fdt_check_header(blob);        
        printf("Check Header Returned: %d\n", ret);  
        printf("Version: %d\n", fdt_version(blob));     
        return ret;
}
 
 
