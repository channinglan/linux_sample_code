/*
 *  TOPPERS/SafeG Dual-OS monitor
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Safety Gate Dual-OS monitor
 *
 *  Copyright (C) 2012-2015 by Embedded and Real-Time Systems Laboratory
 *     Graduate School of Information Science, Nagoya University, JAPAN
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
 *  @(#) $Id: doscom_kernel.c 820 2015-10-08 07:22:03Z ertl-honda $
 */

#include "doscom.h"
#include "doscom_kernel.h"
#include "kernel_cfg.h"

/*
 * CONTROL OFFSET
 */
#define CTL_T2NT_REQ		0x00
#define CTL_NT2T_REQ		0x04
#define CTL_NT2T_REQ_ARG	0x08

#define CTL_NT2T_REQ_PROCESSEXIT	0x01
#define CTL_NT2T_REQ_NOTIFY			0x02
#define CTL_NT2T_REQ_PROCESSOPEN	0x03
#define CTL_NT2T_REQ_REGFIFOCH		0x04
#define CTL_NT2T_REQ_INITFIFOCH		0x05
#define CTL_NT2T_REQ_INIT			0x87654321
#define CTL_T2NT_REQ_INIT			0x12345678
#define CTL_T2NT_REQ_ACK			0x06

#define get_nt2t_req()			(*(uint32_t *)(SREGION_PHYS_ADDRESS + CTL_NT2T_REQ))
#define set_nt2t_req(cmd)		(*(uint32_t *)(SREGION_PHYS_ADDRESS + CTL_NT2T_REQ) = cmd)
#define set_t2nt_req(cmd)		(*(uint32_t *)(SREGION_PHYS_ADDRESS + CTL_T2NT_REQ) = cmd)
#define get_nt2t_req_arg()		(*(uint32_t *)(SREGION_PHYS_ADDRESS + CTL_NT2T_REQ_ARG))

/*
 *  initialized task id;
 */
static ID init_task_id;

/*
 * INTERNAL TYPES
 */
typedef struct {
	ID			rtos_sem;
	ID			rtos_tskid;
	uint32_t	*t2nt_pending;
	uint32_t	*nt2t_pending;
} doscom_fifoch_event_t;


typedef struct {
	bool_t					initialized;
	uint32_t				doscom_num_process;
	doscom_mask_t			doscom_fifoch_mask;
	doscom_fifoch_event_t	fifoch_events[DOSCOM_NUM_FIFOCHS];
} doscom_kernel_t;

/*
 * DEBUG
 */
// #define DOSCOM_DEBUG

#define DEBUG_OUT false

#ifdef DOSCOM_DEBUG

static void
__doscom_debug_print_fifoch_event(doscom_fifoch_event_t *ev)
{
	ER      ercd;
	T_RSEM  rsem;

	syslog(LOG_NOTICE, "fifoch_event->t2nt_pending: 0x%X (val:0x%X)\n", (uint32_t)ev->t2nt_pending, *(ev->t2nt_pending));
	syslog(LOG_NOTICE, "fifoch_event->nt2t_pending: 0x%X (val:0x%X)\n", (uint32_t)ev->nt2t_pending, *(ev->nt2t_pending));

	ercd = ref_sem(ev->rtos_sem, &rsem);
	assert(ercd == E_OK);

	if (rsem.wtskid == TSK_NONE) {
		syslog(LOG_NOTICE, "rtos_sem no task waiting");
	} else {
		syslog(LOG_NOTICE, "rtos_sem task with id %d waiting", rsem.wtskid);
	}
	syslog(LOG_NOTICE, "rtos_sem value: %d", rsem.semcnt);
}

#else /* !DOSCOM_DEBUG */
#define  __doscom_debug_print_fifoch_event(ev)
#endif /* DOSCOM_DEBUG */

/*
 *  GCC call memcpy in case of more than 17 channes.
 */
void
*memcpy(void *dest, const void *src, size_t len)
{
	while (len-- > 0)
    {
		*((char *)dest) = *((char *)src);
		dest++;
		src++;
	}

	return( dest );
}

/*
 * MODULE VARIABLES
 */
static doscom_kernel_t the_doscom_kernel;

static void
__doscom_kernel_fifoch_release_waittask(const doscom_id_t fifoch_id)
{
	doscom_fifoch_event_t *p_fifoch_event = &the_doscom_kernel.fifoch_events[fifoch_id];

	*(p_fifoch_event->nt2t_pending) = 0;
	*(p_fifoch_event->t2nt_pending) = 0;

	if (p_fifoch_event->rtos_tskid != 0){
		DEBUG(DEBUG_OUT, "irel_wai to task %d", p_fifoch_event->rtos_tskid);
		if(sns_ctx()) {
			irel_wai(p_fifoch_event->rtos_tskid);
		}
		else {
			rel_wai(p_fifoch_event->rtos_tskid);
		}
	}
}


