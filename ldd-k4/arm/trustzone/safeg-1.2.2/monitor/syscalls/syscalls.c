/*
 *  TOPPERS/SafeG Dual-OS monitor
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Safety Gate Dual-OS monitor
 *
 *  Copyright (C) 2009-2015 by Embedded and Real-Time Systems Laboratory
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
 * [syscalls.c]
 *
 * This file contains the implementation of the system calls module.
 * There is a total 6 static system calls, through which the T-OS
 * can register additional (dynamic) system calls. Registration of
 * dynamic system calls and notifiers must be done sequentially. The
 * T-OS must make sure that they are not called in parallel by different
 * cores (e.g., through a T-OS spinlock or by restricting that task to
 * the primary cores). Registered system calls and notifiers cannot be
 * unregistered at present in order to avoid possible racing conditions
 * and security leaks (e.g., if a NT-callable system call is replaced
 * on another core just before execution by a non NT-callable system call).
 * The inclusion of a spinlock inside the monitor for addressing this
 * issue is left for future work.
 *
 * @(#) $Id: syscalls.c 765 2015-09-02 07:34:28Z ertl-honda $
 */
#include "types.h"        /* uint32_t, uint8_t, UNUSED */
#include "target.h"       /* TARGET_XXX, target_xxx */
#include "arm.h"          /* arm_xxx */
#include "syscalls.h"     /* SYSCALL_XXX */
#include "notifier.h"     /* notifier_xxx */
#include "utils.h"        /* string_xxx */
#include "debug.h"        /* DEBUG */
#include "monitor.h"      /* MONITOR */

/* Memory used for the monitor to store T/NT contexts and its own stack (in start.S) */
extern uint32_t monitor_space[];

#if (MONITOR_ENABLE_SYSCALLS == 1)

/**************************************************************
 * [Static calls prototypes]                                  *
 * Declare static call prototypes for insertion in the table. *
 **************************************************************/
static uint32_t syscall_getid_function(uint32_t, uint32_t, uint32_t,
                                       uint32_t, uint32_t);

static uint32_t syscall_setperm_function(uint32_t, uint32_t, uint32_t,
                                         uint32_t, uint32_t);

static uint32_t syscall_switch_function(uint32_t, uint32_t, uint32_t,
                                        uint32_t, uint32_t);

static uint32_t syscall_signal_function(uint32_t, uint32_t, uint32_t,
                                        uint32_t, uint32_t);

static uint32_t syscall_regdyn_function(uint32_t, uint32_t, uint32_t,
                                        uint32_t, uint32_t);

static uint32_t syscall_regnot_function(uint32_t, uint32_t, uint32_t,
                                        uint32_t, uint32_t);

static uint32_t syscall_readl_function(uint32_t, uint32_t, uint32_t,
                                        uint32_t, uint32_t);

static uint32_t syscall_writel_function(uint32_t, uint32_t, uint32_t,
                                        uint32_t, uint32_t);

static uint32_t syscall_restarnt_function(uint32_t, uint32_t, uint32_t,
                                        uint32_t, uint32_t);

static uint32_t syscall_setntpc_function(uint32_t, uint32_t, uint32_t,
                                        uint32_t, uint32_t);

/*************************************************************
 * [System calls table]                                      *
 * All system calls are stored in a single fixed-size table. *
 *************************************************************/
#define NUM_STATIC_SYSCALLS  (10)
#define NUM_SYSCALLS (NUM_STATIC_SYSCALLS + MONITOR_MAX_DYN_SYSCALLS)

/* System call structure */
#define SYSCALL_NAME_LENGTH (8)
struct syscall {
    uint32_t is_used;
    uint32_t is_t_callable;
    uint32_t is_nt_callable;
    uint8_t  name[SYSCALL_NAME_LENGTH];
    uint32_t (*function)(uint32_t core_id,
                         uint32_t ns,
                         uint32_t a,
                         uint32_t b,
                         uint32_t c);
};

