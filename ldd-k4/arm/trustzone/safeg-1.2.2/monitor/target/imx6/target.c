/*
 *  TOPPERS/SafeG Dual-OS monitor
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Safety Gate Dual-OS monitor
 *
 *  Copyright (C) 2009-2013 by Embedded and Real-Time Systems Laboratory
 *     Graduate School of Information Science, Nagoya University, JAPAN
 *
 *  上記著作権者は，以下の(1)〜(4)の条件を満たす場合に限り，本ソフトウェ
 *  ア（本ソフトウェアを改変したものを含む．以下同じ）を使用・複製・改
 *  変・再配布（以下，利用と呼ぶ）することを無償で許諾する．
 *  (1) 本ソフトウェアをソースコードの形で利用する場合には，上記の著作
 *      権表示，この利用条件および下記の無保証規定が，そのままの形でソー
 *      スコード中に含まれていること．
 *  (2) 本ソフトウェアを，ライブラリ形式など，他のソフトウェア開発に使
 *      用できる形で再配布する場合には，再配布に伴うドキュメント（利用
 *      者マニュアルなど）に，上記の著作権表示，この利用条件および下記
 *      の無保証規定を掲載すること．
 *  (3) 本ソフトウェアを，機器に組み込むなど，他のソフトウェア開発に使
 *      用できない形で再配布する場合には，次のいずれかの条件を満たすこ
 *      と．
 *    (a) 再配布に伴うドキュメント（利用者マニュアルなど）に，上記の著
 *        作権表示，この利用条件および下記の無保証規定を掲載すること．
 *    (b) 再配布の形態を，別に定める方法によって，TOPPERSプロジェクトに
 *        報告すること．
 *  (4) 本ソフトウェアの利用により直接的または間接的に生じるいかなる損
 *      害からも，上記著作権者およびTOPPERSプロジェクトを免責すること．
 *      また，本ソフトウェアのユーザまたはエンドユーザからのいかなる理
 *      由に基づく請求からも，上記著作権者およびTOPPERSプロジェクトを
 *      免責すること．
 *
 *  本ソフトウェアは，無保証で提供されているものである．上記著作権者お
 *  よびTOPPERSプロジェクトは，本ソフトウェアに関して，特定の使用目的
 *  に対する適合性も含めて，いかなる保証も行わない．また，本ソフトウェ
 *  アの利用により直接的または間接的に生じたいかなる損害に関しても，そ
 *  の責任を負わない．
 *
 * [target.c]
 *
 * This file contains the implementation of target-dependent functions.
 *
 * @(#) $Id: target.c 587 2013-11-17 15:51:22Z ertl-honda $  
 */
#include "types.h"  /* uint32_t */
#include "utils.h"  /* UNUSED, REG */
#include "target.h" /* prototypes */
#include "arm.h"    /* ARM_DATA_MEMORY_BARRIER */

/*
 *  System Reset Controller
 */
#define SRC_SCR  0x020D8000
#define SRC_GPR3 0x020D8028  /* CORE1 Entry */
#define SRC_GPR5 0x020D8030  /* CORE2 Entry */
#define SRC_GPR7 0x020D8038  /* CORE3 Entry */

#define SRC_SCR_CORE1_ENA 0x00400000
#define SRC_SCR_CORE2_ENA 0x00800000
#define SRC_SCR_CORE3_ENA 0x01000000

#define SRC_SCR_CORE1_RST 0x00002000
#define SRC_SCR_CORE2_RST 0x00004000
#define SRC_SCR_CORE3_RST 0x00008000

#include "security.h"

static void target_trustzone_init(void)
{
	csu_init();
}


/*
 *  entry point (start.S)
 */
extern void start(void);

/*
 * Target-dependent initialization.
 */
void target_init(uint32_t core_id)
{
	if (core_id == TARGET_PRIMARY_CORE_ID) {
		target_trustzone_init();
	}
}

/*
 *  L2(PL310) Base Address
 */
#define PL310_L2CACHE_BASE	0x00A02000

#define PL310_CTRL			0x100
#define PL310_AUX_CTRL			0x104
#define PL310_DEBUG_CTRL			0xF40
#define PL310_CACHE_SYNC			0x730
#define PL310_CLEAN_INV_WAY		0x7FC

