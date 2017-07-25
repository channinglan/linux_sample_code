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
 * [target.c]
 *
 * This file contains the implementation of target-dependent functions.
 */
#include "types.h"  /* uint32_t */
#include "utils.h"  /* UNUSED */
#include "target.h" /* prototypes */

/*
 * TrustZone initialization.
 */
static void target_trustzone_init(void)
{
    <--"TrustZone initialization code"
}

/*
 * Target-dependent initialization.
 */
void target_init(uint32_t core_id)
{
    if (core_id == TARGET_PRIMARY_CORE_ID) {
        target_trustzone_init();
        <--"modify if necessary to initialize the target"
    }
    <--"modify if necessary to initialize the target"
}

/*
 * Signal an interrupt to the opposite world.
 */
uint32_t target_signal(uint32_t core_id, uint32_t ns)
{
    <--"Support for this function is not strictly necessary for running SafeG"
    if (ns == 1) {
        <--"signal the T-OS with TARGET_T_INT_SIGNAL_ID"
    } else {
        <--"signal the NT-OS with TARGET_NT_INT_SIGNAL_ID"
    }
    return 0;
}

/*
 * Check if the memory region is valid for the corresponding world.
 */
uint32_t target_is_valid_memory(uint32_t ns,
                                uint32_t address,
                                uint32_t bytes)
{
    <--"Support for this function is not strictly necessary for running SafeG"

    if (ns == 0) {
#if (MONITOR_OMIT_T_CHECKS == 1)
        return true;
#else /* (MONITOR_OMIT_T_CHECKS == 0) */
        <--"validate the memory region [address..address+bytes]. If the monitor can safely write/read return true. Otherwise return false."
#endif /* (MONITOR_OMIT_T_CHECKS == 0) */
    } else {
        <--"validate the memory region [address..address+bytes]. If the monitor can safely write/read return true. Otherwise return false."
    }

    return false;
}
