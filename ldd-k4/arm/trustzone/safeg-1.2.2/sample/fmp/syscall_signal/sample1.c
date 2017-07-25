#include <kernel.h>
#include <t_syslog.h>
#include <t_stdlib.h>
#include <sil.h>
#include "syssvc/serial.h"
#include "syssvc/syslog.h"
#include "kernel_cfg.h"
#include "sample1.h"
#include "syssvc/safeg_load.h"
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
#define WORLD_CHAR "T "
#else /* !TOPPERS_SAFEG_SECURE */
#define WORLD_CHAR "NT"
#endif /* TOPPERS_SAFEG_SECURE */

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

void safeg_t_signal_handler(intptr_t exinf)
{
	uint32_t ret;
	static int cnt[TNUM_PRCID];

	syslog(LOG_EMERG, "PE%d : "WORLD_CHAR" : ISR received signal %d.", exinf, cnt[exinf-1]++);
}

void safeg_signal_handler_1(void)
{
	safeg_t_signal_handler(1);
}

void safeg_signal_handler_2(void)
{
	safeg_t_signal_handler(2);
}

void safeg_signal_handler_3(void)
{
	safeg_t_signal_handler(3);
}

void safeg_signal_handler_4(void)
{
	safeg_t_signal_handler(4);
}

/*
 * Periodic task
 */
void task(intptr_t exinf)
{
	uint32_t ret;

	ret = safeg_syscall_signal();
	if (ret != SAFEG_SYSCALL_ERROR__OK) {
		syslog(LOG_NOTICE, "SafeG syscall ERROR: %u", ret);
	}

	syslog(LOG_EMERG, "PE%d : "WORLD_CHAR" : task Send Signal", exinf);
}

uint_t const task_tskid[TNUM_PRCID] = {
	TASK1,
#if TNUM_PRCID >= 2
	TASK2,
#endif /* TNUM_PRCID >= 2 */
#if TNUM_PRCID >= 3
	TASK3,
#endif /* TNUM_PRCID >= 3 */
#if TNUM_PRCID >= 4
	TASK4,
#endif /* TNUM_PRCID >= 4 */
};

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
 * Main task
 */
void main_task(intptr_t exinf)
{
	ER_UINT ercd;

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
#else
	syslog(LOG_NOTICE, "Hello NT-FMP");
#endif /* TOPPERS_SAFEG_SECURE */

	SVC_PERROR(sta_cyc(CYCHDR1)); // start the cyclic handler

	syslog(LOG_NOTICE, "Main goes to sleep.");
	SVC_PERROR(slp_tsk());
	SVC_PERROR(ext_ker());
	assert(0);
}
