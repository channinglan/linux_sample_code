/*
 * SAFEG Dual-OS Communications Driver for Linux
 *
 * Copyright (C) Nagoya University
 * Daniel Sangorrin <daniel.sangorrin@gmail.com>
 * Shinya Honda     <honda@ertl.jp>  
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 *  @(#) $Id: doscom.c 759 2015-07-30 13:22:42Z ertl-honda $
 */
#include <linux/kernel.h>
#include <linux/module.h>      /* for MODULE_AUTHOR..*/
#include <linux/init.h>        /* for module_init */
#include <asm/io.h>            /* for virt_to_phys, ioremap */
#include <linux/semaphore.h>
#include <linux/wait.h>        /* for wait queues */
#include <linux/sched.h>
#include <linux/interrupt.h>   /* for request_irq */
#include <linux/fs.h>          /* for alloc_chrdev_region */
#include <linux/cdev.h>        /* for cdev_alloc */
#include <linux/dma-mapping.h> /* for dma_alloc_coherent */
#include <linux/ioctl.h>
#include <asm/bitops.h>        /* for test_and_set_bit */
#include <linux/slab.h>        /* for kmalloc/kfree */
#include <asm/uaccess.h>       /* for copy_to/from_user */
#include <linux/miscdevice.h>  /* for misc_register */
#include "doscom_driver_hw.h"  /* for shared memory and interrupts */

/*
 * DEBUG
 */
#ifndef DEBUG
#define DEBUG(enable,x,args...)  \
		if(enable) printk(KERN_INFO "" x, ##args)
#endif

#define DBG_INIT		false
#define DBG_OPEN		false
#define DBG_CALLS		false
#define DBG_REGFIFOCH	false
#define DBG_NOTIFY		false
#define DBG_RELEASE		false
#define DBG_NT2TREQ		false
#define DBG_IRQ			false

/*
 * MAX VALUES (can be adjusted)
 */
#define MAX_NUM_FIFOCHS		32
#define MAX_NUM_PROCESS		32
#define MAX_FIFOCH_PENDING_OFFSET	(SREGION_SIZE - MAX_NUM_FIFOCHS*8)

/*
 * CONTROL OFFSET
 */
#define CTL_T2NT_REQ		0x00
#define CTL_NT2T_REQ		0x04
#define CTL_NT2T_REQ_ARG	0x08

/*
 * CONTROL VAL
 */
#define CTL_NT2T_REQ_PROCESSEXIT	0x01
#define CTL_NT2T_REQ_NOTIFY			0x02
#define CTL_NT2T_REQ_PROCESSOPEN	0x03
#define CTL_NT2T_REQ_REGFIFOCH		0x04
#define CTL_NT2T_REQ_INITFIFOCH		0x05
#define CTL_NT2T_REQ_INIT			0x87654321
#define CTL_T2NT_REQ_INIT			0x12345678
#define CTL_T2NT_REQ_ACK			0x06

/*
 * CONTROL MACRO
 */
#define set_nt2t_req(cmd)		(*(uint32_t *)(sregion + CTL_NT2T_REQ) = cmd)
#define get_t2nt_req()			(*(uint32_t *)(sregion + CTL_T2NT_REQ))
#define get_nt2t_req()			(*(uint32_t *)(sregion + CTL_NT2T_REQ))
#define set_nt2t_req_arg(arg)	(*(uint32_t *)(sregion + CTL_NT2T_REQ_ARG) = arg)
#define get_nt2t_req_arg()		(*(uint32_t *)(sregion + CTL_NT2T_REQ_ARG))

/*
 * FIFOCH T2NT request
 */
#define FIFOCH_T2NT_NONE  0
#define FIFOCH_T2NT_INIT  2

/*
 *  CHECK_MACRO
 */
#define CHECK_INITIALIZED() do {				\
	if (!initialized) {							\
		return -ENOTTY; /* DOSCOM_NOINIT */		\
	}											\
} while(false)

#define CHECK_T_OS_INITIALIZED() do {				\
	if (get_t2nt_req() != CTL_T2NT_REQ_INIT) {		\
		return -EAGAIN; /* DOSCOM_EINIT */			\
	}												\
} while(false)

