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
 *  $Id: sample1.c 606 2013-11-19 03:08:19Z ertl-honda $
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
#include "sil.h"

#define ALL_SEQ 0
#define MAIN_SEQ 1
#define TEX_SEQ 2
#define FIFO_SEQ 3
#define SHMEM_SEQ 4
#define INIT_SEQ 5
#define RUNNING_CORE 0
#define SEQ_SUCCESS 0
#define SEQ_ERROR 1
#define CON_ERROR 2
#define MAX_CHANNELS 32

/*
 * seqs[0] = all sequence
 * seqs[1] = main_sequence
 * seqs[2] = main_task_tex_routine_sequence
 * seqs[3] = fifo_sequence
 * seqs[4] = shmem_seq
 * seqs[5] = fifoinit_seq
 */
uint32_t seqs[6];
/*
 * connection managemet
 */
uint32_t connection[MAX_CHANNELS];
/* activate tex_routine counter */
uint32_t act_tex;

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

Inline void pr_tskid(char str[])
{
	ID tskid;
	get_tid(&tskid);
	syslog(LOG_NOTICE, "%s tskid = %d", str, tskid);
}

#define SVC_PERROR(expr) svc_perror(__FILE__, __LINE__, #expr, (expr))

int strcmp(char *str1, char *str2)
{
	int i;
	for(i = 0;*(str1 + i) == *(str2 + i);i++){
		if (*(str1 + i) == '\0') return 0;
	}
	return *(str1 + i) - *(str2 + i);
}

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
void alm_handler(intptr_t exinf)
{
	syslog(LOG_NOTICE, "Timeout. Test Failed.");
	while(1){}
}

static void *memcpy(void *dest, const void *src, size_t count) {
	char *dst8 = (char *)dest;
	char *src8 = (char *)src;

	while (count--) *dst8++ = *src8++;
	return dest;
}

#define SHMEM_SIZE 64

void connection_init(int com_mask){
	int i;
	for(i = 0; com_mask; i++){
		if (com_mask & 0x1){
			connection[i] = 0;
		}
		com_mask = com_mask >> 1;
	}
}

void add_seq(int types){
	seqs[types]++;
	seqs[ALL_SEQ]++;
}

