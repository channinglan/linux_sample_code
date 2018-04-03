
#include <linux/uaccess.h>
#include "pci_core.h"
#include "pci_drv.h"







int bar_read(struct skeleton *ske,unsigned  int bar, unsigned int reg_offset)
{
	unsigned int *p= ske->bar[bar];
	unsigned int value=0,c=0;
	unsigned int offset;
	
	if(ske == NULL) {
		ERR_PR("%s ske == NULL",__FUNCTION__);
		return 0;
	}


	offset=reg_offset>>2;
	
retry:	
	mb();
	udelay(1);
	value = ioread32(p+offset);

	if(value == 0xffffffff) {
		c++;
		if(c > 20) {
			ERR_PR("[R]0x%x = 0x%x!!",reg_offset,value);
			return 0;	
		}
		udelay(1);
		goto retry;
	}
	//if((c > 0) && (reg_offset != REG_DMA_FLAG)) {
	if(c > 1) {		
		WARN_PR(" %s[R]0x%x = 0x%x!!try %d",__FUNCTION__,reg_offset,value,c);	
	}

	udelay(1);	//[WARN-B0][1] W0x64=0x2 But R=0xf
	mb();		

	
	return value;

}



void bar_write(struct skeleton *ske, unsigned int bar, unsigned int offset, unsigned int value)
{
	unsigned int *p= ske->bar[bar];
	unsigned int fpga_offset=offset>>2;
	if(ske == NULL) {
		ERR_PR("%s ske == NULL",__FUNCTION__);
		return;
	}
	
	mb();	
	udelay(2);	//[WARN-B0][1] W0x64=0x2 But R=0xf	

	iowrite32(value, p+fpga_offset);
	wmb();
	udelay(1);	//[WARN-B0][1] W0x64=0x2 But R=0xf

}




static int skeleton_GetReg(struct skeleton *ptPrivate,
							unsigned long arg)
{
	struct ALTPCIE_REG_INFO reg_info;
//	struct skeleton_bar_info *ptBar_info;
	struct skeleton *ske = ptPrivate;
	
	INFO_PR("Start");

	if (copy_from_user(&reg_info, (void*)arg, sizeof(struct ALTPCIE_REG_INFO))) {
		ERR_PR( "Failed in a copy from user's area.");
		return -EFAULT;
	}
	
	if (PCI_BAR_NUM <= reg_info.bar) {
		WARN_PR("Failed bar:0x%lx", reg_info.bar);
		return -EINVAL;
	}


	reg_info.reg_data = bar_read(ske,reg_info.bar,reg_info.reg_add);


	if ( copy_to_user((void*)arg, &reg_info, sizeof(struct ALTPCIE_REG_INFO)) ) {
		ERR_PR("Failed in a copy to user's area.");
		return -EFAULT;
	}	
	
	INFO_PR("Success");
	
	return 0;
}

static int skeleton_SetReg(struct skeleton *ske,
							unsigned long arg)
{
	struct ALTPCIE_REG_INFO reg_info;

	INFO_PR("Start");

	if (copy_from_user(&reg_info, (void*)arg, sizeof(struct ALTPCIE_REG_INFO))) {
		ERR_PR( "Failed in a copy from user's area.");
		return -EFAULT;
	}

	if (PCI_BAR_NUM <= reg_info.bar) {
		WARN_PR("Failed bar:0x%lx", reg_info.bar);
		return -EINVAL;
	}


	bar_write(ske,reg_info.bar,reg_info.reg_add,reg_info.reg_data);
		
	INFO_PR("Success");
	
	return 0;
}




static int skeleton_get_info(struct skeleton *ske,unsigned long arg)
{
	struct pci_dev* pDev;
	struct ALTPCIE_INFO pci_info;
	int i;

	
	INFO_PR("Start");

	pDev = ske->pci_dev;
	if (copy_from_user(&pci_info, (void*)arg, sizeof(struct ALTPCIE_INFO))) {
		ERR_PR( "Failed in a copy from user's area.");
		return -EFAULT;
	}

	pci_info.fw_ver = 0x0001;
	pci_info.fpga_ver = 0x0002;	
	for(i=0;i>PCI_BAR_NUM;i++) {
		pci_info.bar[i].num = i;
		pci_info.bar[i].length = ske->bar_length[i];		
	}
	
	
	if ( copy_to_user((void*)arg, &pci_info, sizeof(struct ALTPCIE_INFO)) ) {
		ERR_PR("Failed in a copy to user's area.");
		return -EFAULT;
	}
	
	INFO_PR("Success");
	
	return 0;
}


