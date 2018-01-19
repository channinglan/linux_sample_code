/*
http://nano-chicken.blogspot.tw/2010/01/linux-modules9-kthread.html
在kernel中建立thread可以使用kthread_create()，建立一個task，
然後在調用wake_up_process(task)讓task真正的運行，
如果要kill一個kthread可以使用kthread_stop()。
在kernel中，將kthread_create()和wake_up_process()包裝成kthread_run()，
也就是調用了kthread_run()之後，該thread會立刻被執行。


int thread_function(void *data);
struct task_struct *kthread_create(int (*threadfn)(void *data),
                                       void *data,
				       const char *namefmt, ...);
				       
				           
struct task_struct *kthread_run(int (*threadfn)(void *data),
                                    void *data,
				    const char *namefmt, ...);
				    
				    
int kthread_stop(struct task_struct *thread);
 
 
void kthread_bind(struct task_struct *thread, int cpu);				    

*/
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kthread.h>

MODULE_LICENSE("GPL");

static struct task_struct *brook_tsk;
static int data;
static int kbrook(void *arg);

static struct task_struct *brook_tsk2;
static int data2;
static int kbrook2(void *arg);



static int kbrook(void *arg)
{
    unsigned int timeout;
    int *d = (int *) arg;

    for(;;) {
        if (kthread_should_stop()) break;
        printk("%s(): %d\n", __FUNCTION__, (*d)++);
        do {
            set_current_state(TASK_INTERRUPTIBLE);
            timeout = schedule_timeout(2 * HZ);
        } while(timeout);
    }
    printk("break\n");

    return 0;
}



static int kbrook2(void *arg)
{
    unsigned int timeout;
    int *d = (int *) arg;

    for(;;) {
        if (kthread_should_stop()) break;
        printk("%s(): %d\n", __FUNCTION__, (*d)++);
        do {
            set_current_state(TASK_INTERRUPTIBLE);
            timeout = schedule_timeout(3 * HZ);
        } while(timeout);
    }
    printk("break2\n");

    return 0;
}


static int __init init_modules(void)
{
    int ret;

    brook_tsk = kthread_create(kbrook, &data, "brook");
    if (IS_ERR(brook_tsk)) {
        ret = PTR_ERR(brook_tsk);
        brook_tsk = NULL;
        goto out;
    }
    wake_up_process(brook_tsk);

	
	brook_tsk2 = kthread_run(kbrook2, &data2, "brook2");
	if (IS_ERR(brook_tsk2)) {
        ret = PTR_ERR(brook_tsk2);
        brook_tsk2 = NULL;
        goto out;
    }

    return 0;

out:
    return ret;
}

static void __exit exit_modules(void)
{
	if(brook_tsk != NULL) {
    		kthread_stop(brook_tsk);
	}
	
	if(brook_tsk2 != NULL) {
    		kthread_stop(brook_tsk2);
	}  
}

module_init(init_modules);
module_exit(exit_modules);
