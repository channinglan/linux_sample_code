/*
Linux Module允許使用者在insmod時帶入相關的parameters，這些parameters必須被宣告成golbal，
並且使用module_param()宣告資料型態與權限，目前支援的資料型態有byte, short, ushort, int, 
uint, long, ulong, charp, bool等等。也可以使用module_param_array(name, type, num, perm)
宣告成陣列。perm(權限)會決定/sys/module/顯示該參數的權限。
*/
#include <linux/init.h>
#include <linux/module.h>

MODULE_LICENSE("GPL");

static unsigned char b_byte = 1;
module_param(b_byte, byte, S_IRUGO|S_IWUSR);

static short int b_short = 2;
module_param(b_short, short, S_IRUGO|S_IWUSR);

static unsigned short int b_ushort = 3;
module_param(b_ushort, ushort, S_IRUGO|S_IWUSR);

static int b_int = 6;
module_param(b_int, int, S_IRUGO|S_IWUSR);

static unsigned int b_uint = 5;
module_param(b_uint, uint, S_IRUGO|S_IWUSR);

static long b_long = 6;
module_param(b_long, long, S_IRUGO|S_IWUSR);

static unsigned long b_ulong = 7;
module_param(b_ulong, ulong, S_IRUGO|S_IWUSR);

static char *b_charp = "brook";
module_param(b_charp, charp, S_IRUGO|S_IWUSR);

static int b_bool = 1;
module_param(b_bool, bool, S_IRUGO|S_IWUSR);

static int __init init_modules(void)
{
	printk(KERN_ALERT "Hello world\n");
	printk("b_byte: %d\n", b_byte);
	printk("b_short: %d\n", b_short);
	printk("b_ushort: %u\n", b_ushort);
	printk("b_int: %d\n", b_int);
	printk("b_uint: %u\n", b_uint);
	printk("b_long: %ld\n", b_long);
	printk("b_ulong: %lu\n", b_ulong);
	printk("b_charp: %s\n", b_charp);
	printk("b_bool: %d\n", b_bool);

	return 0;
}

static void __exit exit_modules(void)
{
	printk(KERN_ALERT "Goodbye world\n");
}

module_init(init_modules);
module_exit(exit_modules);