static int skeleton_GetPciConf(struct skeleton *ske,unsigned long arg)
{
	struct pci_dev* pDev;
	struct ALTPCIE_PCI_CONF pci_config;
	
	
	INFO_PR("Start");

	pDev = ske->pci_dev;
	if (copy_from_user(&pci_config, (void*)arg, sizeof(struct ALTPCIE_PCI_CONF))) {
		ERR_PR( "Failed in a copy from user's area.");
		return -EFAULT;
	}

	pci_get_conf(ske,&pci_config);	
	
	if ( copy_to_user((void*)arg, &pci_config, sizeof(struct ALTPCIE_PCI_CONF)) ) {
		ERR_PR("Failed in a copy to user's area.");
		return -EFAULT;
	}
	
	INFO_PR("Success");
	
	return 0;
}

int mem_test(struct skeleton *ske,unsigned int bar, unsigned int offset, unsigned int length, unsigned int value)
{
	
	unsigned int *p= ske->bar[bar];
	unsigned int  l=0;
	

	
	
	
	if(ske == NULL) {
		ERR_PR("%s ske == NULL",__FUNCTION__);
		return -1;
	}

	
	if(ske->bar_length[bar]==0) {
		return -1;	
	}
	
	
	if((offset + length) > ske->bar_length[bar])  {
		return -1;
	}
	
	p= ske->bar[bar];
	for(l=0;l < length;l++) {
		iowrite32(value, p+((offset+l)>>2));	
	}
	wmb();		
	
	
	
	
	
	return 0;
	
}

static int skeleton_mem_test(struct skeleton *ske,unsigned long arg)
{
	int i;
	struct mem_test_info test_info;

	if(ske == NULL) {
		ERR_PR("%s ske == NULL",__FUNCTION__);
		return -EFAULT;
	}
	
	INFO_PR("Start");

	if (copy_from_user(&test_info, (void*)arg, sizeof(struct mem_test_info))) {
		ERR_PR( "Failed in a copy from user's area.");
		return -EFAULT;
	}

	
	if(test_info.num == MEM_TEST_ALL) {
		for(i=0;i<PCI_BAR_NUM;i++) {
			mem_test(ske,i,0,ske->bar_length[i],0x55aa55aa);
		}
	} else {
		if(test_info.num < PCI_BAR_NUM) {
			mem_test(ske,test_info.num,test_info.offset,test_info.length,0x55aa55aa);
		} else {
			return -1;
		}
	}
	

	
	
	if ( copy_to_user((void*)arg, &test_info, sizeof(struct mem_test_info)) ) {
		ERR_PR("Failed in a copy to user's area.");
		return -EFAULT;
	}
	
	INFO_PR("Success");
	
	return 0;
}

//===============================================================================//





static int skeleton_cdev_open(struct inode *inode, struct file *filp)
{
	struct skeleton *ske;

	
	INFO_PR("Start");
	ske = container_of((void *)inode->i_cdev, struct skeleton, cdev);
	filp->private_data = ske;
	
	INFO_PR("Success");
	
	return 0;
}

static int skeleton_cdev_release(struct inode *inode, struct file *filp)
{
	INFO_PR("Start");
	
	INFO_PR("Success");

	return 0;
}


#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,36)
static int skeleton_cdev_ioctl(struct inode *inode, struct file *filp,
					unsigned int cmd, unsigned long arg)	
{					

#else
static long skeleton_cdev_ioctl(struct file *fp, unsigned int cmd, unsigned long arg)
#endif	

{
	struct skeleton *ptPrivate;
	int ret = 0;

	#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,36)
	ptPrivate = filp->private_data;
	#else
	ptPrivate = (struct skeleton *) fp->private_data;
	#endif	
	

	INFO_PR("Start");

	switch(cmd) {
	case ALTPCIE_IOCG_GET_INFO:
		 ret = skeleton_get_info(ptPrivate, arg);
		break;
	case ALTPCIE_IOCG_REG:
		 ret = skeleton_GetReg(ptPrivate, arg);
		break;
	case ALTPCIE_IOCS_REG:
		 ret = skeleton_SetReg(ptPrivate, arg);
		break;
	case ALTPCIE_IOCG_PCI_CONF:
		 ret = skeleton_GetPciConf(ptPrivate, arg);
		break;		
	case ALTPCIE_IOCG_MEM_TEST:
		 ret = skeleton_mem_test(ptPrivate, arg);
		break;							
	default:
		WARN_PR("ioctl: unknown command %x", cmd);
		ret = -EINVAL;
		break;
	}
	
	//INFO_PR("Success");
	
	return ret;
}

/*
#ifdef CONFIG_COMPAT
static long skeleton_compat_ioctl(struct file *fp, unsigned int cmd, unsigned long arg)
{
	return skeleton_ioctl(fp,cmd,arg);
}
#endif
*/
static ssize_t skeleton_cdev_read(struct file* fp, char __user* buffp_p,
							size_t len, loff_t* offset_p)
{
	INFO_PR("Start");
	
	INFO_PR("Success");
	
	return 0;
}

static ssize_t skeleton_cdev_write(struct file* fp, const char __user* buffp_p,
							size_t len, loff_t* offset_p)
{
	INFO_PR("Start");
	
	INFO_PR("Success");
	return 0;
}


static struct file_operations skeleton_fops = {
	.owner		= THIS_MODULE,
	.read		= skeleton_cdev_read,
	.write		= skeleton_cdev_write,

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,36)
	.ioctl	= skeleton_cdev_ioctl,
#else
	.unlocked_ioctl	= skeleton_cdev_ioctl,
#endif
#ifdef CONFIG_COMPAT
	.compat_ioctl	= skeleton_cdev_ioctl,
#endif

	.open		= skeleton_cdev_open,
	.release	= skeleton_cdev_release,
};