/*
 * EVENTS HANDLER
 */
void doscom_handler(void)
{
	ER ercd;
	int i;
	uint32_t mask;
	doscom_fifoch_event_t *p_fifoch_event;

	DEBUG(DEBUG_OUT, "Doscom handler");

	//Initialize
	if (get_nt2t_req() == CTL_NT2T_REQ_INIT){
		DEBUG(DEBUG_OUT, "INIT");
		if (the_doscom_kernel.initialized == false) {
			set_t2nt_req(0);
			ercd = isig_sem(DOSCOM_SEM_INIT);
			assert(ercd == E_OK);
			the_doscom_kernel.initialized = true;
		}
	}
	//NT_OS application has been exit. Restart dualos communications system
	else if(get_nt2t_req()  == CTL_NT2T_REQ_PROCESSEXIT) {
		DEBUG(DEBUG_OUT, "PROCESSEXIT");

		assert(the_doscom_kernel.doscom_num_process > 0);
		the_doscom_kernel.doscom_num_process--;
		DEBUG(DEBUG_OUT, "PROCESSEXIT. num_process = %d", the_doscom_kernel.doscom_num_process);

		/* update mask */
		mask = get_nt2t_req_arg();
		DEBUG(DEBUG_OUT, "PROCESSEXIT. mask 0x%x", mask);
		the_doscom_kernel.doscom_fifoch_mask &= ~mask;
		for (i = 0; mask; i++){
			if (mask & 0x1){
				__doscom_kernel_fifoch_release_waittask(i);
			}
			mask = mask >> 1;
		}
		DEBUG(DEBUG_OUT, "PROCESSEXIT. iras_tex(%d)", init_task_id);
		iras_tex(init_task_id, TASK_EXC_ARG_PROCESSEXIT);
	}
	// signal all fifochs with pending events
	else if(get_nt2t_req() == CTL_NT2T_REQ_NOTIFY) {
		DEBUG(DEBUG_OUT, "NOTIFY");
		for(i=0; i < DOSCOM_NUM_FIFOCHS; i++) {
			p_fifoch_event = &the_doscom_kernel.fifoch_events[i];
			if (*(p_fifoch_event->nt2t_pending) == 0) continue;
			DEBUG(DEBUG_OUT, "NOTIFY : isig_sem");
			isig_sem(p_fifoch_event->rtos_sem);
			*(p_fifoch_event->nt2t_pending) = 0;
		}
	}
	/* called from doscom_user.c in user space */
	else if(get_nt2t_req()  == CTL_NT2T_REQ_PROCESSOPEN) {
		DEBUG(DEBUG_OUT, "PROCESSOPEN");
		the_doscom_kernel.doscom_num_process++;
		DEBUG(DEBUG_OUT, "PROCESSOPEN. num_process = %d", the_doscom_kernel.doscom_num_process);
	}
	else if(get_nt2t_req() == CTL_NT2T_REQ_REGFIFOCH) {
		DEBUG(DEBUG_OUT, "REGFIFOCH");
		mask = get_nt2t_req_arg();
		DEBUG(DEBUG_OUT, "REGFIFOCH. mask 0x%x.", mask);
		the_doscom_kernel.doscom_fifoch_mask |= mask;
	}
	else if(get_nt2t_req() == CTL_NT2T_REQ_INITFIFOCH) {
		DEBUG(DEBUG_OUT, "INITFIFOCH");
		__doscom_kernel_fifoch_init(get_nt2t_req_arg());
	}
	else {
		DEBUG(DEBUG_OUT, "CTL_NT2T_REQ = %u, init = %u", get_nt2t_req(),
			  the_doscom_kernel.initialized);
		assert(0);
	}

	// Ack
	set_nt2t_req(CTL_T2NT_REQ_ACK);
	__doscom_send_irq();
}

void
*memset (void *str, int c, size_t len )
{
	register char *st = (char *)str;
	while (len-- > 0) *st++ = c;
	return (void *)str;
}

/*
 * INIT
 */
