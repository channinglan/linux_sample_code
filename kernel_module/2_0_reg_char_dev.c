/*
II我們將介紹如何向Linux註冊char device，並且read/write該device。
當module被load進來時，init_modules()會被執行，而init_modules()做了幾件事情，

首先向Linux要求分配char device number 
"alloc_chrdev_region(dev_t *dev, unsigned baseminor, unsigned count, const char *name)"。
	dev是分配到的dev_t(output parameter)，
	baseminor是起始的minor number，當baseminor=0會由系統分配，否則會試圖尋找可以符合baseminor的minor number，
	count是minor的數量，
	name是註冊的名稱。

接著設定struct cdev "cdev_init()"，並且註冊到系統"cdev_add()"，在cdev_init()的同時也設定了該device的file operations。

struct file_operations就是device的file operations，
簡單的說，UNIX所有的東西都可以視為檔案，當然包含device，既然是檔案，就會有open、close/release、read和write等等操作，
而這些操作都會觸發對應的function來反應，這就是file operations的功能了。

我們的read會回應"brook"，而write則是藉由printk()印出來，open和close/release都單純的回應成功而已。

而exit_modules()當然就是負責歸還當初和OS要的資源了"kfree()"，當然包含之前註冊的char device number，
現在也要unregister "unregister_chrdev_region()"。
*/


#include <linux/init.h>
#include <linux/init.h>
#include <linux/module.h>

#include <linux/fs.h> // chrdev
#include <linux/cdev.h> // cdev_add()/cdev_del()
#include <asm/uaccess.h> // copy_*_user()
#include <linux/slab.h>	//fix error: implicit declaration of function ‘kzalloc’
MODULE_LICENSE("GPL");

#define DEV_BUFSIZE 1024

int dev_major;
int dev_minor;
struct cdev *dev_cdevp = NULL;

int dev_open(struct inode*, struct file*);
int dev_release(struct inode*, struct file*);
ssize_t dev_read(struct file*, char __user*, size_t, loff_t*);
ssize_t dev_write(struct file*, const char __user *, size_t, loff_t*);
static void __exit exit_modules(void);

struct file_operations dev_fops = {
    .owner = THIS_MODULE,
    .open = dev_open,
    .release = dev_release,
    .read = dev_read,
    .write = dev_write,
};

int dev_open(struct inode *inode, struct file *filp)
{
    printk("%s():\n", __FUNCTION__);
    return 0;
}

int dev_release(struct inode *inode, struct file *filp)
{
    printk("%s():\n", __FUNCTION__);
    return 0;
}

ssize_t 
dev_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
    char data[] = "brook";
    ssize_t ret = 0;

    printk("%s():\n", __FUNCTION__);

    if (*f_pos >= sizeof(data)) {
        goto out;
    }

    if (count > sizeof(data)) {
        count = sizeof(data);
    }

    if (copy_to_user(buf, data, count) < 0) {
        ret = -EFAULT;
        goto out;
    }
    *f_pos += count;
    ret = count;

out:
    return ret;
}

ssize_t 
dev_write(struct file *filp, const char __user *buf, size_t count,
          loff_t *f_pos)
{
    char *data;
    ssize_t ret = 0;

    printk("%s():\n", __FUNCTION__);

    data = kzalloc(sizeof(char) * DEV_BUFSIZE, GFP_KERNEL);
    if (!data) {
        return -ENOMEM;
    }

    if (count > DEV_BUFSIZE) {
        count = DEV_BUFSIZE;
    }

    if (copy_from_user(data, buf, count) < 0) {
        ret = -EFAULT;
        goto out;
    }
    printk("%s(): %s\n", __FUNCTION__, data);
    *f_pos += count;
    ret = count;
out:
    kfree(data);
    return ret;
}

static int __init init_modules(void)
{
    dev_t dev;
    int ret;

    ret = alloc_chrdev_region(&dev, 0, 1, "brook");
    if (ret) {
        printk("can't alloc chrdev\n");
        return ret;
    }

    dev_major = MAJOR(dev);
    dev_minor = MINOR(dev);
    printk("register chrdev(%d,%d)\n", dev_major, dev_minor);

    dev_cdevp = kzalloc(sizeof(struct cdev), GFP_KERNEL);
    if (dev_cdevp == NULL) {
        printk("kzalloc failed\n");
        goto failed;
    }
    cdev_init(dev_cdevp, &dev_fops);
    dev_cdevp->owner = THIS_MODULE;
    ret = cdev_add(dev_cdevp, MKDEV(dev_major, dev_minor), 1);
    if (ret < 0) {
        printk("add chr dev failed\n");
        goto failed;
    }

    return 0;

failed:
    if (dev_cdevp) {
        kfree(dev_cdevp);
        dev_cdevp = NULL;
    }
    return 0;
}

static void __exit exit_modules(void)
{
    dev_t dev;

    dev = MKDEV(dev_major, dev_minor);
    if (dev_cdevp) {
        cdev_del(dev_cdevp);
        kfree(dev_cdevp);
    }
    unregister_chrdev_region(dev, 1);
    printk("unregister chrdev\n");
}

module_init(init_modules);
module_exit(exit_modules);



