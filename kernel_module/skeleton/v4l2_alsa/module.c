#include <linux/init.h>
#include <linux/module.h>

MODULE_LICENSE("GPL");

extern int  vivi_init(void);
extern void vivi_exit(void);
extern int  alsa_card_minivosc_init(void);
extern void  alsa_card_minivosc_exit(void);

static int __init init_modules(void)
{
	int ret;
	ret = vivi_init();
	ret = alsa_card_minivosc_init();
    printk("hello world\n");
    return 0;
}


static void __exit exit_modules(void)
{
	vivi_exit();
	alsa_card_minivosc_exit();
    printk("goodbye\n");
}

module_init(init_modules);
module_exit(exit_modules);