/*
 * Wrapper for declaring a static system call.
 */
#define STATIC_SYSCALL(suffix, t, nt)               \
{                                                   \
    .is_used        = true,                         \
    .is_t_callable  = (t),                          \
    .is_nt_callable = (nt),                         \
    .name           = #suffix,                      \
    .function       = syscall_##suffix##_function   \
}

static struct syscall the_syscalls[NUM_SYSCALLS] = {
    STATIC_SYSCALL(getid,   true, true),    // id=0
    STATIC_SYSCALL(setperm, true, false),   // id=1
    STATIC_SYSCALL(switch,  true, true),    // id=2
    STATIC_SYSCALL(signal,  true, true),    // id=3
    STATIC_SYSCALL(regdyn,  true, false),   // id=4
    STATIC_SYSCALL(regnot,  true, false),   // id=5
    STATIC_SYSCALL(readl,   true, true),    // id=6
    STATIC_SYSCALL(writel,  true, true),    // id=7
    STATIC_SYSCALL(restarnt,true, false),   // id=8
    STATIC_SYSCALL(setntpc, true, false),   // id=9
};

/********************************************************************
 * [System calls initialization]                                    *
 * Initialize the system calls library. Called from monitor_init.c. *
 ********************************************************************/
/*
 * Syscalls library initialization
 */
void syscalls_init(void)
{
#if (MONITOR_OMIT_BSS_INIT == 1)
    for(int i=NUM_STATIC_SYSCALLS; i<NUM_SYSCALLS; i++)
        the_syscalls[i].is_used = false;
#endif /* (MONITOR_OMIT_BSS_INIT == 1) */
}

/************************************************
 * [Static system calls]                        *
 * System calls registered at compilation time. *
 ************************************************/
/*
 * GETID: obtains the ID of a system call.
 */
static uint32_t syscall_getid_function(uint32_t UNUSED(core_id),
                                       uint32_t ns,
                                       uint32_t string_ref,
                                       uint32_t id_ref,
                                       uint32_t UNUSED(c))
{
    uint32_t i;

    /* validate the system call parameters */
#if (MONITOR_OMIT_T_CHECKS == 1)
    if (ns == 1)
#endif /* (MONITOR_OMIT_T_CHECKS == 1) */

    if (!target_is_valid_memory(ns, id_ref, 4) ||
        !target_is_valid_memory(ns, string_ref, SYSCALL_NAME_LENGTH))
            return SYSCALL_ERROR(ERROR__PARAMS);

    /* look up the string in the system calls table */
    for(i = 0; i < NUM_SYSCALLS; i++)
        if (string_is_equal(the_syscalls[i].name,
                            (uint8_t *)string_ref,
                            SYSCALL_NAME_LENGTH))
            break;

    if (i == NUM_SYSCALLS)
        return SYSCALL_ERROR(ERROR__INEXISTENT);

    /* return the id corresponding to the string */
    *((uint32_t *)id_ref) = i;

    return SYSCALL_OK(SYSCALL_RET__DONT_SWITCH);
}

/*
 * SETPERM: set permissions for a certain system call.
 */
static uint32_t syscall_setperm_function(uint32_t UNUSED(core_id),
                                         uint32_t UNUSED(ns),
                                         uint32_t id,
                                         uint32_t is_t_callable,
                                         uint32_t is_nt_callable)
{
    struct syscall *syscall;

#if (MONITOR_OMIT_T_CHECKS == 0)
    if (id >= NUM_SYSCALLS)
        return SYSCALL_ERROR(ERROR__PARAMS);
#endif /* (MONITOR_OMIT_T_CHECKS == 0) */

    syscall = &the_syscalls[id];

#if (MONITOR_OMIT_T_CHECKS == 0)
    if (!syscall->is_used)
        return SYSCALL_ERROR(ERROR__INEXISTENT);
#endif  /* (MONITOR_OMIT_T_CHECKS == 0) */

    syscall->is_t_callable  = is_t_callable;
    syscall->is_nt_callable = is_nt_callable;

    return SYSCALL_OK(SYSCALL_RET__DONT_SWITCH);
}

