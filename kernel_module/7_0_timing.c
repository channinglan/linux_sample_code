/*
kernel�|�w������timer interrupt�AHZ�w�q�C����timer interrupt�����ơA
�w�q�blinux/param.h�A�ھڥ��x�����P�q50~1200�����C��jiffies�C��o�ͤ@��
timer interrupt�N�|���W�@���Ajiffies�w�q��linux/jiffies.h�A�ҥH²�檺���A
jiffies�N����1/HZ�A���ަb64bit��32bit�W�������ALinux kernel���ϥ�64�줸��
��jiffies_64�A��jiffies���Ojiffies_64���C32�줸���A���FŪ���~�A�ڭ̳���
���Ӫ����ק�jiffies/jiffies_64�Ckernel���ѴX��macro�Ӥ���ɶ�������A
time_after()/timer_before()/time_after_eq()/time_before_eq()�C
*/
/*
 * These inlines deal with timer wrapping correctly. You are 
 * strongly encouraged to use them
 * 1. Because people otherwise forget
 * 2. Because if the timer wrap changes in future you won't have to
 *    alter your driver code.
 *
 * time_after(a,b) returns true if the time a is after time b.
 *
 * Do this with "<0" and ">=0" to only test the sign of the result. A
 * good compiler would generate better code (and a really good compiler
 * wouldn't care). Gcc is currently neither.
 */
 
/* 
 * #define time_after(a,b)  \
 *  (typecheck(unsigned long, a) && \
 *   typecheck(unsigned long, b) && \
 *   ((long)(b) - (long)(a) < 0))
 * #define time_before(a,b) time_after(b,a)
 * 
 * #define time_after_eq(a,b) \
 *  (typecheck(unsigned long, a) && \
 *   typecheck(unsigned long, b) && \
 *   ((long)(a) - (long)(b) >= 0))
 * #define time_before_eq(a,b) time_after_eq(b,a)
 * 
 * 
 * �t�~�Akernel������خɶ���structure�Astruct timeval�Mstruct timespec�C
 * 
 * #ifndef _STRUCT_TIMESPEC
 * #define _STRUCT_TIMESPEC
 * struct timespec {
 *  __kernel_time_t tv_sec;	// seconds 
 *  long  tv_nsec;		// nanoseconds 
 * };
 * #endif
 * 
 * struct timeval {
 *  __kernel_time_t  tv_sec;	// seconds 
 *  __kernel_suseconds_t tv_usec; // microseconds 
 * };
*/

//�����Htimeval���D�A��Ӧ]����K�ת��ݨD�A���Ftimespec���ϥ͡Ckernel�]���ѤF�Mjiffies���ഫ��ơC��h���ഫ�i�H�Ѧ�linux/jiffies.h

//unsigned long timespec_to_jiffies(const struct timespec *value);
//void jiffies_to_timespec(const unsigned long jiffies,
//    struct timespec *value);
//unsigned long timeval_to_jiffies(const struct timeval *value);
//void jiffies_to_timeval(const unsigned long jiffies,
//          struct timeval *value);
#include <linux/init.h>
#include <linux/module.h>
#include <linux/timer.h>
#include <linux/delay.h> 


MODULE_LICENSE("GPL");


static int __init init_modules(void)
{
	unsigned long tmp=0,tmp2=0;
	struct timespec timespec;
	struct timeval timeval;
	
	printk("\n %s",__func__);
	
	tmp = jiffies;
	
	jiffies_to_timespec(tmp,&timespec);
	jiffies_to_timeval(tmp,&timeval);
	
	
	printk("\n jiffies=%lx",tmp);
	printk("\n timespec %ld s .%ld ns",timespec.tv_sec,timespec.tv_nsec);	
	printk("\n timeval  %ld s .%ld us",timeval.tv_sec,timeval.tv_usec);
	
	printk("\n timespec2jiffies=%lx",timespec_to_jiffies(&timespec));
	printk("\n timeval2jiffies=%lx",timeval_to_jiffies(&timeval));
	
	printk("\n sleep 1 s");
	msleep(1000);

	tmp2 = jiffies;		
	jiffies_to_timespec(tmp2,&timespec);
	jiffies_to_timeval(tmp2,&timeval);
	
	
	printk("\n jiffies=%lx",tmp2);
	printk("\n timespec %ld s .%ld ns",timespec.tv_sec,timespec.tv_nsec);	
	printk("\n timeval  %ld s .%ld us",timeval.tv_sec,timeval.tv_usec);	
	
	
	
	if(time_after(tmp,tmp2)) {
		printk("\n a time_after jiffies");		
	} else {
		printk("\n b time_after jiffies2");		
	}
	
	if(time_before(tmp2,tmp)) {
		printk("\n a time_before jiffies2");		
	} else {
		printk("\n b time_before jiffies");		
	}	
	
	
	if(time_after_eq(tmp,tmp)) {
		printk("\n time_after_eq jiffies");		
	}	
	
	if(time_before_eq(tmp,tmp)) {
		printk("\n time_before_eq jiffies");		
	}	
	
	return 0;
}

static void __exit exit_modules(void)
{
	printk("\n %s",__func__);
}

module_init(init_modules);
module_exit(exit_modules);