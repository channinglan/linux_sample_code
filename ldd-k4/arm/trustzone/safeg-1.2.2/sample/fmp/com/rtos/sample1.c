/*
 *  TOPPERS/ASP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Advanced Standard Profile Kernel
 *
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2004-2013 by Embedded and Real-Time Systems Laboratory
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
 *  $Id: sample1.c 818 2015-10-02 12:56:52Z ertl-honda $
 */

#include <kernel.h>
#include <t_syslog.h>
#include <t_stdlib.h>
#include "syssvc/serial.h"
#include "syssvc/syslog.h"
#include "kernel_cfg.h"
#include "sample1.h"
#include "syscalls_api.h"
#include "doscom.h"

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

#define SVC_PERROR(expr) svc_perror(__FILE__, __LINE__, #expr, (expr))

/*
 * Periodic task
 */
void task(intptr_t exinf)
{
	static int i = 0;

	syslog(LOG_EMERG, "task %d", i++);
}

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
		ret = safeg_syscall_switch();
		SIL_UNL_INT();

		if (ret != SAFEG_SYSCALL_ERROR__OK) {
			syslog(LOG_NOTICE, "%d : SafeG ERROR: switch %u", exinf, ret);
			break;
		}
	}
}
#endif /* TOPPERS_SAFEG_SECURE */

/*
 * Cyclic handler
 */
void cyclic_handler(intptr_t exinf)
{
	SVC_PERROR(iact_tsk(TASK1_1));
}

static void
*memcpy(void *dest, const void *src, size_t count) {
	char *dst8 = (char *)dest;
	char *src8 = (char *)src;

	while (count--) *dst8++ = *src8++;
	return dest;
}

#define SMEMCH_SMEM_SIZE 64

void
smemch_test() {
	int ret;
	void *p_smem;
	int data;
	int i;

	syslog(LOG_NOTICE, "SMEM Channel test start!.");

	ret = doscom_smemch_get(DOSCOM_SMEMCH_1, &p_smem);
	if(ret != 0) {
		syslog(LOG_NOTICE, "Get Buffer Error!! %d",ret);
	}

	syslog(LOG_NOTICE, "Shared memory 0x%x.", p_smem);

	data = 0;
	while(1) {
		for(i = 0; i < SMEMCH_SMEM_SIZE; i++){
			*((uint32_t *)p_smem + i) = data + i;
		}
		syslog(LOG_NOTICE, "Write data %d to %d", data, data + SMEMCH_SMEM_SIZE);
		data++;
		tslp_tsk(4000);
	}
}

void
main_task_tex_routine(TEXPTN texptn, intptr_t exinf){
	syslog(LOG_NOTICE, "Main Task TEX Routeine!");
	syslog(LOG_NOTICE, "Restart Main Task.");
	act_tsk(MAIN_TASK1);
	ext_tsk();
}

