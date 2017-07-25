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
 * [monitor_asm.inc]
 *
 * This file contains a set of macros used for the monitor.S to store/load
 * the register context of each world and set-up the return state. We keep
 * them separated from monitor.S for hiding configuration-dependent code for
 * the sake of clarity.
 *
 * @(#) $Id: monitor_asm.inc 779 2015-09-11 04:39:23Z ertl-honda $  
 */
#ifndef _SAFEG_MONITOR_ASM_INC_
#define _SAFEG_MONITOR_ASM_INC_

#include "monitor.h"       /* MON_NT_SCR, MON_STACK_SIZE_BYTES.. */
#include "target.h"        /* TARGET_XXX */
#include "target_asm.inc"  /* target_xxx */
#include "arm_asm.inc"     /* arm_xxx */

/**************************************************
 * [VFPV3 MACROS]                                 *
 * Constants and macros related to the VFPV3.     *
 **************************************************/
/*
 * [SAVE_VFPV3_NEON_REGS]
 * 
 * To acesss VFP registers (d0-d31), set EN on FPEXC.
 * To acesss FPEXC, enable access cp11/cp10 by CPACR.
 */
.macro SAVE_VFPV3_NEON_REGS
    /* save VFPv3 registers */
    arm_cpacr_read(r2)        /* Save CPACR */
    stmfd  r1!, {r2}
    mov    r2, #0x00f00000    /* Write CPACR to enable access cp11/cp10*/
    arm_cpacr_write(r2)
    fmrx   r2,  fpexc         /* Save FPEXC */
    stmfd  r1!, {r2}
    mov    r2, #0x40000000    /* Enable FPU to enable acess FPU registers*/
    vmsr   fpexc, r2
    isb    sy
    fmrx   r2,  fpscr
    stmfd  r1!, {r2}
    vstmdb r1!, {d0-d7}   /* = vstmfd */
    vstmdb r1!, {d8-d15}  /* = vstmfd */
    vstmdb r1!, {d16-d31} /* = vstmfd */   
.endm

/*
 * [LOAD_VFPV3_NEON_REGS]
 */
.macro LOAD_VFPV3_NEON_REGS
    mov   r2, #0x00f00000    /* Write CPACR to enable access cp11/cp10*/
    arm_cpacr_write(r2)
    mov   r2, #0x40000000    /* Enable FPU to enable acess FPU registers*/
    vmsr  fpexc, r2
    isb   sy
    vldm  r1!, {d16-d31} /* = vldmfd */
    vldm  r1!, {d8-d15}  /* = vldmfd */
    vldm  r1!, {d0-d7}   /* = vldmfd */
    ldmfd r1!, {r2}
    vmsr  fpscr, r2
    ldmfd r1!, {r2}
    vmsr  fpexc, r2
    ldmfd r1!, {r2}
    arm_cpacr_write(r2)   /* Load CPACR */
.endm



/**************************************************
 * [T CONTEXT MACROS]                             *
 * Constants and macros related to the T context. *
 **************************************************/
/*
 * [MON_SMC_SAVE_T_CONTEXT]
 *
 * Pre-conditions:
 *      - mode: Monitor
 *      - sp_mon = t_context_n - 8 (PC and CPSR saved)
 *      - r0 = syscall error value
 *      - r1-r12,lr_svc saved by user
 *      - NS=0
 * Post-condition:
 *      - sp_mon = t_context_n - 8
 *      - all T registers saved
 *      - r1 = monitor_stack_n
 *      - NS=0
 *      - mode: Monitor
 */
.macro MON_SMC_SAVE_T_CONTEXT
    mov r1, sp
    cps #CPSR_MODE_SVC
    mrs r2, spsr
    stmfd r1!, {r0, r2, sp}
#if (MONITOR_T_SAVE_SVC_ONLY == 0)
    stmfd r1, {sp, lr}^
    sub   r1, r1, #8
    cps   #CPSR_MODE_ABORT
    mrs   r2, spsr
    stmfd r1!, {r2, sp, lr}
    cps   #CPSR_MODE_UNDEF
    mrs   r2, spsr
    stmfd r1!, {r2, sp, lr}
#if (MONITOR_T_FIQS_TO_IRQS == 1)
    cps   #CPSR_MODE_IRQ
    mrs   r2, spsr
    stmfd r1!, {r2, sp, lr}
#else /* (MONITOR_T_FIQS_TO_IRQS == 0) */
    cps   #CPSR_MODE_FIQ
    mrs   r2, spsr
    stmfd r1!, {r2, r8-lr}
#endif /* (MONITOR_T_FIQS_TO_IRQS == 0) */
#endif /* (MONITOR_T_SAVE_SVC_ONLY == 0) */
#if (MONITOR_T_SAVE_VFPV3_NEON == 1)
    SAVE_VFPV3_NEON_REGS
#endif /* (MONITOR_T_SAVE_VFPV3_NEON == 1) */
    cps #CPSR_MODE_MONITOR
    target_save_t_context(r1, r2)
.endm

/*
 * [MON_LOAD_T_CONTEXT]
 *
 * Pre-conditions:
 *      - mode: Monitor
 *      - NS=0
 *      - sp_mon = monitor_stack_n
 *      - all NT registers saved
 * Post-condition:
 *      - sp_mon = t_context_n - 8
 *      - all T registers loaded
 *      - r1-r12, lr_svc, lr_mon can be used (saved in caller stack)
 *      - r0 = syscall error value from previous call
 *      - NS=0
 *      - mode: Monitor
 */
.macro MON_LOAD_T_CONTEXT
    mov r1, sp
    target_load_t_context(r1, r2)
#if (MONITOR_T_SAVE_VFPV3_NEON == 1)
    LOAD_VFPV3_NEON_REGS
#endif /* (MONITOR_T_SAVE_VFPV3_NEON == 1) */
#if (MONITOR_T_SAVE_SVC_ONLY == 0)
#if (MONITOR_T_FIQS_TO_IRQS == 1)
    cps   #CPSR_MODE_IRQ
    ldmfd r1!, {r2, sp, lr}
    msr   spsr_fsxc, r2
#else /* (MONITOR_T_FIQS_TO_IRQS == 0) */
    cps   #CPSR_MODE_FIQ
    ldmfd r1!, {r2, r8-lr}
    msr   spsr_fsxc, r2
#endif /* (MONITOR_T_FIQS_TO_IRQS == 0) */
    cps #CPSR_MODE_UNDEF
    ldmfd r1!, {r2, sp, lr}
    msr spsr_fsxc, r2
    cps #CPSR_MODE_ABORT
    ldmfd r1!, {r2, sp, lr}
    msr spsr_fsxc, r2
    ldmfd r1, {sp,lr}^
    add r1, r1, #8
#endif /* (MONITOR_T_SAVE_SVC_ONLY == 0) */
    cps #CPSR_MODE_SVC
    ldmfd r1!, {r0, r2, sp}
    msr   spsr_fsxc, r2
    cps #CPSR_MODE_MONITOR
    mov sp, r1
.endm

/***************************************************
 * [NT CONTEXT MACROS]                             *
 * Constants and macros related to the NT context. *
 ***************************************************/
/*
 * [MON_SMC_SAVE_NT_CONTEXT]
 *
 * Pre-conditions:
 *      - mode = Monitor
 *      - sp_mon = monitor_stack - 8 (PC and CPSR saved)
 *      - r0 = syscall error value
 *      - r1-r12, lr_svc saved by user
 *      - lr_mon can be used
 *      - NS = 1
 * Post-conditions:
 *      - mode = Monitor
 *      - NS = 0
 *      - sp_mon = monitor_stack_n
 *      - all NT registers saved
 */
.macro MON_SMC_SAVE_NT_CONTEXT
    /* go secure (changing mode while NS=1 is insecure) */
    mov r1, #MON_T_SCR
    arm_scr_write(r1)
    /* copy return state from stack to nt_context_n */
    ldmfd sp!, {r2, r3}
    sub r1, sp, #MON_STACK_SIZE_BYTES
    stmfd r1!, {r2, r3}
    /* save USER banked registers (skipping those that were saved) */
    stmfd r1, {sp, lr}^
    sub r1, r1, #(14*4) /* skip r1-r12 */
    stmfd r1!, {r0}
    /* save SVC banked registers (skipping those that were saved) */
    cps #CPSR_MODE_SVC
    sub r1, r1, #(1*4) /* skip lr_svc */
    mrs r2, spsr
    stmfd r1!, {r2, sp}
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
#if (MONITOR_NT_SAVE_VFPV3_NEON == 1)
    SAVE_VFPV3_NEON_REGS
#endif /* (MONITOR_NT_SAVE_VFPV3_NEON == 1) */
    /* save target-dependent registers */
    cps #CPSR_MODE_MONITOR
    target_save_nt_context(r1, r2)
.endm

/*
 * [MON_FIQ_SAVE_NT_CONTEXT]
 *
 * Pre-conditions:
 *      - mode = Monitor
 *      - sp_mon = monitor_stack - 8 (PC and CPSR saved)
 *      - lr_mon can be used
 *      - NS = 1
 * Post-conditions:
 *      - mode = Monitor
 *      - NS = 0
 *      - sp_mon = monitor_stack_n
 *      - all NT registers saved
 */
.macro MON_FIQ_SAVE_NT_CONTEXT
    /* go secure (changing mode while NS=1 is insecure) */
    mov lr, #MON_T_SCR
    arm_scr_write(lr)
    /* move stack to nt_context_n - 8 */
    sub sp, sp, #MON_STACK_SIZE_BYTES
    /* save USER registers */
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
#if (MONITOR_NT_SAVE_VFPV3_NEON == 1)
    SAVE_VFPV3_NEON_REGS
#endif /* (MONITOR_NT_SAVE_VFPV3_NEON == 1) */
    /* save target-dependent registers */
    cps #CPSR_MODE_MONITOR
    target_save_nt_context(r1, r2)
    /* copy return state from stack to nt_context_n */
    add sp, sp, #MON_STACK_SIZE_BYTES
    ldmfd sp!, {r2, r3}
    sub r2, r2, #4 /* save pc instead of pc+4 */
    sub r1, sp, #MON_STACK_SIZE_BYTES
    stmfd r1!, {r2, r3}
.endm

/*
 * [MON_LOAD_NT_CONTEXT]
 *
 * Pre-conditions:
 *      - sp_mon = t_context_n - 8
 *      - all T registers saved
 *      - r1 = monitor_stack_n
 *      - NS = 0
 *      - mode = Monitor
 * Post-conditions:
 *      - sp_mon = monitor_stack_n
 *      - all NT registers loaded
 *      - lr = nt_context_n - 8
 *      - r0 = syscall error value (if previous nt2t switch was by SMC)
 *      - NS = 1
 *      - mode = Monitor
 */
.macro MON_LOAD_NT_CONTEXT
    /* we use r1, r2 and lr as temporary registers for loading NT context */
    sub r1, r1, #(MON_STACK_SIZE_BYTES + MON_NT_CONTEXT_SIZE_BYTES)
    /* load target-dependent registers */
    target_load_nt_context(r1, r2)
    /* reset fiq registers if they are being used by T OS (security) */
#ifndef TARGET_T_DONT_USE_FIQS
    cps #CPSR_MODE_FIQ
    mov r8, #0
    mov r9, #0
    mov r10, #0
    mov r11, #0
    mov r12, #0
    mov sp, #0
    mov lr, #0
    msr spsr_fsxc, #0
#endif
#if (MONITOR_NT_SAVE_VFPV3_NEON == 1)
    LOAD_VFPV3_NEON_REGS
#endif /* (MONITOR_NT_SAVE_VFPV3_NEON == 1) */
    /* load IRQ mode spsr-sp-lr */
    cps #CPSR_MODE_IRQ
    ldmfd r1!, {r2, sp, lr}
    msr spsr_fsxc, r2
    /* load UNDEFINED mode's banked registers */
    cps #CPSR_MODE_UNDEF
    ldmfd r1!, {r2, sp, lr}
    msr spsr_fsxc, r2
    /* load ABORT mode's banked registers */
    cps #CPSR_MODE_ABORT
    ldmfd r1!, {r2, sp, lr}
    msr spsr_fsxc, r2
    /* load SVC's banked registers */
    cps #CPSR_MODE_SVC
    ldmfd r1!, {r2, sp, lr}
    msr spsr_fsxc, r2
    /* load USR mode registers while in monitor mode */
    cps #CPSR_MODE_MONITOR
    mov sp, r1
    ldmfd sp, {r0-lr}^
    /* set the NT SCR */
    mov lr, #MON_NT_SCR
    arm_scr_write(lr)
    /* flush all branch predictors to avoid security leaks */
    arm_flush_branch_predictors(lr)
    /* adjust sp_mon and lr to meet post-conditions */
    add lr, sp, #(15*4)
    add sp, lr, #(MON_STACK_SIZE_BYTES + 8)
.endm

/********************************************
 * [FIQ HANDLING MACROS]                    *
 * Macros for handling FIQs in the monitor. *
 ********************************************/
#if (MONITOR_T_FIQS_TO_IRQS == 1)
#define MON_T_FIQ__SET_T_RETURN_STATE  mon_t_fiq__convert_to_irq
#define MON_NT_FIQ__SET_T_RETURN_STATE mon_nt_fiq__convert_to_irq
#else /* (MONITOR_T_FIQS_TO_IRQS == 0) */
#define MON_T_FIQ__SET_T_RETURN_STATE  mon_panic(MON_PANIC_REASON__FIQ_IN_T)
#define MON_NT_FIQ__SET_T_RETURN_STATE mon_nt_fiq__convert_to_fiq
#endif /* (MONITOR_T_FIQS_TO_IRQS == 0) */

/*
 * [mon_nt_fiq__convert_to_irq]
 *
 * Pre-conditions:
 *      - sp_mon = t_context_n - 8
 *      - return state (pc and cpsr) at t_context_n
 *      - r1, r2, lr_mon can be used
 *      - [sp, #-4] and [sp, #-8] cannot be used
 *      - NS = 0
 *      - mode: Monitor
 * Post-conditions:
 *      - The same as pre-conditions except:
 *          + lr_irq   = pc + 4
 *          + spsr_irq = cpsr
 *          + new return state (pc: T irq vector, cpsr: irq/fiq/abort disabled)
 */
.macro mon_nt_fiq__convert_to_irq
    /* set lr_irq  = PC + 4, spsr_irq = CPSR */
    ldmfd sp, {r1, r2}
    cps #CPSR_MODE_IRQ
    add lr, r1, #4
    msr spsr_fsxc, r2
    cps #CPSR_MODE_MONITOR
    /* set new return state (pc: T irq vector, cpsr: irq/fiq/abort disabled) */
    arm_irq_vector_read(lr)
    str lr, [sp]
    ldr lr, =(CPSR_MODE_IRQ | CPSR_BIT_F | CPSR_BIT_I | CPSR_BIT_A)
    str lr, [sp, #4]
.endm

/*
 * [mon_nt_fiq__convert_to_fiq]
 *
 * Pre-conditions:
 *      - sp_mon = t_context_n - 8
 *      - return state (pc and cpsr) at t_context_n
 *      - only lr_mon can be used
 *      - NS = 0
 *      - mode: Monitor
 * Post-conditions:
 *      - The same as pre-conditions except:
 *          + lr_fiq   = pc + 4
 *          + spsr_fiq = cpsr
 *          + new return state (pc: T fiq vector, cpsr: irq/fiq/abort disabled)
 */
.macro mon_nt_fiq__convert_to_fiq
    /* save r1, r2 on the stack temporarily */
    stmfd sp, {r1, r2}
    /* set lr_fiq  = PC + 4, spsr_fiq = CPSR */
    ldmfd sp, {r1, r2}
    cps #CPSR_MODE_FIQ
    add lr, r1, #4
    msr spsr_fsxc, r2
    cps #CPSR_MODE_MONITOR
    /* set new return state (pc: T fiq vector, cpsr: irq/fiq/abort disabled) */
    arm_fiq_vector_read(lr)
    str lr, [sp]
    ldr lr, =(CPSR_MODE_IRQ | CPSR_BIT_F | CPSR_BIT_I | CPSR_BIT_A)
    str lr, [sp, #4]
    /* restore r1 and r2 */
    ldmdb sp, {r1, r2}
.endm

/*
 * [mon_t_fiq__convert_to_irq]
 *
 * Pre-conditions:
 *      - sp_mon = t_context_n - 8
 *      - return state (pc and cpsr) at t_context_n
 *      - only lr_mon can be used
 *      - NS = 0
 *      - mode: Monitor
 * Post-conditions:
 *      - The same as pre-conditions except:
 *          + lr_irq   = pc + 4
 *          + spsr_irq = cpsr
 *          + new return state (pc: T irq vector, cpsr: irq/fiq/abort disabled)
 */
.macro mon_t_fiq__convert_to_irq
    /* save r1, r2 on the stack temporarily */
    stmfd sp, {r1, r2}
    /* set lr_irq  = PC + 4, spsr_irq = CPSR */
    ldmfd sp, {r1, r2}
    cps #CPSR_MODE_IRQ
    mov lr, r1
    msr spsr_fsxc, r2
    cps #CPSR_MODE_MONITOR
    /* set new return state (pc: T irq vector, cpsr: irq/fiq/abort disabled) */
    arm_irq_vector_read(lr)
    str lr, [sp]
    ldr lr, =(CPSR_MODE_IRQ | CPSR_BIT_F | CPSR_BIT_I | CPSR_BIT_A)
    str lr, [sp, #4]
    /* restore r1 and r2 */
    ldmdb sp, {r1, r2}
.endm

/********************************************
 * [IRQ HANDLING MACROS]                    *
 * Macros for handling IRQs in the monitor. *
 ********************************************/
#if (MONITOR_TRAP_NT_IRQS == 1)
#define MON_NT_IRQ__SET_NT_RETURN_STATE mon_nt_irq__return
#else /* (MONITOR_TRAP_NT_IRQS == 0) */
#define MON_NT_IRQ__SET_NT_RETURN_STATE notify_panic_irq_in_nt
#endif /* (MONITOR_TRAP_NT_IRQS == 0) */

/*
 * [mon_nt_irq__return]
 *
 * Pre-conditions:
 *      - mode = Monitor
 *      - sp_mon = monitor_stack - 8 (PC and CPSR saved)
 *      - only lr_mon can be used
 *      - NS = 1
 * Post-conditions:
 *      - The same as pre-conditions except:
 *          + lr_irq   = pc + 4
 *          + spsr_irq = cpsr
 *          + new return state (pc: NT irq vector, cpsr: irq/abort disabled)
 */
.macro mon_nt_irq__return
    /* go secure (changing mode while NS=1 is insecure) */
    mov lr, #MON_T_SCR
    arm_scr_write(lr)
    /* save r1, r2 on the stack temporarily */
    stmfd sp, {r1, r2}
    /* set lr_irq  = PC + 4, spsr_irq = CPSR */
    ldmfd sp, {r1, r2}
    cps #CPSR_MODE_IRQ
    mov lr, r1
    msr spsr_fsxc, r2
    cps #CPSR_MODE_MONITOR
    /* go back to non-secure */
    mov lr, #MON_NT_SCR
    arm_scr_write(lr)
    /* set new return state (pc: NT irq vector, cpsr: irq/abort disabled) */
    arm_irq_or_high_vector_read(lr)
    str lr, [sp]
    ldr lr, =(CPSR_MODE_IRQ | CPSR_BIT_I)
    str lr, [sp, #4]
    /* restore r1 and r2 */
    ldmdb sp, {r1, r2}
.endm

#endif /* _SAFEG_MONITOR_ASM_INC_ */
