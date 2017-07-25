/*
 *  TOPPERS/SafeG Dual-OS monitor
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Safety Gate Dual-OS monitor
 *
 *  Copyright (C) 2009-2015 by Embedded and Real-Time Systems Laboratory
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
 * [monitor.h]
 *
 * This file contains a few constant values used by the monitor. The name of
 * all exported constants use the 'MON_XXX' naming convention.
 *
 * @(#) $Id: monitor.h 779 2015-09-11 04:39:23Z ertl-honda $  
 */
#ifndef _SAFEG_MONITOR_H_
#define _SAFEG_MONITOR_H_

#include "target.h" /* TARGET_XXX */
#include "arm.h"    /* ARM constants */

/*****************************************************
 * [TRUSTZONE REGISTER CONSTANTS]                    *
 * Constants written to TrustZone-related registers. *
 *****************************************************/
/*
 * NT: CPSR initialization value.
 */
#define MON_NT_CPSR_INIT_VALUE (CPSR_BIT_I | CPSR_MODE_SVC)

/*
 * T: Non-Secure Access Control register (NSCAR).
 */
#define MON_NSACR TARGET_NSACR

/*
 * T and NT: Secure Configuration Register (SCR).
 */
/* Configuration of the "SCR.EA bit" (external aborts) */
#if (MONITOR_TRAP_EXT_ABORTS == 1)
#define T_SCR_ABORT   SCR_EA
#define NT_SCR_ABORT  SCR_EA
#elif (MONITOR_TRAP_EXT_ABORTS == 0)
#define T_SCR_ABORT   (0)
#define NT_SCR_ABORT  (0)
#endif /* (MONITOR_TRAP_EXT_ABORTS == 0) */

/* Configuration of the "SCR.F bit" (FIQ interrupt) */
#if (MONITOR_T_FIQS_TO_IRQS == 1) && !(MONITOR_TRAP_T_FIQS == 1)
#error "FIQs must be trapped if conversion to IRQs is needed"
#endif /* (MONITOR_T_FIQS_TO_IRQS==true) && !(MONITOR_TRAP_T_FIQS==true) */

#if (MONITOR_TRAP_T_FIQS == 1)
#define T_SCR_FIQ   SCR_FIQ
#elif (MONITOR_TRAP_T_FIQS == 0)
#define T_SCR_FIQ   (0)
#endif /* (MONITOR_T_DONT_USE_FIQS == 0) */

#define NT_SCR_FIQ  SCR_FIQ   /* NT FIQs always trap */

/* Configuration of the "SCR.I bit" (IRQ interrupt) */
#if (MONITOR_TRAP_NT_IRQS == 1)
#define NT_SCR_IRQ  SCR_IRQ
#elif (MONITOR_TRAP_NT_IRQS == 0)
#define NT_SCR_IRQ  (0)
#endif /* (MONITOR_TRAP_NT_IRQS == 0) */

#define T_SCR_IRQ   (0)  /* T devices must use FIQs */

/* Configuration of the SCR including target-dependent bits */
#define MON_T_SCR  (T_SCR_ABORT | T_SCR_FIQ | T_SCR_IRQ  | TARGET_T_SCR)
#define MON_NT_SCR (NT_SCR_ABORT | NT_SCR_FIQ | NT_SCR_IRQ | TARGET_NT_SCR | \
                    SCR_NS)

/**************************************************************
 * [T CONTEXT SIZE]                                           *
 * Size of the memory region per core to store the T context. *
 **************************************************************/
/*
 * The layout of the T Context memory is as follows:
 *
 * (-) +-----------------+
 *     |  Target-depend  |
 *     |    registers    |
 *     +-----------------+    
 * #if (MONITOR_T_SAVE_VFPV3_NEON == 1)  
 *     +-----------------+  
 *     |   VFPv3 D0-D31  |  
 *     +-----------------+
 * #endif (MONITOR_T_SAVE_VFPV3_NEON == 0)
 *     +-----------------+  
 * #if (MONITOR_T_SAVE_SVC_ONLY == 0)
 *   #if (MONITOR_T_FIQS_TO_IRQS == 1)
 *     +-----------------+
 *     |  IRQ spsr-sp-lr |
 *     +-----------------+
 *   #elif (MONITOR_T_FIQS_TO_IRQS == 0)
 *     +-----------------+
 *     |  FIQ spsr,r8-lr |
 *     +-----------------+
 *   #endif (MONITOR_T_FIQS_TO_IRQS == 0)
 *     +-----------------+
 *     |  UND spsr-sp-lr |
 *     +-----------------+
 *     |  ABT spsr-sp-lr |
 *     +-----------------+
 *     |     USR sp-lr   |
 *     +-----------------+
 * #endif (MONITOR_T_SAVE_SVC_ONLY == 0)
 *     +-----------------+
 *     |     USR r0      |
 *     +-----------------+
 *     |    SVC spsr-sp  |
 *     +-----------------+
 *     |       PC        |
 *     +-----------------+
 *     |      CPSR       |
 * (+) +-----------------+
 *
 */
#define T_MODES_BYTES (8*4)
#define T_FIQ_BYTES   (8*4)
#define T_IRQ_BYTES   (3*4)
#define VFPV3_BYTES   ((32*8) + (3*4))