volatile int seq_check(int types){
	int check_seq = seqs[types];
	int all_seq = seqs[ALL_SEQ];
	uint32_t ret;
	doscom_mask_t mask;
	uint32_t process;
	syslog(LOG_NOTICE, "SEQ %d check", all_seq);
	switch(types){
	case MAIN_SEQ:
		switch(check_seq){
		case 0:
			seqs[0] = 0;
			seqs[1] = 1;
			seqs[2] = 0;
			seqs[3] = 0;
			seqs[4] = 0;
			seqs[5] = 0;
			return SEQ_SUCCESS;
			break;
		case 1:
			if (all_seq != 4){
				syslog(LOG_NOTICE, "MAIN_SEQ seq %d error", check_seq);
				return SEQ_ERROR;
			}
			break;
		case 2:
			if (all_seq != 9){
				syslog(LOG_NOTICE, "MAIN_SEQ seq %d error", check_seq);
				return SEQ_ERROR;
			}
			break;
		case 3:
			if (all_seq != 10){
				syslog(LOG_NOTICE, "MAIN_SEQ seq %d error", check_seq);
				return SEQ_ERROR;
			}
			break;
		case 4:
			if (all_seq != 15){
				syslog(LOG_NOTICE, "MAIN_SEQ seq %d error", check_seq);
				return SEQ_ERROR;
			}
			break;
		case 5:
			if (all_seq != 17){
				syslog(LOG_NOTICE, "MAIN_SEQ seq %d error", check_seq);
				return SEQ_ERROR;
			}
			break;
		case 6:
			if (all_seq != 21){
				syslog(LOG_NOTICE, "MAIN_SEQ seq %d error", check_seq);
				return SEQ_ERROR;
			}
			break;
		case 7:
			if (all_seq != 27){
				syslog(LOG_NOTICE, "MAIN_SEQ seq %d error", check_seq);
				return SEQ_ERROR;
			}
			break;
		case 8:
			if (all_seq != 31){
				syslog(LOG_NOTICE, "MAIN_SEQ seq %d error", check_seq);
				return SEQ_ERROR;
			}
			break;
		case 9:
			if (all_seq != 34){
				syslog(LOG_NOTICE, "MAIN_SEQ seq %d error", check_seq);
				return SEQ_ERROR;
			}
			break;
		case 10:
			if (all_seq != 37){
				syslog(LOG_NOTICE, "MAIN_SEQ seq %d error", check_seq);
				return SEQ_ERROR;
			}
			break;
		case 11:
			if (all_seq != 42){
				syslog(LOG_NOTICE, "MAIN_SEQ seq %d error", check_seq);
				return SEQ_ERROR;
			}
			break;
		case 12:
			if (all_seq != 45){
				syslog(LOG_NOTICE, "MAIN_SEQ seq %d error", check_seq);
				return SEQ_ERROR;
			}
			break;
		case 13:
			if (all_seq != 50){
				syslog(LOG_NOTICE, "MAIN_SEQ seq %d error", check_seq);
				return SEQ_ERROR;
			}
			break;
		case 14:
			if (all_seq != 52){
				syslog(LOG_NOTICE, "MAIN_SEQ seq %d error", check_seq);
				return SEQ_ERROR;
			}
			break;
		case 15:
			if (all_seq != 58){
				syslog(LOG_NOTICE, "MAIN_SEQ seq %d error", check_seq);
				return SEQ_ERROR;
			}
			break;
		case 16:
			if (all_seq != 61){
				syslog(LOG_NOTICE, "MAIN_SEQ seq %d error", check_seq);
				return SEQ_ERROR;
			}
			break;
		case 17:
			if (all_seq != 62){
				syslog(LOG_NOTICE, "MAIN_SEQ seq %d error", check_seq);
				return SEQ_ERROR;
			}
			break;
		case 18:
			if (all_seq != 68){
				syslog(LOG_NOTICE, "MAIN_SEQ seq %d error", check_seq);
				return SEQ_ERROR;
			}
			break;
		}
		add_seq(types);
		break;
	case TEX_SEQ:
		act_tex++;
		switch(act_tex){
		case 1:
			if (all_seq != 1){
				syslog(LOG_NOTICE, "TEX_SEQ seq %d error", check_seq);
				return SEQ_ERROR;
			}
			ret = doscom_sense_process(&process);
			if (ret != DOSCOM_SUCCESS || process != 1){
				syslog(LOG_NOTICE, "doscom_sense_process ERROR. connect = %d", process);
				return SEQ_ERROR;
			}
			ret = doscom_sense_fifoch(&mask);
			if (ret != DOSCOM_SUCCESS || mask != 0x6){
				syslog(LOG_NOTICE, "doscom_sense_fifoch ERROR.active_bit = %d", mask);
				return SEQ_ERROR;
			}
#if 0
			if ((!connection[0]) || (!connection[1]) || connection[0] <= connection[1] ){
				syslog(LOG_NOTICE, "connection error");
				return CON_ERROR;
			}
#endif
			break;
		case 2:
			if (all_seq != 3){
				syslog(LOG_NOTICE, "TEX_SEQ seq %d error", check_seq);
				return SEQ_ERROR;
			}
			ret = doscom_sense_process(&process);
			if (ret != DOSCOM_SUCCESS || process != 0){
				syslog(LOG_NOTICE, "doscom_sense_process ERROR. connect = %d", process);
				return SEQ_ERROR;
			}
			ret = doscom_sense_fifoch(&mask);
			if (ret != DOSCOM_SUCCESS || mask != 0){
				syslog(LOG_NOTICE, "doscom_sense_fifoch ERROR.active_bit = %d", mask);
				return SEQ_ERROR;
			}
#if 0
			if ((!connection[0]) || (!connection[1]) || connection[0] > connection[1] ){
				syslog(LOG_NOTICE, "connection error");
				return CON_ERROR;
			}
#endif
			break;
		case 3:
			if (all_seq != 6){
				syslog(LOG_NOTICE, "TEX_SEQ seq %d error", check_seq);
				return SEQ_ERROR;
			}
			ret = doscom_sense_process(&process);
			if (ret != DOSCOM_SUCCESS || process != 1){
				syslog(LOG_NOTICE, "doscom_sense_process ERROR. connect = %d", process);
				return SEQ_ERROR;
			}
			ret = doscom_sense_fifoch(&mask);
			if (ret != DOSCOM_SUCCESS || mask != 0x1){
				syslog(LOG_NOTICE, "doscom_sense_fifoch ERROR.active_bit = %d", mask);
				return SEQ_ERROR;
			}
#if 0
			if ((!connection[0]) || (!connection[1]) || connection[0] > connection[0] ){
				syslog(LOG_NOTICE, "connection error.connection = [%d, %d]", connection[0], connection[1]);
				return CON_ERROR;
			}
#endif
			break;
		case 4:
			if (all_seq != 8){
				syslog(LOG_NOTICE, "TEX_SEQ seq %d error", check_seq);
				return SEQ_ERROR;
			}
			ret = doscom_sense_process(&process);
			if (ret != DOSCOM_SUCCESS || process != 0){
				syslog(LOG_NOTICE, "doscom_sense_process ERROR. connect = %d", process);
				return SEQ_ERROR;
			}
			ret = doscom_sense_fifoch(&mask);
			if (ret != DOSCOM_SUCCESS || mask != 0){
				syslog(LOG_NOTICE, "doscom_sense_fifoch ERROR.active_bit = %d", mask);
				return SEQ_ERROR;
			}
#if 0
			if ((!connection[0]) || (!connection[1]) || connection[0] <= connection[1] ){
				syslog(LOG_NOTICE, "connection error.connection = [%d, %d]", connection[0], connection[1]);
				return CON_ERROR;
			}
#endif
			break;
		case 5:
			if (all_seq != 12){
				syslog(LOG_NOTICE, "TEX_SEQ seq %d error", check_seq);
				return SEQ_ERROR;
			}
			break;
		case 6:
			if (all_seq != 14){
				syslog(LOG_NOTICE, "TEX_SEQ seq %d error", check_seq);
				return SEQ_ERROR;
			}
			ret = doscom_sense_process(&process);
			if (ret != DOSCOM_SUCCESS || process != 0){
				syslog(LOG_NOTICE, "doscom_sense_process ERROR. connect = %d", process);
				return SEQ_ERROR;
			}
			ret = doscom_sense_fifoch(&mask);
			if (ret != DOSCOM_SUCCESS || mask != 0){
				syslog(LOG_NOTICE, "doscom_sense_fifoch ERROR.active_bit = %d", mask);
				return SEQ_ERROR;
			}
#if 0
			/* TODO : Add connection requirement*/
			if ((!connection[0]) || (!connection[1])){
				syslog(LOG_NOTICE, "connection error");
				return CON_ERROR;
			}
#endif
			break;
		case 7:
			if (all_seq != 19){
				syslog(LOG_NOTICE, "TEX_SEQ seq %d error", check_seq);
				return SEQ_ERROR;
			}
			ret = doscom_sense_process(&process);
			if (ret != DOSCOM_SUCCESS || process != 1){
				syslog(LOG_NOTICE, "doscom_sense_process ERROR. connect = %d", process);
				return SEQ_ERROR;
			}
			ret = doscom_sense_fifoch(&mask);
			if (ret != DOSCOM_SUCCESS || mask != 0x6){
				syslog(LOG_NOTICE, "doscom_sense_fifoch ERROR.active_bit = %d", mask);
				return SEQ_ERROR;
			}
#if 0
			if ((!connection[0]) || (!connection[1])){
				syslog(LOG_NOTICE, "connection error");
				return CON_ERROR;
			}
#endif
			break;
		case 8:
			if (all_seq != 23){
				syslog(LOG_NOTICE, "TEX_SEQ seq %d error", check_seq);
				return SEQ_ERROR;
			}
			break;
		case 9:
			if (all_seq != 25){
				syslog(LOG_NOTICE, "TEX_SEQ seq %d error", check_seq);
				return SEQ_ERROR;
			}
			ret = doscom_sense_process(&process);
			if (ret != DOSCOM_SUCCESS || process != 0){
				syslog(LOG_NOTICE, "doscom_sense_process ERROR");
				return SEQ_ERROR;
			}
#if 0
			ret = doscom_sense_fifoch(&tmp);
			if (ret != DOSCOM_SUCCESS || tmp != 0){
				syslog(LOG_NOTICE, "doscom_sense_fifoch ERROR");
				return SEQ_ERROR;
			}
			if ((!connection[0]) || (!connection[1]) || connection[0] >= connection[1] ){
				syslog(LOG_NOTICE, "connection error");
				return CON_ERROR;
			}
#endif
			break;
		case 10:
			if (all_seq != 29){
				syslog(LOG_NOTICE, "TEX_SEQ seq %d error", check_seq);
				return SEQ_ERROR;
			}
			ret = doscom_sense_process(&process);
			if (ret != DOSCOM_SUCCESS || process != 0){
				syslog(LOG_NOTICE, "doscom_sense_process ERROR");
				return SEQ_ERROR;
			}
#if 0
			ret = doscom_sense_fifoch(&tmp);
			if (ret != DOSCOM_SUCCESS || tmp != 0){
				syslog(LOG_NOTICE, "doscom_sense_fifoch ERROR");
				return SEQ_ERROR;
			}
			if ((!connection[0]) || (!connection[1]) || connection[0] >= connection[1] ){
				syslog(LOG_NOTICE, "connection error");
				return CON_ERROR;
			}
#endif
			break;
		case 11:
			if (all_seq != 33){
				syslog(LOG_NOTICE, "TEX_SEQ seq %d error", check_seq);
				return SEQ_ERROR;
			}
			ret = doscom_sense_process(&process);
			if (ret != DOSCOM_SUCCESS || process != 0){
				syslog(LOG_NOTICE, "doscom_sense_process ERROR");
				return SEQ_ERROR;
			}
			ret = doscom_sense_fifoch(&mask);
			if (ret != DOSCOM_SUCCESS || mask != 0){
				syslog(LOG_NOTICE, "doscom_sense_fifoch ERROR");
				return SEQ_ERROR;
			}
#if 0
			if ((!connection[0]) || (!connection[1])){
				syslog(LOG_NOTICE, "connection error");
				return CON_ERROR;
			}
#endif
			break;
		case 12:
			if (all_seq != 39){
				syslog(LOG_NOTICE, "TEX_SEQ seq %d error", check_seq);
				return SEQ_ERROR;
			}
			ret = doscom_sense_process(&process);
			if (ret != DOSCOM_SUCCESS || process != 1){
				syslog(LOG_NOTICE, "doscom_sense_process ERROR. connect = %d", process);
				return SEQ_ERROR;
			}
			ret = doscom_sense_fifoch(&mask);
			if (ret != DOSCOM_SUCCESS || mask != 0x2){
				syslog(LOG_NOTICE, "doscom_sense_fifoch ERROR.active_bit = %d", mask);
				return SEQ_ERROR;
			}
#if 0
			if ((!connection[0]) || (!connection[1])){
				syslog(LOG_NOTICE, "connection error.connection = [%d, %d]", connection[0], connection[1]);
				return CON_ERROR;
			}
#endif
			break;
		case 13:
			if (all_seq != 41){
				syslog(LOG_NOTICE, "TEX_SEQ seq %d error", check_seq);
				return SEQ_ERROR;
			}
			ret = doscom_sense_process(&process);
			if (ret != DOSCOM_SUCCESS || process != 0){
				syslog(LOG_NOTICE, "doscom_sense_process ERROR");
				return SEQ_ERROR;
			}
			ret = doscom_sense_fifoch(&mask);
			if (ret != DOSCOM_SUCCESS || mask != 0){
				syslog(LOG_NOTICE, "doscom_sense_fifoch ERROR");
				return SEQ_ERROR;
			}
#if 0
			if ((!connection[0]) || (!connection[1])){
				syslog(LOG_NOTICE, "connection error");
				return CON_ERROR;
			}
#endif
			break;
		case 14:
			if (all_seq != 47){
				syslog(LOG_NOTICE, "TEX_SEQ seq %d error", check_seq);
				return SEQ_ERROR;
			}
			ret = doscom_sense_process(&process);
			if (ret != DOSCOM_SUCCESS || process != 1){
				syslog(LOG_NOTICE, "doscom_sense_process ERROR. connect = %d", process);
				return SEQ_ERROR;
			}
			ret = doscom_sense_fifoch(&mask);
			if (ret != DOSCOM_SUCCESS || mask != 0x2){
				syslog(LOG_NOTICE, "doscom_sense_fifoch ERROR.active_bit = %d", mask);
				return SEQ_ERROR;
			}
#if 0
			if ((!connection[0]) || (!connection[1])){
				syslog(LOG_NOTICE, "connection error.connection = [%d, %d]", connection[0], connection[1]);
				return CON_ERROR;
			}
#endif
			break;
		case 15:
			if (all_seq != 49){
				syslog(LOG_NOTICE, "TEX_SEQ seq %d error", check_seq);
				return SEQ_ERROR;
			}
			ret = doscom_sense_process(&process);
			if (ret != DOSCOM_SUCCESS || process != 0){
				syslog(LOG_NOTICE, "doscom_sense_process ERROR");
				return SEQ_ERROR;
			}
			ret = doscom_sense_fifoch(&mask);
			if (ret != DOSCOM_SUCCESS || mask != 0){
				syslog(LOG_NOTICE, "doscom_sense_fifoch ERROR");
				return SEQ_ERROR;
			}
#if 0
			if ((!connection[0]) || (!connection[1])){
				syslog(LOG_NOTICE, "connection error");
				return CON_ERROR;
			}
#endif
			break;
		case 16:
			if (all_seq != 54){
				syslog(LOG_NOTICE, "TEX_SEQ seq %d error", check_seq);
				return SEQ_ERROR;
			}
			ret = doscom_sense_process(&process);
			if (ret != DOSCOM_SUCCESS || process != 1){
				syslog(LOG_NOTICE, "doscom_sense_process ERROR");
				return SEQ_ERROR;
			}
			ret = doscom_sense_fifoch(&mask);
			if (ret != DOSCOM_SUCCESS || mask != 0x6){
				syslog(LOG_NOTICE, "doscom_sense_fifoch ERROR");
				return SEQ_ERROR;
			}
#if 0
			if ((!connection[0]) || (!connection[1])){
				syslog(LOG_NOTICE, "connection error");
				return CON_ERROR;
			}
#endif
			break;
		case 17:
			if (all_seq != 56){
				syslog(LOG_NOTICE, "TEX_SEQ seq %d error", check_seq);
				return SEQ_ERROR;
			}
			ret = doscom_sense_process(&process);
			if (ret != DOSCOM_SUCCESS || process != 0){
				syslog(LOG_NOTICE, "doscom_sense_process ERROR");
				return SEQ_ERROR;
			}
			ret = doscom_sense_fifoch(&mask);
			if (ret != DOSCOM_SUCCESS || mask != 0x0){
				syslog(LOG_NOTICE, "doscom_sense_fifoch ERROR");
				return SEQ_ERROR;
			}
#if 0
			if ((!connection[0]) || (!connection[1])){
				syslog(LOG_NOTICE, "connection error");
				return CON_ERROR;
			}
#endif
			break;
		case 18:
			if (all_seq != 60){
				syslog(LOG_NOTICE, "TEX_SEQ seq %d error", check_seq);
				return SEQ_ERROR;
			}
			ret = doscom_sense_process(&process);
			if (ret != DOSCOM_SUCCESS || process != 0){
				syslog(LOG_NOTICE, "doscom_sense_process ERROR");
				return SEQ_ERROR;
			}
			ret = doscom_sense_fifoch(&mask);
			if (ret != DOSCOM_SUCCESS || mask != 0x0){
				syslog(LOG_NOTICE, "doscom_sense_fifoch ERROR");
				return SEQ_ERROR;
			}
#if 0
			if ((!connection[0]) || (!connection[1])){
				syslog(LOG_NOTICE, "connection error");
				return CON_ERROR;
			}
#endif
			break;
		case 19:
			if (all_seq != 65){
				syslog(LOG_NOTICE, "TEX_SEQ seq %d error", check_seq);
				return SEQ_ERROR;
			}
			ret = doscom_sense_process(&process);
			if (ret != DOSCOM_SUCCESS || process != 1){
				syslog(LOG_NOTICE, "doscom_sense_process ERROR");
				return SEQ_ERROR;
			}
			ret = doscom_sense_fifoch(&mask);
			if (ret != DOSCOM_SUCCESS || mask != 0x6){
				syslog(LOG_NOTICE, "doscom_sense_fifoch ERROR");
				return SEQ_ERROR;
			}
#if 0
			if ((!connection[0]) || (!connection[1])){
				syslog(LOG_NOTICE, "connection error");
				return CON_ERROR;
			}
#endif
			break;
		case 20:
			if (all_seq != 67){
				syslog(LOG_NOTICE, "TEX_SEQ seq %d error", check_seq);
				return SEQ_ERROR;
			}
			ret = doscom_sense_process(&process);
			if (ret != DOSCOM_SUCCESS || process != 0){
				syslog(LOG_NOTICE, "doscom_sense_process ERROR");
				return SEQ_ERROR;
			}
			ret = doscom_sense_fifoch(&mask);
			if (ret != DOSCOM_SUCCESS || mask != 0x0){
				syslog(LOG_NOTICE, "doscom_sense_fifoch ERROR");
				return SEQ_ERROR;
			}
#if 0
			if ((!connection[0]) || (!connection[1])){
				syslog(LOG_NOTICE, "connection error");
				return CON_ERROR;
			}
#endif
			break;
		case 21:
			if (all_seq != 71){
				syslog(LOG_NOTICE, "TEX_SEQ seq %d error", check_seq);
				return SEQ_ERROR;
			}
			ret = doscom_sense_process(&process);
			if (ret != DOSCOM_SUCCESS || process != 0){
				syslog(LOG_NOTICE, "doscom_sense_process ERROR");
				return SEQ_ERROR;
			}
			ret = doscom_sense_fifoch(&mask);
			if (ret != DOSCOM_SUCCESS || mask != 0x0){
				syslog(LOG_NOTICE, "doscom_sense_fifoch ERROR");
				return SEQ_ERROR;
			}
#if 0
			if ((!connection[0]) || (!connection[1])){
				syslog(LOG_NOTICE, "connection error");
				return CON_ERROR;
			}
#endif
			break;
		}
		add_seq(types);
		break;
	case FIFO_SEQ:
		switch(check_seq){
		case 0:
			if (all_seq != 0){
				syslog(LOG_NOTICE, "FIFO_SEQ seq %d error", check_seq);
				return SEQ_ERROR;
			}
			syslog(LOG_NOTICE, "FIFO_SEQ seq %d OK", check_seq);
			break;
		case 1:
			if (all_seq != 2){
				syslog(LOG_NOTICE, "FIFO_SEQ seq %d error", check_seq);
				syslog(LOG_NOTICE, "all_seq = %d", all_seq);
				return SEQ_ERROR;
			}
			break;
		case 2:
			if (all_seq != 5){
				syslog(LOG_NOTICE, "FIFO_SEQ seq %d error", check_seq);
				return SEQ_ERROR;
			}
			break;
		case 3:
			if (all_seq != 7){
				syslog(LOG_NOTICE, "FIFO_SEQ seq %d error", check_seq);
				return SEQ_ERROR;
			}
			break;
		case 4:
			if (all_seq != 11){
				syslog(LOG_NOTICE, "FIFO_SEQ seq %d error", check_seq);
				return SEQ_ERROR;
			}
			break;
		case 5:
			if (all_seq != 13){
				syslog(LOG_NOTICE, "FIFO_SEQ seq %d error", check_seq);
				return SEQ_ERROR;
			}
			break;
		case 6:
			if (all_seq != 16){
				syslog(LOG_NOTICE, "FIFO_SEQ seq %d error", check_seq);
				return SEQ_ERROR;
			}
			break;
		case 7:
			if (all_seq != 18){
				syslog(LOG_NOTICE, "FIFO_SEQ seq %d error", check_seq);
				return SEQ_ERROR;
			}
			break;
		case 8:
			if (all_seq != 20){
				syslog(LOG_NOTICE, "FIFO_SEQ seq %d error", check_seq);
				return SEQ_ERROR;
			}
			break;
		case 9:
			if (all_seq != 22){
				syslog(LOG_NOTICE, "FIFO_SEQ seq %d error", check_seq);
				return SEQ_ERROR;
			}
			break;
		case 10:
			if (all_seq != 24){
				syslog(LOG_NOTICE, "FIFO_SEQ seq %d error", check_seq);
				return SEQ_ERROR;
			}
			break;
		case 11:
			if (all_seq != 26){
				syslog(LOG_NOTICE, "FIFO_SEQ seq %d error", check_seq);
				return SEQ_ERROR;
			}
			break;
		case 12:
			if (all_seq != 28){
				syslog(LOG_NOTICE, "FIFO_SEQ seq %d error", check_seq);
				return SEQ_ERROR;
			}
			break;
		case 13:
			if (all_seq != 30){
				syslog(LOG_NOTICE, "FIFO_SEQ seq %d error", check_seq);
				return SEQ_ERROR;
			}
			break;
		case 14:
			if (all_seq != 32){
				syslog(LOG_NOTICE, "FIFO_SEQ seq %d error", check_seq);
				return SEQ_ERROR;
			}
			break;
		case 15:
			if (all_seq != 55){
				syslog(LOG_NOTICE, "FIFO_SEQ seq %d error", check_seq);
				return SEQ_ERROR;
			}
			break;
		case 16:
			if (all_seq != 66){
				syslog(LOG_NOTICE, "SHMEM_SEQ seq %d error", check_seq);
				return SEQ_ERROR;
			}
			break;
		}
		add_seq(types);
		break;
	case SHMEM_SEQ:
		switch(check_seq){
		case 0:
			if (all_seq != 35){
				syslog(LOG_NOTICE, "SHMEM_SEQ seq %d error", check_seq);
				return SEQ_ERROR;
			}
			break;
		case 1:
			if (all_seq != 36){
				syslog(LOG_NOTICE, "SHMEM_SEQ seq %d error", check_seq);
				return SEQ_ERROR;
			}
			break;
		case 2:
			if (all_seq != 38){
				syslog(LOG_NOTICE, "SHMEM_SEQ seq %d error", check_seq);
				return SEQ_ERROR;
			}
			break;
		case 3:
			if (all_seq != 40){
				syslog(LOG_NOTICE, "SHMEM_SEQ seq %d error", check_seq);
				return SEQ_ERROR;
			}
			break;
		case 4:
			if (all_seq != 43){
				syslog(LOG_NOTICE, "SHMEM_SEQ seq %d error", check_seq);
				return SEQ_ERROR;
			}
			break;
		case 5:
			if (all_seq != 44){
				syslog(LOG_NOTICE, "SHMEM_SEQ seq %d error", check_seq);
				return SEQ_ERROR;
			}
			break;
		case 6:
			if (all_seq != 46){
				syslog(LOG_NOTICE, "SHMEM_SEQ seq %d error", check_seq);
				return SEQ_ERROR;
			}
			break;
		case 7:
			if (all_seq != 48){
				syslog(LOG_NOTICE, "SHMEM_SEQ seq %d error", check_seq);
				return SEQ_ERROR;
			}
			break;
		}
		add_seq(types);
		break;
	case INIT_SEQ:
		switch(check_seq){
		case 0:
			if (all_seq != 51){
				syslog(LOG_NOTICE, "INIT_SEQ seq %d error", check_seq);
				return SEQ_ERROR;
			}
			break;
		case 1:
			if (all_seq != 53){
				syslog(LOG_NOTICE, "INIT_SEQ seq %d error", check_seq);
				return SEQ_ERROR;
			}
			break;
		case 2:
			if (all_seq != 57){
				syslog(LOG_NOTICE, "INIT_SEQ seq %d error", check_seq);
				return SEQ_ERROR;
			}
			break;
		case 3:
			if (all_seq != 59){
				syslog(LOG_NOTICE, "INIT_SEQ seq %d error", check_seq);
				return SEQ_ERROR;
			}
			break;
		case 4:
			if (all_seq != 63){
				syslog(LOG_NOTICE, "INIT_SEQ seq %d error", check_seq);
				return SEQ_ERROR;
			}
			break;
		case 5:
			if (all_seq != 64){
				syslog(LOG_NOTICE, "INIT_SEQ seq %d error", check_seq);
				return SEQ_ERROR;
			}
			break;
		case 6:
			if (all_seq != 69){
				syslog(LOG_NOTICE, "INIT_SEQ seq %d error", check_seq);
				return SEQ_ERROR;
			}
			break;
		case 7:
			if (all_seq != 70){
				syslog(LOG_NOTICE, "INIT_SEQ seq %d error", check_seq);
				return SEQ_ERROR;
			}
			break;
		}
		add_seq(types);
		break;
	}
	return SEQ_SUCCESS;
}

