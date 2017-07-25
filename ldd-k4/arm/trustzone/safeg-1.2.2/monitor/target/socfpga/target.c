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
 * TrustZone initialization.
 */
#define L3REGS_BASE 0xFF800000

#define L3REGS_SECGRP_BASE            (L3REGS_BASE + 0x08)

#define L3REGS_SECGRP_L4MAIN          (L3REGS_SECGRP_BASE + 0x00)
#define L3REGS_SECGRP_L4SP            (L3REGS_SECGRP_BASE + 0x04)
#define L3REGS_SECGRP_L4MP            (L3REGS_SECGRP_BASE + 0x08)
#define L3REGS_SECGRP_L4OSC1          (L3REGS_SECGRP_BASE + 0x0c)
#define L3REGS_SECGRP_L4SPIM          (L3REGS_SECGRP_BASE + 0x10)
#define L3REGS_SECGRP_L4STM           (L3REGS_SECGRP_BASE + 0x14)
#define L3REGS_SECGRP_LWHPS2FPGAREGS  (L3REGS_SECGRP_BASE + 0x18)
#define L3REGS_SECGRP_USB1            (L3REGS_SECGRP_BASE + 0x20)
#define L3REGS_SECGRP_NANDDATA        (L3REGS_SECGRP_BASE + 0x24)
#define L3REGS_SECGRP_USB0            (L3REGS_SECGRP_BASE + 0x78)
#define L3REGS_SECGRP_NANDREGS        (L3REGS_SECGRP_BASE + 0x7c)
#define L3REGS_SECGRP_QSPIDATA        (L3REGS_SECGRP_BASE + 0x80)
#define L3REGS_SECGRP_FPGAMGRDATA     (L3REGS_SECGRP_BASE + 0x84)
#define L3REGS_SECGRP_HPS2FPGAREGS    (L3REGS_SECGRP_BASE + 0x88)
#define L3REGS_SECGRP_ACP             (L3REGS_SECGRP_BASE + 0x8C)
#define L3REGS_SECGRP_ROM             (L3REGS_SECGRP_BASE + 0x90)
#define L3REGS_SECGRP_OCRAM           (L3REGS_SECGRP_BASE + 0x94)
#define L3REGS_SECGRP_SDRDATA         (L3REGS_SECGRP_BASE + 0x94)

#define L3REGS_REMAP (L3REGS_BASE + 0x00)

#define L3REGS_REMAP_MPUZERO     0x01
#define L3REGS_REMAP_NONMPUZERO  0x02
#define L3REGS_REMAP_HPS2FPGA    0x08
#define L3REGS_REMAP_LWHP2FPGA   0x10

/*
 * Memory controller register
 */
#define SDR_BASE         0xffc20000
#define SDR_PORTDEFAULT  (SDR_BASE + 0x508C)  /* [9:0] portdefault */
#define SDR_PORTRULEADDR (SDR_BASE + 0x5090)  /* [21:12]High address, [11:0]Low address */
#define SDR_PORTRULEID   (SDR_BASE + 0x5094)  /* [21:12]High ID, [11:0]Low ID */
#define SDR_PORTRULEDATA (SDR_BASE + 0x5098)  /* [13]rule result [12:3]portmask [2]validrule [1:0]security */
#define SDR_PORTRULEDWR  (SDR_BASE + 0x509C)  /* [6]readrule [5]writerule [4:0]ruleoffset */

/*
 * Remap register
 */
#define MPUL2_BASE 0xFFFEF000
#define MPUL2_ADRFILTER (MPUL2_BASE  + 0xC00)

/*
 *  SYSMGR
 */
#define SYSMGR_BASE 0xFFD08000
#define SYSMGR_CPU1STARTADDR (SYSMGR_BASE + 0xC4)

/*
 *  RSTMGR
 */
#define RSTMGR_BASE 0xFFD05000
#define RSTMGR_MPUMODRST  (RSTMGR_BASE + 0x10)
#define RSTMGR_MPUMODRST_CPU1  0x02

void set_memory_protection_rule(uint8_t  id,       /* 0 - 19 */
								uint8_t  security, /* 0:secure, 1:non-secure */
								uint16_t portmask, /* 10bit */
								uint16_t lid,      /* 12bit */
								uint16_t hid,      /* 12bit */
								uint16_t laddr,    /* 12bit */
								uint16_t haddr,    /* 12bit */
								uint8_t  allow)    /*0:allow, 1:denie */
{
	REG(SDR_PORTRULEDWR)    = ((1 << 5) | (id & 0x1f)); 
	REG(SDR_PORTRULEADDR)   = (((haddr & 0xfff) << 12) | (laddr & 0xfff));
	REG(SDR_PORTRULEID)     = (((hid & 0xfff) << 12) | (lid & 0xfff));
	REG(SDR_PORTRULEDATA)   = ((allow << 13)|(portmask & 0x3ff)<< 3 | (1 << 2) | (security & 0x03));
}

