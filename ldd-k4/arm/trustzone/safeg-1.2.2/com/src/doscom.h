/*
 *  TOPPERS/SafeG Dual-OS monitor
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Safety Gate Dual-OS monitor
 *
 *  Copyright (C) 2012-2015 by Embedded and Real-Time Systems Laboratory
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
 *  @(#) $Id: doscom.h 806 2015-09-24 13:42:11Z ertl-honda $
 */

#ifndef DOSCOM_H
#define DOSCOM_H

#include "doscom_target.h"

/*
 * ERRORS
 */
#define DOSCOM_SUCCESS    (0)  // success
#define DOSCOM_NOINIT    (-1)  // dualos communications system not initialized yet.
#define DOSCOM_EPARAM    (-2)  // incorrect parameter.
#define DOSCOM_FILTER    (-3)  // the block was discarded
#define DOSCOM_EALLOC    (-4)  // the block is not allocated.
#define DOSCOM_TIMEOUT   (-5)  // a timeout occurred
#define DOSCOM_LOCKED    (-6)  // the shmem is locked
#define DOSCOM_UNLOCKED  (-7)  // the shmem is unlocked
#define DOSCOM_ESREGION  (-8)  // the shared regison initialize error
#define DOSCOM_RESTART   (-9)  // restart fifoch
#define DOSCOM_ECTX      (-10) // context error
#define DOSCOM_EASSIGN   (-11) // the fifo channel is assigned other process
#define DOSCOM_EMPTY     (-12) // no block is enqueued.
#define DOSCOM_FULL      (-13) // no block is in buffer.
#define DOSCOM_WAITOVER  (-14) // an other task is waitting for the channel
#define DOSCOM_OPROCESS  (-15) // number of connected process is over
#define DOSCOM_EINIT     (-16) // NT_OS calls init before T_OS calls init


/*
 * TYPES
 */
typedef int32_t doscom_id_t;
typedef int32_t doscom_time_t;
typedef bool_t (*doscom_filter_t)(void *buffer, uint32_t size);
typedef int32_t doscom_mask_t;

/*
 * MACRO
 */
#define NULL_FILTER			(-1)
#define DEFAULT_FILTER		(-2)


/*
 *  API for Global
 */

/*
 * doscom_init
 *
 * Initializates the Dual-OS Communications system.
 *
 * Errors: DOSCOM_EASSIGN, DOSCOM_TIMEOUT, DOSCOM_SHREGION, DOSCOM_ECTX,
 *         DOSCOM_EPARAM, DOSCOM_OPROCESS
 *
 */
extern int doscom_init(doscom_mask_t fifoch_mask, doscom_time_t timeout);

/*
 * doscom_term
 *
 * Terminate the Dual-OS Communications system.
 *
 * Errors: DOSCOM_NOINIT
 *
 */
extern int doscom_term(void);

/*
 * doscom_sense_process
 *
 * Sense number of process(in T_OS) that open /dev/safeg.
 *
 * Errors: DOSCOM_NOINIT
 */
extern int doscom_sense_process(uint32_t *p_num_process);

/*
 * doscom_sense_fifoch
 *
 * Sense FIFO Channel status
 *
 * Errors: DOSCOM_NOINIT  
 */
extern int doscom_sense_fifoch(doscom_mask_t *p_fifoch_mask);

/*
 *  API for FIFO CHANNEL
 */

/*
 * doscom_fifoch_alloc
 *
 * Allocates a block from a fifo channel block pool and get a pointer to
 * the beginning of the memory region of the allocated block.
 * This function never blocks the calling task or process.
 *
 * Errors: DOSCOM_NOINIT, DOSCOM_EPARAM, DOSCOM_EASSIGN, DOSCOM_EALLOC
 *
 */
extern int doscom_fifoch_alloc(const doscom_id_t fifoch_id,
								doscom_id_t *p_block_id);

/*
 * doscom_fifoch_free
 *
 * Releases a block back to the fifo channel block pool where it belongs.
 *
 * Errors: DOSCOM_NOINIT, DOSCOM_EPARAM, DOSCOM_EASSIGN, DOSCOM_EALLOC
 *
 */
