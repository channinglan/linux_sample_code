
/*
�N�M�j�a����procfs�A���M���procfs�w�g�����A�]���A��ĳ�j�a�ϥΡA���O�٬O���@�w���γ~�A
�ҥH�A�����쪺�H�i�H�@�_���Q�@�U�C

/proc�O�@�ӯS���ɮרt�ΡA��Ū��/proc���U���ɮ׮ɡA�䤺�e��kernel�ʺA���͡A
�ܦh���ε{���]���O�s��/proc���U���ɮסA�ڭ��ǥѨ�ӽd�Ҩӻ����C

�Ĥ@�ӥΩ��/proc�u����read-only���ɭԡA�ĤG�ӥΩ�/proc���Ѩ㦳write�\�઺�ɭԡC

�b�o�ӨҤl���ڭ̨ϥΤ@��globa variable "lines"�A�Ω�s��W���C�L��ĴX�ӼƭȡAMAX_LINE�O�W���C
�@�}�l���Jmodule�N�����t�O���鵹lines�A���۩I�screate_proc_read_entry()�إߤ@�Ӱ�Ū��procfs�ɮסC


static inline struct proc_dir_entry *
    create_proc_read_entry(const char *name,
        mode_t mode, struct proc_dir_entry *base,
        read_proc_t *read_proc, void * data);

	name	�Oproc���U���ɮצW�١Cmode�O�ɮת�mode�A�i�H������0666����ܪk��ܡA
		�p�G�O�إߥؿ��h�nor S_IFDIR�C
	base	�p�G��null�A�h�|��/proc��ڥؿ��A�i�H�O��L��proc_dir_entry pointer�A
		����N�|�Hproc_dir_entry��ڥؿ��}�l���U���C
	read_proc�h�O��userŪ�����ɮ׮ɪ�call-back function�C
	data	�h�Ω�s���B�~��T���a��A�i�H��alloc�A�M��C������read�ɡA���i�H���X�ӨϥΡC


typedef int (read_proc_t)(char *page, char **start, off_t off,
    int count, int *eof, void *data);
	page	�O�t�ε���buffer�A�]�O�ڭ̭n�g�J���a��A�g��buffer�̭�����ơA�|�Q��proc�����e�C
	start	�p�G�u�n�@���N�i�Hread���A�i�H����start�A�_�h�u�n�Npage�]�w��start�N�i�H�F�C
	count	�h�Ouser-space�Q�nŪ������ƶq�Coffset���ܥثe�ɮצ�m�C
	eof	���ܬO�_�w�gŪ�쵲���Cdata�Ω�s���B�~����T�C
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