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
 * [startup.c]
 *
 * This file contains the 'monitor_init' routine, where the initialization
 * of the monitor and its libraries is carried out. When 'monitor_init'
 * finishes its execution, the monitor is initialized on all cores.
 *
 * @(#) $Id: startup.c 587 2013-11-17 15:51:22Z ertl-honda $    
 */
#include "types.h"     /* uint32_t */
#include "target.h"    /* TARGET_xxx, target_xxx */
#include "arm.h"       /* CP15_XXX */
#include "monitor.h"   /* MON_T_SCR, MON_NSACR */
#include "syscalls.h"  /* syscalls_init */
#include "notifier.h"  /* notifier_init */

/************************************************************************
 * [Initialization Barriers]                                            *
 * Barrier to make sure all cores have initialized before jumping to T. *
 ************************************************************************/
#if TARGET_NUM_CORES > 1

#define MAGIC_INIT_NUMBER 0xDEADBEEF
static volatile uint32_t percore_init_flag[TARGET_NUM_CORES];
static volatile uint32_t all_cores_init_flag[TARGET_NUM_CORES];
static volatile uint32_t current_phase;

/*
 * First barrier is based on a MAGIC NUMBER
 */
static void initialization_barrier(uint32_t core_id)
{
    volatile uint32_t i, j;
    volatile uint32_t flag;

    percore_init_flag[core_id] = MAGIC_INIT_NUMBER;

    if (core_id == TARGET_PRIMARY_CORE_ID) {
        do {
            /* check if all cores have arrived */
            flag = 0;
            for (i = 0; i < TARGET_NUM_CORES; i++)
                if (percore_init_flag[i] == MAGIC_INIT_NUMBER) flag++;

            /* loop to reduce pressure on the memory system */
            for (j = 0; j < 100; j++);
        } while (flag < TARGET_NUM_CORES);

        /* all cores are initialized */
        current_phase = 0;
        for (i = 0; i < TARGET_NUM_CORES; i++) {
            all_cores_init_flag[i] = MAGIC_INIT_NUMBER;
        }
    } else {
        while (all_cores_init_flag[core_id] != MAGIC_INIT_NUMBER)
            for (j = 0; j < 100; j++);
    }

    /* clean up the initialization flags for the next boot */
    percore_init_flag[core_id]   = 0;
    all_cores_init_flag[core_id] = 0;
}

/*
 *  Next barriers are based on a phase number (phase: 1..N)
 */
static void phase_barrier(uint32_t core_id, uint32_t phase)
{
    volatile uint32_t i, j;
    volatile uint32_t flag;

    percore_init_flag[core_id] = phase;

    if (core_id == TARGET_PRIMARY_CORE_ID) {
        do {
            /* check if all cores have arrived */
            flag = 0;
            for(i = 0; i < TARGET_NUM_CORES; i++)
                if(percore_init_flag[i] == phase) flag++;

            /* loop to reduce pressure on the memory system */
            for(j = 0; j < 100; j++);
        } while (flag < TARGET_NUM_CORES);

        /* all cores have arrived */
        current_phase = phase;
    } else {
        while(current_phase != phase)
            for(j = 0; j < 100; j++);
    }
}
#else /* TARGET_NUM_CORES == 1 */
#define initialization_barrier(x)
#define phase_barrier(x, y)
#endif /* TARGET_NUM_CORES > 1 */

/**********************************************************************
 * [Monitor Initialization]                                           *
 * Initialize the monitor environment, target hardware and libraries. *
 **********************************************************************/
extern uint32_t monitor_vector_table;

void monitor_init(uint32_t core_id)
{

#if TARGET_NUM_CORES > 1
    /* Target-dependent master initialization */
    if (core_id == TARGET_PRIMARY_CORE_ID) {
        target_primary_core_init();
    }
#endif /* TARGET_NUM_CORES > 1 */

    /* Barrier: .bss and .data sections is initialization */
    initialization_barrier(core_id);

    /* Set up the Monitor Vector Base Address Register (MVBAR) */
    CP15_MVBAR_WRITE(&monitor_vector_table);

    /* Set up the Secure Configuration Register (SCR) for the T OS */
    CP15_SCR_WRITE(MON_T_SCR);

    /* Set up the Non-Secure Access Control Register (NSACR) */
    CP15_NSACR_WRITE(MON_NSACR);

    /* Target-dependent initialization */
    target_init(core_id);

    /* Barrier: target-dependent initialization */
    phase_barrier(core_id, 1);

    /* Initialize libraries in the primary core only */
    if (core_id == TARGET_PRIMARY_CORE_ID) {
        syscalls_init();
        notifier_init();
    }

    /* Barrier: monitor initialization is finished */
    phase_barrier(core_id, 2);
}