#if (MONITOR_T_SAVE_VFPV3_NEON == 0)
#define T_MIN_BYTES   (5*4 + TARGET_T_CONTEXT_SIZE_BYTES)
#else /* (MONITOR_T_SAVE_VFPV3_NEON == 1) */
#define T_MIN_BYTES   (5*4 + TARGET_T_CONTEXT_SIZE_BYTES + VFPV3_BYTES)
#endif /* (MONITOR_T_SAVE_VFPV3_NEON == 0) */

#if (MONITOR_T_SAVE_SVC_ONLY == 1)
#define MON_T_CONTEXT_SIZE_BYTES  (T_MIN_BYTES)
#elif (MONITOR_T_SAVE_SVC_ONLY == 0) && (MONITOR_T_FIQS_TO_IRQS == 1)
#define MON_T_CONTEXT_SIZE_BYTES  (T_MIN_BYTES + T_MODES_BYTES + T_IRQ_BYTES)
#elif (MONITOR_T_SAVE_SVC_ONLY == 0) && (MONITOR_T_FIQS_TO_IRQS == 0)
#define MON_T_CONTEXT_SIZE_BYTES  (T_MIN_BYTES + T_MODES_BYTES + T_FIQ_BYTES)
#endif /*(MONITOR_T_SAVE_SVC_ONLY==false)&&(MONITOR_T_FIQS_TO_IRQS==false)*/



/***************************************************************
 * [NT CONTEXT SIZE]                                           *
 * Size of the memory region per core to store the NT context. *
 ***************************************************************/
/*
 * The layout of the NT Context memory is as follows:
 *
 * (-) +----------------+
 *     |                |
 *     | Target-depend  |
 *     |                |
 *     +----------------+
 * #if (MONITOR_NT_SAVE_VFPV3_NEON == 1)  
 *     +----------------+  
 *     |   VFPv3 D0-D31 |  
 *     +----------------+
 * #endif (MONITOR_NT_SAVE_VFPV3_NEON == 0)
 *     +----------------+  
 *     | IRQ spsr-sp-lr |
 *     +----------------+
 *     | UND spsr-sp-lr |
 *     +----------------+
 *     | ABT spsr-sp-lr |
 *     +----------------+
 *     | SVC spsr-sp-lr | (lr_svc already saved in case of smc)
 *     +----------------+
 *     |    USR r0-lr   | (r1-r12 already saved in case of smc)
 *     +----------------+
 *     |       PC       |
 *     +----------------+
 *     |     CPSR       |
 * (+) +----------------+
 *
 */
#if (MONITOR_NT_SAVE_VFPV3_NEON == 0)
#define MON_NT_CONTEXT_SIZE_BYTES  (29*4 + TARGET_NT_CONTEXT_SIZE_BYTES)
#else /* (MONITOR_NT_SAVE_VFPV3_NEON == 1) */
#define MON_NT_CONTEXT_SIZE_BYTES  (29*4 + TARGET_NT_CONTEXT_SIZE_BYTES + VFPV3_BYTES)
#endif /* (MONITOR_NT_SAVE_VFPV3_NEON == 0) */



/**************************************************************
 * [MONITOR SPACE]                                            *
 * Space for storing the T/NT contexts and the monitor stack. *
 **************************************************************/
/*
 * Each core has a space of memory for storing the T/NT contexts and the
 * monitor stack. The default size for this space is 2**10 = 1024 bytes/core
 * but it can be changed by defining the TARGET_MON_SPACE_SHIFT constant.
 *
 * (-)
 *      +-------> +-----------------+ <-----+
 *      |         |       NT        |       |
 *      |         |     Context     |       | MON_NT_CONTEXT_SIZE_BYTES
 *      |         |    (core n)     |       |
 *   Monitor      +-----------------+ <-----+-- [nt_context_n]
 *    Space       |     Monitor     |       |
 *   (core n)     |     Stack       |       | MON_STACK_SIZE_BYTES
 *      |         |    (core n)     |       |
 *      |         +-----------------+ <-----+-- [monitor_stack_n]
 *      |         |       T         |       |
 *      |         |    Context      |       | MON_T_CONTEXT_SIZE_BYTES
 *      |         |    (core n)     |       |
 *      +-------> +-----------------+ <-----+-- [t_context_n]
 * (+)
 *
 */
#if !defined(TARGET_MON_SPACE_SHIFT)
#define MON_SPACE_SHIFT (10)
#else /* defined(TARGET_MON_SPACE_SHIFT) */
#define MON_SPACE_SHIFT (TARGET_MON_SPACE_SHIFT)
#endif /* defined(TARGET_MON_SPACE_SHIFT) */

#define MON_SPACE_BYTES_PERCORE (1 << MON_SPACE_SHIFT)
#define MON_SPACE_BYTES_TOTAL (TARGET_NUM_CORES * MON_SPACE_BYTES_PERCORE)

/***************************************
 * [MONITOR STACK SIZE]                *
 * Size of the monitor stack per-core. *
 ***************************************/
#define MON_STACK_SIZE_BYTES (MON_SPACE_BYTES_PERCORE - \
        MON_T_CONTEXT_SIZE_BYTES - MON_NT_CONTEXT_SIZE_BYTES)

#if MON_STACK_SIZE_BYTES < 128
#warning "The monitor stack size per-core is less than 128 bytes."
#endif /* MON_STACK_SIZE_BYTES_PERCORE < 128 */

#endif /* _SAFEG_MONITOR_H_ */