/*
 * SWITCH: initiates a switch to the opposite world.
 */
static uint32_t syscall_switch_function(uint32_t UNUSED(core_id),
                                        uint32_t ns,
                                        uint32_t UNUSED(a),
                                        uint32_t UNUSED(b),
                                        uint32_t UNUSED(c))
{
    if (ns == 1)
        return SYSCALL_OK(SYSCALL_RET__SWITCH_TO_T);
    else
        return SYSCALL_OK(SYSCALL_RET__SWITCH_TO_NT);
}

/*
 * SIGNAL: signals an interrupt to the opposite world.
 */
static uint32_t syscall_signal_function(uint32_t core_id,
                                        uint32_t ns,
                                        uint32_t UNUSED(a),
                                        uint32_t UNUSED(b),
                                        uint32_t UNUSED(c))
{
    target_signal(core_id, ns);
    return SYSCALL_OK(SYSCALL_RET__DONT_SWITCH);
}

/*
 * REGDYN: register a dynamic system call.
 */
struct syscall_user {
    uint32_t is_t_callable;
    uint32_t is_nt_callable;
    uint8_t  name[8];
    uint32_t (*function)(uint32_t core_id, uint32_t ns,
                         uint32_t a, uint32_t b, uint32_t c);
};

static uint32_t syscall_regdyn_function(uint32_t UNUSED(core_id),
                                        uint32_t ns,
                                        uint32_t call_ref,
                                        uint32_t id_ref,
                                        uint32_t UNUSED(c))
{
    uint32_t tmp_id;
    struct syscall_user *call;

#if (MONITOR_OMIT_T_CHECKS == 0)
    if (!target_is_valid_memory(ns, id_ref, sizeof(uint32_t)) ||
        !target_is_valid_memory(ns, call_ref, sizeof(struct syscall_user)))
        return SYSCALL_ERROR(ERROR__PARAMS);
#endif /* (MONITOR_OMIT_T_CHECKS == 0) */

    /* Get a free syscall id */
    for(tmp_id=0; tmp_id<NUM_SYSCALLS; tmp_id++) {
        if (!the_syscalls[tmp_id].is_used) break;
    }

    /* Return error if no slots available */
    if (tmp_id == NUM_SYSCALLS) return SYSCALL_ERROR(ERROR__FULL);

    /* copy the system call information */
    call = (struct syscall_user *)call_ref;
    the_syscalls[tmp_id].is_t_callable  = call->is_t_callable;
    the_syscalls[tmp_id].is_nt_callable = call->is_nt_callable;
    the_syscalls[tmp_id].function       = call->function;
    string_copy(the_syscalls[tmp_id].name, call->name, SYSCALL_NAME_LENGTH);

    /* Flag it as used after a memory barrier to avoid race conditions */
    ARM_DATA_MEMORY_BARRIER;
    the_syscalls[tmp_id].is_used = true;

    /* Set the id */
    *((uint32_t *)id_ref) = tmp_id;

    return SYSCALL_OK(SYSCALL_RET__DONT_SWITCH);
}


/*
 * REGNOT: register a notifier call.
 */
static uint32_t syscall_regnot_function(uint32_t UNUSED(core_id),
                                        uint32_t ns,
                                        uint32_t notifier,
                                        uint32_t id_ref,
                                        uint32_t UNUSED(c))
{
    uint32_t ret;

    DEBUG(MONITOR_DEBUG_SYSCALLS, "syscall_regnot_function\n\r");

#if (MONITOR_OMIT_T_CHECKS == 0)
    if (!target_is_valid_memory(ns, id_ref, sizeof(uint32_t)) ||
        !target_is_valid_memory(ns, notifier, sizeof(struct notifier_user)))
        return SYSCALL_ERROR(ERROR__PARAMS);
#endif /* (MONITOR_OMIT_T_CHECKS == 0) */

    ret = notifier_register((struct notifier_user *)notifier,
                            (uint32_t *)id_ref);

    if (ret != ERROR__OK)
        return SYSCALL_ERROR(ret);

    return SYSCALL_OK(SYSCALL_RET__DONT_SWITCH);
}

