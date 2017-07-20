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
#define time_after(a,b)  \
 (typecheck(unsigned long, a) && \
  typecheck(unsigned long, b) && \
  ((long)(b) - (long)(a) < 0))
#define time_before(a,b) time_after(b,a)

#define time_after_eq(a,b) \
 (typecheck(unsigned long, a) && \
  typecheck(unsigned long, b) && \
  ((long)(a) - (long)(b) >= 0))
#define time_before_eq(a,b) time_after_eq(b,a)


//�t�~�Akernel������خɶ���structure�Astruct timeval�Mstruct timespec�C

#ifndef _STRUCT_TIMESPEC
#define _STRUCT_TIMESPEC
struct timespec {
 __kernel_time_t tv_sec;   /* seconds */
 long  tv_nsec;  /* nanoseconds */
};
#endif

struct timeval {
 __kernel_time_t  tv_sec;  /* seconds */
 __kernel_suseconds_t tv_usec; /* microseconds */
};

//�����Htimeval���D�A��Ӧ]����K�ת��ݨD�A���Ftimespec���ϥ͡Ckernel�]���ѤF�Mjiffies���ഫ��ơC��h���ഫ�i�H�Ѧ�linux/jiffies.h

unsigned long timespec_to_jiffies(const struct timespec *value);
void jiffies_to_timespec(const unsigned long jiffies,
    struct timespec *value);
unsigned long timeval_to_jiffies(const struct timeval *value);
void jiffies_to_timeval(const unsigned long jiffies,
          struct timeval *value);


