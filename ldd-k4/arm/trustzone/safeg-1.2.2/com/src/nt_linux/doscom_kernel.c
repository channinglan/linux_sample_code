/*
 *  @(#) $Id: doscom_kernel.c 806 2015-09-24 13:42:11Z ertl-honda $
 */
#include <errno.h>
#include <assert.h>
#include "doscom.h"
#include "doscom_kernel.h"

/*
 * KERNEL INTERFACE
 */
static int fd;
pthread_mutex_t mutex;

int
__doscom_kernel_init(doscom_mask_t fifoch_mask, doscom_time_t timeout, void *sregion)
{
	int ret;
	doscom_init_data_t init_data;

	init_data.num_fifochs	= DOSCOM_NUM_FIFOCHS;
	init_data.num_smemch	= DOSCOM_NUM_SMEMCHS;
	init_data.sregion_pending_events = DOSCOM_SREGION_PENDING_EVENTS;
	pthread_mutex_init(&mutex, NULL);

	errno = 0; /* clear last error */
	ret = ioctl(fd, DOSCOM_IOCTL_CMD_INIT, &init_data);
	if (ret == -1) {
		if(errno == EAGAIN) return DOSCOM_EINIT;
		if(errno == EFAULT) return DOSCOM_ESREGION;
		if(errno == EINVAL) return DOSCOM_EPARAM;
		if(errno == ENOMEM) return DOSCOM_ESREGION;
		assert(0);
	}

	errno = 0; /* clear last error */
	ret = ioctl(fd, DOSCOM_IOCTL_CMD_REGFIFOCH, fifoch_mask);
	if (ret == -1) {
		if(errno == ENOTTY) return DOSCOM_NOINIT;
		if(errno == EALREADY) return DOSCOM_EASSIGN;
		assert(0);
	}

	return 0;
}

int
__doscom_kernel_term(void)
{
	close(fd);
	return 0;
}

/*
 * INIT Shared Region(Memory)
 */
int
__doscom_kernel_sregion_init(void **sregion)
{
	errno = 0; /* clear last error */
	fd = open("/dev/safeg", O_RDWR);
	if (fd < 0) {
		if(errno == EAGAIN) return DOSCOM_EINIT;
		if(errno == EUSERS) return DOSCOM_OPROCESS;
		assert(0);
	}

	errno = 0; /* clear last error */
	*sregion = mmap(NULL, DOSCOM_SREGION_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	if (*sregion == MAP_FAILED){
		if(errno == EAGAIN) return DOSCOM_ESREGION;
		if(errno == EINVAL) return DOSCOM_ESREGION;
		assert(0);
	}
	// Note: the shared memory is zero-ed by the RTOS at initialization

	return 0;
}

int
__doscom_kernel_notify(doscom_id_t fifoch_id)
{
	int ret;

	errno = 0; /* clear last error */
	ret = ioctl(fd, DOSCOM_IOCTL_CMD_NOTIFY, fifoch_id);
	if (ret == -1) {
		if(errno == ENOTTY) return DOSCOM_NOINIT;
		if(errno == EINVAL) return DOSCOM_EPARAM;
		if(errno == EACCES) return DOSCOM_EASSIGN;
		assert(0);
	}

	return 0;
}

int
__doscom_kernel_wait(doscom_id_t fifoch_id, doscom_time_t timeout)
{
	int ret;

	errno = 0; /* clear last error */
	ret = ioctl(fd, DOSCOM_IOCTL_CMD_WAIT, fifoch_id);
	if (ret == -1) {
		if(errno == ENOTTY) return DOSCOM_NOINIT;
		if(errno == EINVAL) return DOSCOM_EPARAM;
		if(errno == EACCES) return DOSCOM_EASSIGN;
		if(errno == ERESTART) return DOSCOM_RESTART;
		assert(0);
	}

	return 0;
}

int
__doscom_kernel_get_fifoch_mask(doscom_mask_t *fifoch_mask)
{
	int ret;

	errno = 0; /* clear last error */
	ret = ioctl(fd, DOSCOM_IOCTL_CMD_GETFIFOCHMASK, fifoch_mask);
	if (ret == -1) {
		if(errno == ENOTTY) return DOSCOM_NOINIT;
		assert(0);
	}

	return 0;
}

int
__doscom_kernel_fifoch_init(const doscom_id_t fifoch_id)
{
	int ret;

	errno = 0; /* clear last error */
	ret = ioctl(fd, DOSCOM_IOCTL_CMD_INITFIFO, fifoch_id);
	if (ret == -1) {
		if(errno == ENOTTY) return DOSCOM_NOINIT;
		if(errno == EINVAL) return DOSCOM_EPARAM;
		if(errno == EACCES) return DOSCOM_EASSIGN;
		assert(0);
	}

	return 0;
}

int
__doscom_kernel_get_num_process(uint32_t *addr)
{
	return 0;
}
