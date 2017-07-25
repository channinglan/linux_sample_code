/*
 *  TOPPERS/FMP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Flexible MultiProcessor Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2004-2010 by Embedded and Real-Time Systems Laboratory
 *              Graduate School of Information Science, Nagoya Univ., JAPAN
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
 *  @(#) $Id: sample1.c 786 2015-09-13 03:58:42Z ertl-honda $
 */
#include <kernel.h>
#include <t_syslog.h>
#include <t_stdlib.h>
#include <sil.h>
#include "syssvc/serial.h"
#include "syssvc/syslog.h"
#include "kernel_cfg.h"
#include "sample1.h"
#include "syscalls_api.h"
#include "syssvc/safeg_load.h"
#include "target_common.h"

/*
 *  �����ӥ�������Υ��顼�Υ�����
 */
Inline void
svc_perror(const char *file, int_t line, const char *expr, ER ercd)
{
	if (ercd < 0) {
		t_perror(LOG_ERROR, file, line, expr, ercd);
	}
}

#define	SVC_PERROR(expr)	svc_perror(__FILE__, __LINE__, #expr, (expr))

#ifdef TOPPERS_SAFEG_SECURE
#define WORLD_CHAR "T "
#else /* !TOPPERS_SAFEG_SECURE */
#define WORLD_CHAR "NT"
#endif /* TOPPERS_SAFEG_SECURE */

#ifdef TOPPERS_SAFEG_SECURE
extern int  target_get_nt_start_address();
void btask(intptr_t exinf)
{
	uint32_t ret;
	uint32_t nt_start_address;

	syslog(LOG_NOTICE, "BTASK for Core%d Starts", exinf);

	while((nt_start_address = target_get_nt_start_address()) == -1);

	syslog(LOG_NOTICE, "%d : SafeG First switch to NT : address 0x%x", exinf, nt_start_address);
	ret = safeg_syscall_setntpc(nt_start_address);

	if (ret != SAFEG_SYSCALL_ERROR__OK) {
		syslog(LOG_NOTICE, "%d : SafeG ERROR: setntpc %u", exinf, ret);
		return;
	}

	while(1) {
		SIL_PRE_LOC;

		SIL_LOC_INT();
		safeg_load_idle();
		ret = safeg_syscall_switch();
		SIL_UNL_INT();

		if (ret != SAFEG_SYSCALL_ERROR__OK) {
			syslog(LOG_NOTICE, "%d : SafeG ERROR: switch %u", exinf, ret);
			break;
		}
	}
}
#endif /* TOPPERS_SAFEG_SECURE */

const uint32_t mem_offset[2][4] = {
	{
		0x10,
		0x20,
		0x30,
		0x40,
	},
	{
		0x50,
		0x60,
		0x70,
		0x80,
	},
};

/*
 * Periodic task
 */
void smem_task(intptr_t exinf)
{
	static int cnt[TNUM_PRCID] = {0, 0, 0, 0};
	int nt;
	int write_data;
	int data[4];
	int i;

	cnt[exinf-1]++;

#ifdef TOPPERS_SAFEG_SECURE
#define WORLD_CHAR "T "
	nt = 0;
	write_data = cnt[exinf-1];
#else /* !TOPPERS_SAFEG_SECURE */
#define WORLD_CHAR "NT"
	nt = 1;
	write_data = cnt[exinf-1]|0x80000000;
#endif /* TOPPERS_SAFEG_SECURE */

	*((volatile unsigned int *)(TARGET_COM_SHMEM_ADDRESS + mem_offset[nt][exinf-1])) = write_data;

	syslog(LOG_EMERG, "PE%d : "WORLD_CHAR" : Write Memory 0x%08x to 0x%08x", exinf,
		   (TARGET_COM_SHMEM_ADDRESS + mem_offset[nt][exinf-1]), write_data);

	for(i = 0; i < 4; i++) {
		data[i] = *((volatile unsigned int *)(TARGET_COM_SHMEM_ADDRESS + mem_offset[0][i]));
	}
	syslog(LOG_NOTICE, "PE%d : "WORLD_CHAR" : Read Memory for T,  0x%08x, 0x%08x, 0x%08x, 0x%08x",
		   exinf,  data[0], data[1], data[2], data[3]);

	for(i = 0; i < 4; i++) {
		data[i] = *((volatile unsigned int *)(TARGET_COM_SHMEM_ADDRESS + mem_offset[1][i]));
	}
	syslog(LOG_NOTICE, "PE%d : "WORLD_CHAR" : Read Memory for NT, 0x%08x, 0x%08x, 0x%08x, 0x%08x",
		   exinf, data[0], data[1], data[2], data[3]);
}

uint_t const task_tskid[TNUM_PRCID] = {
	TASK1,
#if TNUM_PRCID >= 2
	TASK2,
#endif /* TNUM_PRCID >= 2 */
#if TNUM_PRCID >= 3
	TASK3,
#endif /* TNUM_PRCID >= 3 */
#if TNUM_PRCID >= 4
	TASK4,
#endif /* TNUM_PRCID >= 4 */
};

/*
 * Cyclic handler
 */
void cyclic_handler(intptr_t exinf)
{
	SVC_PERROR(iact_tsk(TASK1));
#if TNUM_PRCID >= 2
	SVC_PERROR(iact_tsk(TASK2));
#endif /* TNUM_PRCID >= 2 */
#if TNUM_PRCID >= 3
	SVC_PERROR(iact_tsk(TASK3));
#endif /* TNUM_PRCID >= 3 */
#if TNUM_PRCID >= 4
	SVC_PERROR(iact_tsk(TASK4));
#endif /* TNUM_PRCID >= 4 */
}

/*
 *  �ᥤ�󥿥���
 */
void main_task(intptr_t exinf)
{
	ER_UINT ercd;

	SVC_PERROR(syslog_msk_log(0, LOG_UPTO(LOG_DEBUG)));
	syslog(LOG_NOTICE, "Simple starts (exinf = %d).", (int_t) exinf);

	ercd = serial_opn_por(TASK_PORTID_G_SYSLOG);
	if (ercd < 0 && MERCD(ercd) != E_OBJ) {
		syslog(LOG_ERROR, "%s (%d) reported by `serial_opn_por'.",
			   itron_strerror(ercd), SERCD(ercd));
	}
	SVC_PERROR(serial_ctl_por(TASK_PORTID_G_SYSLOG,
							  (IOCTL_CRLF | IOCTL_FCSND | IOCTL_FCRCV)));

#ifdef TOPPERS_SAFEG_SECURE
	syslog(LOG_NOTICE, "Hello T-FMP");
#else /* TOPPERS_SAFEG_SECURE */
	syslog(LOG_NOTICE, "Hello NT-FMP");
#endif /* TOPPERS_SAFEG_SECURE */

	SVC_PERROR(sta_cyc(CYCHDR1)); // start the cyclic handler

	syslog(LOG_NOTICE, "Main goes to sleep.");
	SVC_PERROR(slp_tsk());
	SVC_PERROR(ext_ker());
	assert(0);
}
