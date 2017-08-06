/*
在procfs底下無法直接使用mkdir/ln等指令建立目錄和建立link，不過有提供兩個API讓user達成這兩件事情。

static struct proc_dir_entry *proc_symlink(const char *src,
  struct proc_dir_entry *parent,const char *dest);

static struct proc_dir_entry *proc_mkdir(const char *name,
 struct proc_dir_entry *parent);

看名字就知道proc_symlink()是用來建立link的，
src是檔名(basename)，
parent是src所在的目錄，
dest是要link的對象。

proc_mkdir()就更容易了，要在那個目錄(parent)下建立新的目錄(name)。
下面是範例：
*/
#include <linux/init.h>
#include <linux/module.h>
#include <linux/proc_fs.h>

MODULE_LICENSE("GPL");

static char *bdir = "brook_dir";
module_param(bdir, charp, 0644);
MODULE_PARM_DESC(dir, "brook's dir");

static char *bfile = "brook_file";
module_param(bfile, charp, 0644);
MODULE_PARM_DESC(bfile, "brook's file");

static struct proc_dir_entry *ent = NULL;

static int __init init_modules(void)
{
    if (!(ent = proc_mkdir(bdir, NULL))) {
        printk("create dir \"%s\" failed\n", bdir);
        return -1;
    }

    if (!proc_symlink(bfile, ent, "../uptime")) {
        printk("create symlink \"%s\" failed\n", bfile);
        return -1;
    }

    return 0;
}

static void __exit exit_modules(void)
{
    remove_proc_entry(bfile, ent);
    if (ent) {
        remove_proc_entry(bdir, NULL);
    }
}

module_init(init_modules);
module_exit(exit_modules);
