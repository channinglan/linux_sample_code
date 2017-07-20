#include <linux/init.h>
#include <linux/module.h>
/*
<linux/init.h>和#include <linux/module.h>是Linux 任何的module都會用到的header file，
init.h主要定義module的init和cleanup，如module_init()和module_exit()。
而module.h定義了module所需要的資料結構與macro。

*/
MODULE_LICENSE("GPL");

/*
對於__init的解釋在init.h有非常好的解釋：
The kernel can take this as hint that the function is used only during the initialization 
phase and free up used memory resources after.
簡單的說就是這個function在初始化後（執行完）就被free了。
*/
static int __init init_modules(void)
{
    printk("hello world\n");
    return 0;
}

/*
而__exit的解釋是：
__exit is used to declare a function which is only required on exit: 
the function will be dropped if this file is not compiled as a module.
*/

static void __exit exit_modules(void)
{
    printk("goodbye\n");
}

module_init(init_modules);
/*

module_init()的解釋是：
The module_init() macro defines which function is to be called at module insertion time (if the file is compiled as a module), or at boot time: if the file is not compiled as a module the module_init() macro becomes equivalent to __initcall(), which through linker magic ensures that the function is called on boot.
主要是用來設定當insert該module後，應該要被執行的進入點（enrty point）。
*/
module_exit(exit_modules);
/*
module_exit()的解釋是：
This macro defines the function to be called at module removal time (or never, in the case of the file compiled into the kernel). It will only be called if the module usage count has reached zero. This function can also sleep, but cannot fail: everything must be cleaned up by the time it returns.
Note that this macro is optional: if it is not present, your module will not be removable (except for 'rmmod -f').
簡言之，就是當user執行rmmod時，會被執行到的function。沒有module_exit()，module就不能被rmmod。

*/