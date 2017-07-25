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
 *  @(#) $Id: sample1.h 786 2015-09-13 03:58:42Z ertl-honda $
 */

/*
 *		����ץ�ץ����(1)�Υإå��ե�����
 */

/*
 *  �������åȰ�¸�����
 */
#include "target_test.h"
#include "syssvc/safeg_load.h"

/*
 *  �ƥ�������ͥ���٤����
 */

#define MAIN_PRIORITY	5		/* �ᥤ�󥿥�����ͥ���� */
								/* HIGH_PRIORITY���⤯���뤳�� */

#define SERVER_PRIORITY	4		/* �����Х�������ͥ���� */

#define HIGH_PRIORITY	9		/* ����˼¹Ԥ���륿������ͥ���� */
#define MID_PRIORITY	10
#define LOW_PRIORITY	11

/*
 *  SERVER�������ؤ��׵�Τ����DTQ�ΥХåե���
 */
#define NUM_SERVER_DTQ		10

/*
 *  �������åȤ˰�¸�����ǽ���Τ�����������
 */

#ifndef TASK_PORTID_PRC1
#define	TASK_PORTID_PRC1		1	/* �ץ��å�1��ʸ�����Ϥ��륷�ꥢ��ݡ���ID */
#endif /* TASK_PORTID_PRC1 */

#ifndef TASK_PORTID_PRC2
#define	TASK_PORTID_PRC2		2	/* �ץ��å�2��ʸ�����Ϥ��륷�ꥢ��ݡ���ID */
#endif /* TASK_PORTID_PRC2 */

#ifndef TASK_PORTID_PRC3
#define	TASK_PORTID_PRC3		3	/* �ץ��å�3��ʸ�����Ϥ��륷�ꥢ��ݡ���ID */
#endif /* TASK_PORTID_PRC3 */

#ifndef TASK_PORTID_PRC4
#define	TASK_PORTID_PRC4		4	/* �ץ��å�4��ʸ�����Ϥ��륷�ꥢ��ݡ���ID */
#endif /* TASK_PORTID_PRC4 */

#ifndef TASK_PORTID_G_SYSLOG
#define	TASK_PORTID_G_SYSLOG	1	/* G_SYSLOG����ʸ�����Ϥ��륷�ꥢ��ݡ���ID */
#endif /* TASK_PORTID_G_SYSLOG */

#ifndef STACK_SIZE
#define	STACK_SIZE		4096		/* �������Υ����å������� */
#endif /* STACK_SIZE */

#ifndef LOOP_REF
#define LOOP_REF		ULONG_C(1000000)	/* ®�ٷ�¬�ѤΥ롼�ײ�� */
#endif /* LOOP_REF */

#ifndef ALM_TIME
#define ALM_TIME 5000
#endif /* ALMTIM */

#ifndef CYC_TIME
#define CYC_TIME 1000
#endif /* ALMTIM */

/*
 *  �ؿ��Υץ�ȥ��������
 */
#ifndef TOPPERS_MACRO_ONLY

extern void	main_task(intptr_t exinf);
extern void	task(intptr_t exinf);

extern void	cyclic_handler(intptr_t exinf);

#ifdef TOPPERS_SAFEG_SECURE
extern void btask(intptr_t exinf);
#endif /* TOPPERS_SAFEG_SECURE */

#endif /* TOPPERS_MACRO_ONLY */
