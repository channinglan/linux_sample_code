    /* Linux/drivers/ker-driver.c 
     * Author: Woodpecker <Pecker.hu@gmail.com> 
     * 
     * kernel-driver 
     * 
     * This file is subject to the terms and conditions of the GNU General Public 
     * License.  See the file COPYING in the main directory of this archive for 
     * more details. 
     * 
     */  
      
    #include <linux/module.h>     /* MODULE_LICENSE     */  
    #include <linux/kernel.h>     /* printk,pr_info     */  
    #include <linux/errno.h>      /* EINVAL,EAGAIN,etc. */  
    #include <linux/err.h>            /* IS_ERR             */  
    #include <linux/fb.h>         /* FB header file     */  
    #include <linux/init.h>           /* module_init        */  
    #include <linux/semaphore.h>  /* init_MUTEX APIs    */  
    #include <linux/mm.h>         /* vm_area_struct     */  
    #include <linux/dma-mapping.h>  /* DMA APIs             */  
    #include <linux/delay.h>      /* mdelay,msleep      */  
    #include <linux/hrtimer.h>  
    #include <linux/time.h>           /* struct timespec    */  
    #include <linux/timekeeping.h>
    #define KER_PRINT(fmt, ...) printk("<ker-driver>"fmt, ##__VA_ARGS__);  
    static struct hrtimer vibe_timer;  
    static struct work_struct vibe_work;  
      
    static void vibe_work_func(struct work_struct *work)  
    {  
        KER_PRINT("vibe_work_func:msleep(50)/n");  
        msleep(50); /* CPU sleep */  
    }  
      
    static enum hrtimer_restart vibrator_timer_func(struct hrtimer *timer)   
    {             
        struct timespec uptime;  
          
        ktime_get_ts(&uptime);  
        KER_PRINT("Time:%lu.%02lu/n",  
                (unsigned long) uptime.tv_sec,  
                (uptime.tv_nsec / (NSEC_PER_SEC / 100)));  
          
        KER_PRINT("vibrator_timer_func/n");   
        schedule_work(&vibe_work);  
        return HRTIMER_NORESTART;  
    }  
      
    static int __init ker_driver_init(void)  
    {  
      
        int value = 2000;   /* Time out setting,2 seconds */  
        struct timespec uptime;  
          
        KER_PRINT("ker_driver_init/n");  
        hrtimer_init(&vibe_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);  
        vibe_timer.function = vibrator_timer_func;  
        hrtimer_start(&vibe_timer,  
            ktime_set(value / 1000, (value % 1000) * 1000000),HRTIMER_MODE_REL);  
          
        ktime_get_ts(&uptime);  
        KER_PRINT("Time:%lu.%02lu/n",  
                (unsigned long) uptime.tv_sec,  
                (uptime.tv_nsec / (NSEC_PER_SEC / 100)));  
      
        INIT_WORK(&vibe_work, vibe_work_func);  /* Intialize the work queue */  
        return 0;  
      
    }  
      
    static void __exit ker_driver_exit(void)  
    {  
        hrtimer_cancel(&vibe_timer);  
    }  
      
    module_init(ker_driver_init);  
    module_exit(ker_driver_exit);  
      
    MODULE_AUTHOR("Woodpecker <Pecker.hu@gmail.com>");  
    MODULE_DESCRIPTION("Kernel driver");  
    MODULE_LICENSE("GPL");  