/*************************************************************
 * [System call invoked]                                     *
 * Called from monitor.S when the T or NT OS execute an SMC. *
 *************************************************************/
uint32_t syscall_invoked(uint32_t id, uint32_t a, uint32_t b, uint32_t c)
{
    struct syscall *call;
    uint32_t ns, core_id, ret;
#if (MONITOR_ENABLE_NOTIFIERS == 1)
    struct notifier_smc_enter_params params = {id, a, b, c};
#endif /* (MONITOR_ENABLE_NOTIFIERS == 1) */

    /* send notification */
    notifier_call(NOTIFIER_CHAIN_ID_SMC,
                  NOTIFIER_EVENT_ID_SMC_ENTER,
                  (void *)&params);

    /* get which world the request came from */
    CP15_SCR_READ(ns);
    ns = ns & SCR_NS;

    /* get which core are we */
    core_id = arm_get_core_id();

#if (MONITOR_OMIT_T_CHECKS == 1)
    if (ns == 0) {
        call = &the_syscalls[id];
        ret = call->function(core_id, ns, a, b, c);
        goto final;
    }
#endif /* (MONITOR_OMIT_T_CHECKS == 1) */

    /* parameter checking */
    if (id >= NUM_SYSCALLS) {
        call = 0;
        ret = SYSCALL_ERROR(ERROR__PARAMS);
        goto final;
    }

    call = &the_syscalls[id];

    if (!call->is_used) {
        ret = SYSCALL_ERROR(ERROR__INEXISTENT);
        goto final;
    }

    if ( ((!call->is_nt_callable) && (ns == 1)) ||
         ((!call->is_t_callable)  && (ns == 0)) )
    {
        ret = SYSCALL_ERROR(ERROR__PERM);
        goto final;
    }

    /* call the appropriate system call function */
    ret = call->function(core_id, ns, a, b, c);

final:
    return ret;
}

/*
 * READL: read specified address
 */
static uint32_t syscall_readl_function(uint32_t core_id,
                                        uint32_t ns,
                                        uint32_t address,
                                        uint32_t rdata_ref,
                                        uint32_t UNUSED(c))
{
    if (!target_is_valid_memory(ns, rdata_ref, 4)) {
        return SYSCALL_ERROR(ERROR__PARAMS);
    }

    if (!target_is_valid_readl_address(core_id, ns, address)) {
        return SYSCALL_ERROR(ERROR__PARAMS);
    }

    *((uint32_t *)rdata_ref) = *((volatile uint32_t *)(address));

    return SYSCALL_OK(SYSCALL_RET__DONT_SWITCH);
}

/*
 * WRITEL: write specified address
 */
static uint32_t syscall_writel_function(uint32_t core_id,
                                        uint32_t ns,
                                        uint32_t address,
                                        uint32_t wdata,
                                        uint32_t UNUSED(c))
{
    if (!target_is_valid_writel_address(core_id, ns, address)) {
        return SYSCALL_ERROR(ERROR__PARAMS);
    }

    *((volatile uint32_t *)(address)) = wdata;

    return SYSCALL_OK(SYSCALL_RET__DONT_SWITCH);
}

uint32_t *test_pc;
uint32_t *test_cpsr;

/* cp15 ctrl init value */
#define NT_CP15_CTRL_INIT_VALUE 0x50078

/*
 * RESTARNT: restart NT OS
 */
