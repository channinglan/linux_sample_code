/*
�bprocfs���U�L�k�����ϥ�mkdir/ln�����O�إߥؿ��M�إ�link�A���L�����Ѩ��API��user�F���o���Ʊ��C

static struct proc_dir_entry *proc_symlink(const char *src,
  struct proc_dir_entry *parent,const char *dest);

static struct proc_dir_entry *proc_mkdir(const char *name,
 struct proc_dir_entry *parent);

�ݦW�r�N���Dproc_symlink()�O�Ψӫإ�link���A
src�O�ɦW(basename)�A
parent�Osrc�Ҧb���ؿ��A
dest�O�nlink����H�C

proc_mkdir()�N��e���F�A�n�b���ӥؿ�(parent)�U�إ߷s���ؿ�(name)�C
�U���O�d�ҡG
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
