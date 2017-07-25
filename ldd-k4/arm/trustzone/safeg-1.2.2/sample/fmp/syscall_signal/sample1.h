#include "target_test.h"

#define MAIN_PRIORITY   5
#define MID_PRIORITY    8
#define LOW_PRIORITY    11

#define TASK_PORTID_G_SYSLOG  1

#define STACK_SIZE      4096

#include "target_common.h"

#define INTNO_SAFEG_T_SIGNAL_1  TARGET_T_INT_SIGNAL_ID_0
#define INHNO_SAFEG_T_SIGNAL_1  (0x00010000 | INTNO_SAFEG_T_SIGNAL_1)
#define INTPRI_T_SIGNAL_1 -20

#define INTNO_SAFEG_NT_SIGNAL_1 TARGET_NT_INT_SIGNAL_ID_0
#define INHNO_SAFEG_NT_SIGNAL_1 (0x00010000 | INTNO_SAFEG_NT_SIGNAL_1)
#define INTPRI_NT_SIGNAL_1 -6

#define INTNO_SAFEG_T_SIGNAL_2  TARGET_T_INT_SIGNAL_ID_1
#define INHNO_SAFEG_T_SIGNAL_2  (0x00020000 | INTNO_SAFEG_T_SIGNAL_2)
#define INTPRI_T_SIGNAL_2 -20

#define INTNO_SAFEG_NT_SIGNAL_2 TARGET_NT_INT_SIGNAL_ID_1
#define INHNO_SAFEG_NT_SIGNAL_2 (0x00020000 | INTNO_SAFEG_NT_SIGNAL_2)
#define INTPRI_NT_SIGNAL_2 -6

#define INTNO_SAFEG_T_SIGNAL_3  TARGET_T_INT_SIGNAL_ID_2
#define INHNO_SAFEG_T_SIGNAL_3  (0x00030000 | INTNO_SAFEG_T_SIGNAL_3)
#define INTPRI_T_SIGNAL_3 -20

#define INTNO_SAFEG_NT_SIGNAL_3 TARGET_NT_INT_SIGNAL_ID_2
#define INHNO_SAFEG_NT_SIGNAL_3 (0x00030000 | INTNO_SAFEG_NT_SIGNAL_3)
#define INTPRI_NT_SIGNAL_3 -6

#define INTNO_SAFEG_T_SIGNAL_4  TARGET_T_INT_SIGNAL_ID_3
#define INHNO_SAFEG_T_SIGNAL_4  (0x00040000 | INTNO_SAFEG_T_SIGNAL_4)
#define INTPRI_T_SIGNAL_4 -20

#define INTNO_SAFEG_NT_SIGNAL_4 TARGET_NT_INT_SIGNAL_ID_3
#define INHNO_SAFEG_NT_SIGNAL_4 (0x00040000 | INTNO_SAFEG_NT_SIGNAL_4)
#define INTPRI_NT_SIGNAL_4 -6

#define CYC_TIME 1000

#ifndef TOPPERS_MACRO_ONLY

extern void main_task(intptr_t exinf);
extern void task(intptr_t exinf);

extern void safeg_signal_handler_1(void);
extern void safeg_signal_handler_2(void);
extern void safeg_signal_handler_3(void);
extern void safeg_signal_handler_4(void);

extern void	cyclic_handler(intptr_t exinf);

#ifdef TOPPERS_SAFEG_SECURE
extern void btask(intptr_t exinf);
#endif /* !defined(TOPPERS_SAFEG_SECURE) */

#endif /* TOPPERS_MACRO_ONLY */
