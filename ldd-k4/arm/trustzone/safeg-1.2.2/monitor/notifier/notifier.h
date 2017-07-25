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
 *  [notifier.h]
 *
 * This file contains the interface of the notifier module. The ID number
 * for each notification chain and the events that can be sent through
 * each chain are defined.
 *
 * @(#) $Id: notifier.h 587 2013-11-17 15:51:22Z ertl-honda $  
 */
#ifndef _SAFEG_MONITOR_NOTIFIER_H_
#define _SAFEG_MONITOR_NOTIFIER_H_

/*****************************************************
 * [Notifier channel and event IDs]                  *
 * Definitions of all notifier chains and event IDs. *
 *****************************************************/
#define NOTIFIER_CHAIN_ID_FIQ    (0)
#define     NOTIFIER_EVENT_ID_FIQ_ENTER             (0)
#define     NOTIFIER_EVENT_ID_FIQ_T_EXIT            (1)
#define     NOTIFIER_EVENT_ID_FIQ_NT_EXIT           (2)
#define NOTIFIER_CHAIN_ID_SMC    (1)
#define     NOTIFIER_EVENT_ID_SMC_ENTER             (0)
#define     NOTIFIER_EVENT_ID_SMC_DONT_SWITCH_EXIT  (1)
#define     NOTIFIER_EVENT_ID_SMC_SWITCH_TO_NT_EXIT (2)
#define     NOTIFIER_EVENT_ID_SMC_SWITCH_TO_T_EXIT  (3)
#define NOTIFIER_CHAIN_ID_IRQ    (2)
#define     NOTIFIER_EVENT_ID_IRQ_ENTER             (0)
#define     NOTIFIER_EVENT_ID_IRQ_NT_EXIT           (1)
#define NOTIFIER_CHAIN_ID_PANIC  (3)
#define     NOTIFIER_EVENT_ID_PANIC_IRQ_IN_T        (0)
#define     NOTIFIER_EVENT_ID_PANIC_IRQ_IN_NT       (1)
#define     NOTIFIER_EVENT_ID_PANIC_PREFETCH        (2)
#define     NOTIFIER_EVENT_ID_PANIC_DATA            (3)

#ifndef __ASSEMBLER__

/****************************************************************
 * [Notifier initialization]                                    *
 * Initialize the notifier library. Called from monitor_init.c. *
 ****************************************************************/
#if (MONITOR_ENABLE_NOTIFIERS == 1)
extern void notifier_init(void);
#else /* (MONITOR_ENABLE_NOTIFIERS == 0) */
#define notifier_init()
#endif /* (MONITOR_ENABLE_NOTIFIERS == 0) */

/********************************************************************
 * [Notifier callback registration]                                 *
 * Register a callback to receive event notifications from a chain. *
 ********************************************************************/
/*
 * Structure for registering a new notifier.
 */
struct notifier_user {
    uint32_t chain_id;
    uint32_t (*function)(uint32_t core_id,
                         uint32_t ns,
                         uint32_t event_id,
                         void     *notifier_params,
                         void     *user_params);
    void  *user_params;
};

/*
 * Register a notifier.
 */
extern uint32_t notifier_register(const struct notifier_user *notifier,
                                  uint32_t *id);

/*************************************************
 * [Notifier calls]                              *
 * Send notification events to a notifier chain. *
 *************************************************/
/*
 * Parameters for the NOTIFIER_EVENT_ID_SMC_ENTER call.
 */
struct notifier_smc_enter_params {
    uint32_t id;
    uint32_t a;
    uint32_t b;
    uint32_t c;
};

/*
 * Generic function to send an event to a notifier chain.
 */
#if (MONITOR_ENABLE_NOTIFIERS == 1)
extern void notifier_call(uint32_t chain_id,
                          uint32_t event_id,
                          void *notifier_params);
#else  /* (MONITOR_ENABLE_NOTIFIERS == 0) */
#define notifier_call(x,y,z)
#endif /* (MONITOR_ENABLE_NOTIFIERS == 0) */

#endif /* __ASSEMBLER__ */
#endif /* _SAFEG_MONITOR_NOTIFIER_H_ */
