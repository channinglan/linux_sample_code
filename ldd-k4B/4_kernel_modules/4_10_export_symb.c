#include <linux/init.h>
#include <linux/module.h>


extern int add_integar(int a, int b);
extern int sub_integar(int a, int b);


int add_integar2(int a, int b)
{
	return add_integar(a,b);
}
EXPORT_SYMBOL(add_integar2);

int sub_integar2(int a, int b)
{
	return sub_integar(a,b);
}
EXPORT_SYMBOL_GPL(sub_integar2);

MODULE_LICENSE("GPL v2");
