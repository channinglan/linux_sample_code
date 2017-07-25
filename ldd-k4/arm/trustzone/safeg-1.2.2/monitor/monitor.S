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
 * [monitor.S]
 *
 * This file contains the monitor vector table and the monitor vector
 * handlers. Execution of the monitor always start here (except during
 * initialization).
 *
 * @(#) $Id: monitor.S 778 2015-09-11 04:10:52Z ertl-honda $  
 */
#include "arm.h"            /* CPSR_MODE_MONITOR */
#include "syscalls.h"       /* SYSCALL_RET__XXX, syscall_invoked */
#include "notifier_asm.inc" /* notify_xxx */
#include "monitor_asm.inc"  /* MON_XXX macros */

/************************************************
 * [MONITOR VECTOR TABLE]                       *
 * Execution of the monitor always starts here. *
 ************************************************/
    .section .vector, "ax"
    .p2align 5, 0x33
    .global monitor_vector_table
monitor_vector_table:
    b .           /* (NOT USED) reset                   */
    b .           /* (NOT USED) undefined instruction   */
    b smc_vector  /* (USED ALWAYS) System monitor call  */
    b pref_vector /* (NOT USED NORMALLY) prefetch abort */
    b data_vector /* (NOT USED NORMALLY) data abort     */
    b .           /* (NOT USED) reserved                */
    b irq_vector  /* (NOT USED NORMALLY) IRQ interrupt  */
    b fiq_vector  /* (USED ALWAYS) FIQ interrupt        */

/*********************************************
 * [System calls handling]                   *
 * System calls invoked from the T or NT OS. *
 *********************************************/
smc_vector:
    /* save return state in NT ? sp_mon=monitor_stack : sp_mon=t_context_n */
    srsdb sp!, #CPSR_MODE_MONITOR

    /* flash BTAC if we come from the NT world */
    arm_flush_branch_predictors_nt(lr)

    /* jump to the generic system call stub [corrupts r0-r3, ip, lr_mon] */
    bl syscall_invoked

    /* check the return argument (switch [31:28] and error[27:0]) */
    ror r1, r0, #28
    bic r0, r0, #0xF0000000
    cmp r1, #SYSCALL_RET__DONT_SWITCH
    beq smc_return_dont_switch
    cmp r1, #SYSCALL_RET__SWITCH_TO_NT
    beq smc_return_switch_to_nt
    cmp r1, #SYSCALL_RET__SWITCH_TO_T
    beq smc_return_switch_to_t

smc_return_dont_switch:
    notify_smc_dont_switch_exit
    rfeia sp!

smc_return_switch_to_nt:
    MON_SMC_SAVE_T_CONTEXT
    MON_LOAD_NT_CONTEXT
    notify_smc_switch_to_nt_exit
    rfeia lr

smc_return_switch_to_t:
    MON_SMC_SAVE_NT_CONTEXT
    MON_LOAD_T_CONTEXT
    notify_smc_switch_to_t_exit
    rfeia sp!

/*******************************
 * [FIQ handling]              *
 * Handling of FIQ interrupts. *
 *******************************/
fiq_vector:
    /* save return state in NT ? sp_mon=monitor_stack : sp_mon=t_context_n */
    srsdb sp!, #CPSR_MODE_MONITOR

    /* flash BTAC if we come from the NT world */
    arm_flush_branch_predictors_nt(lr)

    notify_fiq_enter

    /* check if we come from the T or NT world */
    arm_is_trust(lr)
    bne fiq_nt_vector

fiq_t_vector:
    MON_T_FIQ__SET_T_RETURN_STATE
    notify_fiq_t_exit
    rfeia sp!

fiq_nt_vector:
    clrex
    MON_FIQ_SAVE_NT_CONTEXT
    MON_LOAD_T_CONTEXT
    MON_NT_FIQ__SET_T_RETURN_STATE
    notify_fiq_nt_exit
    rfeia sp!

/*******************************
 * [IRQ handling]              *
 * Handling of IRQ interrupts. *
 *******************************/
irq_vector:
    /* save return state in NT ? sp_mon=monitor_stack : sp_mon=t_context_n */
    srsdb sp!, #CPSR_MODE_MONITOR
    notify_irq_enter

    /* check if we come from the T or NT world */
    arm_is_trust(lr)
    bne irq_nt_vector

irq_t_vector:
    notify_panic_irq_in_t
    rfeia sp!

irq_nt_vector:
    MON_NT_IRQ__SET_NT_RETURN_STATE
    notify_irq_nt_exit
    rfeia sp!

/********************************
 * [ABORT handling]             *
 * Handling of external aborts. *
 ********************************/
pref_vector:
    srsdb sp!, #CPSR_MODE_MONITOR
    notify_panic_prefetch
    rfeia sp!

data_vector:
    srsdb sp!, #CPSR_MODE_MONITOR
    notify_panic_data
    rfeia sp!
