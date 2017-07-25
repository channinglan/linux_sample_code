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
 *  @(#) $Id: syscalls_api.h 765 2015-09-02 07:34:28Z ertl-honda $
 */
#ifndef _SAFEG_SDK_SYSCALLS_API_H_
#define _SAFEG_SDK_SYSCALLS_API_H_

/**********************************************************************
 * [Static system calls API]                                          *
 * Definitions for static system calls (installed at initialization). *
 **********************************************************************/
/*
 * Error definitions.
 *
 * OK: The system call executed succesfully without errors.
 * PARAMS: One or more parameters contains an invalid value.
 * PERM: The system call cannot be called from the current TrustZone world.
 * INEXISTENT: The system call does not exist.
 * FULL: Memory for storing dynamic system calls is full.
 */
#define SAFEG_SYSCALL_ERROR__OK             (0x0000000)
#define SAFEG_SYSCALL_ERROR__PARAMS         (0x0000001)
#define SAFEG_SYSCALL_ERROR__PERM           (0x0000002)
#define SAFEG_SYSCALL_ERROR__INEXISTENT     (0x0000003)
#define SAFEG_SYSCALL_ERROR__FULL           (0x0000004)

/*
 * GETID: obtains the ID of a dynamic system call by name.
 *
 * Params:
 *   [IN] @name: the name of the dynamic system call (max. 8 characters).
 *   [OUT] @id: the returned ID
 *
 * Initial permissions: callable by T and NT OS.
 * Switching: doesn't switch.
 * Return values: OK, PARAMS, PERM, INEXISTENT.
 */
extern uint32_t safeg_syscall_getid(const char *name, uint32_t *id);

/*
 * SETPERM: set the permissions for a static or dynamic system call.
 *
 * Params:
 *   [IN] @id: the system call ID.
 *   [IN] @is_t_callable: if '1' (true) the call can be called by the T OS.
 *   [IN] @is_nt_callable: if '1' (true) the call can be called by the NT OS.
 *
 * Initial permissions: callable by the T OS.
 * Switching: doesn't switch.
 * Return values: OK, PARAMS, PERM, INEXISTENT.
 */
extern uint32_t safeg_syscall_setperm(uint32_t id,
                                      uint32_t is_t_callable,
                                      uint32_t is_nt_callable);

/*
 * SWITCH: initiates a switch to the opposite world.
 *
 * Params: none
 *
 * Initial permissions: callable by T and NT OS.
 * Switching: switches unless there is an error.
 * Return values: OK, PARAMS, PERM, INEXISTENT.
 */
extern uint32_t safeg_syscall_switch(void);

/*
 * SIGNAL: signals an interrupt to the opposite world.
 *
 * Params: none  
 *
 * Initial permissions: callable by T and NT OS.
 * Return values: OK, PARAMS, PERM, INEXISTENT.
 */
extern uint32_t safeg_syscall_signal(void);

/***************************************************
 * [Dynamic system calls API]                      *
 * Definitions related to the dynamic system calls.*
 ***************************************************/
/*
 * Definition of a system call for registration through safeg_syscall_reg().
 */
struct safeg_syscall {
    uint32_t is_t_callable;
    uint32_t is_nt_callable;
    uint8_t  name[8];
    uint32_t (*function)(uint32_t core_id, uint32_t ns,
                         uint32_t a, uint32_t b, uint32_t c);
};

/*
 * Return value (switch [31:28] and error[27:0])
 *
 * Defines the possible return values for a dynamic system call.
 *
 * - If there is an error, the monitor will return control to the caller.
 *   Dynamic system calls should return an error value using the
 *   SAFEG_ERROR macro.
 *
 * - If no errors occur, the system call can decide the world to switch
 *   using the SAFEG_OK macro.
 */
/* Switch values (0..0xF) */
#define SAFEG_SYSCALL_RET__DONT_SWITCH     (0x0)
#define SAFEG_SYSCALL_RET__SWITCH_TO_T     (0x1)
#define SAFEG_SYSCALL_RET__SWITCH_TO_NT    (0x2)

/*
 * Error macros.
 */
#define SAFEG_RETVAL(switch, error) (((switch) << 28) | (error))
#define SAFEG_ERROR(x) SAFEG_RETVAL(SAFEG_SYSCALL_RET__DONT_SWITCH, (x))
#define SAFEG_OK(x)    SAFEG_RETVAL(x, SAFEG_SYSCALL_ERROR__OK)

/*
 * REGDYN: register a dynamic system call.
 *
 * Params:
 *   [IN] @call: the system call and initial parameters.
 *   [OUT] @id: the allocated id.
 *
 * Initial permissions: callable by the T OS (only primary core).
 * Switching: doesn't switch.
 * Return values: OK, PARAMS, PERM, INEXISTENT, FULL.
 */
extern uint32_t safeg_syscall_regdyn(const struct safeg_syscall *call,
                                     uint32_t *id);

/*
 * Invoke a dynamic system call.
 *
 * Params:
 *   [IN] @id: the ID of the dynamic system call to invoke.
 *
 * Initial permissions: depends on the system call id.
 * Switching: depends on the system call id.
 * Return values: depends on the system call id.
 */
extern uint32_t safeg_syscall_invoke(uint32_t id,
                                     uint32_t a,
                                     uint32_t b,
                                     uint32_t c);

/**************************************************
 * [Notifiers API]                                *
 * Definitions related to the notifier callbacks. *
 **************************************************/
#define NOTIFIER_CHAIN_ID_FIQ    (0)
#define     NOTIFIER_EVENT_ID_FIQ_ENTER             (0)
#define     NOTIFIER_EVENT_ID_FIQ_T_EXIT            (1)
#define     NOTIFIER_EVENT_ID_FIQ_NT_EXIT           (2)
#define NOTIFIER_CHAIN_ID_SMC    (1)
#define     NOTIFIER_EVENT_ID_SMC_ENTER             (0)
#define     NOTIFIER_EVENT_ID_SMC_DONT_SWITCH_EXIT  (1)
#define     NOTIFIER_EVENT_ID_SMC_SWITCH_TO_NT_EXIT (2)
#define     NOTIFIER_EVENT_ID_SMC_SWITCH_TO_T_EXIT  (3)
#define NOTIFIER_CHAIN_ID_IRQ    (2)
#define     NOTIFIER_EVENT_ID_IRQ_ENTER             (0)
#define     NOTIFIER_EVENT_ID_IRQ_NT_EXIT           (1)
#define NOTIFIER_CHAIN_ID_PANIC  (3)
#define     NOTIFIER_EVENT_ID_PANIC_IRQ_IN_T        (0)
#define     NOTIFIER_EVENT_ID_PANIC_IRQ_IN_NT       (1)
#define     NOTIFIER_EVENT_ID_PANIC_PREFETCH        (2)
#define     NOTIFIER_EVENT_ID_PANIC_DATA            (3)

/*
 * Possible return values for notifier callbacks
 */
#define NOTIFIER_CONTINUE 0  /* continue with next calls in the chain*/
#define NOTIFIER_STOP     1  /* stop executing next calls in the chain */

/*
 * Notifier params for events:
 *
 *  NOTIFIER_EVENT_ID_PANIC_PREFETCH
 *  NOTIFIER_EVENT_ID_PANIC_DATA
 *
 * (-) +----------------+
 *     |   IFAR, IFSR   |
 *     +----------------+
 *     |   DFAR, DFSR   |
 *     +----------------+
 *     | FIQ spsr,r8-lr |
 *     +----------------+
 *     | IRQ spsr-sp-lr |
 *     +----------------+
 *     | UND spsr-sp-lr |
 *     +----------------+
 *     | ABT spsr-sp-lr |
 *     +----------------+
 *     | SVC spsr-sp-lr |
 *     +----------------+
 *     |    USR r0-lr   |
 *     +----------------+
 *     |      SCR       |
 *     +----------------+
 *     |     PC + 8     |
 *     +----------------+
 *     |     CPSR       |
 * (+) +----------------+
 *
 */
struct notifier_params_panic_abort {
    uint32_t cp15_ifar;
    uint32_t cp15_ifsr;
    uint32_t cp15_dfar;
    uint32_t cp15_dfsr;
    uint32_t fiq_spsr;
    uint32_t fiq_r8;
    uint32_t fiq_r9;
    uint32_t fiq_r10;
    uint32_t fiq_r11;
    uint32_t fiq_r12;
    uint32_t fiq_sp;
    uint32_t fiq_lr;
    uint32_t irq_spsr;
    uint32_t irq_sp;
    uint32_t irq_lr;
    uint32_t und_spsr;
    uint32_t und_sp;
    uint32_t und_lr;
    uint32_t abt_spsr;
    uint32_t abt_sp;
    uint32_t abt_lr;
    uint32_t svc_spsr;
    uint32_t svc_sp;
    uint32_t svc_lr;
    uint32_t usr_r0;
    uint32_t usr_r1;
    uint32_t usr_r2;
    uint32_t usr_r3;
    uint32_t usr_r4;
    uint32_t usr_r5;
    uint32_t usr_r6;
    uint32_t usr_r7;
    uint32_t usr_r8;
    uint32_t usr_r9;
    uint32_t usr_r10;
    uint32_t usr_r11;
    uint32_t usr_r12;
    uint32_t usr_sp;
    uint32_t usr_lr;
    uint32_t cp15_scr;
    uint32_t pc_plus_8;
    uint32_t cpsr;
} __attribute__ ((packed));

/*
 * Definition of a notifier for registration.
 */
struct safeg_notifier {
    uint32_t chain_id;
    uint32_t (*function)(uint32_t core_id,
                         uint32_t ns,
                         uint32_t event_id,
                         void     *notifier_params,
                         void     *user_params);
    void  *user_params;
};

/*
 * REGNOT: register a notifier callback.
 *
 * Params:
 *   [IN] @notifier: the notifier callback and the params pointer.
 *   [OUT] @id: the allocated id.
 *
 * Initial permissions: callable by the T OS (only by the primary core).
 * Switching: doesn't switch.
 * Return values: OK, PARAMS, PERM, INEXISTENT, FULL.
 */
extern uint32_t safeg_syscall_regnot(const struct safeg_notifier *notifier,
                                     uint32_t *id);

/*
 * READL: read specified address
 *
 * Params:
 *   [IN] @address: target address to read
 *   [OUT] @rdata_ref: the read data
 *
 * Initial permissions: callable by T and NT OS.
 * Switching: doesn't switch.
 * Return values: OK, PARAMS
 */
extern uint32_t safeg_syscall_readl(uint32_t address, uint32_t *rdata_ref);

/*
 * WRITEL: write specified address
 *
 * Params:
 *   [IN] @address: target address to write
 *   [IN] @wdata: write data
 *
 * Initial permissions: callable by T and NT OS.
 * Switching: doesn't switch.
 * Return values: OK, PARAMS
 */
extern uint32_t safeg_syscall_writel(uint32_t address, uint32_t wdata);


/*
 * RESTARNT: restart NT OS
 *
 * Params:
 *
 * Initial permissions: callable by T OS.
 * Switching: doesn't switch.
 * Return values: OK, PARAMS
 */
extern uint32_t safeg_syscall_restarnt(void);

/*
 * SETNTPC: set NT OS PC(swith address).
 *
 * Params: pc
 *
 * Initial permissions: callable by T OS.
 * Switching: doesn't switch.
 * Return values: OK
 */
extern uint32_t safeg_syscall_setntpc(uint32_t pc);

#endif /* _SAFEG_SDK_SYSCALLS_API_H_ */
