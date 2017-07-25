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
 * [arm.h]
 *
 * This file defines a set of ARM-related constants and utilities.
 *
 * @(#) $Id: arm.h 614 2013-11-23 16:22:37Z ertl-honda $
 */
#ifndef _SAFEG_MONITOR_COMMON_ARM_H_
#define _SAFEG_MONITOR_COMMON_ARM_H_

/*************************************************
 * [ARM Constants]                               *
 * Definitions of several ARM-related constants. *
 *************************************************/
/*
 * Current Program Status Register (CPSR)
 */
#ifdef __ASSEMBLER__
#define CPSR_MODE_USER      0b10000
#define CPSR_MODE_FIQ       0b10001
#define CPSR_MODE_IRQ       0b10010
#define CPSR_MODE_SVC       0b10011
#define CPSR_MODE_ABORT     0b10111
#define CPSR_MODE_UNDEF     0b11011
#define CPSR_MODE_SYSTEM    0b11111
#define CPSR_MODE_MONITOR   0b10110
#else /* !__ASSEMBLER__ */
#define CPSR_MODE_USER      0x10
#define CPSR_MODE_FIQ       0x11
#define CPSR_MODE_IRQ       0x12
#define CPSR_MODE_SVC       0x13
#define CPSR_MODE_ABORT     0x17
#define CPSR_MODE_UNDEF     0x1b
#define CPSR_MODE_SYSTEM    0x1f
#define CPSR_MODE_MONITOR   0x16
#endif /* __ASSEMBLER__ */

#define CPSR_BIT_A  0x00000100 /* Mask asynchronous abort */
#define CPSR_BIT_I  0x00000080 /* Mask IRQ interrupts     */
#define CPSR_BIT_F  0x00000040 /* Mask FIQ interrupts     */

/*
 * Secure Configuration Register (SCR) bits.
 */
#define SCR_NS    (1 << 0) /* if 1, the core is in NS state              */
#define SCR_IRQ   (1 << 1) /* if 1, IRQ interrupts trap to monitor mode  */
#define SCR_FIQ   (1 << 2) /* if 1, FIQ interrupts trap to monitor mode  */
#define SCR_EA    (1 << 3) /* if 1, External Aborts trap to monitor mode */
#define SCR_AW    (1 << 5) /* if 1, CPSR.A can be modified by the NS OS  */

/*
 * Non-Secure Access Control register (NSCAR) bits.
 */
#define NSACR_CP11 (1 << 11) /* NS access to coprocessor 11 */
#define NSACR_CP10 (1 << 10) /* NS access to coprocessor 10 */

/*
 * System Control Register bits.
 */
#define SCTLR_BIT_V (1 << 13)

/*
 * High exception IRQ vector address
 */
#define HIGH_EXCEPTION_VECTOR_IRQ  0xFFFF0018

/*********************************************************
 * [ARM macros: C interface]                             *
 * Macros for accessing ARM registers from a C function. *
 *********************************************************/
#if TARGET_ARCH_VERSION == 7

#define CP15_SCR_READ(x) \
    __asm__ volatile ("mrc p15, 0, %0, c1, c1, 0":"=r"(x))

#define CP15_SCR_WRITE(x) \
    __asm__ volatile ("mcr p15, 0, %0, c1, c1, 0"::"r"(x))

#define CP15_MVBAR_WRITE(x) \
    __asm__ volatile ("mcr p15, 0, %0, c12, c0, 1"::"r"(x))

#define CP15_NSACR_WRITE(x) \
    __asm__ volatile ("mcr p15, 0, %0, c1, c1, 2"::"r"(x))

#define CP15_MPIDR_READ(x) \
    __asm__ volatile ("mrc p15, 0, %0, c0, c0, 5":"=r"(x))

#define CP15_CTRL_WRITE(x) \
    __asm__ volatile ("mcr p15, 0, %0, c1, c0, 0"::"r"(x))

#define CP15_CTRL_READ(x) \
    __asm__ volatile ("mrc p15, 0, %0, c1, c0, 0":"=r"(x))

#define ARM_DATA_MEMORY_BARRIER \
    __asm__ __volatile__ ("dmb" : : : "memory")

#ifndef __ASSEMBLER__

static inline uint32_t arm_get_core_id(void)
{
    uint32_t core_id;
    CP15_MPIDR_READ(core_id);
    return core_id & 0xFF;
}

#endif /* __ASSEMBLER__ */

#endif /* TARGET_ARCH_VERSION */

#endif /* _SAFEG_MONITOR_COMMON_ARM_H_ */
