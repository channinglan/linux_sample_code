/*
Tasklet和timer類似(基本上都是運作在Softirqs上面)，但是不同於timer會在特定時間執行，
tasklet會在下一次interrupt來臨時執行。
Tasklet有兩種implement，分別為TASKLET_SOFTIRQ和HI_SOFTIRQ，
這兩種的差別在於HI_SOFTIRQ筆TASKLET_SOFTIRQ早執行。
另外Tasklet只在註冊的CPU上面執行，而且註冊的tasklet同一時間只會被某個CPU執行。

您可以dynamically或statically的建立tasklet，
DECLARE_TASKLET(task, func, data);
DECLARE_TASKLET_DISABLED(task, func, data);

tasklet_init(task, func, data);

宣告後，還必須呼叫tasklet_schedule(task)才會被執行，但如果是用
DECLARE_TASKLET_DISABLED()宣告成disabled狀態，
那就還必須用tasklet_enable()將其狀態設成enabled才能被執行。
您也可以透過tasklet_disabled() disabled某個tasklet。tasklet_kill()可以保證tasklet不會被schedule，
如果已經在執行，就會等它執行結束。
*/
/*
Tasklet and timer similar (basically are operating in Softirqs above), but unlike the timer will be implemented at a specific time,
The tasklet will execute at the next interrupt.
Tasklet has two implement, respectively, TASKLET_SOFTIRQ and HI_SOFTIRQ,
The difference between the two is that the HI_SOFTIRQ pen TASKLET_SOFTIRQ is executed early.
In addition, the Tasklet is executed only on the registered CPU, and the registered tasklet will only be executed by a CPU at the same time.

You can dynamically or statically create a tasklet,
DECLARE_TASKLET (task, func, data);
DECLARE_TASKLET_DISABLED (task, func, data);

Tasklet_init (task, func, data);


After the announcement, must also call tasklet_schedule (task) will be implemented, but if it is used
DECLARE_TASKLET_DISABLED () declared into disabled state,
It must also use tasklet_enable () to set its state to enabled to be executed.
You can also disable a tasklet via tasklet_disabled (). Tasklet_kill () can ensure that the tasklet will not be schedule,
If it is already executing, it will wait for it to finish.

*/


#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/slab.h>

MODULE_LICENSE("GPL");

static void f(unsigned long name);

// create tasklet statically
static DECLARE_TASKLET(t1, f, (unsigned long)"t1");
static DECLARE_TASKLET_DISABLED(t2, f, (unsigned long)"t2");

static struct tasklet_struct *t3;

static void f(unsigned long name)
{
    printk("%s(): on cpu %d\n", (char*)name, smp_processor_id());
}

static void f3(unsigned long name)
{
    static u32 c = 0;
    tasklet_schedule(t3);
    if (!(c++ % 2000000)) { // A message is printed every 2 million calls
        printk("%s(): on cpu %d\n", (char*)name, smp_processor_id());
    }
}

static int __init init_modules(void)
{
	printk("\n %s",__func__);	
    // create tasklet dynamically
    t3 = kzalloc(sizeof(struct tasklet_struct), GFP_KERNEL);
    tasklet_init(t3, f3, (unsigned long)"t3");

    tasklet_schedule(&t1);
    tasklet_schedule(&t2);
    tasklet_schedule(t3);
    tasklet_enable(&t2); // No enable will not be activated
    return 0;
}

static void __exit exit_modules(void)
{
	printk("\n %s",__func__);	
    // Remove module should ensure that the tasklet has been removed
    tasklet_kill(&t1);
    tasklet_kill(&t2);
    tasklet_kill(t3);
}

module_init(init_modules);
module_exit(exit_modules);

