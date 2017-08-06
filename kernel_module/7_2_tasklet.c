/*
Tasklet�Mtimer����(�򥻤W���O�B�@�bSoftirqs�W��)�A���O���P��timer�|�b�S�w�ɶ�����A
tasklet�|�b�U�@��interrupt���{�ɰ���C
Tasklet�����implement�A���O��TASKLET_SOFTIRQ�MHI_SOFTIRQ�A
�o��ت��t�O�b��HI_SOFTIRQ��TASKLET_SOFTIRQ������C
�t�~Tasklet�u�b���U��CPU�W������A�ӥB���U��tasklet�P�@�ɶ��u�|�Q�Y��CPU����C

�z�i�Hdynamically��statically���إ�tasklet�A
DECLARE_TASKLET(task, func, data);
DECLARE_TASKLET_DISABLED(task, func, data);

tasklet_init(task, func, data);

�ŧi��A�٥����I�stasklet_schedule(task)�~�|�Q����A���p�G�O��
DECLARE_TASKLET_DISABLED()�ŧi��disabled���A�A
���N�٥�����tasklet_enable()�N�䪬�A�]��enabled�~��Q����C
�z�]�i�H�z�Ltasklet_disabled() disabled�Y��tasklet�Ctasklet_kill()�i�H�O��tasklet���|�Qschedule�A
�p�G�w�g�b����A�N�|�������浲���C
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