void
main_task(intptr_t exinf)
{
	ER_UINT ercd;
	int ret;
	doscom_id_t block_id;
	void *p_block;
	char msg[] = "hello world";

	SVC_PERROR(syslog_msk_log(0, LOG_UPTO(LOG_DEBUG)));
	syslog(LOG_NOTICE, "SafeG-COM test program starts (exinf = %d).", (int_t) exinf);

		ercd = serial_opn_por(TASK_PORTID_G_SYSLOG);
		if (ercd < 0 && MERCD(ercd) != E_OBJ) {
				syslog(LOG_ERROR, "%s (%d) reported by `serial_opn_por'.",
					   itron_strerror(ercd), SERCD(ercd));
		}
		SVC_PERROR(serial_ctl_por(TASK_PORTID_G_SYSLOG,
								  (IOCTL_CRLF | IOCTL_FCSND | IOCTL_FCRCV)));
#ifdef ENABLE_TASK
	SVC_PERROR(sta_cyc(CYCHDR1));
#endif

#ifdef DISABLE_TIMER
	SVC_PERROR(dis_int(IRQNO_TIMER));
#endif

	ena_tex();

	syslog(LOG_NOTICE, "RTOS doscom_init");

	SVC_PERROR(doscom_init(0x00, -1));

	syslog(LOG_NOTICE, "Dual-OS communications initialized");

	/* Wait GPOS initilize */
	ret = doscom_fifoch_wait(DOSCOM_FIFOCH_0, -1);
	assert(ret == 0);

	/* phase 1 */
	syslog(LOG_NOTICE, "RTOS send message in fifoch 0");
   
	ret = doscom_fifoch_alloc(DOSCOM_FIFOCH_0, &block_id);
	assert(ret == 0);
	syslog(LOG_NOTICE, "ret = %d", ret);

	ret = doscom_fifoch_get(DOSCOM_FIFOCH_0, block_id, &p_block);
	assert(ret == 0);
	syslog(LOG_NOTICE, "buffer 0x%x.", p_block);

	memcpy(p_block, (void *)msg, sizeof(msg));

	ret = doscom_fifoch_enqueue(DOSCOM_FIFOCH_0, block_id);
	assert(ret == 0);

	syslog(LOG_NOTICE, "RTOS send event");
	ret = doscom_fifoch_notify(DOSCOM_FIFOCH_0);
	assert(ret == 0);

	syslog(LOG_NOTICE, "RTOS wait for ack");
	ret = doscom_fifoch_wait(DOSCOM_FIFOCH_0, -1);
	assert(ret == 0);

	/* phase 2 */
	syslog(LOG_NOTICE, "RTOS send params 3 and 5");

	*((uint32_t *)p_block) = 3;
	*((uint32_t *)(p_block + 4)) = 5;

	ret = doscom_fifoch_notify(DOSCOM_FIFOCH_0);
	assert(ret == 0);

	syslog(LOG_NOTICE, "RTOS wait result");
	ret = doscom_fifoch_wait(DOSCOM_FIFOCH_0, -1);
	assert(ret == 0);

	syslog(LOG_NOTICE, "RTOS rpc result: 3 x 5 = %u", *((uint32_t *)p_block));


	/* phase 3 */
	syslog(LOG_NOTICE, "RTOS send params 4 and 6");

	*((uint32_t *)p_block) = 4;
	*((uint32_t *)(p_block + 4)) = 6;

	ret = doscom_fifoch_notify(DOSCOM_FIFOCH_0);
	assert(ret == 0);

	syslog(LOG_NOTICE, "RTOS wait result");
	ret = doscom_fifoch_wait(DOSCOM_FIFOCH_0, -1);
	assert(ret == 0);

	syslog(LOG_NOTICE, "RTOS rpc result: 4 x 6 = %u", *((uint32_t *)p_block));

	syslog(LOG_NOTICE, "RTOS doscom_fifoch_free\n");
	ret = doscom_fifoch_free(DOSCOM_FIFOCH_0, block_id);
	assert(ret == 0);

	ret = doscom_fifoch_notify(DOSCOM_FIFOCH_0);
	assert(ret == 0);

	/* phase 4 */
	syslog(LOG_NOTICE, "RTOS wait for sentence from GPOS in fifoch 1");
	ret = doscom_fifoch_wait(DOSCOM_FIFOCH_1, -1);
	assert(ret == 0);

	ret = doscom_fifoch_dequeue(DOSCOM_FIFOCH_1, &block_id);
	assert(ret == 0);

	ret = doscom_fifoch_get(DOSCOM_FIFOCH_1, block_id, &p_block);
	assert(ret == 0);

	syslog(LOG_NOTICE, "MSG: %s", (char *)p_block);

	syslog(LOG_NOTICE, "RTOS doscom_block_free");
	ret = doscom_fifoch_free(DOSCOM_FIFOCH_1, block_id);
	assert(ret == 0);


	/* phase 5 */
	syslog(LOG_NOTICE, "RTOS wait for EVEN messages");
	ret = doscom_filter_set(DOSCOM_FIFOCH_0, DOSCOM_FILTER_ID_EVEN);
	assert(ret == 0);

	ret = doscom_fifoch_notify(DOSCOM_FIFOCH_0);
	assert(ret == 0);

	ret = doscom_fifoch_wait(DOSCOM_FIFOCH_0, -1);
	assert(ret == 0);

	while(1) {
		ret = doscom_fifoch_dequeue(DOSCOM_FIFOCH_0, &block_id);
		if (ret == DOSCOM_EMPTY) break;
		assert(ret == 0);

		ret = doscom_fifoch_get(DOSCOM_FIFOCH_0, block_id, &p_block);
		assert(ret == 0);

		syslog(LOG_NOTICE, "number: %u", *((uint32_t *)p_block));

		ret = doscom_fifoch_free(DOSCOM_FIFOCH_0, block_id);
		assert(ret == 0);
	}

	syslog(LOG_NOTICE, "RTOS com done!!");

	smemch_test();

	SVC_PERROR(slp_tsk());

	SVC_PERROR(ext_ker());
	assert(0);
}
