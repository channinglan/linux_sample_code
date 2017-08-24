
/*

���ɭԧڭ̧Ʊ��b�Y�Ӯɶ��I����Y�ǰʧ@�A�o�ɭԫK�i�H�ϥ�timer�A
�b�ϥ�timer���ǳW�x�����Q��u�C�]�����Ouser-space�ӳ�_�A�ҥH�����\
�s��user-space�Acurrent�]�N�S���N�q�C�����v�A�]����schedule()�Ϊ�
���󦳥i���v���ʧ@������C


Sometimes we want to be able to perform certain actions at a certain point in time, 
and then you can use the timer, in the use of timer some rules must be observed. 
Because not user-space to arouse, so do not allow access to user-space, 
current also does not make sense. Can not sleep, nor schedule () or any action 
may be dormant are not allowed.

kernel timer�̵u�����j�O1��jiffies�A�ӥB�|����w�餤�_�A�M��L�D�P�B�ƥ󪺤z�Z�A
�ҥH���A�X�D�`��K�����ΡC

The minimum interval of the kernel timer is a jiffies, and is subject to 
hardware interruption, and other non-synchronous events, so it is not suitable 
for very sophisticated applications
*/





/*
timer_list������l�Ƥ���~��ϥΡA�z�i�H���init_timer()��TIMER_INITIALIZER()�A
���۴N�i�H�]�wexpires/callback function/data(�Ѽ�)�A�åB�ϥ�add_timer()�N��[�Jtimer���A
�Ϊ̨ϥ�del_timer()����pending����timer�A�]�i�H�ϥ�mod_timer()�ק�Ϊ̭��s�]�wtimer�C

Timer_list must be initialized before it can be used. 
You can select init_timer () or TIMER_INITIALIZER (), 
then you can set expires / callback function / data (parameters), 
add it to timer using add_timer (), or use del_timer () to remove Pending timer, 
you can also use mod_timer () to modify or reset the timer.
*/

#include <linux/init.h>
#include <linux/module.h>
#include <linux/timer.h>

MODULE_LICENSE("GPL");



/*
struct timer_list {
 struct list_head entry;
 unsigned long expires;

 void (*function)(unsigned long);
 unsigned long data;

 struct tvec_base *base;
#ifdef CONFIG_TIMER_STATS
 void *start_site;
 char start_comm[16];
 int start_pid;
#endif
#ifdef CONFIG_LOCKDEP
 struct lockdep_map lockdep_map;
#endif
};

*/





struct timer_list brook_timer;
static void timer_callback(unsigned long);
struct data {
    int count;
};
static struct data data;

static void timer_callback(unsigned long data)
{
	struct timespec timespec;	
	struct data *dp = (struct data*) data;
	
	jiffies_to_timespec(jiffies,&timespec);		
	printk("\n %s",__func__);	
	
	printk("\n timespec %ld s %ld ns",timespec.tv_sec,timespec.tv_nsec);	
	printk("%s(): %d\n", __FUNCTION__, dp->count++);
	mod_timer(&brook_timer, jiffies + 5 * HZ);
}

static int __init init_modules(void)
{
	printk("\n %s",__func__);	
    init_timer(&brook_timer);
    brook_timer.expires = jiffies + 5 * HZ;
    brook_timer.function = &timer_callback;
    brook_timer.data = (unsigned long) &data;
    add_timer(&brook_timer);
    return 0;
}

static void __exit exit_modules(void)
{
	printk("\n %s",__func__);	
    del_timer(&brook_timer);
}

module_init(init_modules);
module_exit(exit_modules);