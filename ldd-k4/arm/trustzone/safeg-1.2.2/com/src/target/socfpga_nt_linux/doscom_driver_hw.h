/*
 *  @(#) $Id: doscom_driver_hw.h 724 2015-03-11 09:57:33Z ertl-honda $
 */
#include <linux/version.h>
#include <linux/irq.h>  /* for safeg_raise_softfiq */

//#define ALLOC_DEVICE_MANUALLY

/*
 * ARCHITECTURE
 */
#define __TARGET_ARCH_ARM 7

#if (LINUX_VERSION_CODE == KERNEL_VERSION(3,7,0))
#define DOSCOM_T2NT_IRQ  (135+496)
#define DOSCOM_NT2T_IRQ  (134+496)
#elif (LINUX_VERSION_CODE == KERNEL_VERSION(3,10,31))
#define DOSCOM_T2NT_IRQ  (135)
#define DOSCOM_NT2T_IRQ  (134)
#else
#error unsupported version
#endif

#define TARGET_INT_INIT

#include <asm/irq.h>           /* for set_irq_flags */
#include <asm/mach/irq.h>      /* for set_irq_flags */

static void
target_int_init(void)
{
	irq_set_status_flags(DOSCOM_T2NT_IRQ, IRQ_LEVEL|IRQ_TYPE_LEVEL_HIGH);
}

/*
 * SHARED MEMORY (allocated in kernel parameters)
 */
#define SREGION_PHYS_ADDRESS	0x3F800000UL
#define SREGION_SIZE			0x00800000UL /* 8MB */

#define SAFEG_SMC_SAVE_ALL()									\
	asm volatile (".arch_extension sec\n\t"						\
				"push {r11}\n\t"								\
				"dsb\n\t"										\
				"smc #0\n\t"									\
				"pop {r11}\n\t"									\
				: "=r" (arg_r0)									\
				: "0" (arg_r0), "r" (arg_r1), "r" (arg_r2), "r" (arg_r3)	\
				: "r4", "r5", "r6", "r7", "r8", "r9", "r10", "ip", "lr",	\
				"memory", "cc")

#define SAFEG_SYSCALL_ID__SIGNAL    (3)

static noinline
u32 safeg_syscall_signal(u32 do_switch)
{
	register u32 arg_r0 asm("r0") = SAFEG_SYSCALL_ID__SIGNAL;
	register u32 arg_r1 asm("r1") = do_switch;
	register u32 arg_r2 asm("r2") = 0;
	register u32 arg_r3 asm("r3") = 0;

	SAFEG_SMC_SAVE_ALL();

#if 0
  u32 arg_r0;

  asm volatile ("mov r0, %1 \n\t"
                "mov r1, %2 \n\t"
                "mov r2, %3 \n\t"
                "mov r3, %4 \n\t"
                "dsb\n\t"
                "smc #0\n\t"
                "mov %0, r0 \n\t"
      : "=r" (arg_r0)             
      : "r"(SAFEG_SYSCALL_ID__SIGNAL), "r" (do_switch), "r" (0), "r" (0)
	        : "r0","r1","r2","r3","r4", "r5", "r6", "r7","r8","r9","r10", "memory", "cc");
#endif
	return arg_r0;
}

static void
send_nt2t_irq(void)
{
	u32 err;
	err = safeg_syscall_signal(0);
}

/*
 * MAX VALUES (can be adjusted)
 */
#define MAX_NUM_CHANNELS   1024
#define MAX_NUM_GROUPS     64
#define MAX_PENDING_OFFSET (SHREGION_SIZE - MAX_NUM_CHANNELS*8)