static uint32_t syscall_restarnt_function(uint32_t  core_id,
                                        uint32_t UNUSED(ns),
                                        uint32_t UNUSED(a),
                                        uint32_t UNUSED(b),
                                        uint32_t UNUSED(c))
{
	uint32_t *nt_context_base;
	uint32_t scr;

	/*
	 *  Set PC and CPSR
	 */
	/* Get target core context base */
	nt_context_base = monitor_space + (MON_SPACE_BYTES_TOTAL - (core_id << MON_SPACE_SHIFT)
									   - MON_SPACE_BYTES_PERCORE)/4;
	/* set PC */
	test_pc = &(nt_context_base[((MON_NT_CONTEXT_SIZE_BYTES/4) - 2)]);
	nt_context_base[(MON_NT_CONTEXT_SIZE_BYTES/4) - 2] = TARGET_NT_OS_START_ADDRESS;
	/* set CPSR */
	test_cpsr = &(nt_context_base[((MON_NT_CONTEXT_SIZE_BYTES/4) - 1)]);
	nt_context_base[(MON_NT_CONTEXT_SIZE_BYTES/4) - 1] = MON_NT_CPSR_INIT_VALUE;

	/* To access NT cp15 registers */
	CP15_SCR_READ(scr);
	CP15_SCR_WRITE(scr|SCR_NS);
	/* Disable MMU and Cache  */
	CP15_CTRL_WRITE(NT_CP15_CTRL_INIT_VALUE);

	/* Back to s T cp15 registers */
	CP15_SCR_WRITE(scr);

	return SYSCALL_OK(SYSCALL_RET__DONT_SWITCH);
}

/*
 * SETNTPC: set NT OS PC(swith address)
 */
static uint32_t syscall_setntpc_function(uint32_t  core_id,
                                        uint32_t UNUSED(ns),
                                        uint32_t pc,
                                        uint32_t UNUSED(b),
                                        uint32_t UNUSED(c))
{
	uint32_t *nt_context_base;

	/* Get target core context base */
	nt_context_base = monitor_space + (MON_SPACE_BYTES_TOTAL - (core_id << MON_SPACE_SHIFT)
									   - MON_SPACE_BYTES_PERCORE)/4;
	/* set PC */
	nt_context_base[(MON_NT_CONTEXT_SIZE_BYTES/4) - 2] = pc;

	return SYSCALL_OK(SYSCALL_RET__DONT_SWITCH);
}

#else /* (MONITOR_ENABLE_SYSCALLS == 0) */

/*
 * When MONITOR_ENABLE_SYSCALLS is false, all system calls are disabled.
 * However, we allow the T-OS to trigger a switch to the NT-OS.
 */
#if (MONITOR_ENABLE_NOTIFIERS == 1)
uint32_t syscall_invoked(uint32_t id, uint32_t a, uint32_t b, uint32_t c)
#else /* (MONITOR_ENABLE_NOTIFIERS == 0) */
uint32_t syscall_invoked(uint32_t UNUSED(id),
                         uint32_t UNUSED(a),
                         uint32_t UNUSED(b),
                         uint32_t UNUSED(c))
#endif /* (MONITOR_ENABLE_NOTIFIERS == 0) */
{
    uint32_t ret, ns;
#if (MONITOR_ENABLE_NOTIFIERS == 1)
    struct notifier_smc_enter_params params = {id, a, b, c};
#endif /* (MONITOR_ENABLE_NOTIFIERS == 1) */

    /* send notification */
    notifier_call(NOTIFIER_CHAIN_ID_SMC,
                  NOTIFIER_EVENT_ID_SMC_ENTER,
                  (void *)&params);

    /* get which world the request came from */
    CP15_SCR_READ(ns);
    ns = ns & SCR_NS;

    if (ns == 0)
        ret = SYSCALL_OK(SYSCALL_RET__SWITCH_TO_NT);
    else
        ret = SYSCALL_ERROR(ERROR__INEXISTENT);

    return ret;
}

#endif /* (MONITOR_ENABLE_SYSCALLS == 0) */
