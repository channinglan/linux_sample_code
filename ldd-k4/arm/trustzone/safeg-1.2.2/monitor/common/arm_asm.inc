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
 * [arm_asm.inc]
 *
 * This file contains a set of ARM-related macros. All macros follow the
 * 'arm_xxx' naming convention.
 *
 * @(#) $Id: arm_asm.inc 778 2015-09-11 04:10:52Z ertl-honda $
 */
#ifndef _SAFEG_MONITOR_COMMON_ARM_MACROS_INC_
#define _SAFEG_MONITOR_COMMON_ARM_MACROS_INC_

#include "arm.h"     /* HIGH_EXCEPTION_VECTOR_IRQ */
#include "monitor.h" /* MON_T_SCR */

#if TARGET_ARCH_VERSION == 7

/* Get our core id from the Multiprocessor Affinity Register (MPIDR) */
#define arm_get_core_id(reg) arm_get_core_id reg
.macro arm_get_core_id reg
    mrc p15, 0, \reg, c0, c0, 5
    and \reg, \reg, #0xF
.endm

/* Read/Write to the Secure Configuration Register */
#define arm_scr_read(reg) arm_scr_read reg
.macro arm_scr_read reg
    mrc p15, 0, \reg, c1, c1, 0
.endm

#define arm_scr_write(reg) arm_scr_write reg
.macro arm_scr_write reg
    mcr p15, 0, \reg, c1, c1, 0
    isb
.endm

/* Flush all branch predictors */
#define arm_flush_branch_predictors(reg) arm_flush_branch_predictors reg
.macro arm_flush_branch_predictors reg
    mov \reg, #0
    mcr p15, 0, \reg, c7, c5, 6
    dsb
    isb
.endm

/* Flush all branch predictors if we are come from non-trust (Non-secure) */
#define arm_flush_branch_predictors_nt(reg) arm_flush_branch_predictors_nt reg
.macro arm_flush_branch_predictors_nt reg
    arm_is_trust \reg
    movne \reg, #0
    mcrne p15, 0, \reg, c7, c5, 6
    dsb
    isb
.endm

/* Check if we are in trust (Secure) or non-trust (Non-secure) */
#define arm_is_trust(reg) arm_is_trust reg
.macro arm_is_trust reg
    arm_scr_read(\reg)
    tst \reg, #SCR_NS
.endm

/* Read the Vector Base Address register (VBAR) of the current world */
#define arm_vbar_read(reg) arm_vbar_read reg
.macro arm_vbar_read reg
    mrc p15, 0, \reg, c12, c0, 0
.endm

/* Get the IRQ vector address of the current world */
#define arm_irq_vector_read(reg) arm_irq_vector_read reg
.macro arm_irq_vector_read reg
    arm_vbar_read(\reg)
    add \reg, \reg, #0x18
.endm

/* Read/Write to the Coprocessor Access Control Register(CPACR) */
#define arm_cpacr_read(reg) arm_cpacr_read reg
.macro arm_cpacr_read reg
    mrc p15, 0, \reg, c1, c0, 2
.endm

#define arm_cpacr_write(reg) arm_cpacr_write reg
.macro arm_cpacr_write reg
    mcr p15, 0, \reg, c1, c0, 2
    isb sy
.endm

#if (MONITOR_NT_HIGH_VECTOR == 1)

/* Return the high exception IRQ vector (more efficient) */
#define arm_irq_or_high_vector_read(reg) arm_irq_or_high_vector_read reg
.macro arm_irq_or_high_vector_read reg
    ldr \reg, =HIGH_EXCEPTION_VECTOR_IRQ
.endm

#else /* (MONITOR_NT_HIGH_VECTOR == 0) */

/* Return the IRQ vector address with V bit checking (more portable) */
#define arm_irq_or_high_vector_read(reg) arm_irq_or_high_vector_read reg
.macro arm_irq_or_high_vector_read reg
    mrc p15, 0, \reg, c1, c0, 0
    tst \reg, #SCTLR_BIT_V
    ldrne \reg, =HIGH_EXCEPTION_VECTOR_IRQ
    mrceq p15, 0, \reg, c12, c0, 0
    addeq \reg, \reg, #0x18
.endm

#endif /* (MONITOR_NT_HIGH_VECTOR == 0) */

/* Get the FIQ vector address of the current world */
#define arm_fiq_vector_read(reg) arm_fiq_vector_read reg
.macro arm_fiq_vector_read reg
    arm_vbar_read(\reg)
    add \reg, \reg, #0x1C
.endm

/* Perform a APCS-compliant call to a C function with 2 arguments */
/* TODO: sp should be 8-byte aligned before calling function */
#define arm_apcs_call_2(function, a, b) arm_apcs_call_2 function, a, b
.macro arm_apcs_call_2 function, chain_id, event_id
    stmfd sp!, {r0-r3, ip, lr}
    ldr r0, =\chain_id
    ldr r1, =\event_id
    bl \function
    ldmfd sp!, {r0-r3, ip, lr}
.endm

#define arm_save_abort_state() arm_save_abort_state
.macro arm_save_abort_state
    /* clear the exclusive monitor */
    clrex
    /* store the current Secure Configuration Register (SCR) */
    arm_scr_read(lr)
    stmfd sp!, {lr}
    /* make sure NS=0 (changing mode while NS=1 is insecure) */
    mov lr, #MON_T_SCR
    arm_scr_write(lr)
    /* save USER registers  */
    stmfd sp, {r0-lr}^
    sub r1, sp, #(15*4)
    /* save SVC mode's banked registers */
    cps   #CPSR_MODE_SVC
    mrs   r2, spsr
    stmfd r1!, {r2, sp, lr}
    /* save ABORT mode's banked registers */
    cps   #CPSR_MODE_ABORT
    mrs   r2, spsr
    stmfd r1!, {r2, sp, lr}
    /* save UNDEFINED mode's banked registers */
    cps   #CPSR_MODE_UNDEF
    mrs   r2, spsr
    stmfd r1!, {r2, sp, lr}
    /* save IRQ mode's banked registers */
    cps   #CPSR_MODE_IRQ
    mrs   r2, spsr
    stmfd r1!, {r2, sp, lr}
    /* save FIQ mode's banked registers */
    cps   #CPSR_MODE_FIQ
    mrs   r2, spsr
    stmfd r1!, {r2, r8-lr}
    /* restore the current SCR (for saving banked registers) */
    cps #CPSR_MODE_MONITOR
    ldr lr, [sp]
    arm_scr_write(lr)
    /* save [Data,Instruction] Fault [Status,Address] registers */
    mrc p15, 0, r2, c6, c0, 2       @ get IFAR
    mrc p15, 0, r3, c5, c0, 1       @ get IFSR
    mrc p15, 0, r4, c6, c0, 0       @ get DFAR
    mrc p15, 0, r5, c5, c0, 0       @ get DFSR
    stmfd r1!, {r2-r5}
    /* Update the stack pointer */
    mov sp, r1
.endm

#define arm_restore_from_abort_state() arm_restore_from_abort_state
.macro arm_restore_from_abort_state
    /* TODO */
.endm

#endif /* TARGET_ARCH_VERSION */

#endif /* _SAFEG_MONITOR_COMMON_ARM_MACROS_INC_ */
