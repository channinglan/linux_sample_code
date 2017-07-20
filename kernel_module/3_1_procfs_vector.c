/*
相信很多人有讀寫過/proc/sys/kernel/printk來控制printk的level，
於是乎我就仿照了kernel/sysctl.c的do_proc_dointvec()寫了一個這樣的code，
我的write_proc_t就是在做do_proc_dointvec()當中的write。
kernel因為沒有豐富的library，所以作這樣的事情得小繞一下。
*/

#include <linux/init.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/ctype.h>

MODULE_LICENSE("GPL");

static int int_vec[] = {1, 1, 1};

static int write_proc(struct file *file, const char __user *buf,
                       unsigned long count, void *data)
{
    int *i, vleft, neg, left = count;
    char __user *s = buf;
    char tmpbuf[128], *p;
    size_t len;
    unsigned long ulval;

    i = int_vec;
    vleft = sizeof(int_vec)/sizeof(int_vec[0]);

    for(;left && vleft--; i++) {
        while(left) {
            char c;
            if (get_user(c, s)) {
                return -EFAULT;
            }
            if (!isspace(c)) {
                break;
            }
            left--;
            s++;
        }
        if (!left) {
            break;
        }
        neg = 0;
        len = left;
        if (len > sizeof(tmpbuf) - 1) {
            len = sizeof(tmpbuf) - 1;
        }
        if (copy_from_user(tmpbuf, s, len)) {
            return -EFAULT;
        }
        tmpbuf[len] = 0;
        p = tmpbuf;
        if (*p == '-' && left > 1) {
            neg = 1;
            p++;
        }
        if (*p < '0' || *p > '9') {
            break;
        }
        ulval = simple_strtoul(p, &p, 0);
        len = p - tmpbuf;
        if ((len < left) && *p && !isspace(*p)) {
            break;
        }
        *i = neg ? -ulval : ulval;
        s += len;
        left -= len;
    }
    return count;
}

static int read_proc(char *page, char **start, off_t off,
                       int count, int *eof, void *data)
{
    int *i, vleft;
    char *p;

    i = (int *) int_vec;
    vleft = sizeof(int_vec)/sizeof(int_vec[0]);

    for (p = page, i = int_vec; vleft--; i++) {
        p += sprintf(p, "%d\t", *i);
    }
    *(p++) = '\n';
    *eof = 1;
    return (p - page);
}

static int __init init_modules(void)
{

    struct proc_dir_entry *ent;

    ent = create_proc_entry("brook_vec", S_IFREG | S_IRWXU, NULL);
    if (!ent) {
        printk("create proc child failed\n");
    } else {
        ent->write_proc = write_proc;
        ent->read_proc = read_proc;
    }
    return 0;
}

static void __exit exit_modules(void)
{
    remove_proc_entry("brook_vec", NULL);
}

module_init(init_modules);
module_exit(exit_modules);
