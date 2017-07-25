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
 *  @(#) $Id: sample1.c 782 2015-09-11 05:30:24Z ertl-honda $
 */

/* 
 *  ����ץ�ץ����(1)������
 *
 *  FMP�����ͥ�δ���Ū��ư����ǧ���뤿��Υ���ץ�ץ���ࡥ
 *
 *  �ץ����γ���:
 *
 *  �桼�����󥿥ե�������������ĥᥤ�󥿥����ʥ�����ID: MAIN_TASK��ͥ
 *  ����: MAIN_PRIORITY�ˤȡ�3�Ĥ�����¹Ԥ���륿�����ʥ�����ID:
 *  TASK1��TASK3�����ͥ����: MID_PRIORITY�ˤǹ�������롥�ޤ�����ư��
 *  ����2�äμ����ϥ�ɥ�ʼ����ϥ�ɥ�ID: CYCHDR1�ˤ��Ѥ��롥
 *
 *  ����¹Ԥ���륿�����ϡ�task_loop����롼�פ�¹Ԥ����٤ˡ���������
 *  �¹���Ǥ��뤳�Ȥ򤢤�魯��å�������ɽ�����롥
 *
 *  �����ϥ�ɥ�ϡ����Ĥ�ͥ���١�HIGH_PRIORITY��MID_PRIORITY��
 *  LOW_PRIORITY�ˤΥ�ǥ����塼���ž�����롥�ץ����ε�ưľ��ϡ�
 *  �����ϥ�ɥ����߾��֤ˤʤäƤ��롥
 *
 *  �ᥤ�󥿥����ϡ����ꥢ��I/O�ݡ��Ȥ����ʸ�����Ϥ�Ԥ���ʸ�����Ϥ�
 *  �ԤäƤ���֤ϡ�����¹Ԥ���륿�������¹Ԥ���Ƥ���ˡ����Ϥ��줿
 *  ʸ�����б�����������¹Ԥ��롥���Ϥ��줿ʸ���Ƚ����δط��ϼ����̤ꡥ
 *  Control-C�ޤ���'Q'�����Ϥ����ȡ��ץ�����λ���롥
 *
 *  '1' : �оݥ�������TASK1���ڤ괹����ʽ������ˡ�
 *  '2' : �оݥ�������TASK2���ڤ괹���롥
 *  '3' : �оݥ�������TASK3���ڤ괹���롥
 *  '4' : ���饹1�˽�°���륿��������ꤹ�롥
 *        �оݼ����ϥ�ɥ��CYCHDR1���ڤ��ؤ��롥
 *        �оݥ��顼��ϥ�ɥ��ALMHDR1���ڤ��ؤ��롥
 *  '5' : ���饹2�˽�°���륿��������ꤹ�롥
 *        �оݼ����ϥ�ɥ��CYCHDR2���ڤ��ؤ��롥
 *        �оݥ��顼��ϥ�ɥ��ALMHDR2���ڤ��ؤ��롥
 *  '6' : ���饹3�˽�°���륿��������ꤹ�롥
 *        �оݼ����ϥ�ɥ��CYCHDR3���ڤ��ؤ��롥
 *        �оݥ��顼��ϥ�ɥ��ALMHDR3���ڤ��ؤ��롥
 *  '7' : ���饹4�˽�°���륿��������ꤹ�롥
 *        �оݼ����ϥ�ɥ��CYCHDR4���ڤ��ؤ��롥
 *        �оݥ��顼��ϥ�ɥ��ALMHDR4���ڤ��ؤ��롥
 *  '8' : �ץ��å�1����ꤹ�롥
 *  '9' : �ץ��å�2����ꤹ�롥
 *  '0' : �ץ��å�3����ꤹ�롥
 *  '-' : �ץ��å�4����ꤹ�롥
 *  'a' : �оݥ�������act_tsk�ˤ�굯ư���롥
 *  'A' : �оݥ��������Ф��뵯ư�׵��can_act�ˤ�ꥭ��󥻥뤹�롥
 *  'e' : �оݥ�������ext_tsk��ƤӽФ�������λ�����롥
 *  't' : �оݥ�������ter_tsk�ˤ�궯����λ���롥
 *  '>' : �оݥ�������ͥ���٤�HIGH_PRIORITY�ˤ��롥
 *  '=' : �оݥ�������ͥ���٤�MID_PRIORITY�ˤ��롥
 *  '<' : �оݥ�������ͥ���٤�LOW_PRIORITY�ˤ��롥
 *  'G' : �оݥ�������ͥ���٤�get_pri���ɤ߽Ф���
 *  's' : �оݥ�������slp_tsk��ƤӽФ����������Ԥ��ˤ����롥
 *  'S' : �оݥ�������tslp_tsk(10��)��ƤӽФ����������Ԥ��ˤ����롥
 *  'w' : �оݥ�������wup_tsk�ˤ�굯�����롥
 *  'W' : �оݥ��������Ф��뵯���׵��can_wup�ˤ�ꥭ��󥻥뤹�롥
 *  'l' : �оݥ�������rel_wai�ˤ�궯��Ū���Ԥ�����ˤ��롥
 *  'u' : �оݥ�������sus_tsk�ˤ�궯���Ԥ����֤ˤ��롥
 *  'm' : �оݥ������ζ����Ԥ����֤�rsm_tsk�ˤ�������롥
 *  'd' : �оݥ�������dly_tsk(10��)��ƤӽФ��������ַв��Ԥ��ˤ����롥
 *  'x' : �оݥ��������㳰�ѥ�����0x0001���㳰�������׵᤹�롥
 *  'X' : �оݥ��������㳰�ѥ�����0x0002���㳰�������׵᤹�롥
 *  'y' : �оݥ�������dis_tex��ƤӽФ������������㳰��ػߤ��롥
 *  'Y' : �оݥ�������ena_tex��ƤӽФ������������㳰����Ĥ��롥
 *  'r' : 3�Ĥ�ͥ���١�HIGH_PRIORITY��MID_PRIORITY��LOW_PRIORITY�ˤΥ�
 *        �ǥ����塼���ž�����롥
 *  'c' : �����ϥ�ɥ��ư��Ϥ����롥
 *  'C' : �����ϥ�ɥ��ư����ߤ����롥
 *  'I' : �����ϥ�ɥ�γ��դ��ץ��å����ѹ����롥 
 *  'b' : ���顼��ϥ�ɥ��5�ø�˵�ư����褦ư��Ϥ����롥
 *  'B' : ���顼��ϥ�ɥ��ư����ߤ����롥
 *  'E' : ���顼��ϥ�ɥ�γ��դ��ץ��å����ѹ����롥   
 *  'z' : �оݥ�������CPU�㳰��ȯ��������ʥ�������λ������ˡ�
 *  'Z' : �оݥ�������CPU��å����֤�CPU�㳰��ȯ��������ʥץ�����
 *        ��λ����ˡ�
 *  'i' : �оݥ�������mig_tsk�ˤ����դ��ץ��å����ѹ����롥 
 *  'g' : �оݥ�������mig_tsk��ƤӽФ��������դ��ץ��å����ѹ����롥 
 *  'f' : �оݥ�������mact_tsk�ˤ�굯ư���롥  
 *  'p' : �оݥ�������get_pid��ƤӽФ������ץ��å��ֹ��ɽ�������롥
 *  'V' : get_utm����ǽɾ���ѥ����ƥ�����2���ɤࡥ
 *  'v' : ȯ�Ԥ��������ƥॳ�����ɽ������ʥǥե���ȡˡ�
 *  'q' : ȯ�Ԥ��������ƥॳ�����ɽ�����ʤ���
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

extern void target_do_reboot();
extern void target_prepare_reboot();
extern int  target_get_nt_start_address();

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

#define SERVER_REQ_MIG_TSK  0x01
#define SERVER_REQ_TER_TSK  0x02

ID const server_dtqid[TNUM_PRCID] = {
    SERVER_DTQ1,
#if TNUM_PRCID >= 2
    SERVER_DTQ2,
#endif /* TNUM_PRCID >= 2 */
#if TNUM_PRCID >= 3
    SERVER_DTQ3,
#endif /* TNUM_PRCID >= 3 */
#if TNUM_PRCID >= 4
    SERVER_DTQ4,
#endif /* TNUM_PRCID >= 4 */
};