#define CHECK_FIFOCH_ID(id) do {					\
	if (id >= the_doscom_kernel.num_fifochs) {		\
		return -EINVAL; /* DOSCOM_EPARAM */			\
	}												\
} while(false)

#define CHECK_OWN_FIFOCH(id) do {							\
	if (the_doscom_kernel.fifoch_pid[id] != current->pid){	\
		return -EACCES; /*  DOSCOM_EASSIGN */				\
	}														\
} while(false)


/*
 * MODULE TYPES
 */
typedef struct {
	uint32_t	num_fifochs;				// number of fifo channels
	uint32_t	num_smemchs;				// number of smem channels
	uint32_t	sregion_pending_events;		// offset in shared memory to pending events
} doscom_init_data_t;

typedef struct {
	uint32_t			id;				// fifo channel id
	uint32_t			*t2nt_pending;	// pointer to shared regsion
	uint32_t			*nt2t_pending;	// pointer to shared regsion
	uint32_t			wercd;			// wait ercd
	wait_queue_head_t	fifoch_wq;		// wait queue for channels
} doscom_fifoch_event_t;

typedef struct {
	uint32_t				num_fifochs;					// number of fifo channels
	doscom_fifoch_event_t	*fifoch_events;					// array of channel events
	uint32_t				fifoch_mask;					// mask of assigned channel
	pid_t					fifoch_pid[MAX_NUM_FIFOCHS];	// pid of each channel
	uint32_t				num_process;					// number of processs to connect this driver
	bool					nt2t_req_ack;					//
	wait_queue_head_t		nt2t_req_ack_wq;				// wait queue for channels
} doscom_kernel_t;

/*
 * MODULE VARIABLE DECLARATIONS
 *	  initialized: flag to indicate if ioctl init was already called
 *	  the_init_sem: semaphore used to protect the driver
 *	  sregion: shared memory pointer (kernel virtual address)
 *	  the_doscom_kernel: big structure containing all the information  
 *	  major: device major number (necessary when unregistering the device)
 *	  safeg_cdev: character device (necessary when removing the device)
 */
static bool initialized = false;
static DEFINE_SEMAPHORE(the_doscom_kernel_sem);
static void *sregion;
static doscom_kernel_t the_doscom_kernel;

#ifdef ALLOC_DEVICE_MANUALLY
static int major;
static struct cdev safeg_cdev;
#endif /* ALLOC_DEVICE_MANUALLY */


static void
send_nt2t_req(uint32_t req_no, uint32_t arg)
{
	DEBUG(DBG_NT2TREQ, "Send NT2T Reauest no %d, arg 0x%x\n", req_no, arg);
	set_nt2t_req(req_no);
	set_nt2t_req_arg(arg);

	init_waitqueue_head(&(the_doscom_kernel.nt2t_req_ack_wq));
	the_doscom_kernel.nt2t_req_ack = false;
	send_nt2t_irq();
	wait_event_interruptible(the_doscom_kernel.nt2t_req_ack_wq,
							 the_doscom_kernel.nt2t_req_ack == true);
}

/*
 * OPEN
 */
static int
safeg_open(struct inode *inode, struct file *filp)
{
	if (!initialized) {
		CHECK_T_OS_INITIALIZED();
	}

	down(&the_doscom_kernel_sem);

	DEBUG(DBG_OPEN, "OPEN called\n");

	if (the_doscom_kernel.num_process >= MAX_NUM_PROCESS) {
		DEBUG(DBG_INIT, "Number of proess is over.\n");
		up(&the_doscom_kernel_sem);
		return -EUSERS; /* DOSCOM_OPROCESS */
	}

	the_doscom_kernel.num_process++;

	send_nt2t_req(CTL_NT2T_REQ_PROCESSOPEN, 0);

	up(&the_doscom_kernel_sem);

	return 0;
}

/*
 * RELEASE
 */
