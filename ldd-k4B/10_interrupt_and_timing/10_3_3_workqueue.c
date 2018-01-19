
/*
http://nano-chicken.blogspot.tw/2010/12/linux-modules73-work-queue.html

Work queue提供一個interface，讓使用者輕易的建立kernel thread並且將work綁在這個kernel thread上面，

由於work queue是建立一個kernel thread來執行，所以是在process context，不同於tasklet的interrupt context，
因此，work queue可以sleep(設定semaphore或者執行block I/O等等)。


#### Creating Work ####

透過 DECLARE_WORK(name, void (work_func_t)(struct work_struct *work)); // statically
或者
INIT_WORK(struct work_struct*, void (work_func_t)(struct work_struct *work)); //dynamically
建立work，就是要執行的工作。
有了work還需要將它和work thread結合，
您可以透過create_singlethread_workqueue("name")建立一個名為name的single thread(執行work的thread就稱為work thread)，
或者create_workqueue("name")建立per cpu的thread。接著就是要將work和work thread做關聯了，
透過queue_work(work_thread, work)就可以將work送給work thread執行了。

queue_delayed_work(work_thread, delayed_work, delay)為queue_work()的delay版本。
flush_workqueue(work_thread)會wait直到這個work_thread的work都做完。
flush_workqueue()並不會取消任何被delay執行的work，
如果要取消delayed的work則需要呼叫cancel_delayed_work(delayed_work)將delayed_work自某個work thread中移除。

最後，要將work_thread摧毀要呼叫destroy_workqueue(work_thread)。


event/n
除了自己建立work thread以外，kernel還建立一個公用的work thread稱為event

kernel/workqueue.c

void __init init_workqueues(void)
{
    …
    keventd_wq = create_workqueue("events");
    …
}


您可以透過schedule_work(&work)將，work送給"events"執行，flush_scheduled_work(void)等待"events"中所有的work執行完畢。

---------------------------------------------------------------------------------------------------------------
Work queue provides an interface, allowing users to easily establish the kernel thread and work will be tied to the kernel thread above,

Because the work queue is to establish a kernel thread to perform, it is in the process context, different from the tasklet interrupt context,
Therefore, the work queue can sleep (set semaphore or execute block I / O, etc.).


#### Creating Work ####

Through DECLARE_WORK (name, void (work_func_t) (struct work_struct * work)); // statically
or
INIT_WORK (struct work_struct *, void (work_func_t) (struct work_struct * work)); // dynamically
The establishment of work, is to carry out the work.
With the work also need to combine it and work thread,
You can create a single thread named name by creating_singlethread_workqueue ("name") (the thread that executes the job is called work thread)
Or create_workqueue ("name") to establish per cpu thread. Then is to work and work thread to do the association,
Through the queue_work (work_thread, work) can be sent to the work work done.

Queue_delayed_work (work_thread, delayed_work, delay) is the delay version of queue_work ().
Flush_workqueue (work_thread) will wait until the work_thread work done.
Flush_workqueue () does not cancel any work done by delay,
If you want to cancel the delayed work, you need to call cancel_delayed_work (delayed_work) to remove the delayed_work from a work thread.

Finally, destroy the work_thread to destroy destroy_workqueue (work_thread).


Event / n
In addition to building their own work thread, kernel also build a common work thread called event

Kernel / workqueue.c

Void __init init_workqueues (void)
{
    ...
    Keventd_wq = create_workqueue ("events");
    ...
}


You can send the "event" execution with schedule_work (& work), flush_scheduled_work (void) waits for all the work in "events" to finish.


*/
#include <linux/init.h>
#include <linux/module.h>

#include <linux/kernel.h>
#include <linux/workqueue.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/delay.h>
MODULE_LICENSE("GPL");



static short stop_wq=0;
//module_param(stop_wq, short, S_IRUGO | S_IWUGO);  //S_IWUGO???
module_param(stop_wq, short, S_IRUGO | S_IWUSR);


static void event_1_routine(struct work_struct *);
static void brook_2_routine(struct work_struct *);
static void delaywork_3_routine(struct work_struct *);

static struct work_struct *event_1_work; // for event

static DECLARE_WORK(brook_2_work, brook_2_routine);

static DECLARE_DELAYED_WORK(delaywork_3_work, delaywork_3_routine);

static struct workqueue_struct *brook_workqueue;


static int __init init_modules(void)
{
	printk("\n %s",__func__);	
    // for event
    event_1_work = kzalloc(sizeof(typeof(*event_1_work)), GFP_KERNEL);
    INIT_WORK(event_1_work, event_1_routine);
    schedule_work(event_1_work);

    // for brook_wq
    brook_workqueue = create_workqueue("brook_wq");
    queue_work(brook_workqueue, &brook_2_work);
    queue_delayed_work(brook_workqueue, &delaywork_3_work, 0);
    stop_wq = 0;
    return 0;
}

static void __exit exit_modules(void)
{
	printk("\n %s",__func__);	
    cancel_delayed_work(&delaywork_3_work);
    flush_workqueue(brook_workqueue);
    stop_wq = 1;
    destroy_workqueue(brook_workqueue);
}

static void event_1_routine(struct work_struct *ws)
{
    printk("%s(): on cpu:%d, pname:%s\n",
            __func__, smp_processor_id(), current->comm);
}

static void brook_2_routine(struct work_struct *ws)
{
    printk("%s(): on cpu:%d, pname:%s\n",
            __func__, smp_processor_id(), current->comm);
    // do something to block/sleep
    // the work in the same workqueue is also deferred.
    msleep(5000);
    if (!stop_wq) {
        queue_work(brook_workqueue, &brook_2_work);
    }
}

static void delaywork_3_routine(struct work_struct *ws)
{
    printk("%s(): on cpu:%d, pname:%s\n",
            __func__, smp_processor_id(), current->comm);
    queue_delayed_work(brook_workqueue, &delaywork_3_work, 50);
}

module_init(init_modules);
module_exit(exit_modules);
