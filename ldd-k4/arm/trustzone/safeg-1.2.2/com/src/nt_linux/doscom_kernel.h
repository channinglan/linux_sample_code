/*
 *  @(#) $Id: doscom_kernel.h 806 2015-09-24 13:42:11Z ertl-honda $
 */
#ifndef DOSCOM_KERNEL_H
#define DOSCOM_KERNEL_H

#include <unistd.h>    /* ftruncate */
#include <fcntl.h>     /* flags: O_CREAT, etc */
#include <sys/stat.h>  /* modes: S_IRWXU, etc */
#include <sys/mman.h>  /* shm_xxx and mmap() */
#include <semaphore.h> /* sem_xxx */
#include <assert.h>    /* assert() */
#include <string.h>    /* memset */
#include <pthread.h>   /* pthread_xxx */
#include <sys/ioctl.h>
#include <stdio.h>
#include "nt_os-doscom_config.h"

#define DOSCOM_NT

#define DEBUG(enable,x,args...) \
			if(enable) printf("DEBUG (%s): " x, __func__ , ##args)

#define PRINT(x, args...) \
			printf(x, ##args);

/*
 * KERNEL INTERFACE
 */
extern int __doscom_kernel_init(doscom_id_t fifoch_id, doscom_time_t timeout, void *sregion);
extern int __doscom_kernel_term(void);
extern int __doscom_kernel_sregion_init(void **sregion);
extern int __doscom_kernel_notify(doscom_id_t fifoch_id);
extern int __doscom_kernel_wait(doscom_id_t fifoch_id, doscom_time_t timeout);
extern int __doscom_kernel_get_fifoch_mask(doscom_mask_t *p_fifoch_mask);
extern int __doscom_kernel_fifoch_init(const doscom_id_t fifoch_id);
extern int __doscom_kernel_get_num_process(uint32_t *p_numb_process);

static inline bool_t
__doscom_kernel_check_tskctx()
{
	return true;
}

extern pthread_mutex_t mutex;

static inline void
__doscom_kernel_lock()
{
	pthread_mutex_lock(&mutex);
}

static inline void
__doscom_kernel_unlock()
{
	pthread_mutex_unlock(&mutex);
}

#endif /* DOSCOM_KERNEL_H */