int error_check(int ret, int types){
	int tmp = SEQ_SUCCESS;
	switch(ret){
	case DOSCOM_SUCCESS:
		add_seq(types);
		break;
	case DOSCOM_TIMEOUT:
		syslog(LOG_NOTICE, "DOSCOM_TIMEOUT");
		tmp = seq_check(types);
		break;
	case DOSCOM_RESTART:
		syslog(LOG_NOTICE, "DOSCOM_RESTART");
		tmp = seq_check(types);
		break;
	default:
		syslog(LOG_NOTICE, "UNDEFINED ERROR");
		ext_tsk();
		break;
	}
	return tmp;
}

void
shmem_task_rtg(intptr_t exinf) {
	int ret;
	void *buffer;
	int data;
	uint32_t comnum = (uint32_t)exinf;
	uint32_t shmem_id = DOSCOM_SMEMCH_1;

	connection_init(1 << comnum);
	ret = doscom_smemch_get(shmem_id, &buffer);
	if(ret != 0) {
		syslog(LOG_NOTICE, "Get Buffer Error!! %d",ret);
	}
//	syslog(LOG_NOTICE, "buffer 0x%x, Size %d.", buffer, size);
	seq_check(SHMEM_SEQ);
	ret = doscom_fifoch_wait(comnum, -1);
	assert(ret == 0);

	/* Data Trunsfer Start */
	data = 0;
	while(1) {
#if 0
		for(i = 0; i < SHMEM_SIZE; i++){
			*((uint32_t *)buffer + i) = data + i;
		}
		syslog(LOG_NOTICE, "Write data %d to %d", data, data + SHMEM_SIZE);
#endif
		ret = doscom_fifoch_notify(comnum);
		assert(ret == 0);

		*(uint32_t *)buffer = data;
		ret = doscom_fifoch_wait(comnum, 500);
		if (ret == DOSCOM_TIMEOUT){
			ret = error_check(ret, SHMEM_SEQ);
			if (ret != SEQ_SUCCESS ){
				syslog(LOG_NOTICE, "seq_check ERROR");
				while(1){}
			}
			pr_tskid("Exit shmem_task_rtg.");
			wup_tsk(MAIN_TASK);
		}
		data++;
		connection[comnum]++;
		tslp_tsk(1000);
	}
}