/*
 *  ������ID�Υơ��֥�
 */
uint_t const sample_tskid[TNUM_PRCID][3] = {
    {TASK1_1,TASK1_2,TASK1_3},
#if TNUM_PRCID >= 2
    {TASK2_1,TASK2_2,TASK2_3},
#endif /* TNUM_PRCID >= 2 */
#if TNUM_PRCID >= 3
    {TASK3_1,TASK3_2,TASK3_3},
#endif /* TNUM_PRCID >= 3 */
#if TNUM_PRCID >= 4
    {TASK4_1,TASK4_2,TASK4_3},
#endif /* TNUM_PRCID >= 4 */
};

/*
 *  ������NO(���󥽡���ɽ����)�Υơ��֥�
 */
uint_t const sample_tskno[TNUM_PRCID][3] = {
    {0x11,0x12,0x13},
#if TNUM_PRCID >= 2
    {0x21,0x22,0x23},
#endif /* TNUM_PRCID >= 2 */
#if TNUM_PRCID >= 3
    {0x31,0x32,0x33},
#endif /* TNUM_PRCID >= 3 */
#if TNUM_PRCID >= 4
    {0x41,0x42,0x43},
#endif /* TNUM_PRCID >= 4 */
};

#define TSKNO_TO_TSKID(tskno)  sample_tskid[(tskno >> 4) - 1][(tskno & 0xf) -1]

/*
 *  �����ϥ�ɥ�ID�Υơ��֥�
 */
