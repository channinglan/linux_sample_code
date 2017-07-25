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
 * [syscalls.h]
 *
 * This file contains the interface of the system calls module.
 * The return value of a system call indicates both the error condition
 * and a request to swith or not to the opposite TrustZone world.
 *
 * @(#) $Id: syscalls.h 587 2013-11-17 15:51:22Z ertl-honda $    
 */
#ifndef _SAFEG_MONITOR_SYSCALLS_H_
#define _SAFEG_MONITOR_SYSCALLS_H_

#include "error.h"  /* ERROR__OK */

/**********************************
 * [Return values]                *
 * Return value for system calls. *
 **********************************/
/* Switch values (0..0xF) */
#define SYSCALL_RET__DONT_SWITCH     (0x0)
#define SYSCALL_RET__SWITCH_TO_T     (0x1)
#define SYSCALL_RET__SWITCH_TO_NT    (0x2)

/* Return value (switch [31:28] and error[27:0]). */
#define RETVAL(switch, error) (((switch) << 28) | (error))
#define SYSCALL_ERROR(x) RETVAL(SYSCALL_RET__DONT_SWITCH, (x))
#define SYSCALL_OK(x)    RETVAL(x, ERROR__OK)

/**********************************************
 * [Syscalls Initialization]                  *
 * Initialization of the system calls module. *
 **********************************************/
#ifndef __ASSEMBLER__

#if (MONITOR_ENABLE_SYSCALLS == 1)
extern void syscalls_init(void);
#else /* (MONITOR_ENABLE_SYSCALLS == 0) */
#define syscalls_init()
#endif /* (MONITOR_ENABLE_SYSCALLS == 0) */

#endif /* __ASSEMBLER__ */

#endif /* _SAFEG_MONITOR_SYSCALLS_H_ */