void
shmem_task_gtr(intptr_t exinf) {
	int ret;
	void *buffer;
	int data, rec_data;
	uint32_t comnum = (uint32_t)exinf;
	uint32_t shmem_id = DOSCOM_SMEMCH_1;

	connection_init(1 << comnum);
	ret = doscom_smemch_get(shmem_id, &buffer);
	if(ret != 0) {
		syslog(LOG_NOTICE, "Get Buffer Error!! %d",ret);
	}
	data = 0;
	seq_check(SHMEM_SEQ);
	while(1) {
		syslog(LOG_NOTICE, "channel %d wait for ack", comnum);
		ret = doscom_fifoch_wait(comnum, -1);
		if (ret == DOSCOM_RESTART){
			ret = error_check(ret, SHMEM_SEQ);
			if (ret != SEQ_SUCCESS ){
				syslog(LOG_NOTICE, "seq_check ERROR");
				while(1){}
			}
			pr_tskid("Exit shmem_task_gtr");
			wup_tsk(MAIN_TASK);
		}

		/* receive Number from GPOS */
		rec_data = *(uint32_t *)buffer;
		syslog(LOG_NOTICE, "shmem(related channel %d) receive MSG:%d", comnum, rec_data);
		if (rec_data != data){
			syslog(LOG_NOTICE, "SHMEM data error");
			ext_tsk();
		}
		data++;
		connection[comnum]++;
	}
}

