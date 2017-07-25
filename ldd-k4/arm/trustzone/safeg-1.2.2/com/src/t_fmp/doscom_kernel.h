/*
 *  TOPPERS/SafeG Dual-OS monitor
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Safety Gate Dual-OS monitor
 *
 *  Copyright (C) 2012-2015 by Embedded and Real-Time Systems Laboratory
 *     Graduate School of Information Science, Nagoya University, JAPAN
 *
 *  �嵭����Ԥϡ��ʲ���(1)��(4)�ξ������������˸¤ꡤ�ܥ��եȥ���
 *  �����ܥ��եȥ���������Ѥ�����Τ�ޤࡥ�ʲ�Ʊ���ˤ���ѡ�ʣ������
 *  �ѡ������ۡʰʲ������ѤȸƤ֡ˤ��뤳�Ȥ�̵���ǵ������롥
 *  (1) �ܥ��եȥ������򥽡��������ɤη������Ѥ�����ˤϡ��嵭������
 *      ��ɽ�����������Ѿ�浪��Ӳ�����̵�ݾڵ��꤬�����Τޤޤη��ǥ���
 *      ����������˴ޤޤ�Ƥ��뤳�ȡ�
 *  (2) �ܥ��եȥ������򡤥饤�֥������ʤɡ�¾�Υ��եȥ�������ȯ�˻�
 *      �ѤǤ�����Ǻ����ۤ�����ˤϡ������ۤ�ȼ���ɥ�����ȡ�����
 *      �ԥޥ˥奢��ʤɡˤˡ��嵭�����ɽ�����������Ѿ�浪��Ӳ���
 *      ��̵�ݾڵ����Ǻܤ��뤳�ȡ�
 *  (3) �ܥ��եȥ������򡤵�����Ȥ߹���ʤɡ�¾�Υ��եȥ�������ȯ�˻�
 *      �ѤǤ��ʤ����Ǻ����ۤ�����ˤϡ����Τ����줫�ξ�����������
 *      �ȡ�
 *    (a) �����ۤ�ȼ���ɥ�����ȡ����Ѽԥޥ˥奢��ʤɡˤˡ��嵭����
 *        �ɽ�����������Ѿ�浪��Ӳ�����̵�ݾڵ����Ǻܤ��뤳�ȡ�
 *    (b) �����ۤη��֤��̤�������ˡ�ˤ�äơ�TOPPERS�ץ������Ȥ�
 *        ��𤹤뤳�ȡ�
 *  (4) �ܥ��եȥ����������Ѥˤ��ľ��Ū�ޤ��ϴ���Ū�������뤤���ʤ�»
 *      ������⡤�嵭����Ԥ����TOPPERS�ץ������Ȥ����դ��뤳�ȡ�
 *      �ޤ����ܥ��եȥ������Υ桼���ޤ��ϥ���ɥ桼������Τ����ʤ���
 *      ͳ�˴�Ť����ᤫ��⡤�嵭����Ԥ����TOPPERS�ץ������Ȥ�
 *      ���դ��뤳�ȡ�
 *
 *  �ܥ��եȥ������ϡ�̵�ݾڤ��󶡤���Ƥ����ΤǤ��롥�嵭����Ԥ�
 *  ���TOPPERS�ץ������Ȥϡ��ܥ��եȥ������˴ؤ��ơ�����λ�����Ū
 *  ���Ф���Ŭ������ޤ�ơ������ʤ��ݾڤ�Ԥ�ʤ����ޤ����ܥ��եȥ���
 *  �������Ѥˤ��ľ��Ū�ޤ��ϴ���Ū�������������ʤ�»���˴ؤ��Ƥ⡤��
 *  ����Ǥ�����ʤ���
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
