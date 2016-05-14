/*
 * Copyright (C) 2016 Next Thing Co.
 * Jose Angel Torres <software@nextthing.co>
 *
 * SPDX-License-Identifier:     GPL-2.0+
 */

#ifndef __SOUND_SUNXI_H__
#define __SOUND_SUNXI_H__

/* Codec DAC register offsets and bit fields */
#define SUNXI_DAC_DPC                   (0x00)
#define SUNXI_DAC_DPC_EN_DA                     (31)
#define SUNXI_DAC_DPC_DVOL                      (12)
#define SUNXI_DAC_FIFOC                 (0x04)
#define SUNXI_DAC_FIFOC_DAC_FS                  (29)
#define SUNXI_DAC_FIFOC_FIR_VERSION             (28)
#define SUNXI_DAC_FIFOC_SEND_LASAT              (26)
#define SUNXI_DAC_FIFOC_TX_FIFO_MODE            (24)
#define SUNXI_DAC_FIFOC_DRQ_CLR_CNT             (21)
#define SUNXI_DAC_FIFOC_TX_TRIG_LEVEL           (8)
#define SUNXI_DAC_FIFOC_MONO_EN                 (6)
#define SUNXI_DAC_FIFOC_TX_SAMPLE_BITS          (5)
#define SUNXI_DAC_FIFOC_DAC_DRQ_EN              (4)
#define SUNXI_DAC_FIFOC_FIFO_FLUSH              (0)
#define SUNXI_DAC_FIFOS                 (0x08)
#define SUNXI_DAC_TXDATA                (0x0c)
#define SUNXI_DAC_ACTL                  (0x10)
#define SUNXI_DAC_ACTL_DACAENR                  (31)
#define SUNXI_DAC_ACTL_DACAENL                  (30)
#define SUNXI_DAC_ACTL_MIXEN                    (29)
#define SUNXI_DAC_ACTL_LDACLMIXS                (15)
#define SUNXI_DAC_ACTL_RDACRMIXS                (14)
#define SUNXI_DAC_ACTL_LDACRMIXS                (13)
#define SUNXI_DAC_ACTL_DACPAS                   (8)
#define SUNXI_DAC_ACTL_MIXPAS                   (7)
#define SUNXI_DAC_ACTL_PA_MUTE                  (6)
#define SUNXI_DAC_ACTL_PA_VOL                   (0)
#define SUNXI_DAC_TUNE                  (0x14)
#define SUNXI_DAC_DEBUG                 (0x18)

/* Codec ADC register offsets and bit fields */
#define SUNXI_ADC_FIFOC                 (0x1c)
#define SUNXI_ADC_FIFOC_EN_AD                   (28)
#define SUNXI_ADC_FIFOC_RX_FIFO_MODE            (24)
#define SUNXI_ADC_FIFOC_RX_TRIG_LEVEL           (8)
#define SUNXI_ADC_FIFOC_MONO_EN                 (7)
#define SUNXI_ADC_FIFOC_RX_SAMPLE_BITS          (6)
#define SUNXI_ADC_FIFOC_ADC_DRQ_EN              (4)
#define SUNXI_ADC_FIFOC_FIFO_FLUSH              (0)
#define SUNXI_ADC_FIFOS                 (0x20)
#define SUNXI_ADC_RXDATA                (0x24)
#define SUNXI_ADC_ACTL                  (0x28)
#define SUNXI_ADC_ACTL_ADCREN                   (31)
#define SUNXI_ADC_ACTL_ADCLEN                   (30)
#define SUNXI_ADC_ACTL_PREG1EN                  (29)
#define SUNXI_ADC_ACTL_PREG2EN                  (28)
#define SUNXI_ADC_ACTL_VMICEN                   (27)
#define SUNXI_ADC_ACTL_VADCG                    (20)
#define SUNXI_ADC_ACTL_ADCIS                    (17)
#define SUNXI_ADC_ACTL_PA_EN                    (4)
#define SUNXI_ADC_ACTL_DDE                      (3)
#define SUNXI_ADC_DEBUG                 (0x2c)

/* Other various ADC registers */
#define SUNXI_DAC_TXCNT                 (0x30)
#define SUNXI_ADC_RXCNT                 (0x34)
#define SUNXI_AC_SYS_VERI               (0x38)
#define SUNXI_AC_MIC_PHONE_CAL          (0x3c)

/* Supported SoC families - used for quirks */
enum sunxi_soc_family {
        SUN4IA, /* A10 SoC - revision A */
        SUN4I,  /* A10 SoC - later revisions */
        SUN5I,  /* A10S/A13 SoCs */
        SUN7I,  /* A20 SoC */
};

struct sunxi_priv {
        struct regmap *regmap;
        struct clk *clk_apb, *clk_module;

        enum sunxi_soc_family revision;

        //struct snd_dmaengine_dai_dma_data playback_dma_data;
        //struct snd_dmaengine_dai_dma_data capture_dma_data;
};

int sunxi_codec_init(const void *blob);

#endif /* __SOUND_SUNXI_H__ */



