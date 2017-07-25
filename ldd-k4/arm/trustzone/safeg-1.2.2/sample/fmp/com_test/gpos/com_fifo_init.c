/*
 *  TOPPERS Software
 *      Toyohashi Open Platform for Embedded Real-Time Systems
 *      Software
 * 
 *  Copyright (C) 2012-2013 by Embedded and Real-Time Systems Laboratory
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
