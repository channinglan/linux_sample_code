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
 *  @(#) $Id: doscom_kernel.h 806 2015-09-24 13:42:11Z ertl-honda $
 */

#ifndef DOSCOM_KERNEL_H
#define DOSCOM_KERNEL_H

#include <kernel.h>
#include <t_syslog.h>
#include <t_stddef.h> /* uint32_t, bool_t */
#include "t_os-doscom_config.h"
#include "kernel_cfg.h"

#define DOSCOM_T

#define COPY_UPDATE_ALGORITHM_ENABLE

#define DEBUG(enable,x,args...) \
			if(enable) syslog(LOG_NOTICE, "DEBUG (%s): " x, __func__ , ##args);

#define PRINT(x, args...) \
			syslog(LOG_NOTICE, x, ##args);

/*
 * TASK EXC ARG
 */
#define TASK_EXC_ARG_PROCESSEXIT 1
#define TASK_EXC_ARG_TERM        2

/*
 * KERNEL INTERFACE
 */
extern int __doscom_kernel_init(doscom_id_t fifoch_id, doscom_time_t timeout, void *sregion);
extern int __doscom_kernel_term(void);
extern int __doscom_kernel_sregion_init(void **sregion);
extern int __doscom_kernel_notify(doscom_id_t fifoch_id);
extern int __doscom_kernel_wait(doscom_id_t fifoch_id, doscom_time_t timeout);
extern int __doscom_kernel_get_fifoch_mask(doscom_mask_t *p_fifoch_mask);
extern int __doscom_kernel_fifoch_init(const doscom_id_t fifoch_id);
extern int __doscom_kernel_get_num_process(uint32_t *p_num_process);

static inline bool_t
__doscom_kernel_check_tskctx()
{
	if (sns_ctx() || sns_dsp() || sns_loc()) {
		return false;
	}
	return true;
}

#if TKERNEL_PRID == 0x0007
/*
 *  For ASP Kernel
 */
static inline void
__doscom_kernel_lock()
{
	if(sns_ctx()) {
		iloc_cpu();
	}
	else {
		loc_cpu();
	}
}

static inline void
__doscom_kernel_unlock()
{
	if(sns_ctx()) {
		iunl_cpu();
	}
	else {
		unl_cpu();
	}
}

#endif /* TKERNEL_PRID == 0x0007 */

#if TKERNEL_PRID == 0x0008
/*
 *  For FMP Kernel
 */
static inline void
__doscom_kernel_lock()
{
	if(sns_ctx()) {
		iloc_spn(DOSCOM_SPN);
	}
	else {
		loc_spn(DOSCOM_SPN);
	}
}

static inline void
__doscom_kernel_unlock()
{
	if(sns_ctx()) {
		iunl_spn(DOSCOM_SPN);
	}
	else {
		unl_spn(DOSCOM_SPN);
	}
}
#endif /* TKERNEL_PRID == 0x0008 */

#endif /* DOSCOM_KERNEL_H */
