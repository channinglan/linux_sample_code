/*
Tasklet�Mtimer����(�򥻤W���O�B�@�bSoftirqs�W��)�A���O���P��timer�|�b�S�w�ɶ�����Atasklet�|�b�U�@��interrupt���{�ɰ���CTasklet�����implement�A���O��TASKLET_SOFTIRQ�MHI_SOFTIRQ�A�o��ت��t�O�b��HI_SOFTIRQ��TASKLET_SOFTIRQ������C�t�~Tasklet�u�b���U��CPU�W������A�ӥB���U��tasklet�P�@�ɶ��u�|�Q�Y��CPU����C

�z�i�Hdynamically��statically���إ�tasklet�A
DECLARE_TASKLET(task, func, data);
DECLARE_TASKLET_DISABLED(task, func, data);

tasklet_init(task, func, data);

�ŧi��A�٥����I�stasklet_schedule(task)�~�|�Q����A���p�G�O��
DECLARE_TASKLET_DISABLED()�ŧi��disabled���A�A���N�٥�����tasklet_enable()�N�䪬�A�]��enabled�~��Q����C�z�]�i�H�z�Ltasklet_disabled() disabled�Y��tasklet�Ctasklet_kill()�i�H�O��tasklet���|�Qschedule�A�p�G�w�g�b����A�N�|�������浲���C
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
    if (!(c++ % 2000000)) { // �C�j2000000���I�s�N�L�X�T��
        printk("%s(): on cpu %d\n", (char*)name, smp_processor_id());
    }
}

static int __init init_modules(void)
{
    // create tasklet dynamically
    t3 = kzalloc(sizeof(struct tasklet_struct), GFP_KERNEL);
    tasklet_init(t3, f3, (unsigned long)"t3");

    tasklet_schedule(&t1);
    tasklet_schedule(&t2);
    tasklet_schedule(t3);
    tasklet_enable(&t2); // �S��enable�N���|�Q�Ұ�
    return 0;
}

static void __exit exit_modules(void)
{
    // remove module�N���ӭn�T�Otasklet���Q����
    tasklet_kill(&t1);
    tasklet_kill(&t2);
    tasklet_kill(t3);
}

module_init(init_modules);
module_exit(exit_modules);

