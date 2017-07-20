/*
kernel會定期產生timer interrupt，HZ定義每秒產生timer interrupt的次數，
定義在linux/param.h，根據平台的不同從50~1200不等。而jiffies每當發生一次
timer interrupt就會遞增一次，jiffies定義於linux/jiffies.h，所以簡單的說，
jiffies就等於1/HZ，不管在64bit或32bit上的機器，Linux kernel都使用64位元版
的jiffies_64，而jiffies其實是jiffies_64的低32位元版，除了讀取外，我們都不
應該直接修改jiffies/jiffies_64。kernel提供幾組macro來比較時間的先後，
time_after()/timer_before()/time_after_eq()/time_before_eq()。
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


//另外，kernel中有兩種時間的structure，struct timeval和struct timespec。

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

//早期以timeval為主，後來因為精密度的需求，有了timespec的誕生。kernel也提供了和jiffies的轉換函數。更多的轉換可以參考linux/jiffies.h

unsigned long timespec_to_jiffies(const struct timespec *value);
void jiffies_to_timespec(const unsigned long jiffies,
    struct timespec *value);
unsigned long timeval_to_jiffies(const struct timeval *value);
void jiffies_to_timeval(const unsigned long jiffies,
          struct timeval *value);