static int
safeg_release(struct inode *inode, struct file *filp)
{
	pid_t pid;
	int loop;
	uint32_t fifoch_mask = 0;

	down(&the_doscom_kernel_sem);

	DEBUG(DBG_RELEASE, "RELEASE called\n");

	/* search assigned fifo channel from pid */
	pid = current->pid;
	for (loop = 0; loop < MAX_NUM_FIFOCHS; loop++){
		if(pid == the_doscom_kernel.fifoch_pid[loop]){
			fifoch_mask |= (1 << loop);
			the_doscom_kernel.fifoch_pid[loop] = 0;
		}
	}

	DEBUG(DBG_RELEASE, "RELEASE fifoch_mask 0x%x\n", fifoch_mask);

	/* update fifoch_mask */
	the_doscom_kernel.fifoch_mask &= ~fifoch_mask;

	the_doscom_kernel.num_process--;

	/* Send process exit event to T_OS */
	send_nt2t_req(CTL_NT2T_REQ_PROCESSEXIT, fifoch_mask);

	up(&the_doscom_kernel_sem);

	return 0;
}

/*
 * MMAP
 */
static int
safeg_mmap(struct file *filp, struct vm_area_struct *vma)
{
	DEBUG(DBG_CALLS, "MMAP called\n");
	DEBUG(DBG_CALLS, "start%d , end%d\n", (int)vma->vm_start, (int)(vma->vm_end));

	if ((vma->vm_end - vma->vm_start) > SREGION_SIZE) {
		return -EINVAL; /* DOSCOM_ESREGION */
	}

#if __TARGET_ARCH_ARM == 6
	// note: pgprot_noncached is possible too
	vma->vm_page_prot = pgprot_dmacoherent(vma->vm_page_prot);
#endif

	if (remap_pfn_range(vma,
						vma->vm_start,
						SREGION_PHYS_ADDRESS >> PAGE_SHIFT,
						vma->vm_end - vma->vm_start,
						vma->vm_page_prot)) {
		return -EAGAIN; /* DOSCOM_ESREGION */
	}

	return 0;
}

/*
 * IOCTLs
 */

static int
doscom_ioctl_init(doscom_init_data_t __user *user_init_data)
{
	doscom_init_data_t init_data;
	doscom_fifoch_event_t *p_fifoch_event;
	int loop;

	if (initialized) {
		DEBUG(DBG_INIT, "Doscom was already initialized\n");
		return 0;
	}

	CHECK_T_OS_INITIALIZED();

	DEBUG(DBG_INIT, "Get init data from user space\n");

	if (copy_from_user(&init_data, user_init_data, sizeof(init_data))) {
		return -EFAULT; /* DOSCOM_ESREGION */
	}

	DEBUG(DBG_INIT, "num_fifochs:%u, sregion_pending:0x%X \n",
					init_data.num_fifochs,
					init_data.sregion_pending_events);

	if ((init_data.num_fifochs > MAX_NUM_FIFOCHS) ||
		(init_data.sregion_pending_events > MAX_FIFOCH_PENDING_OFFSET)) {
		DEBUG(DBG_INIT, "Bad init_data parameters\n");
		return -EINVAL; /* DOSCOM_EPARAM */
	}

	the_doscom_kernel.num_fifochs = init_data.num_fifochs;
	if (init_data.num_fifochs > 0) {
		the_doscom_kernel.fifoch_events = kmalloc(
			init_data.num_fifochs*sizeof(doscom_fifoch_event_t), GFP_KERNEL);
		if (!the_doscom_kernel.fifoch_events) {
			return -ENOMEM; /* DOSCOM_ESREGION */
		}
		DEBUG(DBG_INIT, "Initialize channel events\n");
		for(loop=0; loop<init_data.num_fifochs; loop++) {
			p_fifoch_event = &the_doscom_kernel.fifoch_events[loop];
			p_fifoch_event->id = loop;
			p_fifoch_event->t2nt_pending = (uint32_t *)(sregion + init_data.sregion_pending_events) + loop;
			DEBUG(DBG_INIT, "p_fifoch_event->t2nt_pending: 0x%X (val:0x%X)\n",
				  (uint32_t)p_fifoch_event->t2nt_pending, *(p_fifoch_event->t2nt_pending));
			p_fifoch_event->nt2t_pending = p_fifoch_event->t2nt_pending + init_data.num_fifochs;
			DEBUG(DBG_INIT, "p_fifoch_event->nt2t_pending: 0x%X (val:0x%X)\n",
				  (uint32_t)p_fifoch_event->nt2t_pending, *(p_fifoch_event->nt2t_pending));
			init_waitqueue_head(&p_fifoch_event->fifoch_wq);
		}
	}

	initialized = true;

	send_nt2t_req(CTL_NT2T_REQ_INIT, 0);

	return 0;
}

