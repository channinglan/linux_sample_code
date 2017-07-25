/*
 *  TOPPERS Software
 *      Toyohashi Open Platform for Embedded Real-Time Systems
 *      Software
 *
 *  Copyright (C) 2012-2015 by Embedded and Real-Time Systems Laboratory
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
 *  $Id: gpos_app.c 724 2015-03-11 09:57:33Z ertl-honda $
 */

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include "doscom.h"

bool
filter_even(void *buffer, uint32_t size)
{
	uint32_t value;

	value = *((uint32_t *)buffer);

	if ((value % 2) == 0)  return true;
	return false;
}

#define SMEMCH_SMEM_SIZE 64

void
smemch_test() {
	int ret;
	void *p_smem;
	int i;

	printf("SMEM CHANNEL test start!.\n");
	fflush(stdout);

	ret = doscom_smemch_get(DOSCOM_SMEMCH_1, &p_smem);
	if(ret != 0) {
		printf("Get Buffer Error!! %d\n",ret);
		fflush(stdout);
	}

	printf("smem 0x%x.\n", (unsigned int)p_smem);
	fflush(stdout);

	while(1) {
		for(i = 0; i < SMEMCH_SMEM_SIZE/8; i++){
			printf("%d : %d, ", i, *((uint32_t *)p_smem + i));
		}
		printf("\n");
		sleep(2);
	}
}

int
main()
{
	int ret;
	void *p_block;
	doscom_id_t block_id;
	char msg[] = "hey!";

	printf("GPOS doscom_init\n");
	ret = doscom_init(0x03, 0);
	assert(ret == 0);

	printf("Dual-OS communications initialized\n");

	printf("GPOS send start ack\n");
	ret = doscom_fifoch_notify(DOSCOM_FIFOCH_0);
	assert(ret == 0);

	/* phase 1 */
	printf("GPOS wait for message\n");
	ret = doscom_fifoch_wait(DOSCOM_FIFOCH_0, 0);
	assert(ret == 0);

	printf("Event received\n");

	ret = doscom_fifoch_dequeue(DOSCOM_FIFOCH_0, &block_id);
	printf("doscom_fifoch_dequeue() %d\n", ret);
	assert(ret == 0);

	ret = doscom_fifoch_get(DOSCOM_FIFOCH_0, block_id, &p_block);
	assert(ret == 0);

	printf("MSG: %s\n", (char *)p_block);

	printf("GPOS send ack\n");
	ret = doscom_fifoch_notify(DOSCOM_FIFOCH_0);
	assert(ret == 0);

	/* phase 2 */
	printf("GPOS wait for rpc\n");
	ret = doscom_fifoch_wait(DOSCOM_FIFOCH_0, 0);
	assert(ret == 0);

	printf("received params: %u and %u\n",
		   *((uint32_t *)p_block), *((uint32_t *)(p_block + 4)));

	*((uint32_t *)p_block) = *((uint32_t *)p_block) * *((uint32_t *)(p_block + 4));
	printf("GPOS send rpc result (%u)\n", *((uint32_t *)p_block));

	ret = doscom_fifoch_notify(DOSCOM_FIFOCH_0);
	assert(ret == 0);


	/* phase 3 */
	printf("GPOS wait for rpc\n");
	ret = doscom_fifoch_wait(DOSCOM_FIFOCH_0, 0);
	assert(ret == 0);

	printf("received params: %u and %u\n",
		   *((uint32_t *)p_block), *((uint32_t *)(p_block + 4)));

	*((uint32_t *)p_block) = *((uint32_t *)p_block) * *((uint32_t *)(p_block + 4));
	printf("GPOS send rpc result (%u)\n", *((uint32_t *)p_block));

	ret = doscom_fifoch_notify(DOSCOM_FIFOCH_0);
	assert(ret == 0);

	ret = doscom_fifoch_wait(DOSCOM_FIFOCH_0, 0);
	assert(ret == 0);

	/* phase 4 */
	printf("Send a message to the RTOS by fifoch 1\n");
	ret = doscom_fifoch_alloc(DOSCOM_FIFOCH_1, &block_id);
	assert(ret == 0);

	ret = doscom_fifoch_get(DOSCOM_FIFOCH_1, block_id, &p_block);
	assert(ret == 0);

	memcpy(p_block, (void *)msg, sizeof(msg));

	ret = doscom_fifoch_enqueue(DOSCOM_FIFOCH_1, block_id);
	assert(ret == 0);

	ret = doscom_fifoch_notify(DOSCOM_FIFOCH_1);
	assert(ret == 0);

	ret = doscom_fifoch_wait(DOSCOM_FIFOCH_0, 0);
	assert(ret == 0);

	/* phase 5 */
	printf("Send several messages with a filter on\n");

	ret = doscom_fifoch_alloc(DOSCOM_FIFOCH_0, &block_id);
	assert(ret == 0);

	ret = doscom_fifoch_get(DOSCOM_FIFOCH_0, block_id, &p_block);
	assert(ret == 0);

	*((uint32_t *)p_block) = 1;
	printf("call doscom_fifoch_enqueue\n");
	ret = doscom_fifoch_enqueue(DOSCOM_FIFOCH_0, block_id);
	if(ret != 0) printf("data %u filtered\n",*((uint32_t *)p_block));

	ret = doscom_fifoch_alloc(DOSCOM_FIFOCH_0, &block_id);
	assert(ret == 0);

	ret = doscom_fifoch_get(DOSCOM_FIFOCH_0, block_id, &p_block);
	assert(ret == 0);

	*((uint32_t *)p_block) = 2;

	ret = doscom_fifoch_enqueue(DOSCOM_FIFOCH_0, block_id);
	if(ret != 0) printf("data %u filtered\n",*((uint32_t *)p_block));

	ret = doscom_fifoch_alloc(DOSCOM_FIFOCH_0, &block_id);
	assert(ret == 0);

	ret = doscom_fifoch_get(DOSCOM_FIFOCH_0, block_id, &p_block);
	assert(ret == 0);

	*((uint32_t *)p_block) = 3;

	ret = doscom_fifoch_enqueue(DOSCOM_FIFOCH_0, block_id);
	if(ret != 0) printf("data %u filtered\n",*((uint32_t *)p_block));

	ret = doscom_fifoch_notify(DOSCOM_FIFOCH_0);
	assert(ret == 0);

	smemch_test();

	return 0;
}
