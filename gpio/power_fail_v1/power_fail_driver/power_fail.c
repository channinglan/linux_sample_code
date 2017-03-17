#include <linux/fs.h>
#include <linux/init.h>
#include <linux/compat.h>
#include <linux/kernel.h>
#include <asm/gpio.h>
#include <linux/interrupt.h>



#define MISC_DEV_EN	1
	#define SIG_EN	1

#define IEI_GPIO_VERSION		"0.0.1"


#define KEY_POWER_INT 253

#ifndef GPIO_TO_PIN
#define GPIO_TO_PIN(bank, gpio)      (32 * (bank-1) + (gpio))
#endif


#define GPIO_USR0 GPIO_TO_PIN(7, 9)	//GPIO_ACTIVE_HIGH heartbeat
#define GPIO_USR1 GPIO_TO_PIN(7, 8)	//GPIO_ACTIVE_HIGH cpu0
#define GPIO_USR2 GPIO_TO_PIN(7, 14)	//GPIO_ACTIVE_HIGH mmc0
#define GPIO_USR3 GPIO_TO_PIN(7, 15)	//GPIO_ACTIVE_HIGH ide-disk

#define POWER_INT GPIO_TO_PIN(2,23)//GPIO_TO_PIN(1, 3) //default pin
#define POWER_FAIL_NAME  "power_fail"


short int irq_any_gpio = 0;
int count = 0;

static int	number	= POWER_INT;
static int	dbug	= 0;


#define IODEB(fmt, arg...)	if(dbug) {printk("\n[IOINT]" ""fmt,##arg);}




#define GPIO_HIGH gpio_get_value(number)
#define GPIO_LOW (gpio_get_value(number) == 0)

enum { falling, rising } type;


#if (MISC_DEV_EN == 1)
#include <linux/ioctl.h>
#include <linux/miscdevice.h>
#include <asm/uaccess.h>  //copy_from_user

#define SIG_TEST 44	// we choose 44 as our signal number (real-time signals are in the range of 33 to 64)
typedef struct {
	unsigned int user_pid;	
} gpio_params;
/* Use 'g' as magic number */
#define IOC_MAGIC	'g'

#define GPIO_SET_PID		_IOW(IOC_MAGIC, 0x01, unsigned int)
#define GPIO_SEND_SIG_TEST	_IO(IOC_MAGIC, 0x02)



#if SIG_EN
#include <asm/siginfo.h>	//siginfo
#include <linux/rcupdate.h>	//rcu_read_lock
#include <linux/sched.h>	//find_task_by_pid_type

	struct siginfo info;
	struct task_struct *t;

static int send_user_sig(void)
{
	int ret =0;
	if(t == NULL) {
		return -1;	
	}
	/* send the signal */
	ret = send_sig_info(SIG_TEST, &info, t);    //send the signal
	if (ret < 0) {
		printk("error sending signal\n");
		return ret;
	}
	IODEB("send_sig_info\n");
	return 0;
}
#endif
#endif


static irqreturn_t r_irq_handler(int irq, void *dev_id)
{
	//IODEB("%s \n",__FUNCTION__);	
	count++;
	//IODEB("[%d]interrupt received (irq: %d)\n",count, irq);
	if (irq == gpio_to_irq(number)) {

		type = GPIO_LOW ? falling : rising;

		if (type == falling) {
			IODEB("gpio pin is low\n");
		} else {
			IODEB("gpio pin is high\n");
		}
		IODEB("gpio pin is %d\n",gpio_get_value(number));
	}


#if (MISC_DEV_EN == 1)
#if SIG_EN
	//if (type == falling) {
	info.si_int = count;
		send_user_sig();
	//} else {
		
	//}
#endif
#endif
	return IRQ_HANDLED;
}








#if (MISC_DEV_EN == 1)


static int iei_gpio_open(struct inode *inode, struct file *file)
{
        return 0;
};

static int iei_gpio_release(struct inode *inode, struct file *file)
{
        return 0;
};


	
#if SIG_EN	
static int write_pid(int pid)
{
	int ret=0;
	printk("pid = %d\n", pid);

	/* prepare the signal */
	memset(&info, 0, sizeof(struct siginfo));
	info.si_signo = SIG_TEST;
	/*
	this is bit of a trickery: SI_QUEUE is normally used by sigqueue from user space,
	and kernel space should use SI_KERNEL. But if SI_KERNEL is used the real_time data 
	is not delivered to the user space signal handler function. 
	*/
	info.si_code = SI_QUEUE;
	/* real time signals may have 32 bits of data. */
	info.si_int = 1234;

	rcu_read_lock();
	/* find the task with that pid */
	t = pid_task(find_pid_ns(pid, &init_pid_ns), PIDTYPE_PID);	
	if(t == NULL){
		printk("no such pid\n");
		rcu_read_unlock();
		return -ENODEV;
	}
	rcu_read_unlock();
	return ret;
}
#endif