static int
doscom_ioctl_regfifoch(uint32_t fifoch_mask)
{
	int loop;
	uint32_t mask;

	CHECK_INITIALIZED();

	if (!fifoch_mask){
		return 0;
	}

	/* already used */
	if ((the_doscom_kernel.fifoch_mask & fifoch_mask) != 0) {
		DEBUG(DBG_REGFIFOCH, "Used channel is selected\n");
		return -EALREADY; /* DOSCOM_EASSIGN */
	}

	/* set mask value */
	the_doscom_kernel.fifoch_mask = the_doscom_kernel.fifoch_mask | fifoch_mask;

	/*
	 * set pid (theses are used at safeg_release )
	 */
	mask = fifoch_mask;
	for (loop = 0; fifoch_mask != 0; loop++){
		if (fifoch_mask & 0x1){
			the_doscom_kernel.fifoch_pid[loop] = current->pid;
			DEBUG(DBG_REGFIFOCH, "fifo channel %d's  PID = %d\n", loop, current->pid);
		}
		fifoch_mask = fifoch_mask >> 1;
	}

	send_nt2t_req(CTL_NT2T_REQ_REGFIFOCH, mask);

	return 0;
}

static int
doscom_ioctl_notify(uint32_t fifoch_id)
{
	doscom_fifoch_event_t *p_fifoch_event;

	CHECK_INITIALIZED();
	CHECK_FIFOCH_ID(fifoch_id);
	CHECK_OWN_FIFOCH(fifoch_id);

	p_fifoch_event = &the_doscom_kernel.fifoch_events[fifoch_id];
	*(p_fifoch_event->nt2t_pending) = 1;

	/* Caution : This value is channel id (not mask)*/
	send_nt2t_req(CTL_NT2T_REQ_NOTIFY, fifoch_id);

	return 0;
}

static int
doscom_ioctl_wait(uint32_t fifoch_id)
{
	doscom_fifoch_event_t *p_fifoch_event;

	CHECK_INITIALIZED();
	CHECK_FIFOCH_ID(fifoch_id);
	CHECK_OWN_FIFOCH(fifoch_id);

	p_fifoch_event = &the_doscom_kernel.fifoch_events[fifoch_id];

	p_fifoch_event->wercd = FIFOCH_T2NT_NONE;
	up(&the_doscom_kernel_sem);
	wait_event_interruptible(p_fifoch_event->fifoch_wq,
							 (*(p_fifoch_event->t2nt_pending) != FIFOCH_T2NT_NONE) ||
							 (p_fifoch_event->wercd != FIFOCH_T2NT_NONE));
	down(&the_doscom_kernel_sem);
	*(p_fifoch_event->t2nt_pending) = FIFOCH_T2NT_NONE;

	if (p_fifoch_event->wercd == FIFOCH_T2NT_INIT) {
		return -ERESTART; /* DOSCOM_RESTART */
	}

	return 0;
}

static int
doscom_ioctl_get_fifoch_mask(uint32_t addr)
{
	CHECK_INITIALIZED();

	*((unsigned int *)addr) = the_doscom_kernel.fifoch_mask;

	return 0;
}

static int
doscom_ioctl_fifoch_init(uint32_t fifoch_id)
{
	CHECK_INITIALIZED();
	CHECK_FIFOCH_ID(fifoch_id);
	CHECK_OWN_FIFOCH(fifoch_id);

	send_nt2t_req(CTL_NT2T_REQ_INITFIFOCH, fifoch_id);

	return 0;
}

