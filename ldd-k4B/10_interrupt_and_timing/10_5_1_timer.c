
/*

有時候我們希望能在某個時間點執行某些動作，這時候便可以使用timer，
在使用timer有些規矩必須被遵守。因為不是user-space來喚起，所以不允許
存取user-space，current也就沒有意義。不能休眠，也不准schedule()或者
任何有可能休眠的動作都不准。


Sometimes we want to be able to perform certain actions at a certain point in time, 
and then you can use the timer, in the use of timer some rules must be observed. 
Because not user-space to arouse, so do not allow access to user-space, 
current also does not make sense. Can not sleep, nor schedule () or any action 
may be dormant are not allowed.

kernel timer最短的間隔是1個jiffies，而且會受到硬體中斷，和其他非同步事件的干擾，
所以不適合非常精密的應用。

The minimum interval of the kernel timer is a jiffies, and is subject to 
hardware interruption, and other non-synchronous events, so it is not suitable 
for very sophisticated applications
*/





/*
timer_list必須初始化之後才能使用，您可以選擇init_timer()或TIMER_INITIALIZER()，
接著就可以設定expires/callback function/data(參數)，並且使用add_timer()將其加入timer中，
或者使用del_timer()移除pending中的timer，也可以使用mod_timer()修改或者重新設定timer。

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