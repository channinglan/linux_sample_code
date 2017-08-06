
/*
將和大家介紹procfs，雖然後來procfs已經失控，也不再建議大家使用，但是還是有一定的用途，
所以，有興趣的人可以一起探討一下。

/proc是一個特殊的檔案系統，當讀取/proc底下的檔案時，其內容由kernel動態產生，
很多應用程式也都是存取/proc底下的檔案，我們藉由兩個範例來說明。

第一個用於當/proc只提供read-only的時候，第二個用於/proc提供具有write功能的時候。

在這個例子中我們使用一個globa variable "lines"，用於存放上次列印到第幾個數值，MAX_LINE是上限。
一開始載入module就先分配記憶體給lines，接著呼叫create_proc_read_entry()建立一個唯讀的procfs檔案。


static inline struct proc_dir_entry *
    create_proc_read_entry(const char *name,
        mode_t mode, struct proc_dir_entry *base,
        read_proc_t *read_proc, void * data);

	name	是proc底下的檔案名稱。mode是檔案的mode，可以直接用0666的表示法表示，
		如果是建立目錄則要or S_IFDIR。
	base	如果為null，則會把/proc當根目錄，可以是其他的proc_dir_entry pointer，
		那麼就會以proc_dir_entry當根目錄開始往下長。
	read_proc則是當user讀取該檔案時的call-back function。
	data	則用於存放額外資訊的地方，可以先alloc，然後每次執行read時，都可以拿出來使用。


typedef int (read_proc_t)(char *page, char **start, off_t off,
    int count, int *eof, void *data);
	page	是系統給的buffer，也是我們要寫入的地方，寫到buffer裡面的資料，會被當成proc的內容。
	start	如果只要一次就可以read完，可以忽略start，否則只要將page設定給start就可以了。
	count	則是user-space想要讀取的資料量。offset指示目前檔案位置。
	eof	指示是否已經讀到結尾。data用於存放額外的資訊。
*/
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/proc_fs.h>

MODULE_LICENSE("GPL");

#define MAX_LINE        1000

/* read file operations */
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

/* private data */
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

    /* create a procfs entry for read-only */
    ent = create_proc_read_entry ("brook", S_IRUGO, NULL, read_proc, lines);
    if (!ent) {
        printk("create proc failed\n");
        kfree(lines);
    }
    return 0;
}

static void __exit exit_modules(void)
{
    if (lines) {
        kfree(lines);
    }
    /* remove procfs entry */
    remove_proc_entry("brook", NULL);
}

module_init(init_modules);
module_exit(exit_modules);