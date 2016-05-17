
#include <common.h>
#include <i2c.h>

#include "videomodes.h"

#define CH7033_ADDR			0x76

#define CH7033_PAGE_SEL_REG		0x03

#define CH7033_POWER_STATE_4_REG	0x0a
#define CH7033_POWER_STATE_4_MEM_INIT		BIT(7)
#define CH7033_POWER_STATE_4_MEM_STOP		BIT(4)

#define CH7033_INPUT_TIMING_1_REG	0x0b
#define CH7033_INPUT_TIMING_1_HTI(val)		(((val >> 8) & 0xf) << 3)
#define CH7033_INPUT_TIMING_1_HAI(val)		((val >> 8) & 0x7)

#define CH7033_INPUT_TIMING_2_REG	0x0c
#define CH7033_INPUT_TIMING_2_HAI(val)		(val & 0xff)

#define CH7033_INPUT_TIMING_3_REG	0x0d
#define CH7033_INPUT_TIMING_3_HTI(val)		(val & 0xff)

#define CH7033_INPUT_TIMING_4_REG	0x0e
#define CH7033_INPUT_TIMING_4_HWI(val)		(((val >> 8) & 0x7) << 3)
#define CH7033_INPUT_TIMING_4_HOI(val)		((val >> 8) & 0x7)

#define CH7033_INPUT_TIMING_5_REG	0x0f
#define CH7033_INPUT_TIMING_5_HOI(val)		(val & 0xff)

#define CH7033_INPUT_TIMING_6_REG	0x10
#define CH7033_INPUT_TIMING_6_HWI(val)		(val & 0xff)

#define CH7033_INPUT_TIMING_7_REG	0x11
#define CH7033_INPUT_TIMING_7_VTI(val)		(((val >> 8) & 0x7) << 3)
#define CH7033_INPUT_TIMING_7_VAI(val)		((val >> 8) & 0x7)

#define CH7033_INPUT_TIMING_8_REG	0x12
#define CH7033_INPUT_TIMING_8_VAI(val)		(val & 0xff)

#define CH7033_INPUT_TIMING_9_REG	0x13
#define CH7033_INPUT_TIMING_9_VTI(val)		(val & 0xff)

#define CH7033_INPUT_TIMING_10_REG	0x14
#define CH7033_INPUT_TIMING_10_VWI(val)		(((val >> 8) & 0x7) << 3)
#define CH7033_INPUT_TIMING_10_VOI(val)		((val >> 8) & 0x7)

#define CH7033_INPUT_TIMING_11_REG	0x15
#define CH7033_INPUT_TIMING_11_VOI(val)		(val & 0xff)

#define CH7033_INPUT_TIMING_12_REG	0x16
#define CH7033_INPUT_TIMING_12_VWI(val)		(val & 0xff)

#define CH7033_INPUT_POL_REG		0x19
#define CH7033_INPUT_POL_HSYNC_HI		BIT(5)
#define CH7033_INPUT_POL_VSYNC_HI		BIT(4)
#define CH7033_INPUT_POL_DE_HI			BIT(3)
#define CH7033_INPUT_POL_GCLK(val)		((val >> 16) & 0x3)

#define CH7033_GCLK_1_REG		0x1a
#define CH7033_GCLK_1_FREQ(val)			((val >> 8) & 0xff)

#define CH7033_GCLK_2_REG		0x1b
#define CH7033_GCLK_2_FREQ(val)			((val) & 0xff)

#define CH7033_OUTPUT_TIMING_1_REG	0x1f
#define CH7033_OUTPUT_TIMING_1_HTO(val)		(((val >> 8) & 0xf) << 3)
#define CH7033_OUTPUT_TIMING_1_HAO(val)		((val >> 8) & 0x7)

#define CH7033_OUTPUT_TIMING_2_REG	0x20
#define CH7033_OUTPUT_TIMING_2_HAO(val)		(val & 0xff)

#define CH7033_OUTPUT_TIMING_3_REG	0x21
#define CH7033_OUTPUT_TIMING_3_HTO(val)		(val & 0xff)

