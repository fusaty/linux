/* SPDX-License-Identifier: GPL-2.0 */

#ifndef __freax_ISA_DMA_H
#define __freax_ISA_DMA_H

#include <asm/dma.h>

#if defined(CONFIG_PCI) && defined(CONFIG_X86_32)
extern int isa_dma_bridge_buggy;
#else
#define isa_dma_bridge_buggy	(0)
#endif

#endif /* __freax_ISA_DMA_H */
