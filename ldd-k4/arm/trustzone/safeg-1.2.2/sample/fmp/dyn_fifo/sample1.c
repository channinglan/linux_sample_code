#include <kernel.h>
#include <t_syslog.h>
#include <t_stdlib.h>
#include <sil.h>
#include "syssvc/serial.h"
#include "syssvc/syslog.h"
#include "kernel_cfg.h"
#include "sample1.h"
#include "syscalls_api.h"

/*
 *  サービスコールのエラーのログ出力
 */
Inline void
svc_perror(const char *file, int_t line, const char *expr, ER ercd)
{
	if (ercd < 0) {
		t_perror(LOG_ERROR, file, line, expr, ercd);
	}
}

#define SVC_PERROR(expr) svc_perror(__FILE__, __LINE__, #expr, (expr))

#ifdef TOPPERS_SAFEG_SECURE
extern int  target_get_nt_start_address();
void btask(intptr_t exinf)
{
	uint32_t ret;
	uint32_t nt_start_address;

	syslog(LOG_NOTICE, "BTASK for Core%d Starts", exinf);

	while((nt_start_address = target_get_nt_start_address()) == -1);

	syslog(LOG_NOTICE, "%d : SafeG First switch to NT : address 0x%x", exinf, nt_start_address);
	ret = safeg_syscall_setntpc(nt_start_address);

	if (ret != SAFEG_SYSCALL_ERROR__OK) {
		syslog(LOG_NOTICE, "%d : SafeG ERROR: setntpc %u", exinf, ret);
		return;
	}

	while(1) {
		SIL_PRE_LOC;

		SIL_LOC_INT();
		safeg_load_idle();
		ret = safeg_syscall_switch();
		SIL_UNL_INT();

		if (ret != SAFEG_SYSCALL_ERROR__OK) {
			syslog(LOG_NOTICE, "%d : SafeG ERROR: switch %u", exinf, ret);
			break;
		}
	}
}
#endif /* TOPPERS_SAFEG_SECURE */

/*
 * Cyclic handler
 */
void cyclic_handler(intptr_t exinf)
{
	SVC_PERROR(iact_tsk(TASK1));
#if TNUM_PRCID >= 2
	SVC_PERROR(iact_tsk(TASK2));
#endif /* TNUM_PRCID >= 2 */
#if TNUM_PRCID >= 3
	SVC_PERROR(iact_tsk(TASK3));
#endif /* TNUM_PRCID >= 3 */
#if TNUM_PRCID >= 4
	SVC_PERROR(iact_tsk(TASK4));
#endif /* TNUM_PRCID >= 4 */
}

/*
 * FIFO buffer
 */
struct fifo_buffer {
	unsigned char *buffer;
	int write_index;
	int read_index;
	int capacity;
	int count;
};

void fifo_buffer_write(struct fifo_buffer *fifo,
					   const void *data,
					   int len,
					   int *written)
{
	int i;
	int overflow;

	overflow = len > (fifo->capacity - fifo->count);

	if (overflow) {
		*written    = fifo->capacity - fifo->count;
		fifo->count = fifo->capacity;
	} else {
		*written    = len;
		fifo->count = fifo->count + len;
	}

	for (i=0; i<*written; i++) {
		fifo->buffer[fifo->write_index] = ((unsigned char *)data)[i];
		fifo->write_index = (fifo->write_index + 1) % fifo->capacity;
	}
}

void fifo_buffer_read(struct fifo_buffer *fifo,
					  void *data,
					  int len,
					  uint32_t *read)
{
	int i;
	unsigned char *buff = (unsigned char *)data;

	if (len > fifo->count) {
		*read = fifo->count;
	} else {
		*read = len;
	}

	for(i=0; i < *read; i++) {
		buff[i] = fifo->buffer[fifo->read_index];
		fifo->read_index = (fifo->read_index + 1) % fifo->capacity;
	}

	fifo->count = fifo->count - *read;
}

unsigned char the_buffer[TNUM_PRCID][100];

struct fifo_buffer the_fifo[TNUM_PRCID];

uint32_t fifo(const uint32_t core_id,
			  const uint32_t ns,
			  const uint32_t a,
			  const uint32_t b,
			  const uint32_t c)
{
	uint32_t *out_data = (uint32_t *)a; /* TODO: b should be validated */
	uint32_t *out_len  = (uint32_t *)b; /* TODO: b should be validated */

	fifo_buffer_read(&(the_fifo[core_id]), out_data, 4, out_len);
	return SAFEG_OK(SAFEG_SYSCALL_RET__DONT_SWITCH);
}

struct safeg_syscall fifo_call = {
	.is_t_callable  = 1,
	.is_nt_callable = 1,
	.name = "fifo",
	.function = fifo
};

uint32_t id;

/*
 * Periodic task
 */
void task(intptr_t exinf)
{
#ifdef TOPPERS_SAFEG_SECURE
	static int cnt[TNUM_PRCID];
	int written;
	fifo_buffer_write(&the_fifo[exinf-1], &cnt[exinf-1], 4, &written);
	syslog(LOG_EMERG, "PE%d : T  : wrote %d", exinf, cnt[exinf-1]++);
#else /* !TOPPERS_SAFEG_SECURE */
	uint32_t data;
	uint32_t len;
	uint32_t ret;
	ret = safeg_syscall_invoke(id, (uint32_t)&data, (uint32_t)&len, 0);
	syslog(LOG_EMERG, "PE%d : NT  : read (ret:%u data:%u len:%u)", exinf, ret, data, len);
#endif /* TOPPERS_SAFEG_SECURE */
}

/*
 * Main task
 */
void main_task(intptr_t exinf)
{
	ER_UINT ercd;
	uint32_t ret;
	int i;

	SVC_PERROR(syslog_msk_log(0, LOG_UPTO(LOG_DEBUG)));
	syslog(LOG_NOTICE, "Simple starts (exinf = %d).", (int_t) exinf);

	ercd = serial_opn_por(TASK_PORTID_G_SYSLOG);
	if (ercd < 0 && MERCD(ercd) != E_OBJ) {
		syslog(LOG_ERROR, "%s (%d) reported by `serial_opn_por'.",
			   itron_strerror(ercd), SERCD(ercd));
	}
	SVC_PERROR(serial_ctl_por(TASK_PORTID_G_SYSLOG,
							  (IOCTL_CRLF | IOCTL_FCSND | IOCTL_FCRCV)));

#ifdef TOPPERS_SAFEG_SECURE
	syslog(LOG_NOTICE, "Hello T-FMP");
	for(i = 0; i < TNUM_PRCID; i++) {
		(the_fifo[i]).buffer = the_buffer[i];
		(the_fifo[i]).capacity = sizeof(the_buffer[i]);
	}
	ret = safeg_syscall_regdyn(&fifo_call, &id);
	syslog(LOG_NOTICE, "Syscall registered: (ret:%u, id:%u)", ret, id);
#else
	syslog(LOG_NOTICE, "Hello NT-FMP");
	ret = safeg_syscall_getid("fifo", &id);
	syslog(LOG_NOTICE, "Get id: (ret:%u, id:%u)", ret, id);
#endif /* TOPPERS_SAFEG_SECURE */

	SVC_PERROR(sta_cyc(CYCHDR1)); // start the cyclic handler

	syslog(LOG_NOTICE, "Main goes to sleep.");
	SVC_PERROR(slp_tsk());
	SVC_PERROR(ext_ker());
	assert(0);
}