uint_t const sample_cycid[TNUM_PRCID] = {
    CYCHDR1,
#if TNUM_PRCID >= 2
    CYCHDR2,
#endif /* TNUM_PRCID >= 2 */
#if TNUM_PRCID >= 3
    CYCHDR3,
#endif /* TNUM_PRCID >= 3 */
#if TNUM_PRCID >= 4
    CYCHDR4,
#endif /* TNUM_PRCID >= 4 */
};

/*
 *  ���顼��ID�Υơ��֥�
 */
uint_t const sample_almid[TNUM_PRCID] = {
    ALMHDR1,
#if TNUM_PRCID >= 2
    ALMHDR2,
#endif /* TNUM_PRCID >= 2 */
#if TNUM_PRCID >= 3
    ALMHDR3,
#endif /* TNUM_PRCID >= 3 */
#if TNUM_PRCID >= 4
    ALMHDR4,
#endif /* TNUM_PRCID >= 4 */
};

/*
 *  ���ꥢ��ݡ����ֹ�Υơ��֥�
 */
uint_t const serial_port[TNUM_PRCID] = {
#ifndef G_SYSLOG
	TASK_PORTID_PRC1,
#else /* G_SYSLOG */
	TASK_PORTID_G_SYSLOG,
#endif /* G_SYSLOG */
    
#if TNUM_PRCID >= 2
    TASK_PORTID_PRC2,
#endif /* TNUM_PRCID >= 2 */
#if TNUM_PRCID >= 3
    TASK_PORTID_PRC3,
#endif /* TNUM_PRCID >= 3 */
#if TNUM_PRCID >= 4
    TASK_PORTID_PRC4,
#endif /* TNUM_PRCID >= 4 */
};


/*
 *  SERVER������
 *  ¾�Υ�����������׵������� mig_tsk()/ter_tsk() ��¹Ԥ��롥 
 */
void server_task(intptr_t exinf)
{
	intptr_t req;
	ID		tskid;
	int_t	tskno;
	ID		prcid;
	uint8_t syscall;
	ID dtqid = server_dtqid[exinf - 1];

	syslog(LOG_NOTICE, "Server task %d starts.", (int_t) exinf);

	while(1){
		rcv_dtq(dtqid, &req);
		tskno = (req >> 16) & 0xff;
		tskid = TSKNO_TO_TSKID(tskno);
		prcid = (req >>  8) & 0xff;
		syscall = req & 0xff;

		/*
		 *  mig_tsk/ter_tsk�ϡ��оݤΥ��������ɤΥץ��å��˳���դ�����
		 *  ���뤫ʬ����ʤ����ᡤ�ᥤ�󥿥���������׵�����ƤΥ����С���
		 *  �����Ǽ¹Ԥ��뤿�ᡤ�оݥ������Ȥϰۤʤ�ץ��å��˳���դ���
		 *  ��Ƥ���ץ��å��Ǥϥ��顼�Ȥʤ롥
		 */
		if (syscall == SERVER_REQ_MIG_TSK) {
			syslog(LOG_INFO, "#Server task %d : mig_tsk(0x%x, %d)", (int_t) exinf, tskno, prcid);
			SVC_PERROR(mig_tsk(tskid, prcid));
		}
		else if (syscall == SERVER_REQ_TER_TSK) {
			syslog(LOG_INFO, "#Server task %d : ter_tsk(0x%x)", (int_t) exinf, tskno);
			SVC_PERROR(ter_tsk(tskid));
		}
	}
}

/*
 *  �¹Լ¹Ԥ���륿�����ؤΥ�å������ΰ�
 */
char	message[TNUM_PRCID][3];

/*
 *  �������Υޥ����졼�������Υץ��å��λ���
 */
ID  tsk_mig_prc;

/*
 *  �롼�ײ��
 */
ulong_t	task_loop[TNUM_PRCID];		/* ��������ǤΥ롼�ײ�� */
ulong_t	tex_loop[TNUM_PRCID];			/* �㳰�����롼������ǤΥ롼�ײ�� */

/*
 *  �¹Լ¹Ԥ���륿����
 */