/* Bitmask of active ways */
static uint32_t pl310_way_mask;

void
pl310_debug_set(uint32_t val)
{
	REG((PL310_L2CACHE_BASE + PL310_DEBUG_CTRL)) = val;
}

void pl310_wait_way(uint32_t *reg, unsigned long mask)
{
	/* wait for cache operation by line or way to complete */
	while ( REG(reg) & mask );
}

void pl310_sync(void)
{
	REG((PL310_L2CACHE_BASE + PL310_CACHE_SYNC)) = 0;
}

void
pl310_flush_all(void)
{
	pl310_debug_set(0x03);
	REG((PL310_L2CACHE_BASE + PL310_CLEAN_INV_WAY)) = pl310_way_mask;
	pl310_wait_way((void*)(PL310_L2CACHE_BASE + PL310_CLEAN_INV_WAY), pl310_way_mask);
	pl310_sync();
	pl310_debug_set(0x00);
}

void
pl310_disable(void)
{
	int ways;
	uint32_t aux;
	uint32_t tmp;

	/* L2キャッシュがすでにオンになっているか確認する */
	tmp = REG((PL310_L2CACHE_BASE + PL310_CTRL));

	/* L2キャッシュが無効の場合はリターン */
	if ( !(tmp & 1) ) {
		return;
	}

	aux = REG((PL310_L2CACHE_BASE + PL310_AUX_CTRL));

	if (aux & (1 << 16)) {
		ways = 16;
	}
	else {
		ways = 8;
	}

	pl310_way_mask = (1 << ways) - 1;
	
	pl310_flush_all();
	REG((PL310_L2CACHE_BASE + PL310_CTRL)) = 0x0;
}

#define CP15_CONTROL_C_BIT    (1 <<  2)
#define CP15_CONTROL_M_BIT    (1 <<  0)

#define CP15_CONTROL_READ(x)     __asm__ volatile ("mrc p15, 0, %0, c1, c0, 0":"=r"(x))
#define CP15_CONTROL_WRITE(x)    __asm__ volatile ("mcr p15, 0, %0, c1, c0, 0"::"r"(x))
#define CP15_CACHE_SIZE_SELECTION_WRITE(x) __asm__ volatile ("mcr p15, 2, %0, c0, c0, 0"::"r"(x))
#define CP15_PBUFFER_FLUSH()               __asm__ volatile ("isb":::"memory")
#define CP15_DATA_SYNC_BARRIER()           __asm__ volatile ("dsb":::"memory")
#define CP15_DATA_MEMORY_BARRIER()         __asm__ volatile ("dmb":::"memory")
#define CP15_DCACHE_INVALIDATE(x)          __asm__ volatile ("mcr p15, 0, %0, c7, c6, 2"::"r"(x))
#define CP15_DCACHE_CLEAN(x)               __asm__ volatile ("mcr p15, 0, %0, c7, c10,2"::"r"(x))
#define CP15_DCACHE_CLEAN_AND_INVALIDATE(x) __asm__ volatile ("MCR p15, 0, %0, c7, c14, 2"::"r"(x))

/*
 *  Dキャッシュのクリーンと無効化
 */
void
mpcore_dcache_clean_and_invalidate(void)
{
	uint32_t bits = 0;
	uint32_t  ways = 0;
	uint32_t sets = 0;

	CP15_CACHE_SIZE_SELECTION_WRITE(0);
	CP15_PBUFFER_FLUSH();
	for (ways = 0; ways < 4; ways++){
		for (sets = 0; sets < 256; sets++){
			bits = ways << 30 | sets << 5;
			CP15_DCACHE_CLEAN_AND_INVALIDATE(bits);
		}
	}
}

/*
 *  Dキャッシュの無効化
 */
void
mpcore_dcache_invalidate(void)
{
	uint32_t bits = 0;
	uint32_t ways = 0;
	uint32_t sets = 0;

	CP15_CACHE_SIZE_SELECTION_WRITE(0);
	CP15_PBUFFER_FLUSH();
	for (ways = 0; ways < 4; ways++){
		for (sets = 0; sets < 256; sets++){
			bits = ways << 30 | sets << 5;
			CP15_DCACHE_INVALIDATE(bits);
		}
	}
}