// command numbers 'j' 00-3F   linux/joystick.h
#define DOSCOM_IOCTL_CMD_INIT			  _IO('j', 0x40)
#define DOSCOM_IOCTL_CMD_NOTIFY			 _IOW('j', 0x41, int)
#define DOSCOM_IOCTL_CMD_WAIT			 _IOW('j', 0x42, int)
#define DOSCOM_IOCTL_CMD_REGFIFOCH		 _IOW('j', 0x44, int)
#define DOSCOM_IOCTL_CMD_GETFIFOCHMASK	 _IOW('j', 0x45, int)
#define DOSCOM_IOCTL_CMD_INITFIFO		 _IOW('j', 0x46, int)

static long
safeg_ioctl(struct file *file,
			unsigned int cmd,
			unsigned long arg)
{
	long ret;
	DEBUG(DBG_CALLS, "IOCTL called (cmd:%u, arg:%lu)\n", cmd, arg);

	down(&the_doscom_kernel_sem);
	switch(cmd) {
	  case DOSCOM_IOCTL_CMD_INIT:
		DEBUG(DBG_INIT, "DOSCOM_IOCTL_CMD_INIT(%u)\n", cmd);
		ret = doscom_ioctl_init((doscom_init_data_t *)arg);
		break;
	  case DOSCOM_IOCTL_CMD_NOTIFY:
		DEBUG(DBG_CALLS, "DOSCOM_IOCTL_CMD_NOTIFY(%u)\n", cmd);
		ret = doscom_ioctl_notify((uint32_t)arg);
		break;
	  case DOSCOM_IOCTL_CMD_WAIT:
		DEBUG(DBG_CALLS,
			  "DOSCOM_IOCTL_CMD_WAIT(cmd:%u arg:%lu)\n", cmd, arg);
		ret = doscom_ioctl_wait((uint32_t)arg);
		break;
	  case DOSCOM_IOCTL_CMD_REGFIFOCH:
		DEBUG(DBG_CALLS, "DOSCOM_IOCTL_CMD_REGFIFOCH(%u)\n", cmd);
		ret = doscom_ioctl_regfifoch((uint32_t)arg);
		break;
	  case DOSCOM_IOCTL_CMD_GETFIFOCHMASK:
		DEBUG(DBG_CALLS, "DOSCOM_IOCTL_CMD_GETFIFOCHMASK(%u)\n", cmd);
		ret = doscom_ioctl_get_fifoch_mask((uint32_t)arg);
		break;
	  case DOSCOM_IOCTL_CMD_INITFIFO:
		DEBUG(DBG_CALLS, "DOSCOM_IOCTL_CMD_INITFIFO(%u)\n", cmd);
		ret = doscom_ioctl_fifoch_init((uint32_t)arg);
		break;
	  default:
		DEBUG(DBG_CALLS, "IOCTL cmd (%u): not available\n", cmd);
		ret = -ENOTTY;
	}
	up(&the_doscom_kernel_sem);
	return ret;
}

static struct file_operations safeg_fops = {
	.owner	= THIS_MODULE,
	.open	= safeg_open,
	.mmap	= safeg_mmap,
	.unlocked_ioctl = safeg_ioctl,
	.release = safeg_release,
};

#ifndef ALLOC_DEVICE_MANUALLY
static struct miscdevice safeg_dev = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "safeg",
	.fops = &safeg_fops,
};
#else /* !ALLOC_DEVICE_MANUALLY */
static dev_t dev;
#endif /* ALLOC_DEVICE_MANUALLY */

/*
 * IRQ HANDLER
 */
