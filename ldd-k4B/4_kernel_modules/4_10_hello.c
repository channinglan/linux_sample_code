/*
 * a simple kernel module: hello
 *
 * Copyright (C) 2014 Barry Song  (baohua@kernel.org)
 *
 * Licensed under GPLv2 or later.
 */

#include <linux/init.h>
#include <linux/module.h>

extern int add_integar(int a, int b);
extern int add_integar2(int a, int b);

extern int sub_integar(int a, int b);
extern int sub_integar2(int a, int b);

static int __init hello_init(void)
{
	printk(KERN_INFO "Hello World enter\n");
	printk(KERN_INFO "add_integar %d\n",add_integar(1,2));	
	printk(KERN_INFO "add_integar2 %d\n",add_integar2(1,2));		
	printk(KERN_INFO "sub_integar %d\n",sub_integar(1,2));	
	printk(KERN_INFO "sub_integar2 %d\n",sub_integar2(1,2));	
	
	
	
	return 0;
}
module_init(hello_init);

static void __exit hello_exit(void)
{
	printk(KERN_INFO "Hello World exit\n");
}
module_exit(hello_exit);

MODULE_AUTHOR("Barry Song <21cnbao@gmail.com>");
MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("A simple Hello World Module");
MODULE_ALIAS("a simplest module");
