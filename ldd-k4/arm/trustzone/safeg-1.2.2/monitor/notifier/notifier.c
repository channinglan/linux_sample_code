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
 * [notifier.c]
 *
 * This file contains the implementation of the notifier module. T-OS
 * tasks can register a function to a notifier chain by using the
 * NOTREG system call, which in turn calls 'notifier_register'. During
 * the execution of 'monitor.S', events are raised (e.g., notify_xxx).
 * Those events are forwarded to 'notifier_call' which executes one by
 * one all functions registered by the T-OS on the corresponding chain.
 *
 * @(#) $Id: notifier.c 587 2013-11-17 15:51:22Z ertl-honda $
 */
#include "types.h"     /* uint32_t */
#include "utils.h"     /* UNUSED */
#include "error.h"     /* ERROR__XXX */
#include "debug.h"      /* DEBUG */
#include "notifier.h"  /* struct notifier_user */
#include "arm.h"       /* ARM_DATA_MEMORY_BARRIER */

#if (MONITOR_ENABLE_NOTIFIERS == 1)

/****************************************************
 * [Notifier data]                                  *
 * Notifier internal data structures and constants. *
 ****************************************************/
/*
 * Notifier callback
 */
struct notifier_block {
    int   is_used;
    uint32_t (*function)(uint32_t core_id,
                         uint32_t ns,
                         uint32_t event_id,
                         void     *notifier_params,
                         void     *user_params);
    void  *user_params;
    struct notifier_block *next;
};

/*
 * Possible return values for notifier callbacks
 */
#define NOTIFIER_CONTINUE 0  /* continue with next calls in the chain*/
#define NOTIFIER_STOP     1  /* stop executing next calls in the chain */

/*
 * Structure containing all notifier chains [fiq,irq,smc,panic].
 */
static struct notifier_block *the_notifier_chains[4];

/*
 * Table containing notifier blocks
 */
static struct notifier_block the_notifiers[MONITOR_MAX_NOTIFIERS];

/****************************************************************
 * [Notifier initialization]                                    *
 * Initialize the notifier library. Called from monitor_init.c. *
 ****************************************************************/
void notifier_init(void)
{
#if (MONITOR_OMIT_BSS_INIT == 1)
    /* TODO: make a memset util */
    uint32_t *tmp = (uint32_t *)the_notifier_chains;
    uint32_t size_u32_words = sizeof(the_notifier_chains) / 4;
    while (size_u32_words-- > 0) *tmp = 0;

    tmp = (uint32_t *)the_notifiers;
    uint32_t size_u32_words = sizeof(the_notifiers) / 4;
    while (size_u32_words-- > 0) *tmp = 0;
#endif /* (MONITOR_OMIT_BSS_INIT == 1) */
}

/********************************************************************
 * [Notifier callback registration]                                 *
 * Register a callback to receive event notifications from a chain. *
 ********************************************************************/
/*
 * Generic function to register a callback to a notifier chain.
 */
static void notifier_register_in_chain(uint32_t chain_id,
                                       struct notifier_block *block)
{
    if (the_notifier_chains[chain_id] == NULL) {
        block->next = NULL;
        ARM_DATA_MEMORY_BARRIER;
        the_notifier_chains[chain_id] = block;
    } else {
        block->next = the_notifier_chains[chain_id];
        ARM_DATA_MEMORY_BARRIER;
        the_notifier_chains[chain_id] = block;
    }
}

/*
 * Register a notifier.
 */
uint32_t notifier_register(const struct notifier_user *notifier,
                           uint32_t *id)
{
    uint32_t tmp_id;

    DEBUG(MONITOR_DEBUG_NOTIFIERS, "notifier_register\n\r");

    /* Get a free notifier id */
    for(tmp_id=0; tmp_id<MONITOR_MAX_NOTIFIERS; tmp_id++) {
        if (!the_notifiers[tmp_id].is_used) break;
    }

    /* Return error if no slots available */
    if (tmp_id == MONITOR_MAX_NOTIFIERS) return ERROR__FULL;

    /* Copy the notifier information */
    the_notifiers[tmp_id].is_used     = true;
    the_notifiers[tmp_id].function    = notifier->function;
    the_notifiers[tmp_id].user_params = notifier->user_params;

    /* From here it can be called by any core */
    notifier_register_in_chain(notifier->chain_id, &the_notifiers[tmp_id]);

    /* Set the id */
    *id = tmp_id;

    return ERROR__OK;
}

/*************************************************
 * [Notifier calls]                              *
 * Send notification events to a notifier chain. *
 *************************************************/
/*
 * Generic function to send an event to a notifier chain.
 */
void notifier_call(uint32_t chain_id,
                   uint32_t event_id,
                   void *notifier_params)
{
    uint32_t ns, core_id, ret;
    struct notifier_block *block = the_notifier_chains[chain_id];

    DEBUG(MONITOR_DEBUG_NOTIFIERS, "notifier_call\n\r");

    /* get which world the request came from */
    CP15_SCR_READ(ns);
    ns = ns & SCR_NS;

    /* get which core are we */
    core_id = arm_get_core_id();

    /* call all functions registered to the chain */
    while(block) {
        ret = block->function(core_id,
                              ns,
                              event_id,
                              notifier_params,
                              block->user_params);
        if (ret == NOTIFIER_STOP) return;
        block = block->next;
    }
}

#else /* (MONITOR_ENABLE_NOTIFIERS == 0) */

uint32_t notifier_register(const struct notifier_user *UNUSED(notifier),
                           uint32_t *UNUSED(id))
{
    return ERROR__INEXISTENT;
}

#endif /* (MONITOR_ENABLE_NOTIFIERS == 0) */
