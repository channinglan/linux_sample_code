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
 *  サービスコールのエラーのログ出力
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
 *  メインタスク
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