void
mpcore_dcache_disable(void)
{
	uint32_t bits;

	CP15_CONTROL_READ(bits);
	if( bits & CP15_CONTROL_C_BIT ){
		mpcore_dcache_clean_and_invalidate();
		bits &= ~(CP15_CONTROL_C_BIT|CP15_CONTROL_M_BIT);
		CP15_CONTROL_WRITE(bits);
	}
	else{
		mpcore_dcache_invalidate();
	}
}

/*
 * Target-dependent primary core initialization.
 */
void target_primary_core_init(void)
{
	pl310_disable();
	mpcore_dcache_disable();

#if TARGET_NUM_CORES > 1
	/* Wake up processor 2 */
	REG(SRC_GPR3) = (uint32_t)start;
	REG(SRC_SCR)  |= SRC_SCR_CORE1_ENA|SRC_SCR_CORE1_RST;

#endif /* TARGET_NUM_CORES > 1 */

#if TARGET_NUM_CORES > 2
	/* Wake up processor 3 */
	REG(SRC_GPR5) = (uint32_t)start;
	REG(SRC_SCR)  |= SRC_SCR_CORE2_ENA|SRC_SCR_CORE2_RST;
#endif /* TARGET_NUM_CORES > 2 */

#if TARGET_NUM_CORES > 3
		/* Wake up processor 4 */
	REG(SRC_GPR7) = (uint32_t)start;
	REG(SRC_SCR)  |= SRC_SCR_CORE3_ENA|SRC_SCR_CORE3_RST;
#endif /* TARGET_NUM_CORES > 3 */
}

/*
 * Signal an interrupt to the opposite world.
 */
#define MPCORE_PMR_BASE  0x00A00000
#define DIC_SP           (MPCORE_PMR_BASE + 0x1200)

static void dic_set_pending(uint8_t id)
{
    uint16_t offset_addr;
    uint16_t offset_bit;

    offset_addr = (id / 32) * 4;
    offset_bit  = id % 32;

    REG(DIC_SP + offset_addr) = (uint32_t)(1 << offset_bit);
}

static uint32_t signal_intno[2][4] = {
#if 0
	{TARGET_NT_INT_SIGNAL_ID_0, TARGET_NT_INT_SIGNAL_ID_1, TARGET_NT_INT_SIGNAL_ID_2, TARGET_NT_INT_SIGNAL_ID_3},
	{TARGET_T_INT_SIGNAL_ID_0, TARGET_T_INT_SIGNAL_ID_1, TARGET_T_INT_SIGNAL_ID_2, TARGET_T_INT_SIGNAL_ID_3},
#else
	{TARGET_NT_INT_SIGNAL_ID_0, TARGET_NT_INT_SIGNAL_ID_0, TARGET_NT_INT_SIGNAL_ID_0, TARGET_NT_INT_SIGNAL_ID_0},
	{TARGET_T_INT_SIGNAL_ID_0,  TARGET_T_INT_SIGNAL_ID_0,  TARGET_T_INT_SIGNAL_ID_0,  TARGET_T_INT_SIGNAL_ID_0},
#endif
};

uint32_t target_signal(uint32_t core_id, uint32_t ns)
{
	if (ns == 1) {
		dic_set_pending(signal_intno[1][core_id]);
	}
	else {
		dic_set_pending(signal_intno[0][core_id]);
	}
	return 0;
}

/*
 * Check if the memory region is valid for the corresponding world.
 */
uint32_t target_is_valid_memory(uint32_t UNUSED(ns),
                                uint32_t UNUSED(address),
                                uint32_t UNUSED(bytes))
{
	/* TODO: check memory */
    return true;
}

/*
 * Check if the address is valid for read.
 */
uint32_t target_is_valid_readl_address(uint32_t UNUSED(core_id),
                                       uint32_t UNUSED(ns),
                                       uint32_t UNUSED(address))
{
    /* TODO: check memory */
    return true;
}

/*
 * Check if the address is valid for write.
 */
uint32_t target_is_valid_writel_address(uint32_t UNUSED(core_id),
                                        uint32_t UNUSED(ns),
                                        uint32_t UNUSED(address))
{
    /* TODO: check memory */
    return true;
}