int
__doscom_kernel_init(doscom_id_t fifoch_id, doscom_time_t timeout, void *sregion)
{
	ER ret;
	int i;
	doscom_fifoch_event_t *p_fifoch_event;
	ID semaphores[DOSCOM_NUM_FIFOCHS] = DOSCOM_FIFOCH_SEMAPHORES;

	the_doscom_kernel.doscom_num_process = 0;

	get_tid(&init_task_id);

	// 1.- initialize fifoch events
	for(i=0; i<DOSCOM_NUM_FIFOCHS; i++) {
		p_fifoch_event = &the_doscom_kernel.fifoch_events[i];
		p_fifoch_event->rtos_sem = semaphores[i];
		p_fifoch_event->t2nt_pending = (uint32_t *)(sregion + DOSCOM_SREGION_PENDING_EVENTS) + i;
		p_fifoch_event->nt2t_pending = p_fifoch_event->t2nt_pending + DOSCOM_NUM_FIFOCHS;
		__doscom_debug_print_fifoch_event(p_fifoch_event);
	}

	the_doscom_kernel.initialized = false;
	set_t2nt_req(CTL_T2NT_REQ_INIT);

	// Flush D-Cache
	// GPOS(or bootlader) will invalidates the D-Cache.
	// Without flushing D-Cache, the data that has been written to the shared memory
	// will not be write back to the shared memory.
	__doscom_dcache_flush();

	ret = ena_int(DOSCOM_INTNO);
	assert(ret == E_OK);

	ret = twai_sem(DOSCOM_SEM_INIT, timeout);
	if (ret == E_TMOUT) return DOSCOM_TIMEOUT;

	return 0;
}

/*
 * TERM
 */
int
__doscom_kernel_term(void)
{
	int i;

	for(i=0; i < DOSCOM_NUM_FIFOCHS; i++) {
		__doscom_kernel_fifoch_release_waittask(i);
	}

	if(sns_ctx()) {
		iras_tex(init_task_id, TASK_EXC_ARG_TERM);
	}
	else {
		ras_tex(init_task_id, TASK_EXC_ARG_TERM);
	}

	return 0;
}

/*
 * INIT Shared Region(Memory)
 */
int
__doscom_kernel_sregion_init(void **sregion)
{
	*sregion = (void *)SREGION_PHYS_ADDRESS;
	memset(*sregion, 0, DOSCOM_SREGION_SIZE);
	return 0;
}


int
__doscom_kernel_notify(doscom_id_t fifoch_id)
{
	doscom_fifoch_event_t *p_fifoch_event;

	p_fifoch_event = &the_doscom_kernel.fifoch_events[fifoch_id];
	*(p_fifoch_event->t2nt_pending) = 1;
	__doscom_send_irq();

	return 0;
}

int
__doscom_kernel_wait(doscom_id_t fifoch_id, doscom_time_t timeout)
{
	doscom_fifoch_event_t *p_fifoch_event;
	ER ercd;
	ID tskid;

	p_fifoch_event = &the_doscom_kernel.fifoch_events[fifoch_id];

	__doscom_debug_print_fifoch_event(p_fifoch_event);

	if (p_fifoch_event->rtos_tskid != 0) return DOSCOM_WAITOVER;

	get_tid(&tskid);
	p_fifoch_event->rtos_tskid = tskid;
	ercd = twai_sem(p_fifoch_event->rtos_sem, (TMO)timeout);
	p_fifoch_event->rtos_tskid = 0;

	if (ercd == E_TMOUT) return DOSCOM_TIMEOUT;
	if (ercd == E_RLWAI) return DOSCOM_RESTART;

	return 0;
}


int
__doscom_kernel_get_fifoch_mask(doscom_mask_t *p_fifoch_mask)
{
	*p_fifoch_mask = the_doscom_kernel.doscom_fifoch_mask;
	return DOSCOM_SUCCESS;
}

/*
 * Function in doscom_user.c
 */
extern void __doscom_fifoch_init(doscom_id_t fifoch_id);

int
__doscom_kernel_fifoch_init(const doscom_id_t fifoch_id)
{
	doscom_fifoch_event_t *p_fifoch_event;
	p_fifoch_event = &the_doscom_kernel.fifoch_events[fifoch_id];

	DEBUG(DEBUG_OUT, "FIFO INIT", p_fifoch_event->rtos_tskid);

	__doscom_kernel_lock();
	__doscom_fifoch_init(fifoch_id);
	__doscom_kernel_unlock();

	__doscom_kernel_fifoch_release_waittask(fifoch_id);

	*(p_fifoch_event->t2nt_pending) = 2;
	__doscom_send_irq();

	return DOSCOM_SUCCESS;
}

int
__doscom_kernel_get_num_process(uint32_t *p_num_process)
{
	*p_num_process = the_doscom_kernel.doscom_num_process;
	return DOSCOM_SUCCESS;
}
