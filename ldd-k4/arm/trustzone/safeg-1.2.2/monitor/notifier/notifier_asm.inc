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
 * [notifier_asm.inc]
 *
 * This file contains notifier assembly macros that are used within
 * the 'monitor.S' for sending events to the corresponding notifier chain.
 *
 * @(#) $Id: notifier_asm.inc 587 2013-11-17 15:51:22Z ertl-honda $
 */
#ifndef _SAFEG_MONITOR_NOTIFIER_ASM_INC_
#define _SAFEG_MONITOR_NOTIFIER_ASM_INC_

#if (MONITOR_ENABLE_NOTIFIERS == 1)

#include "arm_asm.inc"  /* arm_apcs_call_3 */
#include "notifier.h"   /* NOTIFIER_XXX */

/*
 * Assembler wrappers for sending events to the FIQ notifier chain.
 */
.macro notify_fiq_enter
    arm_apcs_call_2(notifier_call,
                    NOTIFIER_CHAIN_ID_FIQ,
                    NOTIFIER_EVENT_ID_FIQ_ENTER)
.endm

.macro notify_fiq_t_exit
    arm_apcs_call_2(notifier_call,
                    NOTIFIER_CHAIN_ID_FIQ,
                    NOTIFIER_EVENT_ID_FIQ_T_EXIT)
.endm

.macro notify_fiq_nt_exit
    arm_apcs_call_2(notifier_call,
                    NOTIFIER_CHAIN_ID_FIQ,
                    NOTIFIER_EVENT_ID_FIQ_NT_EXIT)
.endm

/*
 * Assembler wrappers for sending events to the IRQ notifier chain.
 */
.macro notify_irq_enter
    arm_apcs_call_2(notifier_call,
                    NOTIFIER_CHAIN_ID_IRQ,
                    NOTIFIER_EVENT_ID_IRQ_ENTER)
.endm

.macro notify_irq_nt_exit
    arm_apcs_call_2(notifier_call,
                    NOTIFIER_CHAIN_ID_IRQ,
                    NOTIFIER_EVENT_ID_IRQ_NT_EXIT)
.endm

/*
 * Assembler wrappers for sending events to the SMC notifier chain.
 */
.macro notify_smc_dont_switch_exit
    arm_apcs_call_2(notifier_call,
                    NOTIFIER_CHAIN_ID_SMC,
                    NOTIFIER_EVENT_ID_SMC_DONT_SWITCH_EXIT)
.endm

.macro notify_smc_switch_to_nt_exit
    arm_apcs_call_2(notifier_call,
                    NOTIFIER_CHAIN_ID_SMC,
                    NOTIFIER_EVENT_ID_SMC_SWITCH_TO_NT_EXIT)
.endm

.macro notify_smc_switch_to_t_exit
    arm_apcs_call_2(notifier_call,
                    NOTIFIER_CHAIN_ID_SMC,
                    NOTIFIER_EVENT_ID_SMC_SWITCH_TO_T_EXIT)
.endm

/*
 * Assembler wrappers for sending events to the panic notifier chain.
 */
.macro notify_panic_irq_in_t
    arm_apcs_call_2(notifier_call,
                    NOTIFIER_CHAIN_ID_PANIC,
                    NOTIFIER_EVENT_ID_PANIC_IRQ_IN_T)
.endm

.macro notify_panic_irq_in_nt
    arm_apcs_call_2(notifier_call,
                    NOTIFIER_CHAIN_ID_PANIC,
                    NOTIFIER_EVENT_ID_PANIC_IRQ_IN_NT)
.endm

.macro notify_panic_prefetch
    arm_save_abort_state()
    ldr r0, =NOTIFIER_CHAIN_ID_PANIC
    ldr r1, =NOTIFIER_EVENT_ID_PANIC_PREFETCH
    mov r2, sp
    bl notifier_call
    arm_restore_from_abort_state()
.endm

.macro notify_panic_data
    arm_save_abort_state()
    ldr r0, =NOTIFIER_CHAIN_ID_PANIC
    ldr r1, =NOTIFIER_EVENT_ID_PANIC_DATA
    mov r2, sp
    bl notifier_call
    arm_restore_from_abort_state()
.endm

/***************************************************
 * [NOTIFIERS DISABLED]                            *
 * If notifiers are disabled, we remove all calls. *
 ***************************************************/
#else /* (MONITOR_ENABLE_NOTIFIERS == false) */

#define notify_fiq_enter
#define notify_fiq_t_exit
#define notify_fiq_nt_exit

#define notify_irq_enter
#define notify_irq_nt_exit

#define notify_smc_dont_switch_exit
#define notify_smc_switch_to_nt_exit
#define notify_smc_switch_to_t_exit

#define notify_panic_irq_in_t
#define notify_panic_irq_in_nt
#define notify_panic_prefetch
#define notify_panic_data

#endif /* (MONITOR_ENABLE_NOTIFIERS == false) */

#endif /* _SAFEG_MONITOR_NOTIFIER_ASM_INC_ */
