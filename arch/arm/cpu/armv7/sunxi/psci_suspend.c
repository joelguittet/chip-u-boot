/*
 * Copyright (C) 2016 Antoine Tenart <antoine.tenart@free-electrons.com>
 *
 * Based on Allwinner code.
 * Copyright 2007-2012 (C) Allwinner Technology Co., Ltd.
 *
 * SPDX-License-Identifier:	GPL-2.0
 */
#include <config.h>
#include <common.h>

#include <asm/arch/clock.h>
#include <asm/arch/dram.h>
#include <asm/armv7.h>
#include <asm/io.h>
#include <asm/psci.h>
#include <asm/secure.h>
#include <asm/system.h>

#include <linux/bitops.h>

static void __secure sunxi_enter_selfrefresh(struct sunxi_dram_reg *dram)
{
#ifndef CONFIG_MACH_SUN7I
	int i;

	/* disable all dram host ports */
	for (i = 0; i < 32; i++)
		clrbits_le32(&dram->hpcr[i], DRAM_HPCR_PORT_EN);
#endif

	/* disable auto-refresh */
	setbits_le32(&dram->drr, DRAM_DRR_AUTO_REFRESH_OFF);

	/* precharge all commands */
	clrsetbits_le32(&dram->dcr, GENMASK(27,31),
			BIT(27) | BIT(29) | BIT(31));

	/* enter into self-refresh */
	clrsetbits_le32(&dram->dcr, GENMASK(27,31), BIT(28) | BIT(31));

#ifndef CONFIG_MACH_SUN7I
	/* disable ITM */
	setbits_le32(&dram->ccr, DRAM_CCR_ITM_OFF);

	/* disable DRAM clock */
	clrbits_le32(&dram->mcr, DRAM_MCR_DCLK_OUT);

	/* disable all DLLs */
	for (i = 0; i < 5; i++)
		clrsetbits_le32(&dram->dllcr[i],
				DRAM_DLLCR_DISABLE | DRAM_DLLCR_NRESET,
				DRAM_DLLCR_DISABLE);
#else
	clrbits_le32(&dram->rdgr1, GENMASK(14,15));
#endif
}

#ifndef CONFIG_MACH_SUN7I
static void __secure sunxi_enable_dll(struct sunxi_dram_reg *dram, u32 pll)
{
	clrsetbits_le32(&dram->dllcr[pll],
			DRAM_DLLCR_DISABLE | DRAM_DLLCR_NRESET,
			DRAM_DLLCR_DISABLE);

	clrbits_le32(&dram->dllcr[pll],
		     DRAM_DLLCR_DISABLE | DRAM_DLLCR_NRESET);

	clrsetbits_le32(&dram->dllcr[pll],
			DRAM_DLLCR_DISABLE | DRAM_DLLCR_NRESET,
			DRAM_DLLCR_NRESET);
}
#endif

static void __secure sunxi_leave_selfrefresh(struct sunxi_dram_reg *dram)
{
#ifndef CONFIG_MACH_SUN7I
	int i;

	/* enable DLL0 */
	sunxi_enable_dll(dram, 0);

	/* enable DRAM clock */
	setbits_le32(&dram->mcr, DRAM_MCR_DCLK_OUT);

	/* enable all other DLLs */
	for (i = 1; i < 5; i++)
		sunxi_enable_dll(dram, i);

	/* enable ITM */
	clrbits_le32(&dram->ccr, DRAM_CCR_ITM_OFF);
#endif

	/* leave self-refresh */
	clrsetbits_le32(&dram->dcr, GENMASK(27,31),
			BIT(27) | BIT(28) | BIT(29) | BIT(31));

	/* manually refresh */
	clrsetbits_le32(&dram->dcr, GENMASK(27,31),
			BIT(27) | BIT(28) | BIT(31));

	/* enable auto-refresh */
	clrbits_le32(&dram->drr, DRAM_DRR_AUTO_REFRESH_OFF);

#ifndef CONFIG_MACH_SUN7I
	/* enable all host ports */
	for (i = 0; i < 32; i++)
		setbits_le32(&dram->hpcr[i], DRAM_HPCR_PORT_EN);
#endif
}

static void __secure sunxi_clock_enter_suspend(struct sunxi_ccm_reg *ccm)
{
#ifndef CONFIG_MACH_SUN7I
	/* gate off DRAM clock */
	clrbits_le32(&ccm->pll5_cfg, CCM_PLL5_CTRL_DDR_CLK);
#endif

	/* switch cpuclk to osc24m */
	clrsetbits_le32(&ccm->cpu_ahb_apb0_cfg, 0x3 << CPU_CLK_SRC_SHIFT,
			CPU_CLK_SRC_OSC24M << CPU_CLK_SRC_SHIFT);

	/* disable PLLs */
	// TODO: check which PLL are safe to disable
	clrbits_le32(&ccm->pll1_cfg, BIT(31));
	clrbits_le32(&ccm->pll2_cfg, BIT(31));
	clrbits_le32(&ccm->pll3_cfg, CCM_PLL3_CTRL_EN);
	clrbits_le32(&ccm->pll4_cfg, BIT(31));
	clrbits_le32(&ccm->pll6_cfg, CCM_PLL6_CTRL_EN);

#ifndef CONFIG_MACH_SUN7I
	/* switch cpuclk to losc */
	clrbits_le32(&ccm->cpu_ahb_apb0_cfg, 0x3 << CPU_CLK_SRC_SHIFT);
#endif
}

static void __secure sunxi_clock_leave_suspend(struct sunxi_ccm_reg *ccm)
{
#ifndef CONFIG_MACH_SUN7I
	/* switch cpuclk to osc24m */
	clrsetbits_le32(&ccm->cpu_ahb_apb0_cfg, 0x3 << CPU_CLK_SRC_SHIFT,
			CPU_CLK_SRC_OSC24M << CPU_CLK_SRC_SHIFT);
#endif

	/* enable PLLs */
	setbits_le32(&ccm->pll1_cfg, BIT(31));
	setbits_le32(&ccm->pll2_cfg, BIT(31));
	setbits_le32(&ccm->pll3_cfg, CCM_PLL3_CTRL_EN);
	setbits_le32(&ccm->pll4_cfg, BIT(31));
	setbits_le32(&ccm->pll6_cfg, CCM_PLL6_CTRL_EN);

	/* switch cpuclk to pll */
	clrsetbits_le32(&ccm->cpu_ahb_apb0_cfg, 0x3 << CPU_CLK_SRC_SHIFT,
			CPU_CLK_SRC_PLL1 << CPU_CLK_SRC_SHIFT);

#ifndef CONFIG_MACH_SUN7I
	/* gate on DRAM clock */
	setbits_le32(&ccm->pll5_cfg, CCM_PLL5_CTRL_DDR_CLK);
#endif
}

void __secure psci_cpu_suspend(void)
{
	struct sunxi_dram_reg *dram =
			(struct sunxi_dram_reg *)SUNXI_DRAMC_BASE;
	struct sunxi_ccm_reg *ccm = (struct sunxi_ccm_reg *)SUNXI_CCM_BASE;

	sunxi_enter_selfrefresh(dram);
	sunxi_clock_enter_suspend(ccm);

	/* idle */
	DSB;
	wfi();

	sunxi_clock_leave_suspend(ccm);
	sunxi_leave_selfrefresh(dram);
}