void fifo_task_reconnect(intptr_t exinf){
	char msg[] = "onetime";
	uint32_t comnum = 0;
	uint32_t com_mask = (uint32_t)exinf;
	doscom_id_t block_id;
	void *buffer;
	int ret;

	seq_check(FIFO_SEQ);
	if (seqs[0] == 17 || seqs[0] == 27){
		while(com_mask){
			if (com_mask & 0x1){
				/* Send message to GPOS */
				syslog(LOG_NOTICE, "reconnect task send message in channel %d", comnum);
				ret = doscom_fifoch_alloc(comnum, &block_id);
				assert(ret == 0);

				ret = doscom_fifoch_get(comnum, block_id, &buffer);
				assert(ret == 0);

				assert(sizeof(msg) < SHMEM_SIZE);
				memcpy(buffer, (void *)(msg), sizeof(msg));

				ret = doscom_fifoch_enqueue(comnum, block_id);
			}
			syslog(LOG_NOTICE, "reconnect task waiting in channel %d", comnum);
			ret = doscom_fifoch_wait(comnum, -1);
			if (ret == DOSCOM_RESTART){
				ret = error_check(ret, FIFO_SEQ);
				if (ret != SEQ_SUCCESS ){
					syslog(LOG_NOTICE, "seq_check ERROR");
					while(1){}
				}
				pr_tskid("Exit fifo_task_reconnect");
				if (seqs[0] != 19 && seqs[0]!= 29){
					ret = doscom_fifoch_free(comnum, block_id);
				}
				assert(ret == 0);
				wup_tsk(MAIN_TASK);
			}
		}
		com_mask = com_mask >> 1;
		comnum++;
	}
	else{
		syslog(LOG_NOTICE, "reconnect task waiting in channel %d", comnum);
		ret = doscom_fifoch_wait(comnum, -1);
		if (ret == DOSCOM_RESTART){
			ret = error_check(ret, FIFO_SEQ);
			if (ret != SEQ_SUCCESS ){
				syslog(LOG_NOTICE, "seq_check ERROR");
				while(1){}
			}
			pr_tskid("Exit fifo_task_reconnect");
			if (seqs[0] != 19 && seqs[0]!= 29){
				ret = doscom_fifoch_free(comnum, block_id);
			}
			assert(ret == 0);
			wup_tsk(MAIN_TASK);
		}
	}
}

void fifo_task_fifoinit_send(intptr_t exinf){
	char msg[] = "fifo init";
	char msg2[] = "fifo init 2nd";
	uint32_t comnum = 0;
	uint32_t com_mask = (uint32_t)exinf;
	doscom_id_t block_id;
	void *buffer;
	int ret;

	while(com_mask){
		if (com_mask & 0x1){
			/* Send message to GPOS */
			syslog(LOG_NOTICE, "RTOS send message in channel %d", comnum);

			ret = doscom_fifoch_alloc(comnum, &block_id);
			assert(ret == 0);

			ret = doscom_fifoch_get(comnum, block_id, &buffer);
			assert(ret == 0);

			memcpy(buffer, (void *)(msg), sizeof(msg));
			syslog(LOG_NOTICE, "block_id = %d, buffer = %s.", block_id, (char *)buffer);

			ret = doscom_fifoch_enqueue(comnum, block_id);
			assert(ret == 0);

			/* Initialize fifo */
			doscom_fifoch_init(comnum);

			/* Reconnect */
			ret = doscom_fifoch_alloc(comnum, &block_id);
			assert(ret == 0);

			ret = doscom_fifoch_get(comnum, block_id, &buffer);

			memcpy(buffer, (void *)(msg2), sizeof(msg2));
			syslog(LOG_NOTICE, "block_id = %d, buffer = %s.", block_id, (char *)buffer);

			ret = doscom_fifoch_enqueue(comnum, block_id);

			error_check(ret, INIT_SEQ);
			ret = doscom_fifoch_wait(comnum, -1);
			if (ret == DOSCOM_RESTART){
				ret = error_check(ret, INIT_SEQ);
				if (ret != SEQ_SUCCESS ){
					syslog(LOG_NOTICE, "seq_check ERROR");
					while(1){}
				}
				pr_tskid("Exit fifo_task_fifoinit_send.");
				doscom_fifoch_free(comnum, block_id);
				wup_tsk(MAIN_TASK);
			}

			ret = doscom_fifoch_free(comnum, block_id);
			assert(ret == 0);
		}
		com_mask = com_mask >> 1;
		comnum++;
	}
}

void fifo_task_fifoinit_receive(intptr_t exinf){
	uint32_t comnum = 0;
	uint32_t com_mask = (uint32_t)exinf;
	doscom_id_t block_id;
	void *buffer;
	int ret;

	while(com_mask){
		if (com_mask & 0x1){
			/* Waiting fifoch_init from GPOS */
			ret = doscom_fifoch_wait(comnum, -1);
			if (ret == DOSCOM_RESTART){
				ret = error_check(ret, INIT_SEQ);
				if (ret != SEQ_SUCCESS ){
					syslog(LOG_NOTICE, "seq_check ERROR");
					while(1){}
				}
				syslog(LOG_NOTICE, "FIFO Initialized");
				doscom_fifoch_free(comnum, block_id);
				wup_tsk(MAIN_TASK);
			}
			/* waiting event_send from GPOS */
			ret = doscom_fifoch_wait(comnum, -1);
			if (ret == DOSCOM_SUCCESS){
				ret = doscom_fifoch_dequeue(comnum, &block_id);
				//printf("doscom_block_dequeue() %d\n", ret);
				assert(ret == 0);

				ret = doscom_fifoch_get(comnum, block_id, &buffer);
				assert(ret == 0);

				/* Verify Message */
				syslog(LOG_NOTICE, "rec_data = %s\n", (char *)buffer);
				if (strcmp((char *)buffer, "fifo init 2nd")){
					syslog(LOG_NOTICE, "Invalid data received. rec_data = %s\n", (char *)buffer);
					while(1){}
				}
			}

			/* waiting Exit event from GPOS */
			ret = doscom_fifoch_wait(comnum, -1);
			if (ret == DOSCOM_RESTART){
				ret = error_check(ret, INIT_SEQ);
				if (ret != SEQ_SUCCESS ){
					syslog(LOG_NOTICE, "seq_check ERROR");
					while(1){}
				}
				pr_tskid("Exit fifo_task_fifoinit_receive.");
				doscom_fifoch_free(comnum, block_id);
				wup_tsk(MAIN_TASK);
			}
		}
		com_mask = com_mask >> 1;
		comnum++;
	}
}

