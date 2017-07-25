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

bool
filter_even(void *buffer, uint32_t size)
{
	uint32_t value;

	value = *((uint32_t *)buffer);

	if ((value % 2) == 0)  return true;
	return false;
}

int main(int argc, char *argv[])
{
	int ret;
	void *buffer;
	doscom_id_t block_id;
	int channel_mask = atoi(argv[1]);
	int channel_id = 0;
	int rec_flag = atoi(argv[2]);
	//FILE *fp = fopen("result.txt", "w");
	char msg[32] = "fifo init";

	printf("This is com_fifo_init.exe %d  %d\n", atoi(argv[1]), atoi(argv[2]));
	////fprintf(fp, "This is com_fifo_init.exe\n");
	ret = doscom_init(channel_mask, 0);

	if (ret == DOSCOM_ECTX) {
		printf("DOSCOM_INIT ret = DOSCOM_ECTX error\n");
		////fprintf(fp, "DOSCOM_INIT ret = DOSCOM_ECTX error\n");
		assert(ret == 0);
		return 0;
	}	
	else if (ret == DOSCOM_EASSIGN) {
		printf("DOSCOM_INIT ret = DOSCOM_EASSIGN error\n");
		////fprintf(fp, "DOSCOM_INIT ret = DOSCOM_EASSIGN error\n");
		assert(ret == 0);
		return 0;
	}
	else if (ret != DOSCOM_SUCCESS) {
		printf("DOSCOM_INIT error %d\n", ret);
		////fprintf(fp, "DOSCOM_INIT error\n");
		assert(ret == 0);
		return 0;
	}
	printf("Dual-OS communications initialized\n");
	////fprintf(fp, "Dual-OS communications initialized\n");

	/* RTOS to GPOS */
	if (!rec_flag){
		while(channel_mask){
			if (channel_mask & 0x1){
				/* message test */
				ret = doscom_fifoch_dequeue(channel_id, &block_id);
				//printf("doscom_block_dequeue() %d\n", ret);
				assert(ret == 0);

				ret = doscom_fifoch_get(channel_id, block_id, &buffer);
				assert(ret == 0);

				/* Verify Message */
				printf("rec_data = %s\n", (char *)buffer);
				if (strcmp((char *)buffer, "fifo init 2nd")){
					printf("Invalid data received. rec_data = %s\n", (char *)buffer);
					while(1){}
				}
				channel_mask = channel_mask >> 1;
				channel_id++;
			}
		}
	}
	else if (rec_flag){
		while(channel_mask){
			if (channel_mask & 0x1){
				/* first time */
				ret = doscom_fifoch_alloc(channel_id, &block_id);
				assert(ret == 0);

				ret = doscom_fifoch_get(channel_id, block_id, &buffer);
				assert(ret == 0);

				memcpy(buffer, (void *)(msg), sizeof(msg));

				ret = doscom_fifoch_enqueue(channel_id, block_id);
				assert(ret == 0);

				/* initialize fifo */
				doscom_fifoch_init(channel_id);

				/* second time */
				strcpy(msg, "fifo init 2nd");
				ret = doscom_fifoch_alloc(channel_id, &block_id);
				assert(ret == 0);

				ret = doscom_fifoch_get(channel_id, block_id, &buffer);
				assert(ret == 0);

				memcpy(buffer, (void *)(msg), sizeof(msg));

				ret = doscom_fifoch_enqueue(channel_id, block_id);
				assert(ret == 0);
				printf("GPOS send event\n");

				/* Event Send. It is correct if RTOS can read fifo init 2nd msg */
				ret = doscom_fifoch_notify(channel_id);
				assert(ret == 0);

			}
			channel_mask = channel_mask >> 1;
			channel_id++;
		}
	}
	return 0;
}
