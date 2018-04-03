#ifndef _PCI_CORE_H_
#define _PCI_CORE_H_

#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/kmod.h>
#include <linux/mutex.h>
#include <linux/pci.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/version.h>
#include <linux/cdev.h>
#include "pci_drv.h"


#define DEBUG_PR(fmt, arg...)	printk("\n[DEB-B]<%s>" "" fmt, __func__,##arg);
#define INFO_PR(fmt, arg...)	printk("\n[INFO-B]<%s>" "" fmt, __func__,##arg);
#define WARN_PR(fmt, arg...)	printk("\n[WARN-B]<%s>" "" fmt, __func__,##arg);				
#define ERR_PR(fmt, arg...)		printk("\n[ERR-B]<%s>" "" fmt, __func__,##arg);			
					
	
#define ALTPCIE_DEVICE_NAME	"skel"
#define ALTPCIE_CDEV_NAME	ALTPCIE_DEVICE_NAME

#define ALTPCIE_CDEV_NUM_MAX	1

struct PCI_CONF {
	unsigned short vendor_id;
	unsigned short device_id;
	unsigned short command;
	unsigned short status;
	unsigned long revision_id:8;
	unsigned long class_code:24;
	unsigned char cache_line;
	unsigned char latency_timer;
	unsigned char header_type;
	unsigned char bist;
	unsigned int base_addr_0;
	unsigned int base_addr_1;
	unsigned int base_addr_2;
	unsigned int base_addr_3;
	unsigned int base_addr_4;
	unsigned int base_addr_5;
	unsigned int card_bus_cis;
	unsigned short sub_vendor_id;
	unsigned short sub_device_id;
	unsigned short rom_addr;
	unsigned char interrupt_line;
	unsigned char interrupt_pin;
	unsigned char mix_gnt;
	unsigned char max_lat;
};



/** number of BARs on the device */
#define PCI_BAR_NUM (6)
/**
 * struct skeleton - All internal data for one instance of device
 * @pdev: PCI device
 * @lock: ioctl serialization mutex
 */
struct skeleton {
	struct mutex		lock;
	struct list_head	devlist;	
	short				vendor;
	short				device;	
	struct pci_dev		*pci_dev;
			
  	/* upstream root node */
 	struct pci_dev		*upstream;
  	int					pci_gen;
  	int					pci_num_lanes;	
	int					board_id;	
	/**
	 * kernel virtual address of the mapped BAR memory and IO regions of
	 * the End Point. Used by pci_map_bars()/pci_unpci_map_bars().
	 */
	void				* __iomem bar[PCI_BAR_NUM];
	unsigned long 		bar_length[PCI_BAR_NUM];	
	/* if the device regions could not be allocated, assume and remember it
	 * is in use by another driver; this driver must not disable the device.
	 */
	int					in_use;
	/* irq line successfully requested by this driver, -1 otherwise */
	int					irq_line;
	/* board revision */
	u8					revision;		
	/* whether this driver could obtain the regions */;
	int					got_devlist;
	int					probe_status;
		#define GOT_REGIONS		(0x01<<1)
		#define GOT_PCI_DEVICE	(0x01<<2)
		#define GOT_PCI_DMA		(0x01<<3)
		#define GOT_PCI_BAR		(0x01<<4)
		#define GOT_DEVLIST		(0x01<<5)
		#define GOT_CDEV		(0x01<<6)
		
	char				name[32];	
	struct cdev			*cdev;
	int					cdev_minor;
	int					board;
};

void skeleton_ioctl_unregister(struct skeleton *ske);
int skeleton_init_cdev(struct skeleton *ske,char *dev_name);
//int skeleton_init_cdev(struct skeleton *ske);
int pci_get_conf(struct skeleton *ske,void *arg) ;

#endif	//
