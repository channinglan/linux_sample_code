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
