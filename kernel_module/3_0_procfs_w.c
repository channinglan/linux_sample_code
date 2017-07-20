#include <linux/init.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>

/*
第二個範例則提供了write的功能，所以不能直接使用create_proc_read_entry()建立procfs檔案，而是要使用create_proc_entry()建立procfs檔案，並且設定read/write file operations，以及"data"。該範例中的write是設定line。

typedef int (write_proc_t)(struct file *file, const char __user *buffer,
    unsigned long count, void *data);

*/


MODULE_LICENSE("GPL");

#define MAX_LINE        1000

/* write file operations */
static int write_proc(struct file *file, const char __user *buf,
                       unsigned long count, void *data)
{
    char num[10], *p;
    int nr, len;

    /* no data be written */
    if (!count) {
        printk("count is 0\n");
        return 0;
    }

    /* Input size is too large to write our buffer(num) */
    if (count > (sizeof(num) - 1)) {
        printk("input is too large\n");
        return -EINVAL;
    }

    if (copy_from_user(num, buf, count)) {
        printk("copy from user failed\n");
        return -EFAULT;
    }

    /* atoi() */
    p = num;
    len = count;
    nr = 0;
    do {
        unsigned int c = *p - '0';
        if (*p == '\n') {
            break;
        }
        if (c > 9) {
            printk("%c is not digital\n", *p);
            return -EINVAL;
        }
        nr = nr * 10 + c;
        p++;
    } while (--len);
    *(uint32_t*) data = nr;

    return count;
}

static int read_proc(char *page, char **start, off_t off,
                       int count, int *eof, void *data)
{
    uint32_t curline;
    char *p, *np;

    *start = p = np = page;
    curline = *(uint32_t*) data;

    printk("count=%d, off=%ld\n", count, off);
    for (;curline < MAX_LINE && (p - page) < count; curline++) {
        np += sprintf(p, "Line #%d: This is Brook's demo\n", curline);
        if ((count - (np - page)) < (np - p)) {
            break;
        }
        p = np;
    }

    if (curline < MAX_LINE) {
        *eof = 1;
    }
    *(uint32_t*)data = curline;
    return (p - page);
}

static uint32_t *lines;

static int __init init_modules(void)
{

    struct proc_dir_entry *ent;
    lines = kzalloc(sizeof(uint32_t), GFP_KERNEL);
    if (!lines) {
        printk("no mem\n");
        return -ENOMEM;
    }
    *lines = 0;

    ent = create_proc_entry("brook", S_IFREG | S_IRWXU, NULL);
    if (!ent) {
        printk("create proc failed\n");
        kfree(lines);
    } else {
        ent->write_proc = write_proc;
        ent->read_proc = read_proc;
        ent->data = lines;
    }
    return 0;
}

static void __exit exit_modules(void)
{
    if (lines) {
        kfree(lines);
    }
    remove_proc_entry("brook", NULL);
}

module_init(init_modules);
module_exit(exit_modules);