void task(intptr_t exinf)
{
	volatile ulong_t	i;
	int_t		n = 0;
	int_t		tskno = (int_t) (exinf & 0xffff);
	int_t		classno = (int_t) (exinf >> 16);
	const char	*graph[] = { "|", "  +", "    *" };
	char		c;
	ID		prcid;
	ER		ercd;

	SVC_PERROR(ena_tex());
	while (1) {
		syslog(LOG_NOTICE, "task%d_%d is running (%03d).   %s",
			   classno, tskno, ++n, graph[tskno-1]);
		for (i = 0; i < task_loop[classno-1]; i++);
		c = message[classno-1][tskno-1];
		message[classno-1][tskno-1] = 0;
		switch (c) {
		case 'e':
			syslog(LOG_INFO, "#%d#ext_tsk()", tskno);
			SVC_PERROR(ext_tsk());
			assert(0);
		case 's':
			syslog(LOG_INFO, "#%d#slp_tsk()", tskno);
			SVC_PERROR(slp_tsk());
			break;
		case 'S':
			syslog(LOG_INFO, "#%d#tslp_tsk(10000)", tskno);
			SVC_PERROR(tslp_tsk(10000));
			break;
		case 'd':
			syslog(LOG_INFO, "#%d#dly_tsk(10000)", tskno);
			SVC_PERROR(dly_tsk(10000));
			break;
		case 'g':
			syslog(LOG_INFO, "#%d#mig_tsk(0, %d)", tskno, tsk_mig_prc);
			SVC_PERROR(mig_tsk(0, tsk_mig_prc));
			break;
		case 'y':
			syslog(LOG_INFO, "#%d#dis_tex()", tskno);
			SVC_PERROR(dis_tex());
			break;
		case 'Y':
			syslog(LOG_INFO, "#%d#ena_tex()", tskno);
			SVC_PERROR(ena_tex());
			break;
		case 'p':
			syslog(LOG_INFO, "#%d#get_pid()", tskno);
			SVC_PERROR(ercd = get_pid(&prcid));
			if (ercd >= 0) {
				syslog(LOG_NOTICE, "Processor id is %d", prcid);
			}			
			break;
#ifdef CPUEXC1
		case 'z':
			syslog(LOG_NOTICE, "#%d#raise CPU exception", tskno);
			RAISE_CPU_EXCEPTION;
			break;
		case 'Z':
			SVC_PERROR(loc_cpu());
			syslog(LOG_NOTICE, "#%d#raise CPU exception", tskno);
			RAISE_CPU_EXCEPTION;
			SVC_PERROR(unl_cpu());
			break;
#endif /* CPUEXC1 */
		default:
			break;
		}
	}
}

/*
 *  �¹Ԥ��Ƽ¹Ԥ���륿�����ѤΥ������㳰�����롼����
 */
void tex_routine(TEXPTN texptn, intptr_t exinf)
{
	volatile ulong_t	i;
	int_t	tskno = (int_t) (exinf & 0xffff);
	int_t	classno = (int_t) (exinf >> 16);

	syslog(LOG_NOTICE, "task%d receives exception 0x%04x.", tskno, texptn);
	for (i = 0; i < tex_loop[classno-1]; i++);

	if ((texptn & 0x8000U) != 0U) {
		syslog(LOG_INFO, "#%d#ext_tsk()", tskno);
		SVC_PERROR(ext_tsk());
		assert(0);
	}
}

/*
 *  CPU�㳰�ϥ�ɥ�
 */
#ifdef CPUEXC1

void
cpuexc_handler(void *p_excinf)
{
	ID		tskid;

	syslog(LOG_NOTICE, "CPU exception handler1 (p_excinf = %08p).", p_excinf);
	if (sns_ctx() != true) {
		syslog(LOG_WARNING,
					"sns_ctx() is not true in CPU exception handler.");
	}
	if (sns_dpn() != true) {
		syslog(LOG_WARNING,
					"sns_dpn() is not true in CPU exception handler.");
	}
	syslog(LOG_INFO, "sns_loc = %d sns_dsp = %d sns_tex = %d",
									sns_loc(), sns_dsp(), sns_tex());

	syslog(LOG_INFO, "xsns_dpn = %d xsns_xpn = %d",
									xsns_dpn(p_excinf), xsns_xpn(p_excinf));

    
	if (xsns_xpn(p_excinf)) {
		syslog(LOG_NOTICE, "Sample program ends with exception.");
		SVC_PERROR(ext_ker());
		assert(0);
	}

	SVC_PERROR(iget_tid(&tskid));
	SVC_PERROR(iras_tex(tskid, 0x8000U));
}

#endif /* CPUEXC1 */

/*
 *  �����ϥ�ɥ�
 *
 *  HIGH_PRIORITY��MID_PRIORITY��LOW_PRIORITY �γ�ͥ���٤Υ�ǥ����塼
 *  ���ž�����롥
 */
void cyclic_handler(intptr_t exinf)
{
	ID		prcid = exinf;

	syslog(LOG_NOTICE, "cyclic_handler %d start!", exinf);
	SVC_PERROR(imrot_rdq(HIGH_PRIORITY, prcid));
	SVC_PERROR(imrot_rdq(MID_PRIORITY, prcid));
	SVC_PERROR(imrot_rdq(LOW_PRIORITY, prcid));
}

/*
 *  ���顼��ϥ�ɥ�
 *
 *  HIGH_PRIORITY��MID_PRIORITY��LOW_PRIORITY �γ�ͥ���٤Υ�ǥ����塼
 *  ���ž�����롥
 */
void alarm_handler(intptr_t exinf)
{
	ID		prcid = exinf;

	syslog(LOG_NOTICE, "alarm_handler %d start!", exinf);
	SVC_PERROR(imrot_rdq(HIGH_PRIORITY, prcid));
	SVC_PERROR(imrot_rdq(MID_PRIORITY, prcid));
	SVC_PERROR(imrot_rdq(LOW_PRIORITY, prcid));
}