#define CDEV_DRIVER_EN 1


#if CDEV_DRIVER_EN
int cdev_major = 0;
int cdev_st_minor = 0;
static int cdev_count      = 0;

struct hsrc_cdev_driver {
	struct module *module;
	struct class class;
	struct bus_type bus;
	struct device_driver driver;
	struct driver_attribute version_attr;
};





/******************************************************************************
 *
 * hsrc_driver_match()
 *
 ******************************************************************************/
static int hsrc_driver_match(struct device *dev, struct device_driver *driver)
{
#if LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,29)
	return !strncmp(dev->bus_id, driver->name, strlen(driver->name));
#else
	return !strncmp(dev_name(dev), driver->name, strlen(driver->name));
#endif
}

/******************************************************************************
 *
 * hsrc_version_show()
 *
 ******************************************************************************/
static ssize_t hsrc_version_show(struct device_driver *driver, char *buf)
{
	int len=0;
	len+=sprintf(buf+len, "name: %s\n", driver->name);
	return len;
}


/******************************************************************************
 *
 * struct hsrc_cdev_driver hsrc_driver
 *
 ******************************************************************************/
static struct hsrc_cdev_driver hsrc_driver = {
	.module = THIS_MODULE,
	.class =
	{
		.name = ALTPCIE_DEVICE_NAME,
	},
	.bus =
	{
		.name = ALTPCIE_DEVICE_NAME,
		.match = hsrc_driver_match
	},
	.driver =
	{
		.name = ALTPCIE_DEVICE_NAME,
		.bus = &hsrc_driver.bus
	},
	.version_attr =
	{
		.attr =
		{
			.name = "version",
			//It's not used in Kernel 38.
			// .owner = THIS_MODULE,
			.mode = S_IALLUGO,
		},
		.show = hsrc_version_show
	}
};

#endif


dev_t gtskeleton_no, dev_minor;

	
	
	
int skeleton_init_cdev(struct skeleton *ske,char *dev_name)
{
	int board = ske->board;
	int error;
	dev_t dev_no, dev_minor;
		
	DEBUG_PR("ioctl_register(), dev = %s, board = %d\n",dev_name, board);

	if(cdev_major == 0) {
		error = alloc_chrdev_region(&dev_no,0, ALTPCIE_CDEV_NUM_MAX, ALTPCIE_CDEV_NAME);

		if(error<0) {
			ERR_PR("Failed to get major %d, error %d.\n", MAJOR(dev_no), error);
			return error;
		}

		cdev_major = MAJOR(dev_no);
		cdev_st_minor = MINOR(dev_no);		
	}
	
	DEBUG_PR("major = 0x%x, minor = 0x%x\n", cdev_major, cdev_st_minor);
	
	dev_minor = cdev_st_minor + board;	
	
	//add device
	ske->cdev = kzalloc(sizeof(struct cdev), GFP_KERNEL);

	if(ske->cdev == NULL) {
		ERR_PR("Error: kzalloc in register_cx_dev()\n");
		goto mem_err;
	}	

	cdev_init( ske->cdev, &skeleton_fops);

	dev_no = MKDEV(cdev_major, dev_minor);
	error = cdev_add(ske->cdev, dev_no, 1);
	
	if (error) {
		ERR_PR("failed to add cdev (%d)", error);
		goto cdev_err;
	}

#if CDEV_DRIVER_EN
	/* The last step is to connect our driver to the /sys filesystem */
	DEBUG_PR( "connect our driver to the /sys filesystem\n");
	if(board==0) {
		error = bus_register(&hsrc_driver.bus);
		if(error<0) {
			ERR_PR("Failed to create cx bus /sys entry, error %d.",error);
			goto bus_register_err;
		}
		DEBUG_PR( "bus_register done");

		error = driver_register(&hsrc_driver.driver);
		if(error<0) {
			ERR_PR("Failed to create cx driver /sys entry, error %d.",error);
			goto driver_register_err;
		}
		DEBUG_PR("driver_register done");
		
		error = driver_create_file(&hsrc_driver.driver, &hsrc_driver.version_attr);

		if(error<0) {
			ERR_PR("Failed to create cx driver file entry in /sys, error %d.",error);
			goto driver_create_file_err;
		}
		DEBUG_PR("driver_create_file done");
		
		/* add devices */
		error = class_register(&hsrc_driver.class);
		if(error<0) {
			ERR_PR("Failed to create cx driver file entry in /sys, error %d.",error);
			goto class_register_err;
		}
		DEBUG_PR("class_register done");		
	}

	//add cdev
	device_create(&hsrc_driver.class, NULL, dev_no, NULL,dev_name);
	DEBUG_PR("created cdev  done: major=%d, minor=%d\n", MAJOR(dev_no), MINOR(dev_no));

	ske->cdev_minor = dev_minor;
	cdev_count++;
	return 0;

class_register_err:
	driver_remove_file(&hsrc_driver.driver, &hsrc_driver.version_attr);

driver_create_file_err:
	driver_unregister(&hsrc_driver.driver);

driver_register_err:
	bus_unregister(&hsrc_driver.bus);

bus_register_err:
	cdev_del(ske->cdev);
#endif
cdev_err:
	kfree(ske->cdev);

mem_err:
	printk("Error: Cannot register devices!!\n");



	return -1;
}