#define CH7033_OUTPUT_TIMING_7_REG	0x25
#define CH7033_OUTPUT_TIMING_7_VTO(val)		(((val >> 8) & 0x7) << 3)
#define CH7033_OUTPUT_TIMING_7_VAO(val)		((val >> 8) & 0x7)

#define CH7033_OUTPUT_TIMING_8_REG	0x26
#define CH7033_OUTPUT_TIMING_8_VAO(val)		(val & 0xff)

#define CH7033_OUTPUT_TIMING_9_REG	0x27
#define CH7033_OUTPUT_TIMING_9_VTO(val)		(val & 0xff)

#define CH7033_OUTPUT_TIMING_4_REG	0x54
#define CH7033_OUTPUT_TIMING_4_HWO(val)		(((val >> 8) & 0x7) << 3)
#define CH7033_OUTPUT_TIMING_4_HOO(val)		((val >> 8) & 0x7)

#define CH7033_OUTPUT_TIMING_5_REG	0x55
#define CH7033_OUTPUT_TIMING_5_HOO(val)		(val & 0xff)

#define CH7033_OUTPUT_TIMING_6_REG	0x56
#define CH7033_OUTPUT_TIMING_6_HWO(val)		(val & 0xff)

#define CH7033_OUTPUT_TIMING_10_REG	0x57
#define CH7033_OUTPUT_TIMING_10_VWO(val)	(((val >> 8) & 0x7) << 3)
#define CH7033_OUTPUT_TIMING_10_VOO(val)	((val >> 8) & 0x7)

#define CH7033_OUTPUT_TIMING_11_REG	0x58
#define CH7033_OUTPUT_TIMING_11_VOO(val)	(val & 0xff)

#define CH7033_OUTPUT_TIMING_12_REG	0x59
#define CH7033_OUTPUT_TIMING_12_VWO(val)	(val & 0xff)

static void ch7033_write(u8 addr, u8 page, u8 reg, u8 value)
{
	i2c_reg_write(addr, CH7033_PAGE_SEL_REG, page);
	i2c_reg_write(addr, reg, value);
}

static int ch7033_update_bits(u8 addr, u8 page, u8 reg, u8 mask, u8 bits)
{
	u8 data;

	i2c_reg_write(addr, CH7033_PAGE_SEL_REG, page);

	data = i2c_reg_read(addr, reg);
	data &= ~(mask);
	i2c_reg_write(addr, reg, data | bits);

	return 0;
}


#define R007_PD_IO			0x20	// always ON
#define R007_DRI_PD			0x08	// HDMI(CH7033/5)/LVDS(CH7034) 4 serial drivers, postpine to power on it for LVDS
#define	R007_PDMIO			0x04	// SDRAM IO power, special sequence
#define R007_PDPLL1			0x02	// SDRAM PLL, special sequence, turn off last

#define	R009_SCLPD			0x10	// SDRAM clock, special sequence
#define	R009_SDPD			0x08	// SDRAM contol logic,  special sequence
#define R009_HDMI_PD			0x01

#define	R00A_MEMPD			0x20	// SDRAM PD, sepcial sequence

#define R16B_DRISER_PD		0x01	// always be 0

#define R16C_DRIPLL_PD			0x02

static void ch7033_memory_init(u8 addr)
{
	ch7033_update_bits(addr, 0, CH7033_POWER_STATE_4_REG,
			   CH7033_POWER_STATE_4_MEM_INIT | CH7033_POWER_STATE_4_MEM_STOP,
			   CH7033_POWER_STATE_4_MEM_INIT | CH7033_POWER_STATE_4_MEM_STOP);

	ch7033_update_bits(addr, 0, CH7033_POWER_STATE_4_REG,
			   CH7033_POWER_STATE_4_MEM_STOP,
			   0);
}

static void ch7033_unknown_init(u8 addr)
{

	ch7033_write(addr, 0, 0x5e, 0x54);
	ch7033_write(addr, 0, 0x74, 0x30);
	ch7033_write(addr, 0, 0x7e, 0x8f);

	ch7033_write(addr, 1, 0x07, 0x66);
	ch7033_write(addr, 1, 0x0b, 0x75);
	ch7033_write(addr, 1, 0x0c, 0x6a);
	ch7033_write(addr, 1, 0x0d, 0x21);
	ch7033_write(addr, 1, 0x0f, 0x9d);

	ch7033_write(addr, 3, 0x28, 0x04);
	ch7033_write(addr, 3, 0x2a, 0x28);
}