static uint_t counter;

/*
 *  �����Х������롼����
 */
void
global_inirtn(intptr_t exinf)
{
	counter = 1;
}

/*
 *  �����Х뽪λ�롼����
 */
void
global_terrtn(intptr_t exinf)
{
	syslog(LOG_EMERG, "global_terrtn exinf = %d, counter = %d", exinf, counter++);
}

/*
 *  �����������롼����
 */
void
local_inirtn(intptr_t exinf)
{
	syslog(LOG_NOTICE, "local_inirtn exinf = %d, counter = %d", exinf, counter++);
}

/*
 *  �����뽪λ�롼����
 */
void
local_terrtn(intptr_t exinf)
{
	syslog(LOG_NOTICE, "local_terrtn exinf = %d, counter = %d", exinf, counter++);
}

#ifdef TOPPERS_SAFEG_SECURE
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

volatile uint32_t rphase[TNUM_PRCID];

uint_t const rtask_tskid[TNUM_PRCID] = {
	RTASK1,
#if TNUM_PRCID >= 2
	RTASK2,
#endif /* TNUM_PRCID >= 2 */
#if TNUM_PRCID >= 3
	RTASK3,
#endif /* TNUM_PRCID >= 3 */
#if TNUM_PRCID >= 4
	RTASK4,
#endif /* TNUM_PRCID >= 4 */
};

void rtask(intptr_t exinf)
{
	uint32_t ret;

#if TNUM_PRCID == 1
	ret = safeg_syscall_restarnt();
	if (ret != SAFEG_SYSCALL_ERROR__OK) {
		syslog(LOG_NOTICE, "SafeG ERROR: %u", ret);
	}
	target_do_reboot();
#else /* TNUM_PRCID > 1 */
	uint32_t rphase_sum;
	int i;

	syslog(LOG_NOTICE, "RTASK for Core%d Starts", exinf);

	ret = safeg_syscall_restarnt();
	if (ret != SAFEG_SYSCALL_ERROR__OK) {
		syslog(LOG_NOTICE, "SafeG ERROR: %u", ret);
	}

	if(exinf == 1) {
		rphase[0] = 0;
		for (i = 1; i < TNUM_PRCID; i++) {
			rphase[i] = 0;
			SVC_PERROR(act_tsk(rtask_tskid[i]));
		}
		ret = safeg_syscall_restarnt();
		if (ret != SAFEG_SYSCALL_ERROR__OK) {
			syslog(LOG_NOTICE, "SafeG ERROR: %u", ret);
		}
		do {
			rphase_sum = 0;
			for (i = 1; i < TNUM_PRCID; i++) {
				rphase_sum = rphase[i];
			}
		}while(rphase_sum != TNUM_PRCID);
		target_do_reboot();
		rphase[0] = 1;
	}
	else {
		rphase[exinf-1] = 1;
		while(rphase[0] == 0);
	}
#endif /* TNUM_PRCID == 1 */
}

void cyclic_handler_s(intptr_t exinf)
{
	static int cnt1 = 0;
	static int cnt2 = 0;
	static int cnt3 = 0;
	static int cnt4 = 0;
	uint32_t trust_load;
	uint32_t nontrust_load;
  
	if(exinf == 1) {
		cnt1++;
		safeg_load_info(1, &trust_load, &nontrust_load);
		syslog(LOG_NOTICE, "Cyclic handler for PE1 %d : t_load = %d\%, nt_load = %d\%.",
			   cnt1, trust_load, nontrust_load);
	}

	if(exinf == 2) {
		cnt2++;
		safeg_load_info(2, &trust_load, &nontrust_load);
		syslog(LOG_NOTICE, "Cyclic handler for PE2 %d : t_load = %d\%, nt_load = %d\%.",
			   cnt2, trust_load, nontrust_load);
	}

	if(exinf == 3) {
		cnt3++;
		safeg_load_info(3, &trust_load, &nontrust_load);
		syslog(LOG_NOTICE, "Cyclic handler for PE3 %d : t_load = %d\%, nt_load = %d\%.",
			   cnt3, trust_load, nontrust_load);
	}

	if(exinf == 4) {
		cnt4++;
		safeg_load_info(4, &trust_load, &nontrust_load);
		syslog(LOG_NOTICE, "Cyclic handler for PE4 %d : t_load = %d\%, nt_load = %d\%.",
			   cnt4, trust_load, nontrust_load);
	}
}

#endif /* TOPPERS_SAFEG_SECURE */

/*
 *  �ᥤ�󥿥���
 */
