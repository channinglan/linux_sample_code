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
 * [target.h]
 *
 * This file contains target-dependent constant definitions and the module's
 * C language interface.
 *
 * @(#) $Id: target.h 587 2013-11-17 15:51:22Z ertl-honda $  
 */
#ifndef _SAFEG_TARGET_H_
#define _SAFEG_TARGET_H_

#include "arm.h" /* ARM common definitions */
#include "target_common.h"

/*
 * Non-Secure Access Control register (NSCAR) configuration.
 *
 * Give the NT OS access to coprocessors cp10 and cp11 (Floating point and
 * advanced SIMD extensions). Since cp10 and cp11 are not banked, they
 * cannot be used by the T OS unless they are saved/restored during
 * world switches.
 */
#define TARGET_NSACR (NSACR_CP11 | NSACR_CP10)

/*
 * Secure Configuration Register (SCR). Target-dependent settings.
 * E.g.: bits nET, SCD, HCE, SIF in ARMv7.
 */
#define TARGET_T_SCR  (0)
#define TARGET_NT_SCR (0)

/*
 * Extra registers to save and load during context switches
 */
#define TARGET_T_CONTEXT_SIZE_BYTES   (0)
#define TARGET_NT_CONTEXT_SIZE_BYTES  (0)

/*
 * Target C interface
 */
#ifndef __ASSEMBLER__

/*
 * Target-dependent initialization.
 */
extern void target_init(uint32_t core_id);

/*
 * Target-dependent primary core initialization.
 */
extern void target_primary_core_init();

/*
 * Signal an interrupt to the opposite world.
 */
extern uint32_t target_signal(uint32_t core_id, uint32_t ns);

/*
 * Check if the memory region is valid for the corresponding world.
 */
extern uint32_t target_is_valid_memory(uint32_t ns,
                                       uint32_t address,
                                       uint32_t bytes);

/*
 * Check if the address is valid for read.
 */
extern uint32_t target_is_valid_readl_address(uint32_t core_id,
                                              uint32_t ns,
                                              uint32_t address);

/*
 * Check if the address is valid for write.
 */
extern uint32_t target_is_valid_writel_address(uint32_t core_id,
                                               uint32_t ns,
                                               uint32_t address);

#endif /* __ASSEMBLER__ */

#endif /* _SAFEG_TARGET_H_ */
