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

bool
filter_even(void *buffer, uint32_t size)
{
	uint32_t value;

	value = *((uint32_t *)buffer);
	printf("filter. *buffer = %d\n", value);

	if ((value % 2) == 0)  return true;
	return false;
}

int main(int argc, char *argv[])
{
	int ret;
	void *buffer;
	doscom_id_t block_id;
	int msg = 0;
	doscom_mask_t mask;
	uint32_t process;
	int channel_mask = atoi(argv[1]);
	int channel_id = 0;
	//FILE *fp = fopen("result.txt", "w");

	printf("This is com_fifo.c\nchannel_mask = %d\n", channel_mask);
	//fprintf(fp, "This is com.c\nchannel_mask = %d\n", channel_mask);
	//fprintf(fp, "GPOS doscom_init\n");

	ret = doscom_init(channel_mask, 0);

	if (ret == DOSCOM_ESREGION) {
		printf("DOSCOM_INIT SHREGION ERROR\n");
		//fprintf(fp, "DOSCOM_INIT SHREGION ERROR\n");
		return 0;
	}
	else if (ret == DOSCOM_TIMEOUT) {
		printf("DOSCOM_INIT KERNEL_INIT ERROR\n");
		//fprintf(fp, "DOSCOM_INIT KERNEL_INIT ERROR\n");
		return 0;
	}
	else if (ret == DOSCOM_EPARAM) {
		printf("DOSCOM_INIT PARAM ERROR\n");
		//fprintf(fp, "DOSCOM_INIT KERNEL_INIT ERROR\n");
		return 0;
	}
	else if (ret != DOSCOM_SUCCESS) {
		printf("DOSCOM_INIT other ERROR %d.\n", ret);
		return 0;
	}
	
	printf("Dual-OS communications initialized\n");
	ret = doscom_sense_process(&process);
	if (ret == -1){
		printf("doscom_sense_process error\n");
		//fprintf(fp, "doscom_sense_process error\n");
		return 0;
	}
	ret = doscom_sense_fifoch(&mask);
	if (ret == -1){
		printf("doscom_sense_fifoch error\n");
		//fprintf(fp, "doscom_sense_fifoch error\n");
		return 0;
	}

	/* message test */
	while(1) {
		channel_mask = atoi(argv[1]);
		channel_id = 0;
		while(channel_mask){
			if (channel_mask & 0x1){
				/* send message to RTOS */
				printf("Send a message to the RTOS by channel %d\n",channel_id);
				ret = doscom_fifoch_alloc(channel_id, &block_id);
				assert(ret == 0);

				ret = doscom_fifoch_get(channel_id, block_id, &buffer);
				assert(ret == 0);

				memcpy(buffer, (void *)(&msg), sizeof(&msg));

				ret = doscom_fifoch_enqueue(channel_id, block_id);
				assert(ret == 0);

				ret = doscom_fifoch_notify(channel_id);
				assert(ret == 0);

				doscom_fifoch_free(channel_id, block_id);
			}
			channel_mask = channel_mask >> 1;
			channel_id++;
		}
		msg++;
		sleep(1);
	}
	return 0;
}