static int ch7033_power(u8 addr)
{
	/* Main Power Up */
	ch7033_update_bits(addr, 0, 0x07,
			   R007_PD_IO | R007_PDPLL1 | R007_PDMIO | R007_DRI_PD,
			   0);

	/* Power up SDRAM clock and power domain, and HDMI block */
	ch7033_update_bits(addr, 0, 0x09,
			   R009_SCLPD | R009_SDPD | R009_HDMI_PD,
			   0);

	ch7033_update_bits(addr, 0, 0x0a, R00A_MEMPD, 0);

	/* Power up DRISER */
	ch7033_update_bits(addr, 1, 0x6b, R16B_DRISER_PD, 0);

	/* Power up DRI PLL */
	ch7033_update_bits(addr, 1, 0x6c, R16C_DRIPLL_PD, 0);

	return 0;
}

static void ch7033_reset(u8 addr)
{
	ch7033_write(addr, 4, 0x52, 0xC3);
	ch7033_write(addr, 4, 0x52, 0xC1);
	ch7033_write(addr, 4, 0x52, 0xC3);

	ch7033_write(addr, 0, 0x1C, 0x69);
	ch7033_write(addr, 0, 0x1D, 0x78);

	ch7033_write(addr, 1, 0x1E, 9);
}

static void ch7033_mode_set(u8 addr,
			    const struct ctfb_res_modes *mode)
{
	unsigned int htotal = mode->left_margin + mode->xres +
		mode->right_margin + mode->hsync_len;
	unsigned int vtotal = mode->upper_margin + mode->yres +
		mode->lower_margin + mode->vsync_len;
	u32 val;

	/* Setup the horizontal timings ... */
	ch7033_write(addr, 0, CH7033_INPUT_TIMING_1_REG,
		     CH7033_INPUT_TIMING_1_HTI(htotal) |
		     CH7033_INPUT_TIMING_1_HAI(mode->xres));

	ch7033_write(addr, 0, CH7033_INPUT_TIMING_2_REG,
		     CH7033_INPUT_TIMING_2_HAI(mode->xres));

	ch7033_write(addr, 0, CH7033_INPUT_TIMING_3_REG,
		     CH7033_INPUT_TIMING_3_HTI(htotal));

	ch7033_write(addr, 0, CH7033_INPUT_TIMING_4_REG,
		     CH7033_INPUT_TIMING_4_HOI(mode->right_margin) |
		     CH7033_INPUT_TIMING_4_HWI(mode->hsync_len));

	ch7033_write(addr, 0, CH7033_INPUT_TIMING_5_REG,
		     CH7033_INPUT_TIMING_5_HOI(mode->right_margin));

	ch7033_write(addr, 0, CH7033_INPUT_TIMING_6_REG,
		     CH7033_INPUT_TIMING_6_HWI(mode->hsync_len));

	/* ... And the vertical ones */
	ch7033_write(addr, 0, CH7033_INPUT_TIMING_7_REG,
		     CH7033_INPUT_TIMING_7_VTI(vtotal) |
		     CH7033_INPUT_TIMING_7_VAI(mode->yres));

	ch7033_write(addr, 0, CH7033_INPUT_TIMING_8_REG,
		     CH7033_INPUT_TIMING_8_VAI(mode->yres));

	ch7033_write(addr, 0, CH7033_INPUT_TIMING_9_REG,
		     CH7033_INPUT_TIMING_9_VTI(vtotal));

	ch7033_write(addr, 0, CH7033_INPUT_TIMING_10_REG,
		     CH7033_INPUT_TIMING_10_VOI(mode->lower_margin) |
		     CH7033_INPUT_TIMING_10_VWI(mode->vsync_len));

	ch7033_write(addr, 0, CH7033_INPUT_TIMING_11_REG,
		     CH7033_INPUT_TIMING_11_VOI(mode->lower_margin));

	ch7033_write(addr, 0, CH7033_INPUT_TIMING_12_REG,
		     CH7033_INPUT_TIMING_12_VWI(mode->vsync_len));

	/* Setup polarities and clock */
	val = CH7033_INPUT_POL_DE_HI;
	val |= (mode->sync & FB_SYNC_HOR_HIGH_ACT) ? CH7033_INPUT_POL_HSYNC_HI : 0;
	val |= (mode->sync & FB_SYNC_VERT_HIGH_ACT) ? CH7033_INPUT_POL_VSYNC_HI : 0;
	val |= CH7033_INPUT_POL_GCLK(mode->pixclock_khz);
	ch7033_write(addr, 0, CH7033_INPUT_POL_REG, val);

	ch7033_write(addr, 0, CH7033_GCLK_1_REG,
		     CH7033_GCLK_1_FREQ(mode->pixclock_khz));

	ch7033_write(addr, 0, CH7033_GCLK_2_REG,
		     CH7033_GCLK_2_FREQ(mode->pixclock_khz));

	/* Horizontal output timings ... */
	ch7033_write(addr, 0, CH7033_OUTPUT_TIMING_1_REG,
		     CH7033_OUTPUT_TIMING_1_HTO(htotal) |
		     CH7033_OUTPUT_TIMING_1_HAO(mode->xres));

	ch7033_write(addr, 0, CH7033_OUTPUT_TIMING_2_REG,
		     CH7033_OUTPUT_TIMING_2_HAO(mode->xres));

	ch7033_write(addr, 0, CH7033_OUTPUT_TIMING_3_REG,
		     CH7033_OUTPUT_TIMING_3_HTO(htotal));

	ch7033_write(addr, 0, CH7033_OUTPUT_TIMING_4_REG,
		     CH7033_OUTPUT_TIMING_4_HOO(mode->right_margin) |
		     CH7033_OUTPUT_TIMING_4_HWO(mode->hsync_len));

	ch7033_write(addr, 0, CH7033_OUTPUT_TIMING_5_REG,
		     CH7033_OUTPUT_TIMING_5_HOO(mode->right_margin));

	ch7033_write(addr, 0, CH7033_OUTPUT_TIMING_6_REG,
		     CH7033_OUTPUT_TIMING_6_HWO(mode->hsync_len));

	/* ... And the vertical ones */
	ch7033_write(addr, 0, CH7033_OUTPUT_TIMING_7_REG,
		     CH7033_OUTPUT_TIMING_7_VTO(vtotal) |
		     CH7033_OUTPUT_TIMING_7_VAO(mode->yres));

	ch7033_write(addr, 0, CH7033_OUTPUT_TIMING_8_REG,
		     CH7033_OUTPUT_TIMING_8_VAO(mode->yres));

	ch7033_write(addr, 0, CH7033_OUTPUT_TIMING_9_REG,
		     CH7033_OUTPUT_TIMING_9_VTO(vtotal));

	ch7033_write(addr, 0, CH7033_OUTPUT_TIMING_10_REG,
		     CH7033_OUTPUT_TIMING_10_VOO(mode->lower_margin) |
		     CH7033_OUTPUT_TIMING_10_VWO(mode->vsync_len));

	ch7033_write(addr, 0, CH7033_OUTPUT_TIMING_11_REG,
		     CH7033_OUTPUT_TIMING_11_VOO(mode->lower_margin));

	ch7033_write(addr, 0, CH7033_OUTPUT_TIMING_12_REG,
		     CH7033_OUTPUT_TIMING_12_VWO(mode->vsync_len));
}

void ch7033_init(unsigned int bus_num, const struct ctfb_res_modes *mode)
{
	unsigned int orig_bus = i2c_get_bus_num();

	i2c_set_bus_num(bus_num);

	ch7033_reset(CH7033_ADDR);
	ch7033_unknown_init(CH7033_ADDR);
	ch7033_power(CH7033_ADDR);
	ch7033_memory_init(CH7033_ADDR);

	ch7033_write(CH7033_ADDR, 0, 0x08, 0x0F);

	ch7033_mode_set(CH7033_ADDR, mode);

	i2c_set_bus_num(orig_bus);
}
