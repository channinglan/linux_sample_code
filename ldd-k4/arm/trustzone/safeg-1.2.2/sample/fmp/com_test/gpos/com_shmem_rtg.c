/*
 *  TOPPERS Software
 *      Toyohashi Open Platform for Embedded Real-Time Systems
 *      Software
 * 
 *  Copyright (C) 2012-2013 by Embedded and Real-Time Systems Laboratory
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
 *  $Id: gpos_app.c 592 2013-11-17 16:01:02Z ertl-honda $
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include "doscom.h"
#include "syscalls_api.h"

//FILE *fp;

bool
filter_even(void *buffer, uint32_t size)
{
	uint32_t value;

	value = *((uint32_t *)buffer);

	if ((value % 2) == 0)  return true;
	return false;
}

#define SHMEM_SIZE 64

void
share_mem_test(int channel_id, int shmem_id) {
	int ret;
	void *buffer;
	int data, rec_data;

	printf("Share mem test start!.\n");
	fflush(stdout);

	ret = doscom_smemch_get(shmem_id, &buffer);
	if(ret != 0) {
		printf("Get Buffer Error!! %d\n",ret);
		return;
	}
	doscom_fifoch_notify(channel_id);

	/* Data Trunsfer Start */
	data = 0;
	while(1) {
		ret = doscom_fifoch_wait(channel_id, -1);
		if (ret != DOSCOM_SUCCESS){
			printf("SHMEM event_wait error\n");
			return;
		}

		rec_data = *(int *)buffer;
		printf("channel %d receive MSG:%d\n from shmem",channel_id, rec_data);
		if (rec_data != data){
			printf("SHMEM data error. data = %d, rec_data = %d\n", data, rec_data);
			//fprintf(fp,"SHMEM data error. data = %d, rec_data = %d\n", data, rec_data);
		}
		ret = doscom_fifoch_notify(channel_id);
		data++;
	}
}

int main(int argc, char *argv[])
{
	int ret;
	int channel_id = atoi(argv[1]);
	int channel_mask = 1 << channel_id;
	int shmem_id = atoi(argv[2]);
	uint32_t process;
	doscom_mask_t mask;

	//fp = fopen("result.txt", "w");
	printf("This is com_shmem.c\n");
	//fprintf(fp, "This is com_shmem.c\n");
	printf("GPOS doscom_init\n");
	//fprintf(fp, "GPOS doscom_init\n");

	ret = doscom_init(channel_mask, 0);
	if (ret == DOSCOM_ECTX) {
		//fprintf(fp, "DOSCOM_INIT ret = DOSCOM_ECTX error\n");
		assert(ret == 0);
		return 0;
	}	
	else if (ret == DOSCOM_EASSIGN) {
		//fprintf(fp, "DOSCOM_INIT ret = DOSCOM_EASSIGN error\n");
		assert(ret == 0);
		return 0;
	}
	else if (ret != DOSCOM_SUCCESS) {
		//fprintf(fp, "DOSCOM_INIT error\n");
		assert(ret == 0);
		return 0;
	}

	//fprintf(fp, "Dual-OS communications initialized\n");

	ret = doscom_sense_process(&process);
	if (ret != DOSCOM_SUCCESS){
		printf("doscom_sense_process error\n");
		//fprintf(fp, "doscom_sense_process error\n");
		return 0;
	}
	ret = doscom_sense_fifoch(&mask);
	if (ret != DOSCOM_SUCCESS){
		printf("doscom_sense_fifoch error\n");
		//fprintf(fp, "doscom_sense_fifoch error\n");
		return 0;
	}
	/* message test */
	share_mem_test(channel_id, shmem_id);
	return 0;
}
