#include "target_test.h"
#include "syssvc/safeg_load.h"

#define MAIN_PRIORITY   5
#define MID_PRIORITY    8
#define LOW_PRIORITY    11

#define TASK_PORTID_G_SYSLOG  1

#define STACK_SIZE      4096
#define CYC_TIME        2000

#ifndef TOPPERS_MACRO_ONLY

extern void     task(intptr_t exinf);
extern void     main_task(intptr_t exinf);
extern void     cyclic_handler(intptr_t exinf);

#ifdef TOPPERS_SAFEG_SECURE
extern void     btask(intptr_t exinf);
#endif /* TOPPERS_SAFEG_SECURE */

#endif /* TOPPERS_MACRO_ONLY */
