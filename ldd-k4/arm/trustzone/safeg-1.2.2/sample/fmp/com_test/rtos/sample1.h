/*
 *  TOPPERS/ASP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Advanced Standard Profile Kernel
 *
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2004-2013 by Embedded and Real-Time Systems Laboratory
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
 *  $Id: sample1.h 606 2013-11-19 03:08:19Z ertl-honda $
 */

#include "target_test.h"

#define MAIN_PRIORITY	5
#define HIGH_PRIORITY	9
#define MID_PRIORITY	10
#define LOW_PRIORITY	11

#define TASK_PORTID_G_SYSLOG  1

#define STACK_SIZE      4096
#define CYC_TIME        10000
#define ALMTIM         20000
// #define ENABLE_TASK
// #define DISABLE_TIMER
#define DBG_VERBOSE

#define DOSCOM_INTHO  (0x00010000 | DOSCOM_INTNO)

#ifndef TOPPERS_MACRO_ONLY

extern void	task(intptr_t exinf);
extern void	alm_handler(intptr_t exinf);
extern void	main_task(intptr_t exinf);
extern void	main_task_tex_routine(TEXPTN texptn, intptr_t exinf);
extern void	fifo_task_gtr(intptr_t exinf);
extern void	fifo_task_rtg(intptr_t exinf);
extern void	fifo_task_reconnect(intptr_t exinf);
extern void	fifo_task_fifoinit_send(intptr_t exinf);
extern void	fifo_task_fifoinit_receive(intptr_t exinf);
extern void	shmem_task_rtg(intptr_t exinf);
extern void	shmem_task_gtr(intptr_t exinf);
#ifdef TOPPERS_SAFEG_SECURE
extern void     btask(intptr_t exinf);
#endif /* TOPPERS_SAFEG_SECURE */

extern void doscom_handler(void);

#endif /* TOPPERS_MACRO_ONLY */
