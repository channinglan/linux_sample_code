/*
 *  TOPPERS Software
 *      Toyohashi Open Platform for Embedded Real-Time Systems
 *      Software
 *
 *  Copyright (C) 2012-2015 by Embedded and Real-Time Systems Laboratory
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
