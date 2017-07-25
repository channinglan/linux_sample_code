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
 *  @(#) $Id: syscalls_api.c 765 2015-09-02 07:34:28Z ertl-honda $  
 */
typedef unsigned char     uint8_t;
typedef unsigned long int uint32_t;

#include "syscalls_api.h"

/*
 * System call IDs for static system calls
 */
#define SAFEG_SYSCALL_ID__GETID     (0)
#define SAFEG_SYSCALL_ID__SETPERM   (1)
#define SAFEG_SYSCALL_ID__SWITCH    (2)
#define SAFEG_SYSCALL_ID__SIGNAL    (3)
#define SAFEG_SYSCALL_ID__REGDYN    (4)
#define SAFEG_SYSCALL_ID__REGNOT    (5)
#define SAFEG_SYSCALL_ID__READL     (6)
#define SAFEG_SYSCALL_ID__WRITEL    (7)
#define SAFEG_SYSCALL_ID__RESTARTNT (8)
#define SAFEG_SYSCALL_ID__SETNTPC   (9)

/*
 * SMC call to SafeG.
 */
#ifndef asm
#define asm __asm__
#endif

/* Perform a Security Monitor Call (SMC), saving r12(ip) and r14(lr) */
#define SAFEG_SMC()                                                        \
    asm volatile ("dsb\n\t"                                                \
                  "smc #0\n\t"                                             \
                  : "=r" (arg_r0)                                          \
                  : "0" (arg_r0), "r" (arg_r1), "r" (arg_r2), "r" (arg_r3) \
                  : "ip", "lr", "memory", "cc")

/* Perform a Security Monitor Call (SMC), saving r4-r11, r12(ip), r14(lr) */
#define SAFEG_SMC_SAVE_ALL()                                                \
    asm volatile ("dsb\n\t"                                                 \
                  "smc #0\n\t"                                              \
                  : "=r" (arg_r0)                                           \
                  : "0" (arg_r0), "r" (arg_r1), "r" (arg_r2), "r" (arg_r3)  \
                  : "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r11", "ip", "lr", \
                    "memory", "cc")

/**********************************************************************
 * [Static system calls API]                                          *
 * Definitions for static system calls (installed at initialization). *
 **********************************************************************/
/*
 * GETID: obtains the ID of a system call by name.
 */
uint32_t safeg_syscall_getid(const char *name, uint32_t *id)
{
    register uint32_t arg_r0 asm("r0") = SAFEG_SYSCALL_ID__GETID;
    register uint32_t arg_r1 asm("r1") = (uint32_t)name;
    register uint32_t arg_r2 asm("r2") = (uint32_t)id;
    register uint32_t arg_r3 asm("r3") = 0;

    SAFEG_SMC();

    return arg_r0;
}

/*
 * SETPERM: set the permissions for a static or dynamic system call.
 */
uint32_t safeg_syscall_setperm(uint32_t id,
                               uint32_t is_t_callable,
                               uint32_t is_nt_callable)
{
    register uint32_t arg_r0 asm("r0") = SAFEG_SYSCALL_ID__SETPERM;
    register uint32_t arg_r1 asm("r1") = id;
    register uint32_t arg_r2 asm("r2") = is_t_callable;
    register uint32_t arg_r3 asm("r3") = is_nt_callable;

    SAFEG_SMC();

    return arg_r0;
}

/*
 * SWITCH: initiates a switch to the opposite world.
 */
uint32_t safeg_syscall_switch(void)
{
    register uint32_t arg_r0 asm("r0") = SAFEG_SYSCALL_ID__SWITCH;
    register uint32_t arg_r1 asm("r1") = 0;
    register uint32_t arg_r2 asm("r2") = 0;
    register uint32_t arg_r3 asm("r3") = 0;

    SAFEG_SMC_SAVE_ALL();

    return arg_r0;
}

/*
 * SIGNAL: signals an interrupt to the opposite world.
 */
uint32_t safeg_syscall_signal(void)
{
    register uint32_t arg_r0 asm("r0") = SAFEG_SYSCALL_ID__SIGNAL;
    register uint32_t arg_r1 asm("r1") = 0;
    register uint32_t arg_r2 asm("r2") = 0;
    register uint32_t arg_r3 asm("r3") = 0;

    SAFEG_SMC_SAVE_ALL();

    return arg_r0;
}

/***************************************************
 * [Dynamic system calls API]                      *
 * Definitions related to the dynamic system calls.*
 ***************************************************/
/*
 * REGDYN: register a dynamic system call.
 */
uint32_t safeg_syscall_regdyn(const struct safeg_syscall *call,
                              uint32_t *id)
{
    register uint32_t arg_r0 asm("r0") = SAFEG_SYSCALL_ID__REGDYN;
    register uint32_t arg_r1 asm("r1") = (uint32_t)call;
    register uint32_t arg_r2 asm("r2") = (uint32_t)id;
    register uint32_t arg_r3 asm("r3") = 0;

    SAFEG_SMC();

    return arg_r0;
}

/*
 * INVOKE: invokes a system call.
 */
uint32_t safeg_syscall_invoke(uint32_t id,
                              uint32_t a,
                              uint32_t b,
                              uint32_t c)
{
    register uint32_t arg_r0 asm("r0") = id;
    register uint32_t arg_r1 asm("r1") = a;
    register uint32_t arg_r2 asm("r2") = b;
    register uint32_t arg_r3 asm("r3") = c;

    SAFEG_SMC_SAVE_ALL();

    return arg_r0;
}

/**************************************************
 * [Notifiers API]                                *
 * Definitions related to the notifier callbacks. *
 **************************************************/
/*
 * REGNOT: register a notifier callback.
 */
uint32_t safeg_syscall_regnot(const struct safeg_notifier *notifier,
                              uint32_t *id)
{
    register uint32_t arg_r0 asm("r0") = SAFEG_SYSCALL_ID__REGNOT;
    register uint32_t arg_r1 asm("r1") = (uint32_t)notifier;
    register uint32_t arg_r2 asm("r2") = (uint32_t)id;
    register uint32_t arg_r3 asm("r3") = 0;

    SAFEG_SMC();

    return arg_r0;
}

/*
 * READL: read specified address
 */
uint32_t safeg_syscall_readl(uint32_t address, uint32_t *rdata_ref)
{
    register uint32_t arg_r0 asm("r0") = SAFEG_SYSCALL_ID__READL;
    register uint32_t arg_r1 asm("r1") = (uint32_t)address;
    register uint32_t arg_r2 asm("r2") = (uint32_t)rdata_ref;
    register uint32_t arg_r3 asm("r3") = 0;

    SAFEG_SMC();

    return arg_r0;
}

/*
 * WRITEL: write specified address
 */
uint32_t safeg_syscall_writel(uint32_t address, uint32_t wdata)
{
    register uint32_t arg_r0 asm("r0") = SAFEG_SYSCALL_ID__WRITEL;
    register uint32_t arg_r1 asm("r1") = (uint32_t)address;
    register uint32_t arg_r2 asm("r2") = (uint32_t)wdata;
    register uint32_t arg_r3 asm("r3") = 0;

    SAFEG_SMC();

    return arg_r0;
}

/*
 * RESTARNT: restart NT OS
 */
uint32_t safeg_syscall_restarnt(void)
{
    register uint32_t arg_r0 asm("r0") = SAFEG_SYSCALL_ID__RESTARTNT;
    register uint32_t arg_r1 asm("r1") = 0;
    register uint32_t arg_r2 asm("r2") = 0;
    register uint32_t arg_r3 asm("r3") = 0;

    SAFEG_SMC();

    return arg_r0;
}

/*
 * SETNTPC: set NT OS PC(swith address)
 */
uint32_t safeg_syscall_setntpc(uint32_t pc)
{
    register uint32_t arg_r0 asm("r0") = SAFEG_SYSCALL_ID__SETNTPC;
    register uint32_t arg_r1 asm("r1") = (uint32_t)pc;
    register uint32_t arg_r2 asm("r2") = 0;
    register uint32_t arg_r3 asm("r3") = 0;

    SAFEG_SMC();

    return arg_r0;
}