static irqreturn_t
safeg_doscom_irq_handler(int irq, void *dev_id)
{
	int i;
	doscom_fifoch_event_t *p_fifoch_event;

	DEBUG(DBG_IRQ, "SafeG IRQ handler\n");

	if (get_nt2t_req() == CTL_T2NT_REQ_ACK) {
		DEBUG(DBG_IRQ, "Receive Ack.\n");
		the_doscom_kernel.nt2t_req_ack = true;
		wake_up_interruptible(&(the_doscom_kernel.nt2t_req_ack_wq));
		set_nt2t_req(0);
	}

	if (!initialized) {
		return IRQ_HANDLED;
	}

	for(i=0; i<the_doscom_kernel.num_fifochs; i++) {
		p_fifoch_event = &the_doscom_kernel.fifoch_events[i];
		if (*(p_fifoch_event->t2nt_pending) == FIFOCH_T2NT_NONE) continue;
		p_fifoch_event->wercd = *(p_fifoch_event->t2nt_pending);
		if (*(p_fifoch_event->t2nt_pending) == FIFOCH_T2NT_INIT) {
			/* case of fifoch initialize, no queuing t2nt request */
			*(p_fifoch_event->t2nt_pending) = FIFOCH_T2NT_NONE;
		}
		wake_up_interruptible(&p_fifoch_event->fifoch_wq);
	}

	return IRQ_HANDLED;
}

/*
 * MODULE INIT
 */
static int __init safeg_init(void)
{
	int err;
	int loop;

	DEBUG(DBG_INIT, "SafeG allocate shared memory\n");
#if __TARGET_ARCH_ARM == 6
	sregion = ioremap_nocache(SREGION_PHYS_ADDRESS, SREGION_SIZE);
#else
	sregion = ioremap_cached(SREGION_PHYS_ADDRESS, SREGION_SIZE);
#endif
	DEBUG(DBG_INIT, "Shared Memory: 0x%X mapped to 0x%X\n",
					(u32)SREGION_PHYS_ADDRESS, (u32)sregion);

	DEBUG(DBG_INIT, "Register SafeG char device\n");
#ifndef ALLOC_DEVICE_MANUALLY
	err = misc_register(&safeg_dev);
	if (err) goto init_error;
#else
	DEBUG(DBG_INIT, "Allocating SafeG major/minor\n");
	err = alloc_chrdev_region(&dev, 0, 1,"safeg");
	major = MAJOR(dev);

	DEBUG(DBG_INIT, "mknod /dev/safeg c %d 0 (/proc/devices)\n", major);
	DEBUG(DBG_INIT, "chmod 0664 /dev/safeg\n");

	DEBUG(DBG_INIT, "Register SafeG char device\n");
	cdev_init(&safeg_cdev, &safeg_fops);
	safeg_cdev.owner = THIS_MODULE;
	err = cdev_add (&safeg_cdev, dev, 1);
	if (err) goto init_error;
#endif

	DEBUG(DBG_INIT, "Request interrupt\n");
#ifdef TARGET_INT_INIT
	target_int_init();
#endif /* TARGET_INT_INIT */
	err = request_irq(DOSCOM_T2NT_IRQ, safeg_doscom_irq_handler, 0, "safeg-comm", NULL);
	if (err) goto init_error;

	the_doscom_kernel.fifoch_mask = 0;
	the_doscom_kernel.num_process = 0;
	for (loop = 0; loop < MAX_NUM_FIFOCHS; loop++){
		the_doscom_kernel.fifoch_pid[loop] = 0;
	}

	return 0;

init_error:
	DEBUG(DBG_INIT, "Error %d at initialization", err);
	return err;
}

/*
 * MODULE EXIT
 */
static void __exit safeg_exit(void)
{
	DEBUG(DBG_INIT, "Goodbye SAFEG\n");
	free_irq(DOSCOM_T2NT_IRQ, NULL);

#ifndef ALLOC_DEVICE_MANUALLY
	misc_deregister(&safeg_dev);
#else /* !ALLOC_DEVICE_MANUALLY */
	cdev_del(&safeg_cdev);
	unregister_chrdev_region(MKDEV(major, 0), 1);
#endif /* ALLOC_DEVICE_MANUALLY */
}

module_init(safeg_init);
module_exit(safeg_exit);

MODULE_AUTHOR("Daniel Sangorrin <daniel.sangorrin@gmail.com>");
MODULE_DESCRIPTION("SAFEG doscom driver");
MODULE_LICENSE("GPL");
