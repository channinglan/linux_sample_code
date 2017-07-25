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
