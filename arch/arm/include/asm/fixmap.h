#ifndef _ASM_FIXMAP_H
#define _ASM_FIXMAP_H

#define FIXADDR_START		0xffc00000UL
#define FIXADDR_END		0xfff00000UL

/* K14AB: 2018년 03월 31일 19:11:43
 * ------------------------------
 * 
 *		0xfff00000 	FIXADDR_END
 *		0x00001000	PAGE_SIZE
 *		----------
 *		0xFFEFF000 	FIXADDR_TOP
 *
 *		0xFFC00000 	FIXADDR_START
 */
#define FIXADDR_TOP		(FIXADDR_END - PAGE_SIZE)

#include <asm/kmap_types.h>
#include <asm/pgtable.h>

enum fixed_addresses {
	FIX_EARLYCON_MEM_BASE,                                          /* 0 */
	__end_of_permanent_fixed_addresses,				/* 1 */


	/* K14AB: 2018년 04월 14일 16:34:36
	 * ------------------------------
	 * FIX_KMAP_BEGIN 이 왜 0 이 아니라 1 일까?
	 * FIX_KMAP_END 는 왜 63 이 아니라 64 일까?
	 */
	FIX_KMAP_BEGIN = __end_of_permanent_fixed_addresses,		/* 1 */
	FIX_KMAP_END = FIX_KMAP_BEGIN + (KM_TYPE_NR * NR_CPUS) - 1,	/* 64 = 1 + (16 * 4 ) -1   */

	/* Support writing RO kernel text via kprobes, jump labels, etc. */
	FIX_TEXT_POKE0, 						/* 65 */
	FIX_TEXT_POKE1,

	__end_of_fixmap_region,

	/*
	 * Share the kmap() region with early_ioremap(): this is guaranteed
	 * not to clash since early_ioremap() is only available before
	 * paging_init(), and kmap() only after.
	 */
#define NR_FIX_BTMAPS		32
#define FIX_BTMAPS_SLOTS	7
#define TOTAL_FIX_BTMAPS	(NR_FIX_BTMAPS * FIX_BTMAPS_SLOTS)     	/* 224 = 32 * 7*/

	FIX_BTMAP_END = __end_of_permanent_fixed_addresses,		/* 1 */
	FIX_BTMAP_BEGIN = FIX_BTMAP_END + TOTAL_FIX_BTMAPS - 1,         /* 224 = 1 + 224 - 1*/
	__end_of_early_ioremap_region					/* 225 */
};

static const enum fixed_addresses __end_of_fixed_addresses =
	__end_of_fixmap_region > __end_of_early_ioremap_region ?
	__end_of_fixmap_region : __end_of_early_ioremap_region;

#define FIXMAP_PAGE_COMMON	(L_PTE_YOUNG | L_PTE_PRESENT | L_PTE_XN | L_PTE_DIRTY)

#define FIXMAP_PAGE_NORMAL	(pgprot_kernel | L_PTE_XN)
#define FIXMAP_PAGE_RO		(FIXMAP_PAGE_NORMAL | L_PTE_RDONLY)

/* Used by set_fixmap_(io|nocache), both meant for mapping a device */
#define FIXMAP_PAGE_IO		(FIXMAP_PAGE_COMMON | L_PTE_MT_DEV_SHARED | L_PTE_SHARED)
#define FIXMAP_PAGE_NOCACHE	FIXMAP_PAGE_IO

#define __early_set_fixmap	__set_fixmap

#ifdef CONFIG_MMU

void __set_fixmap(enum fixed_addresses idx, phys_addr_t phys, pgprot_t prot);
void __init early_fixmap_init(void);

#include <asm-generic/fixmap.h>

#else

static inline void early_fixmap_init(void) { }

#endif
#endif