extern int doscom_fifoch_free(const doscom_id_t fifoch_id,
								const doscom_id_t block_id);

/*
 * doscom_fifoch_get
 *
 * To obtain a pointer to the beginning of the memory region of a block.
 *
 * Errors: DOSCOM_NOINIT, DOSCOM_EPARAM, DOSCOM_EALLOC
 *
 */
extern int doscom_fifoch_get(const doscom_id_t fifoch_id,
								const doscom_id_t block_id, void **p_block);

/*
 * doscom_fifoch_enqueue
 *
 * Enqueues a block to a fifo channel FIFO. Note that the fifo channel is implicit in
 * the block identifier.
 *
 * Errors: DOSCOM_NOINIT, DOSCOM_EPARAM, DOSCOM_EASSIGN, DOSCOM_FILTER, DOSCOM_EALLOC
 *
 */
extern int doscom_fifoch_enqueue(const doscom_id_t fifoch_id,
									const doscom_id_t block_id);

/*
 * doscom_fifoch_dequeue
 *
 * Dequeues a block from a fifo channel FIFO. This function never blocks the
 * calling task or process.
 *
 * Errors: DOSCOM_NOINIT, DOSCOM_EPARAM, DOSCOM_EASSIGN, DOSCOM_EALLOC
 *
 */
extern int doscom_fifoch_dequeue(const doscom_id_t fifoch_id,
									doscom_id_t *p_block_id);

/*
 * doscom_fifoch_notify
 *
 * Sends an asynchronous fifo channel event notification. If a notification
 * was already sent and has not been acknowledged yet by the receiver, the
 * function will return as if the notification was sent successfully.
 *
 * Errors: DOSCOM_NOINIT, DOSCOM_EPARAM, DOSCOM_EASSIGN
 *
 */
extern int doscom_fifoch_notify(const doscom_id_t fifoch_id);

/*
 * doscom_fifoch_wait
 *
 * This function makes the calling task - or process - wait for an
 * asynchronous event notification on a specific fifo channel. If an event was
 * pending, the function acknowledges it and returns immediately.
 * Otherwise, the calling thread is put in waiting state until an event
 * arrives or a timeout occurs. The units in which the timeout is
 * expressed are implementation-dependent.
 *
 * Errors: DOSCOM_NOINIT, DOSCOM_EPARAM, DOSCOM_EASSIGN, DOSCOM_TIMEOUT,
 *         DOSCOM_ECTX, DOSCOM_RESTART
 *
 */
extern int doscom_fifoch_wait(const doscom_id_t fifoch_id,
								const doscom_time_t timeout);

/*
 * doscom_filter_set
 *
 * Used by the receiver to set a communications filter.
 *
 * Errors: DOSCOM_NOINIT, DOSCOM_EPARAM, DOSCOM_EASSIGN
 *  
 */
extern int doscom_filter_set(const doscom_id_t fifoch_id,
								const doscom_id_t  filter_id);

/*
 * doscom_fifoch_init
 *
 * Initialize a fifoch channel.
 * 
 * Errors: DOSCOM_NOINIT, DOSCOM_EPARAM, DOSCOM_EASSIGN
 *  
 */
extern int doscom_fifoch_init(const doscom_id_t fifoch_id);

/*
 *  API for SMEM CHANNEL
 */

/*
 * doscom_smemch_get
 *
 * To obtain a pointer to the beginning of the memory region of a smem channel.
 *
 * Errors: DOSCOM_NOINIT, DOSCOM_EPARAM
 *
 */
extern int doscom_smemch_get(const doscom_id_t smemch_id, void **p_smem);

/*
 * doscom_smemch_trylock
 *
 * Try to get lock of a smem channel.
 *
 * Errors: DOSCOM_NOINIT, DOSCOM_EPARAM, DOSCOM_LOCKED
 *
 */
extern int doscom_smemch_trylock(const doscom_id_t smemch_id);

/*
 * doscom_smemch_unlock
 *
 * Release lock of a smem.
 *
 * Errors: DOSCOM_NOINIT, DOSCOM_EPARAM, DOSCOM_UNLOCKED
 */
extern int doscom_smemch_unlock(const doscom_id_t smemch_id);

#endif /* DOSCOM_H */