/******************************************************************************
 *
 * skeleton_ioctl_unregister()
 *
 ******************************************************************************/
void skeleton_ioctl_unregister(struct skeleton *ske)
{
	int board = ske->board;

	if (!ske) {
		printk("skeleton_ioctl_unregister: null device in %s()!\n", __func__);
		return;
	}

	DEBUG_PR("unregister hsrc-ioctl%d\n", board);

	if (cdev_major <= 0)
		return;
		
#if CDEV_DRIVER_EN
	device_destroy(&hsrc_driver.class, MKDEV(cdev_major, board+cdev_st_minor));
#endif	
	cdev_del(ske->cdev);
	
	kfree(ske->cdev);

	if(--cdev_count <= 0) {
		#if CDEV_DRIVER_EN
		class_unregister(&hsrc_driver.class);
		driver_remove_file(&hsrc_driver.driver, &hsrc_driver.version_attr);
		driver_unregister(&hsrc_driver.driver);
		bus_unregister(&hsrc_driver.bus);
		#endif
		unregister_chrdev_region(MKDEV(cdev_major, cdev_st_minor), ALTPCIE_CDEV_NUM_MAX);
		DEBUG_PR("%s unloaded properly.\n", ALTPCIE_CDEV_NAME);
	}



}	













#if 0

static dev_t gtskeleton_dev_no=0;
int skeleton_major = 0;
int skeleton_minor = 0;

int skeleton_init_cdev(struct skeleton *ske)
{
	//dev_t dev_no;
	int result;
	
	INFO_PR(" %s Start",__FUNCTION__);
#if 1
	if(skeleton_major) {
		gtskeleton_dev_no = MKDEV(skeleton_major, 0);
		result = register_chrdev_region(gtskeleton_dev_no,ALTPCIE_CDEV_NUM_MAX,ALTPCIE_CDEV_NAME);	

	} else {
		result = alloc_chrdev_region(&gtskeleton_dev_no, 0, ALTPCIE_CDEV_NUM_MAX, ALTPCIE_CDEV_NAME);
		skeleton_major = MAJOR(gtskeleton_dev_no);
	}
	
	if (result < 0) {
		WARN_PR("Failed to alloc chrdev: (%d)", result);
		return -1;
	}

	INFO_PR("%s skeleton_major=%d",ALTPCIE_CDEV_NAME,skeleton_major);
#endif	
	

	cdev_init( &ske->cdev, &skeleton_fops);

	ske->cdev.owner = THIS_MODULE;
	ske->cdev.ops = &skeleton_fops;
	
	result = cdev_add(&ske->cdev, gtskeleton_dev_no, 1);
	if (result) {
		ERR_PR("failed to add cdev (%d)", result);
		return -1;
	}
	INFO_PR("skeleton_CDEV Success dev_no=%d MAJOR=%d MINOR=%d",gtskeleton_dev_no,MAJOR(gtskeleton_dev_no),MINOR(gtskeleton_dev_no));

	return 0;
}

#endif