void main_task(intptr_t exinf)
{
	char	c;
	ER_UINT	ercd;
	PRI		tskpri;
	ID		clsid = exinf;
	ID		prcid = exinf;
	ID		tskid = sample_tskid[(int_t)exinf-1][0];
	int_t	tskno = sample_tskno[(int_t)exinf-1][0];
	ID		cycid = sample_cycid[(int_t)exinf-1];
	ID		almid = sample_almid[(int_t)exinf-1];
	bool_t  update_select = true;
	uint_t  tsk_select    = 1;
	uint_t  tme_select    = exinf;
	uint_t  class_select  = exinf;
	uint_t  prc_select    = exinf;
	uint32_t server_req;
#ifdef TOPPERS_SUPPORT_GET_UTM
	SYSUTM	utime1, utime2;
#endif /* TOPPERS_SUPPORT_GET_UTM */ 
#ifndef TASK_LOOP
	volatile ulong_t	i;
	SYSTIM	stime1, stime2;
#endif /* TASK_LOOP */
#ifdef G_SYSLOG
	uint_t e;
#endif /* G_SYSLOG */

	tsk_mig_prc = 1;

	SVC_PERROR(syslog_msk_log(LOG_UPTO(LOG_INFO), LOG_UPTO(LOG_EMERG)));
	syslog(LOG_NOTICE, "Sample program starts (exinf = %d).", (int_t) exinf);

	/*
	 *  ���ꥢ��ݡ��Ȥν����
	 *
	 *  �����ƥ����������Ʊ�����ꥢ��ݡ��Ȥ�Ȥ����ʤɡ����ꥢ��
	 *  �ݡ��Ȥ������ץ�Ѥߤξ��ˤϤ�����E_OBJ���顼�ˤʤ뤬���پ��
	 *  �ʤ���
	 */
	ercd = serial_opn_por(((int_t)serial_port[exinf-1]));
	if (ercd < 0 && MERCD(ercd) != E_OBJ) {
		syslog(LOG_ERROR, "%s (%d) reported by `serial_opn_por'.",
									itron_strerror(ercd), SERCD(ercd));
	}
	SVC_PERROR(serial_ctl_por(serial_port[exinf-1],
							(IOCTL_CRLF | IOCTL_FCSND | IOCTL_FCRCV)));

	/*
	 *  �롼�ײ��������
	 *
	 *  TASK_LOOP���ޥ����������Ƥ����硤¬�ꤻ���ˡ�TASK_LOOP����
	 *  �����줿�ͤ򡤥�������ǤΥ롼�ײ���Ȥ��롥
	 *
	 *  MEASURE_TWICE���ޥ����������Ƥ����硤1���ܤ�¬���̤�Τ�
	 *  �ơ�2���ܤ�¬���̤�Ȥ���1���ܤ�¬���Ĺ��λ��֤��Ф뤿�ᡥ
	 */
#ifdef TASK_LOOP
	task_loop[(int_t)exinf-1] = TASK_LOOP;
#else /* TASK_LOOP */

#ifdef MEASURE_TWICE
	task_loop = LOOP_REF;
	SVC_PERROR(get_tim(&stime1));
	for (i = 0; i < task_loop; i++);
	SVC_PERROR(get_tim(&stime2));
#endif /* MEASURE_TWICE */

	task_loop[(int_t)exinf-1] = LOOP_REF;
	SVC_PERROR(get_tim(&stime1));
	for (i = 0; i < task_loop[(int_t)exinf-1]; i++);
	SVC_PERROR(get_tim(&stime2));
	task_loop[(int_t)exinf-1] = LOOP_REF * 400UL / (stime2 - stime1);
#endif /* TASK_LOOP */
	tex_loop[(int_t)exinf-1] = task_loop[(int_t)exinf-1] / 5;

#ifdef G_SYSLOG
	for (e = 1; e < TNUM_PRCID; e++) {
		task_loop[e] = task_loop[0];
		tex_loop[e] = tex_loop[0];
	}
#endif /* G_SYSLOG */

#ifdef TOPPERS_SAFEG_SECURE
	syslog(LOG_NOTICE, "Hello T-FMP");
	/* �����ϥ�ɥ�ε�ư */
	SVC_PERROR(sta_cyc(CYCHDR_S1)); // start the cyclic handler
#if TNUM_PRCID >= 2
	SVC_PERROR(sta_cyc(CYCHDR_S2)); // start the cyclic handler
#endif /* TNUM_PRCID >= 2 */
#if TNUM_PRCID >= 3
	SVC_PERROR(sta_cyc(CYCHDR_S3)); // start the cyclic handler
#endif /* TNUM_PRCID >= 3 */
#if TNUM_PRCID >= 4
	SVC_PERROR(sta_cyc(CYCHDR_S4)); // start the cyclic handler
#endif /* TNUM_PRCID >= 4 */
	target_prepare_reboot();
#else  /* !TOPPERS_SAFEG_SECURE */
	syslog(LOG_NOTICE, "Hello NT-FMP");
	/*
	 *  �������ε�ư
	 */
	SVC_PERROR(act_tsk(tskid));
#endif /* TOPPERS_SAFEG_SECURE */

	/*
 	 *  �ᥤ��롼��
	 */
	do {
		if (update_select) {
			prcid = prc_select;
			clsid = class_select;
			tskid = sample_tskid[clsid - 1][tsk_select-1];
			tskno = sample_tskno[clsid - 1][tsk_select-1];
			cycid = sample_cycid[tme_select-1];
			almid = sample_almid[tme_select-1];
			tsk_mig_prc = prcid;
			update_select = false;
			syslog(LOG_INFO, "select tskno 0x%x", tskno);
			syslog(LOG_INFO, "select cycid %d", cycid);
			syslog(LOG_INFO, "select almid %d", almid);
			syslog(LOG_INFO, "select processor %d", prcid);
			syslog(LOG_INFO, "select class     %d", clsid);
		}
		
		SVC_PERROR(serial_rea_dat(serial_port[exinf-1], &c, 1));
		
		switch (c) {
		case 'p':
		case 'e':
		case 's':
		case 'S':
		case 'd':
		case 'y':
		case 'Y':
		case 'z':
		case 'Z':
		case 'g':
			message[clsid-1][(tskno & 0x0f) - 1] = c;
			break;
		case '1':
			tsk_select = 1;
			update_select = true;
			break;
		case '2':
			tsk_select = 2;
			update_select = true;
			break;
		case '3':
			tsk_select = 3;
			update_select = true;
			break;
		case '4':
			tme_select = 1;
			class_select = 1;
			update_select = true;
			break;
#if TNUM_PRCID >= 2
		case '5':
			tme_select = 2;
			class_select = 2;
			update_select = true;
			break;
#endif /* TNUM_PRCID >= 2 */
#if TNUM_PRCID >= 3
		case '6':
			tme_select = 3;
			class_select = 3;
			update_select = true;
			break;
#endif /* TNUM_PRCID >= 3 */
#if TNUM_PRCID >= 4
		case '7':
			tme_select = 4;
			class_select = 4;
			update_select = true;
			break;
#endif /* TNUM_PRCID >= 4 */
		case '8':
			prc_select = 1;
			update_select = true;
			break;
#if TNUM_PRCID >= 2
		case '9':
			prc_select = 2;
			update_select = true;
			break;
#endif /* TNUM_PRCID >= 2 */
#if TNUM_PRCID >= 3
		case '0':
			prc_select = 3;
			update_select = true;
			break;
#endif /* TNUM_PRCID >= 3 */
#if TNUM_PRCID >= 4
		case '-':
			prc_select = 4;
			update_select = true;
			break;
#endif /* TNUM_PRCID >= 4 */
		case 'a':
			syslog(LOG_INFO, "#act_tsk(0x%x)", tskno);
			SVC_PERROR(act_tsk(tskid));
			break;
		case 'A':
			syslog(LOG_INFO, "#can_act(0x%x)", tskno);
			SVC_PERROR(ercd = can_act(tskid));
			if (ercd >= 0) {
				syslog(LOG_NOTICE, "can_act(0x%x) returns %d", tskno, ercd);
			}
			break;
		case 'f':
			syslog(LOG_INFO, "#mact_tsk(0x%x, 0x%x)", tskno, tsk_mig_prc);
			SVC_PERROR(mact_tsk(tskid, tsk_mig_prc));
			break;
		case 't':
			server_req = tskno << 16 | SERVER_REQ_TER_TSK;
			SVC_PERROR(snd_dtq(SERVER_DTQ1, server_req));
#if TNUM_PRCID >= 2
			SVC_PERROR(snd_dtq(SERVER_DTQ2, server_req));
#endif /* TNUM_PRCID >= 2 */
#if TNUM_PRCID >= 3
			SVC_PERROR(snd_dtq(SERVER_DTQ3, server_req));
#endif /* TNUM_PRCID >= 3 */
#if TNUM_PRCID >= 4
			SVC_PERROR(snd_dtq(SERVER_DTQ4, server_req));
#endif /* TNUM_PRCID >= 4 */
			break;
		case '>':
			syslog(LOG_INFO, "#chg_pri(0x%x, HIGH_PRIORITY)", tskno);
			SVC_PERROR(chg_pri(tskid, HIGH_PRIORITY));
			break;
		case '=':
			syslog(LOG_INFO, "#chg_pri(0x%x, MID_PRIORITY)", tskno);
			SVC_PERROR(chg_pri(tskid, MID_PRIORITY));
			break;
		case '<':
			syslog(LOG_INFO, "#chg_pri(0x%x, LOW_PRIORITY)", tskno);
			SVC_PERROR(chg_pri(tskid, LOW_PRIORITY));
			break;
		case 'G':
			syslog(LOG_INFO, "#get_pri(0x%x, &tskpri)", tskno);
			SVC_PERROR(ercd = get_pri(tskid, &tskpri));
			if (ercd >= 0) {
				syslog(LOG_NOTICE, "priority of task 0x%x is %d", tskno, tskpri);
			}
			break;
		case 'w':
			syslog(LOG_INFO, "#wup_tsk(0x%x)", tskno);
			SVC_PERROR(wup_tsk(tskid));
			break;
		case 'W':
			syslog(LOG_INFO, "#can_wup(0x%x)", tskno);
			SVC_PERROR(ercd = can_wup(tskid));
			if (ercd >= 0) {
				syslog(LOG_NOTICE, "can_wup(0x%x) returns %d", tskno, ercd);
			}
			break;
		case 'l':
			syslog(LOG_INFO, "#rel_wai(0x%x)", tskno);
			SVC_PERROR(rel_wai(tskid));
			break;
		case 'u':
			syslog(LOG_INFO, "#sus_tsk(0x%x)", tskno);
			SVC_PERROR(sus_tsk(tskid));
			break;
		case 'i':
			server_req = (tskno << 16) | (prcid << 8) | SERVER_REQ_MIG_TSK;
			SVC_PERROR(snd_dtq(SERVER_DTQ1, server_req));
#if TNUM_PRCID >= 2
			SVC_PERROR(snd_dtq(SERVER_DTQ2, server_req));
#endif /* TNUM_PRCID >= 2 */
#if TNUM_PRCID >= 3
			SVC_PERROR(snd_dtq(SERVER_DTQ3, server_req));
#endif /* TNUM_PRCID >= 3 */
#if TNUM_PRCID >= 4
			SVC_PERROR(snd_dtq(SERVER_DTQ4, server_req));
#endif /* TNUM_PRCID >= 4 */
			break;
		case 'm':
			syslog(LOG_INFO, "#rsm_tsk(0x%x)", tskno);
			SVC_PERROR(rsm_tsk(tskid));
			break;
		case 'x':
			syslog(LOG_INFO, "#ras_tex(0x%x, 0x0001U)", tskno);
			SVC_PERROR(ras_tex(tskid, 0x0001U));
			break;
		case 'X':
			syslog(LOG_INFO, "#ras_tex(0x%x, 0x0002U)", tskno);
			SVC_PERROR(ras_tex(tskid, 0x0002U));
			break;
		case 'r':
			syslog(LOG_INFO, "#mrot_rdq(three priorities, %d)", prcid);
			SVC_PERROR(mrot_rdq(HIGH_PRIORITY, prcid));
			SVC_PERROR(mrot_rdq(MID_PRIORITY, prcid));
			SVC_PERROR(mrot_rdq(LOW_PRIORITY, prcid));
			break;
		case 'c':
			syslog(LOG_INFO, "#sta_cyc(%d)", cycid);
			SVC_PERROR(sta_cyc(cycid));
			break;
		case 'C':
			syslog(LOG_INFO, "#stp_cyc(%d)", cycid);
			SVC_PERROR(stp_cyc(cycid));
			break;
		case 'I':
			syslog(LOG_INFO, "#msta_cyc(%d, %d)", cycid, prcid);
			SVC_PERROR(msta_cyc(cycid, prcid));
			break;
		case 'b':
			syslog(LOG_INFO, "#sta_alm(%d, %d)", almid, ALM_TIME);
			SVC_PERROR(sta_alm(almid, ALM_TIME));
			break;
		case 'B':
			syslog(LOG_INFO, "#stp_alm(%d)", almid);
			SVC_PERROR(stp_alm(almid));
			break;
		case 'E':
			syslog(LOG_INFO, "#msta_alm(%d, %d, %d)", almid, ALM_TIME, prcid);
			SVC_PERROR(msta_alm(almid, ALM_TIME, prcid));
			break;
		case 'V':
#ifdef TOPPERS_SUPPORT_GET_UTM
			SVC_PERROR(get_utm(&utime1));
			SVC_PERROR(get_utm(&utime2));
			syslog(LOG_NOTICE, "utime1 = %ld, utime2 = %ld",
										(ulong_t) utime1, (ulong_t) utime2);
#else /* TOPPERS_SUPPORT_GET_UTM */
			syslog(LOG_NOTICE, "get_utm is not supported.");
#endif /* TOPPERS_SUPPORT_GET_UTM */
			break;
		case 'v':
			SVC_PERROR(syslog_msk_log(LOG_UPTO(LOG_INFO), LOG_UPTO(LOG_EMERG)));
			break;
		case 'q':
			SVC_PERROR(syslog_msk_log(LOG_UPTO(LOG_NOTICE), LOG_UPTO(LOG_EMERG)));
			break;
#ifdef TOPPERS_SAFEG_SECURE
		  case 'R':
			act_tsk(RTASK1);
			break;
#endif /* TOPPERS_SAFEG_SECURE */
#ifdef BIT_KERNEL
		case ' ':
			SVC_PERROR(loc_cpu());
			{
				extern ER	bit_kernel(void);

				SVC_PERROR(ercd = bit_kernel());
				if (ercd >= 0) {
					syslog(LOG_NOTICE, "bit_kernel passed.");
				}
			}
			SVC_PERROR(unl_cpu());
			break;
#endif /* BIT_KERNEL */
		default:
			break;
		}
	} while (c != '\003' && c != 'Q');

	syslog(LOG_NOTICE, "Sample program ends.");
	SVC_PERROR(ext_ker());
	assert(0);
}