void
fifo_task_gtr(intptr_t exinf){
	uint32_t comnum = 0, com_mask = (uint32_t)exinf;
	doscom_id_t block_id;
	void *buffer;
	int ret, i;
	volatile int data[MAX_CHANNELS], rec_data;

	syslog(LOG_NOTICE, "This is fifo_task_gtr");

	connection_init(com_mask);
	for (i = 0; com_mask >> i ; i++){
		if ((com_mask >> i)  & 0x1) data[i] = 0;
	}

	while(1){
		while(com_mask){
			if (com_mask & 0x1){
				/* Receive message from GPOS*/
				syslog(LOG_NOTICE, "RTOS wait for sentence from GPOS in channel %d", comnum);
				ret = doscom_fifoch_wait(comnum, -1);
				if (ret == DOSCOM_RESTART){
					ret = error_check(ret, FIFO_SEQ);
					if (ret != SEQ_SUCCESS ){
						syslog(LOG_NOTICE, "seq_check ERROR");
						while(1){}
					}
					pr_tskid("Exit fifo_task_gtr");
					doscom_fifoch_free(comnum, block_id);
					wup_tsk(MAIN_TASK);
				}
				else if(ret != 0){
					syslog(LOG_NOTICE, "doscom_fifoch_wait() ERROR");
				}

				ret = doscom_fifoch_dequeue(comnum, &block_id);
				assert(ret == 0);

				ret = doscom_fifoch_get(comnum, block_id, &buffer);
				assert(ret == 0);

				rec_data = *(int *)buffer;
				syslog(LOG_NOTICE, "FIFO %d receive MSG: %d", comnum, rec_data);
				if (rec_data != data[comnum]){
					syslog(LOG_NOTICE, "FIFO data error.rec_data = %d, data = %d", rec_data, data[comnum]);
					ext_tsk();
				}
				data[comnum]++;
			}
			com_mask = com_mask >> 1;
			comnum++;
		}
		connection[comnum]++;
		com_mask = (uint32_t)exinf;
		comnum = 0;
	}
}

void
fifo_task_rtg(intptr_t exinf){
	uint32_t comnum = 0, com_mask = 0;
	doscom_id_t block_id;
	void *buffer;
	int ret;
	int msg0 = 0;

	syslog(LOG_NOTICE, "This is fifo_task_rtg");

	while(1){
		com_mask = (uint32_t)exinf;
		comnum = 0;
		while(com_mask){
			if (com_mask & 0x1){
				/* Send message to GPOS */
				syslog(LOG_NOTICE, "RTOS send message %d in channel %d", msg0, comnum);

				ret = doscom_fifoch_alloc(comnum, &block_id);
				assert(ret == 0);
				ret = doscom_fifoch_get(comnum, block_id, &buffer);
				assert(ret == 0);

				memcpy(buffer, (void *)(&msg0), sizeof(&msg0));

				ret = doscom_fifoch_enqueue(comnum, block_id);
				assert(ret == 0);

				syslog(LOG_NOTICE, "RTOS wait for ack");
				ret = doscom_fifoch_wait(comnum, -1);
				if (ret == DOSCOM_RESTART){
					ret = error_check(ret, FIFO_SEQ);
					if (ret != SEQ_SUCCESS ){
						syslog(LOG_NOTICE, "seq_check ERROR");
						while(1){}
					}
					pr_tskid("Exit fifo_task_rtg");
					ret = doscom_fifoch_free(comnum, block_id);
					wup_tsk(MAIN_TASK);
				}
				ret = doscom_fifoch_free(comnum, block_id);
				/* connection++ */
				connection[comnum]++;
			}
			com_mask = com_mask >> 1;
			comnum++;
		}
		msg0++;
	}
}

void
main_task_tex_routine(TEXPTN texptn, intptr_t exinf){
#if 0
#ifdef TNUM_PRCID
	T_RTSK tsk;
	/* Verify Running Core ID */
	ref_tsk(TSK_SELF, &tsk);
	if (tsk.prcid != RUNNING_CORE ){
		syslog(LOG_NOTICE, "tex_routine don't run core %d. set this to core %d.", RUNNING_CORE, RUNNING_CORE);
	}
#endif
#endif
	/* seq check */
	syslog(LOG_NOTICE, "TEX_ROUTINE");
	if (seq_check(TEX_SEQ) != SEQ_SUCCESS){
		syslog(LOG_NOTICE, "Not SEQ_SUCCESS");
	}
	//ext_tsk();
}

