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

#define MAX_CHANNELS 32


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
share_mem_test() {
	int ret;
	void *buffer;
	int i;

	printf("Share mem test start!.\n");
	fflush(stdout);

	ret = doscom_smemch_get(DOSCOM_SMEMCH_1, &buffer);
	if(ret != 0) {
		printf("Get Buffer Error!! %d\n",ret);
		fflush(stdout);
	}

	while(1) {
		for(i = 0; i < SHMEM_SIZE/8; i++){
			printf("%d : %d, ", i, *((uint32_t *)buffer + i));
		}
		printf("\n");
		sleep(2);
	}
}

int main(int argc, char *argv[])
{
	int i;
	volatile int ret;
	void *buffer;
	doscom_id_t block_id;
	int channel_mask = atoi(argv[1]);
	int channel_id = 0;
	//FILE *fp = fopen("result.txt", "w");
	int data[MAX_CHANNELS], rec_data;

	printf("This is com_fifo_rtg.c\nchannel_mask = %d\n", channel_mask);
	//fprintf(fp, "This is com.c\nchannel_id = %d\n", channel_id);
	//fprintf(fp, "GPOS doscom_init\n");

	ret = doscom_init(channel_mask, 0);

	if (ret == DOSCOM_ECTX) {
		printf("DOSCOM_INIT ret = DOSCOM_ECTX error! ret = %d\n", ret);
		//fprintf(fp, "DOSCOM_INIT ret = DOSCOM_ECTX error\n");
		assert(ret == 0);
		return 0;
	}	
	else if (ret == DOSCOM_EASSIGN) {
		printf("DOSCOM_INIT ret = DOSCOM_EASSIGN error ret = %d\n", ret);
		//fprintf(fp, "DOSCOM_INIT ret = DOSCOM_EASSIGN error\n");
		assert(ret == 0);
		return 0;
	}
	else if (ret != DOSCOM_SUCCESS) {
		printf("DOSCOM_INIT error ret = %d\n", ret);
		//fprintf(fp, "DOSCOM_INIT error\n");
		assert(ret == 0);
		return 0;
	}

	printf("Dual-OS communications initialized\n");

	for(i = 0; channel_mask >> i ; i++){
		if ((channel_mask >> i) & 0x1) data[i] = 0;
	}

	/* message test */
	while(1) {
		channel_mask = atoi(argv[1]);
		channel_id = 0;
		while(channel_mask){
			if (channel_mask & 0x1){
				/* receive message from RTOS */
				printf("GPOS wait for message\n");
				do{
					ret = doscom_fifoch_dequeue(channel_id, &block_id);
				}while(ret != DOSCOM_SUCCESS);
				printf("Event received from channel %d\n", channel_id);

				ret = doscom_fifoch_get(channel_id, block_id, &buffer);
				assert(ret == 0);

				/* Verify Message */
				rec_data = *(int *)buffer;
				printf("channel %d receive data : %d\n", channel_id, rec_data);
				if (rec_data != data[channel_id]){
					printf("Invalid data received. data = %d, rec_data = %d\n", data[channel_id], *(int *)buffer);
					break;
				}

				//printf("GPOS send ack\n");
				ret = doscom_fifoch_notify(channel_id);
				assert(ret == 0);
				data[channel_id]++;
			}
			channel_mask = channel_mask >> 1;
			channel_id++;
		}
		sleep(1);
	}
	return 0;
}