static void target_trustzone_init(void)
{
	uint32_t loop;

	REG(L3REGS_SECGRP_L4MAIN) = 0xffffffff;
	REG(L3REGS_SECGRP_L4SP) = 0xffffffff;           
	REG(L3REGS_SECGRP_L4MP) = 0xffffffff;           
	REG(L3REGS_SECGRP_L4OSC1) = 0xffffffff;         
	REG(L3REGS_SECGRP_L4SPIM) = 0xffffffff;         
	REG(L3REGS_SECGRP_L4STM) = 0xffffffff;          
	REG(L3REGS_SECGRP_LWHPS2FPGAREGS) = 0xffffffff; 
	REG(L3REGS_SECGRP_USB1) = 0xffffffff;           
	REG(L3REGS_SECGRP_NANDDATA) = 0xffffffff;       
	REG(L3REGS_SECGRP_USB0) = 0xffffffff;           
	REG(L3REGS_SECGRP_NANDREGS) = 0xffffffff;       
	REG(L3REGS_SECGRP_QSPIDATA) = 0xffffffff;       
	REG(L3REGS_SECGRP_FPGAMGRDATA) = 0xffffffff;    
	REG(L3REGS_SECGRP_HPS2FPGAREGS) = 0xffffffff;   
	REG(L3REGS_SECGRP_ACP) = 0xffffffff;            
	REG(L3REGS_SECGRP_ROM) = 0xffffffff;            
	REG(L3REGS_SECGRP_OCRAM) = 0xffffffff;          
	REG(L3REGS_SECGRP_SDRDATA) = 0xffffffff;        

	/* SDR */
	/* Default all pass */
	REG(SDR_PORTDEFAULT) = 0x00000000;
	/* Defulat all rule disable */
	for (loop = 0; loop < 20; loop++){
		REG(SDR_PORTRULEDWR)  = ((1 << 5) | (loop & 0x1f));
		REG(SDR_PORTRULEADDR) = 0x00000000;
		REG(SDR_PORTRULEID)   = 0x00000000;
		REG(SDR_PORTRULEDATA) = 0x00000000; /* invalid */
	}

	/* 0x3f000000 - 0x3f800000 : Secure (Non-Secure deny) */
	set_memory_protection_rule(0, 1, 0x3ff, 0, 0xfff, 0x3f0, 0x3f7, 1);

	ARM_DATA_MEMORY_BARRIER;
	__asm__ volatile("dsb");
	__asm__ volatile("isb");
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
 * Target-dependent primary core initialization.
 */
void target_primary_core_init(void)
{
#if TARGET_NUM_CORES > 1
	/* Unmap DDR from 0x00000000 to 0x000fffff */
	REG(MPUL2_ADRFILTER) = 0x00100001;
	/* Map bootrom to 0x00 */
	REG(L3REGS_REMAP) = REG(L3REGS_REMAP) & ~L3REGS_REMAP_MPUZERO;
	REG(SYSMGR_CPU1STARTADDR) = (uint32_t)start;
	/* Wake up processor 2 */
	REG(RSTMGR_MPUMODRST) = REG(RSTMGR_MPUMODRST) & ~RSTMGR_MPUMODRST_CPU1;
#endif /* TARGET_NUM_CORES > 1 */
}

/*
 * Signal an interrupt to the opposite world.
 */
#define MPCORE_PMR_BASE  0xfffec000
#define DIC_SP           (MPCORE_PMR_BASE + 0x1200)

static void dic_set_pending(uint8_t id)
{
    uint16_t offset_addr;
    uint16_t offset_bit;

    offset_addr = (id / 32) * 4;
    offset_bit  = id % 32;

    REG(DIC_SP + offset_addr) = (uint32_t)(1 << offset_bit);
}

uint32_t target_signal(uint32_t UNUSED(core_id), uint32_t ns)
{
	if (ns == 1) {
		dic_set_pending(TARGET_T_INT_SIGNAL_ID);
	}
	else {
		dic_set_pending(TARGET_NT_INT_SIGNAL_ID);
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
