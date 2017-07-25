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
 * [start.S]
 *
 * Execution of a SafeG system always starts here once the bootloader
 * jumps to the first instruction of the monitor (i.e.,: 'start'). In this
 * file, we initialize the monitor stack and trigger the initialization of
 * the monitor libraries (by calling 'monitor_init'). Once the monitor is
 * initialized (in all cores) we jump to the first instruction of the T OS.
 *
 * @(#) $Id: start.S 614 2013-11-23 16:22:37Z ertl-honda $  
 */
#include "monitor.h"         /* MON_XXX constants  */
#include "target_asm.inc"    /* target_hardware_init_hook */

/**************************************************
 * [.bss and .data initialization]                *
 * Initialization of the .bss and .data sections. *
 **************************************************/
/*
 * Initialize the BSS space with zeros.
 */
#if (MONITOR_OMIT_BSS_INIT == 1)
#define mon_init_bss(r0,r1,r2)
#else  /* (MONITOR_OMIT_BSS_INIT == 0) */
#define mon_init_bss(r0,r1,r2) mon_init_bss r0, r1, r2
.macro mon_init_bss r0, r1, r2
    ldr  \r1, =__bss_start
    ldr  \r2, =__bss_end
    cmp  \r1, \r2
    bhs  mon_init_bss_end
    mov  \r0, #0
mon_init_bss_loop:
    str  \r0, [\r1], #4
    cmp  \r1, \r2
    blo  mon_init_bss_loop
mon_init_bss_end:
.endm
#endif /* (MONITOR_OMIT_BSS_INIT == 0) */

/*
 * Copy the .data section to RAM (useful when running from a ROM).
 */
#if (MONITOR_OMIT_DATA_INIT == 1)
#define mon_init_data(r0,r1,r2,r3)
#else  /* (MONITOR_OMIT_DATA_INIT == 0) */
#define mon_init_data(r0,r1,r2,r3) mon_init_data r0, r1, r2, r3
.macro mon_init_data r0, r1, r2, r3
    ldr  \r1, =__rom_data_start
    ldr  \r3, =__rom_data_end
    cmp  \r1, \r3
    bhs  mon_init_data_end
    ldr  \r2, =__data_start
mon_init_data_loop:
    ldr  \r0, [\r1], #4
    str  \r0, [\r2], #4
    cmp  \r1, \r3
    blo  mon_init_data_loop
mon_init_data_end:
.endm
#endif /* (MONITOR_OMIT_DATA_INIT == 0) */

/*************************
 * [SafeG START]         *
 * SafeG starting point. *
 *************************/
    .text
    .p2align 2, 0x11
    .global start
start:
    /* Disable all interrupts and enter Monitor mode */
    cpsid if, #CPSR_MODE_MONITOR

    /* Initialize the monitor stack (sp_mon) and frame pointer */
    arm_get_core_id(r4)
    ldr sp, =(monitor_space + MON_SPACE_BYTES_TOTAL)
    sub sp, sp, r4, lsl #MON_SPACE_SHIFT
    mov fp, #0

    /* Target-dependent hardware initialization */
    target_hardware_init_hook(r0, r1)

    /* Initialize the .bss and .data sections on the primary core */
    cmp r4, #TARGET_PRIMARY_CORE_ID
    bne skip_bss_data_init
    mon_init_bss(r0, r1, r2)
    mon_init_data(r0, r1, r2, r3)
skip_bss_data_init:

    /* Initialize the monitor and its libraries */
    mov r0, r4
    bl monitor_init

    /* Prepare the first NT OS start address PC and CPSR */
    ldr r0, =TARGET_NT_OS_START_ADDRESS
    ldr r1, =MON_NT_CPSR_INIT_VALUE
    sub r2, sp, #(MON_T_CONTEXT_SIZE_BYTES + MON_STACK_SIZE_BYTES)
    stmfd r2, {r0, r1}

    /* Enter supervisor mode and set the T-OS start address */
    cps #CPSR_MODE_SVC
    ldr r0, =TARGET_T_OS_START_ADDRESS

    /* Jump to the T-OS */
    mov pc, r0

/*************************************************************************
 * [Monitor Space]                                                       *
 * Memory used for the monitor to store T/NT contexts and its own stack. *
 *************************************************************************/
    .bss
    .p2align 3, 0x22
    .global monitor_space
monitor_space:
    .space MON_SPACE_BYTES_TOTAL
