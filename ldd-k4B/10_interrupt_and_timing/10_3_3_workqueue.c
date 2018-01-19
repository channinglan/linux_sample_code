
/*
http://nano-chicken.blogspot.tw/2010/12/linux-modules73-work-queue.html

Work queue���Ѥ@��interface�A���ϥΪ̻������إ�kernel thread�åB�Nwork�j�b�o��kernel thread�W���A

�ѩ�work queue�O�إߤ@��kernel thread�Ӱ���A�ҥH�O�bprocess context�A���P��tasklet��interrupt context�A
�]���Awork queue�i�Hsleep(�]�wsemaphore�Ϊ̰���block I/O����)�C


#### Creating Work ####

�z�L DECLARE_WORK(name, void (work_func_t)(struct work_struct *work)); // statically
�Ϊ�
INIT_WORK(struct work_struct*, void (work_func_t)(struct work_struct *work)); //dynamically
�إ�work�A�N�O�n���檺�u�@�C
���Fwork�ٻݭn�N���Mwork thread���X�A
�z�i�H�z�Lcreate_singlethread_workqueue("name")�إߤ@�ӦW��name��single thread(����work��thread�N�٬�work thread)�A
�Ϊ�create_workqueue("name")�إ�per cpu��thread�C���۴N�O�n�Nwork�Mwork thread�����p�F�A
�z�Lqueue_work(work_thread, work)�N�i�H�Nwork�e��work thread����F�C

queue_delayed_work(work_thread, delayed_work, delay)��queue_work()��delay�����C
flush_workqueue(work_thread)�|wait����o��work_thread��work�������C
flush_workqueue()�ä��|��������Qdelay���檺work�A
�p�G�n����delayed��work�h�ݭn�I�scancel_delayed_work(delayed_work)�Ndelayed_work�۬Y��work thread�������C

�̫�A�n�Nwork_thread�R���n�I�sdestroy_workqueue(work_thread)�C


event/n
���F�ۤv�إ�work thread�H�~�Akernel�٫إߤ@�Ӥ��Ϊ�work thread�٬�event

kernel/workqueue.c

void __init init_workqueues(void)
{
    �K
    keventd_wq = create_workqueue("events");
    �K
}


�z�i�H�z�Lschedule_work(&work)�N�Awork�e��"events"����Aflush_scheduled_work(void)����"events"���Ҧ���work���槹���C

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