static long iei_gpio_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	//gpio_params gpio;
	unsigned int pid;
	switch (cmd) {

	case GPIO_SET_PID:
		if (copy_from_user (&pid, (unsigned int *)arg, sizeof (unsigned int)))
			 return -EFAULT;
		printk("GPIO_SET_PID 0x%x\n",pid);
		#if SIG_EN			 	
		write_pid(pid);
		#endif
		break;
	case GPIO_SEND_SIG_TEST:
		printk("GPIO_SEND_SIG_TEST\n");
		#if SIG_EN	
		send_user_sig();
		#endif
		break;		
	default:
		printk("GPIO ioctl function error\n");
		return -EINVAL;
	}

	return 0;
};



static const struct file_operations iei_gpio_fops = {
	.owner 		= THIS_MODULE,
	.unlocked_ioctl = iei_gpio_ioctl,
	.open		= iei_gpio_open,
	.release	= iei_gpio_release,
};

static struct miscdevice iei_gpio_miscdev = {
	.minor		= MISC_DYNAMIC_MINOR,
	.name		= POWER_FAIL_NAME,
	.fops		= &iei_gpio_fops,
};

#endif
void pxm_io_int_release(void)
{
	printk("%s\n",__FUNCTION__);
	free_irq(gpio_to_irq(number), NULL);
	gpio_free(number);;
	return;
}



void pxm_io_int_config(void)
{
#if (MISC_DEV_EN == 1)	
	int ret=0;
#endif	

	printk("%s\n",__FUNCTION__);
	if (gpio_is_valid(number) == 0) {	// return true if valid.
		printk("GPIO %d not valid\n", number);
		return;
	}

	memset(&info, 0, sizeof(struct siginfo));

	/**
	 *return -EINVAL if GPIO is not valid(same as gpio_is_valid)
	 *return -EBUSY if GPIO is already used
	 *return 0 if GPIO if fine to use
	 *$ cat /sys/kernel/debug/gpio 
	*/
	if (gpio_request(number, POWER_FAIL_NAME)) {
		printk("GPIO %d request failure: %s\n", number,
		       POWER_FAIL_NAME);
		return;
	}
	printk(KERN_NOTICE "gpio_to_irq(%d)\n", number);
	if ((irq_any_gpio = gpio_to_irq(number)) < 0) {
		printk("GPIO to IRQ mapping failure %s\n", POWER_FAIL_NAME);
		return;
	}

	printk(KERN_NOTICE "Mapped int %d \n", irq_any_gpio);

/*
 IRQF_TRIGGER_NONE       
 IRQF_TRIGGER_RISING     
 IRQF_TRIGGER_FALLING    
 IRQF_TRIGGER_HIGH       
 IRQF_TRIGGER_LOW        
*/
	if (request_irq
	    (irq_any_gpio, (irq_handler_t) r_irq_handler,
	     IRQF_TRIGGER_FALLING, POWER_FAIL_NAME, NULL)) {
		printk("%s Irq Request failure\n", POWER_FAIL_NAME);
		return;
	}


	//gpio_export(number, true);	


	
	//printk("GPIO %d = %d\n", number,gpio_direction_input(number));
		type = GPIO_LOW ? falling : rising;  
                                                     
		if (type == falling) {               
			printk("gpio pin is low\n"); 
		} else                               
			printk("gpio pin is high\n");

	
	
	
	
#if (MISC_DEV_EN == 1)	
	printk(KERN_INFO "IEI GPIO INFO [ver: %s] enable successfully!\n", IEI_GPIO_VERSION);
	ret = misc_register(&iei_gpio_miscdev);
	if (ret < 0) {
		printk(KERN_ERR "GPIO: misc_register returns %d.\n", ret);
		return;
	}	
	
#endif	
		
	return;

	
}


int pxm_io_int_init_module(void)
{
	printk("%s\n",__FUNCTION__);
	pxm_io_int_config();
	return 0;
}

void pxm_io_int_cleanup_module(void)
{          
	
	printk("%s\n",__FUNCTION__);	
	pxm_io_int_release();
}





module_param(number, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
MODULE_PARM_DESC(number, "gpio number");

module_param(dbug, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
MODULE_PARM_DESC(dbug, "debug");

module_init(pxm_io_int_init_module);
module_exit(pxm_io_int_cleanup_module);

MODULE_DESCRIPTION("GPIO driver");
MODULE_LICENSE("GPL");
