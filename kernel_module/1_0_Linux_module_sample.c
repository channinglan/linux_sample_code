#include <linux/init.h>
#include <linux/module.h>
/*
<linux/init.h>�M#include <linux/module.h>�OLinux ����module���|�Ψ쪺header file�A
init.h�D�n�w�qmodule��init�Mcleanup�A�pmodule_init()�Mmodule_exit()�C
��module.h�w�q�Fmodule�һݭn����Ƶ��c�Pmacro�C

*/
MODULE_LICENSE("GPL");

/*
���__init�������binit.h���D�`�n�������G
The kernel can take this as hint that the function is used only during the initialization 
phase and free up used memory resources after.
²�檺���N�O�o��function�b��l�ƫ�]���槹�^�N�Qfree�F�C
*/
static int __init init_modules(void)
{
    printk("hello world\n");
    return 0;
}

/*
��__exit�������O�G
__exit is used to declare a function which is only required on exit: 
the function will be dropped if this file is not compiled as a module.
*/

static void __exit exit_modules(void)
{
    printk("goodbye\n");
}

module_init(init_modules);
/*

module_init()�������O�G
The module_init() macro defines which function is to be called at module insertion time (if the file is compiled as a module), or at boot time: if the file is not compiled as a module the module_init() macro becomes equivalent to __initcall(), which through linker magic ensures that the function is called on boot.
�D�n�O�Ψӳ]�w��insert��module��A���ӭn�Q���檺�i�J�I�]enrty point�^�C
*/
module_exit(exit_modules);
/*
module_exit()�������O�G
This macro defines the function to be called at module removal time (or never, in the case of the file compiled into the kernel). It will only be called if the module usage count has reached zero. This function can also sleep, but cannot fail: everything must be cleaned up by the time it returns.
Note that this macro is optional: if it is not present, your module will not be removable (except for 'rmmod -f').
²�����A�N�O��user����rmmod�ɡA�|�Q����쪺function�C�S��module_exit()�Amodule�N����Qrmmod�C

*/