void main_task(intptr_t exinf)
{
	ER_UINT ercd;
	int ret;
	uint32_t process;
	doscom_mask_t mask;
#ifdef TNUM_PRUID
	T_RTSK m_tsk;
#endif
	T_RTSK tsk1stat, tsk2stat;

	SVC_PERROR(syslog_msk_log(0, LOG_UPTO(LOG_DEBUG)));

	ercd = serial_opn_por(TASK_PORTID_G_SYSLOG);
	if (ercd < 0 && MERCD(ercd) != E_OBJ) {
		syslog(LOG_ERROR, "%s (%d) reported by `serial_opn_por'.",
				itron_strerror(ercd), SERCD(ercd));
	}
	SVC_PERROR(serial_ctl_por(TASK_PORTID_G_SYSLOG,
				(IOCTL_CRLF | IOCTL_FCSND | IOCTL_FCRCV)));

#ifdef TNUM_PRUID
	/* Verify Running Core ID */
	ref_tsk(TSK_SELF, &m_tsk);
	if (m_tsk.prcid != RUNNUNG_CORE ){
		syslog(LOG_NOTICE, "MAIN_TASK don't run core %d", RUNNING_CORE);
		ext_tsk();
	}
#endif
	ena_tex();
	/* Test Start */
	syslog(LOG_NOTICE, "SafeG-COM test program starts (exinf = %d).", (int_t) exinf);
	seq_check(MAIN_SEQ);

	syslog(LOG_NOTICE, "Waiting Linux booting... If Linux is booted, Execute test.sh.\nCommand is 'taskset 0x01 test.sh'");
	/* Check doscom_init */
	ret = doscom_init(-1, -1);
	assert (ret == DOSCOM_SUCCESS);
	ret = doscom_init(-1, -1);
	assert (ret == DOSCOM_SUCCESS);

	/* Phase 1 */
	syslog(LOG_NOTICE, "\n--------------------Phase 1--------------------");
	/* act FIFO task */
	act_tsk(FIFO_TASK1);
	act_tsk(FIFO_TASK2);
	/* Wait for fifo tasks */
	sta_alm(ALM1, ALMTIM);
	chg_pri(TSK_SELF, MID_PRIORITY);

	/* Check each task state */
	ref_tsk(FIFO_TASK1, &tsk1stat);
	ref_tsk(FIFO_TASK2, &tsk2stat);
	if (tsk1stat.tskstat != TTS_WAI || tsk2stat.tskstat != TTS_WAI){
		syslog(LOG_NOTICE, "TASKS is not waiting");
		ext_tsk();
	}
	stp_alm(ALM1);
	chg_pri(TSK_SELF, MAIN_PRIORITY);
	/* Check doscom_sense_process */
	ret = doscom_sense_process(&process);
	if (ret != DOSCOM_SUCCESS || process != 1){
		syslog(LOG_NOTICE, "doscom_sense_process ERROR. connect = %d", process);
		ext_tsk();
	}
#if 0
	ret = doscom_sense_fifoch(&mask);
	if (ret != DOSCOM_SUCCESS || mask != 0x1){
		syslog(LOG_NOTICE, "doscom_sense_fifoch ERROR. mask = %x", mask);
		ext_tsk();
	}
#endif
	ret = tslp_tsk(10000);
	/* 10sec passed*/
	if (ret == E_TMOUT){
		syslog(LOG_NOTICE, "Timeout. Exit testing.");
		ext_tsk();
	}
	ter_tsk(FIFO_TASK1);
	syslog(LOG_NOTICE, "Task1 terminated.");
	ret = tslp_tsk(6000);
	/* 10sec passed*/
	if (ret == E_TMOUT){
		syslog(LOG_NOTICE, "Timeout.Exit testing.");
		ext_tsk();
	}
	ter_tsk(FIFO_TASK2);
	syslog(LOG_NOTICE, "Task2 terminated.");

	/* Check other exit pattern */
	syslog(LOG_NOTICE, "Check Other Exit Pattern");
	act_tsk(FIFO_TASK1);
	act_tsk(FIFO_TASK2);
	/* Wait for fifo tasks */
	sta_alm(ALM1, ALMTIM);
	chg_pri(TSK_SELF, MID_PRIORITY);

	/* Check each task state */
	ref_tsk(FIFO_TASK1, &tsk1stat);
	ref_tsk(FIFO_TASK2, &tsk2stat);
	if (tsk1stat.tskstat != TTS_WAI || tsk2stat.tskstat != TTS_WAI){
		syslog(LOG_NOTICE, "TASKS is not waiting");
		ext_tsk();
	}
	if (connection[0] != 0 || connection[1] != 0){
		syslog(LOG_NOTICE, "cannot restart connection check");
		ext_tsk();
	}
	seq_check(MAIN_SEQ);
	stp_alm(ALM1);
	chg_pri(TSK_SELF, MAIN_PRIORITY);
	ret = tslp_tsk(10000);	
	if (ret == E_TMOUT){
		syslog(LOG_NOTICE, "Timeout.Exit testing.");
		ext_tsk();
	}
	ter_tsk(FIFO_TASK2);
	syslog(LOG_NOTICE, "Task2 terminated.");
	ret = tslp_tsk(6000);
	/* 10sec passed*/
	if (ret == E_TMOUT){
		syslog(LOG_NOTICE, "Timeout.Exit testing.");
		ext_tsk();
	}
	ter_tsk(FIFO_TASK1);
	syslog(LOG_NOTICE, "Task1 terminated.");
	syslog(LOG_NOTICE, "Phase1 End.");

	/* Phase 2 */
	syslog(LOG_NOTICE, "\n--------------------Phase 2--------------------");
	seq_check(MAIN_SEQ);
	/* act FIFO task */
	act_tsk(FIFO_TASK3);
	act_tsk(FIFO_TASK4);

	/* Wait for fifo tasks */
	sta_alm(ALM1, ALMTIM);
	chg_pri(TSK_SELF, MID_PRIORITY);
	/* Check each task state */
	chg_pri(TSK_SELF, MAIN_PRIORITY);
	ref_tsk(FIFO_TASK3, &tsk1stat);
	ref_tsk(FIFO_TASK4, &tsk2stat);
	if (tsk1stat.tskstat != TTS_WAI || tsk2stat.tskstat != TTS_WAI){
		syslog(LOG_NOTICE, "TASKS is not waiting");
		ext_tsk();
	}
	seq_check(MAIN_SEQ);
	stp_alm(ALM1);

	ret = tslp_tsk(10000);
	if (ret == E_TMOUT){
		syslog(LOG_NOTICE, "Timeout.Exit testing.");
		ext_tsk();
	}
	ter_tsk(FIFO_TASK3);
	syslog(LOG_NOTICE, "Task2 terminated.");
	ret = tslp_tsk(6000);
	/* 10sec passed*/
	if (ret == E_TMOUT){
		syslog(LOG_NOTICE, "Timeout.Exit testing.");
		ext_tsk();
	}
	ter_tsk(FIFO_TASK4);
	syslog(LOG_NOTICE, "Task1 terminated.");

	/* Phase 3 */
	syslog(LOG_NOTICE, "\n--------------------Phase 3--------------------");
	seq_check(MAIN_SEQ);
	/* act FIFO task */
	act_tsk(FIFO_TASK5);
	act_tsk(FIFO_TASK6);
	sta_alm(ALM1, ALMTIM);
	chg_pri(TSK_SELF, MID_PRIORITY);
	/* Check each task state */
	chg_pri(TSK_SELF, MAIN_PRIORITY);
	ref_tsk(FIFO_TASK5, &tsk1stat);
	ref_tsk(FIFO_TASK6, &tsk2stat);
	if (tsk1stat.tskstat != TTS_WAI || tsk2stat.tskstat != TTS_WAI){
		syslog(LOG_NOTICE, "TASKS is not waiting");
		ext_tsk();
	}
	seq_check(MAIN_SEQ);
	syslog(LOG_NOTICE, "waiting reconnect.exe 1st connect.");
	stp_alm(ALM1);
	ret = tslp_tsk(5000);
	if (ret == E_TMOUT){
		syslog(LOG_NOTICE, "Timeout.Exit testing.");
		ext_tsk();
	}
	/* APP is exit(do nothing) */	
	ter_tsk(FIFO_TASK5);
	act_tsk(FIFO_TASK5);
	sta_alm(ALM1, ALMTIM);
	chg_pri(TSK_SELF, MID_PRIORITY);
	chg_pri(TSK_SELF, MAIN_PRIORITY);
	ref_tsk(FIFO_TASK5, &tsk1stat);
	ref_tsk(FIFO_TASK6, &tsk2stat);
	if (tsk1stat.tskstat != TTS_WAI || tsk2stat.tskstat != TTS_WAI){
		syslog(LOG_NOTICE, "TASKS is not waiting");
		ext_tsk();
	}
	seq_check(MAIN_SEQ);
	stp_alm(ALM1);
	syslog(LOG_NOTICE, "waiting reconnect.exe 2nd connect.");
	ret = tslp_tsk(5000);
	if (ret == E_TMOUT){
		syslog(LOG_NOTICE, "Timeout.Exit testing.");
		ext_tsk();
	}

	/* APP is exit(read from FIFO) */	
	//ter_tsk(FIFO_TASK6);
	syslog(LOG_NOTICE, "waiting com_fifo_gtr.exe exit.");
	ret = tslp_tsk(5000);
	if (ret == E_TMOUT){
		syslog(LOG_NOTICE, "Timeout.Exit testing.");
		ext_tsk();
	}

	/* Reconnect */
	syslog(LOG_NOTICE, "Check reconnect when other task is nothing.");
	ter_tsk(FIFO_TASK6);
	act_tsk(FIFO_TASK5);
	sta_alm(ALM1, ALMTIM);
	chg_pri(TSK_SELF, MID_PRIORITY);
	chg_pri(TSK_SELF, MAIN_PRIORITY);
	ref_tsk(FIFO_TASK5, &tsk1stat);
	if (tsk1stat.tskstat != TTS_WAI){
		syslog(LOG_NOTICE, "TASK is not waiting");
		ext_tsk();
	}
	seq_check(MAIN_SEQ);
	stp_alm(ALM1);
	ret = tslp_tsk(5000);
	if (ret == E_TMOUT){
		syslog(LOG_NOTICE, "Timeout.Exit testing.");
		ext_tsk();
	}

	/* APP is exit(do nothing) */
	syslog(LOG_NOTICE, "reconnect.exe 1st connect.");
	ter_tsk(FIFO_TASK5);
	act_tsk(FIFO_TASK5);
	sta_alm(ALM1, ALMTIM);
	chg_pri(TSK_SELF, MID_PRIORITY);
	chg_pri(TSK_SELF, MAIN_PRIORITY);
	seq_check(MAIN_SEQ);
	stp_alm(ALM1);
	ret = tslp_tsk(5000);
	if (ret == E_TMOUT){
		syslog(LOG_NOTICE, "Timeout.Exit testing.");
		ext_tsk();
	}

	/* APP is exit(read from FIFO) */	
	syslog(LOG_NOTICE, "reconnect.exe 2nd connect.");
	ter_tsk(FIFO_TASK5);

	/* Phase 4 */
	syslog(LOG_NOTICE, "\n--------------------Phase 4--------------------");
	seq_check(MAIN_SEQ);
	act_tsk(SHMEM_TASK1);
	act_tsk(SHMEM_TASK2);
	sta_alm(ALM1, ALMTIM);
	chg_pri(TSK_SELF, MID_PRIORITY);
	/* Check each task state */
	chg_pri(TSK_SELF, MAIN_PRIORITY);
	ref_tsk(SHMEM_TASK1, &tsk1stat);
	ref_tsk(SHMEM_TASK2, &tsk2stat);
	if (tsk1stat.tskstat != TTS_WAI || tsk2stat.tskstat != TTS_WAI){
		syslog(LOG_NOTICE, "TASKS is not waiting");
		ext_tsk();
	}
	seq_check(MAIN_SEQ);
	stp_alm(ALM1);
	tslp_tsk(10000);

	ter_tsk(SHMEM_TASK1);
	tslp_tsk(5000);

	ter_tsk(SHMEM_TASK2);

	/* Phase 5 */
	connection_init(DOSCOM_SMEMCH_1);
	syslog(LOG_NOTICE, "\n--------------------Phase 5--------------------");
	seq_check(MAIN_SEQ);
	act_tsk(SHMEM_TASK3);
	act_tsk(SHMEM_TASK4);
	sta_alm(ALM1, ALMTIM);
	chg_pri(TSK_SELF, MID_PRIORITY);
	/* Check each task state */
	chg_pri(TSK_SELF, MAIN_PRIORITY);
	ref_tsk(SHMEM_TASK3, &tsk1stat);
	ref_tsk(SHMEM_TASK4, &tsk2stat);
	if (tsk1stat.tskstat != TTS_WAI || tsk2stat.tskstat != TTS_WAI){
		syslog(LOG_NOTICE, "TASKS is not waiting");
		ext_tsk();
	}
	seq_check(MAIN_SEQ);
	stp_alm(ALM1);
	tslp_tsk(12000);
	ter_tsk(SHMEM_TASK3);
	tslp_tsk(5000);
	ter_tsk(SHMEM_TASK4);

	/* Phase 6 */
	syslog(LOG_NOTICE, "\n--------------------Phase 6--------------------");
	syslog(LOG_NOTICE, "RTOS fifo init test Start!");
	seq_check(MAIN_SEQ);
	/* act FIFO task */
	act_tsk(FIFO_TASK8);
	act_tsk(FIFO_TASK7);
	sta_alm(ALM1, ALMTIM);
	chg_pri(TSK_SELF, MID_PRIORITY);
	/* Check each task state */
	chg_pri(TSK_SELF, MAIN_PRIORITY);
	ref_tsk(FIFO_TASK7, &tsk1stat);
	ref_tsk(FIFO_TASK8, &tsk2stat);
	if (tsk1stat.tskstat != TTS_WAI || tsk2stat.tskstat != TTS_WAI){
		syslog(LOG_NOTICE, "TASKS is not waiting");
		ext_tsk();
	}
	seq_check(MAIN_SEQ);
	stp_alm(ALM1);
	ret = tslp_tsk(5000);
	if (ret == E_TMOUT){
		syslog(LOG_NOTICE, "Timeout.Exit testing.");
		ext_tsk();
	}

	/* APP is exit(read from FIFO) */
	ter_tsk(FIFO_TASK7);
	syslog(LOG_NOTICE, "com_fifoch_init.exe exit. FIFO_TASK7 terminated.");
	ret = tslp_tsk(5000);
	if (ret == E_TMOUT){
		syslog(LOG_NOTICE, "Timeout.Exit testing.");
		ext_tsk();
	}

	/* Cyclic APP is exit */
	ter_tsk(FIFO_TASK8);
	syslog(LOG_NOTICE, "com_fifo_rtg.exe exit. FIFO_TASK8 terminated.");
	act_tsk(FIFO_TASK7);
	sta_alm(ALM1, ALMTIM);
	chg_pri(TSK_SELF, MID_PRIORITY);
	/* Check each task state */
	seq_check(MAIN_SEQ);
	stp_alm(ALM1);
	chg_pri(TSK_SELF, MAIN_PRIORITY);
	ret = tslp_tsk(5000);
	if (ret == E_TMOUT){
		syslog(LOG_NOTICE, "Timeout.Exit testing.");
		ext_tsk();
	}
	
	/* Receive Test */
	ter_tsk(FIFO_TASK7);
	syslog(LOG_NOTICE, "GPOS fifo init test Start!");
	seq_check(MAIN_SEQ);
	/* act FIFO task */
	act_tsk(FIFO_TASK8);
	act_tsk(FIFO_TASK9);
	sta_alm(ALM1, ALMTIM);
	chg_pri(TSK_SELF, MID_PRIORITY);
	/* Check each task state */
	chg_pri(TSK_SELF, MAIN_PRIORITY);
	ref_tsk(FIFO_TASK9, &tsk1stat);
	ref_tsk(FIFO_TASK8, &tsk2stat);
	if (tsk1stat.tskstat != TTS_WAI || tsk2stat.tskstat != TTS_WAI){
		syslog(LOG_NOTICE, "TASKS is not waiting");
		ext_tsk();
	}
	seq_check(MAIN_SEQ);
	stp_alm(ALM1);
	ret = tslp_tsk(5000);
	if (ret == E_TMOUT){
		syslog(LOG_NOTICE, "Timeout.Exit testing.");
		ext_tsk();
	}

	/* waiting executing wait_event */
	syslog(LOG_NOTICE, "fifoch_init function called");
	chg_pri(TSK_SELF, MID_PRIORITY);
	chg_pri(TSK_SELF, MAIN_PRIORITY);
	syslog(LOG_NOTICE, "RTOS ready to receive message");
	ret = tslp_tsk(5000);
	if (ret == E_TMOUT){
		syslog(LOG_NOTICE, "Timeout.Exit testing.");
		ext_tsk();
	}

	/* APP is exit(read from FIFO) */
	syslog(LOG_NOTICE, "com_fifoch_init.exe exit");
	ter_tsk(FIFO_TASK9);
	ret = tslp_tsk(5000);
	if (ret == E_TMOUT){
		syslog(LOG_NOTICE, "Timeout.Exit testing.");
		ext_tsk();
	}

	/* Cyclic APP is exit */
	syslog(LOG_NOTICE, "com_fifo_rtg.exe exit");
	ter_tsk(FIFO_TASK8);
	act_tsk(FIFO_TASK9);
	sta_alm(ALM1, ALMTIM);
	chg_pri(TSK_SELF, MID_PRIORITY);
	/* Check each task state */
	seq_check(MAIN_SEQ);
	stp_alm(ALM1);
	chg_pri(TSK_SELF, MAIN_PRIORITY);
	/* waiting fifoch_init execution */
	ret = tslp_tsk(5000);
	if (ret == E_TMOUT){
		syslog(LOG_NOTICE, "Timeout.Exit testing.");
		ext_tsk();
	}
	/* waiting RTOS waitevent */
	syslog(LOG_NOTICE, "fifoch_init function called");
	chg_pri(TSK_SELF, MID_PRIORITY);
	chg_pri(TSK_SELF, MAIN_PRIORITY);
	syslog(LOG_NOTICE, "RTOS ready to receive message");
	ret = tslp_tsk(5000);
	if (ret == E_TMOUT){
		syslog(LOG_NOTICE, "Timeout.Exit testing.");
		ext_tsk();
	}
	ter_tsk(FIFO_TASK9);

	/* End */
	syslog(LOG_NOTICE, "Test End.");
	return;
}
