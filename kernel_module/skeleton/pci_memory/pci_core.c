


#include "pci_core.h"

static unsigned int devcount = 0;
	struct skeleton *skel;

static const struct pci_device_id pci_pci_tbl[] = {
	{ PCI_DEVICE(0x180c, 0xd686) },
	{ 0, }
};

/**
 * Specifies those BARs to be mapped and the length of each mapping.
 *
 * Zero (0) means do not map, otherwise specifies the BAR lengths to be mapped.
 * If the actual BAR length is less, this is considered an error; then
 * reconfigure your PCIe core.
 *
 * @see ug_pci_express 8.0, table 7-2 at page 7-13.
 */
static const unsigned long bar_min_len[PCI_BAR_NUM] =
	//{ 32768, 0, 256, 0, 0, 0 };
//	{ 32768, 0, 1024, 0, 32768, 0 };
	{ 1024, 0, 1024, 0, 1024, 0 };

#if 0
/*
 * Interrupt handler: typically interrupts happen after a new frame has been
 * captured. It is the job of the handler to remove the new frame from the
 * internal list and give it back to the vb2 framework, updating the sequence
 * counter and timestamp at the same time.
 */
static irqreturn_t pci_irq(int irq, void *dev_id)
{
	DEBUG_PR("\n pci_irq");
	return IRQ_HANDLED;
}

#endif
/*
 * The initial setup of this device instance. Note that the initial state of
 * the driver should be complete. So the initial format, standard, timings
 * and video input should all be initialized to some reasonable value.
 */

 
/********************************************************************************
 *
 * PCI BUS
 *
 ********************************************************************************/

/* Find upstream PCIe root node.
 * Used for re-training and disabling AER. */
static struct pci_dev* find_upstream_dev (struct pci_dev *dev) {
  struct pci_bus *bus = 0;
  struct pci_dev *bridge = 0;
  struct pci_dev *cur = 0;
  int found_dev = 0;

  bus = dev->bus;
  if (bus == 0) {
    WARN_PR("Device doesn't have an associated bus!\n");
    return 0;
  }

  bridge = bus->self;
  if (bridge == 0) {
    WARN_PR("Can't get the bridge for the bus!\n");
    return 0;
  }

  INFO_PR("Upstream device %x/%x, bus:slot.func %02x:%02x.%02x\n",
             bridge->vendor, bridge->device,
             bridge->bus->number, PCI_SLOT(bridge->devfn), PCI_FUNC(bridge->devfn));

  INFO_PR("List of downstream devices:");
  list_for_each_entry (cur, &bus->devices, bus_list) {
    if (cur != 0) {
     INFO_PR("  %x/%x", cur->vendor, cur->device);
      if (cur == dev) {
        found_dev = 1;
      }
    }
  }
  DEBUG_PR("\n");
  if (found_dev) {
    return bridge;
  } else {
    WARN_PR("Couldn't find upstream device!\n");
    return 0;
  }
}
 /* Check link speed and retrain it to gen2 speeds.
 * After reprogramming, the link defaults to gen1 speedata_byte = DMA_SIZE*dcount*4;ds for some reason.
 * Doing re-training by finding the upstream root device and telling it
 * to retrain itself. Doesn't seem to be a cleaner way to do this. */
void retrain_gen2 (struct skeleton *aclpci) 
{

  struct pci_dev *dev = aclpci->pci_dev;
  u16 linkstat, speed, width;
  struct pci_dev *upstream;
  int pos, upos;
  u16 status, control;
  u32 link_cap;
  int training, timeout,retrain_count=0;

  /* Defines for some special PCIe control bits */
  #define DISABLE_LINK_BIT         (1 << 4)
  #define RETRAIN_LINK_BIT         (1 << 5)
  #define TRAINING_IN_PROGRESS_BIT (1 << 11)
  #define LINKSPEED_2_5_GB         (0x1)
  #define LINKSPEED_5_0_GB         (0x2)
	DEBUG_PR("retrain_gen2! \n");

 aclpci->upstream = find_upstream_dev(dev);
  pos = pci_find_capability (dev, PCI_CAP_ID_EXP);
  if (!pos) {
    WARN_PR("Can't find PCI Express capability!\n");
    return;
  }

  /* Find root node for this bus and tell it to retrain itself. */

  upstream = aclpci->upstream;
  if (upstream == NULL) {
    WARN_PR("upstream == NULL! \n");
    return;
  }

 upos = pci_find_capability (upstream, PCI_CAP_ID_EXP);

  pci_read_config_word (upstream,	upos + PCI_EXP_LNKSTA, &status);
  pci_read_config_word (upstream,	upos + PCI_EXP_LNKCTL, &control);
  pci_read_config_dword(upstream,	upos + PCI_EXP_LNKCAP, &link_cap);
  INFO_PR("upstream PCI_EXP LNKSTA = 0x%x LNKCTL=0x%x LNKCAP=0x%x.\n", status,control,link_cap);

  pci_read_config_word (dev,		pos  + PCI_EXP_LNKSTA, &linkstat);
  pci_read_config_dword(upstream,	upos + PCI_EXP_LNKCAP, &link_cap);

  INFO_PR("upstream PCI_EXP LNKCAP=0x%x.\n", link_cap);

  speed = linkstat & PCI_EXP_LNKSTA_CLS;
  width = (linkstat & PCI_EXP_LNKSTA_NLW) >> PCI_EXP_LNKSTA_NLW_SHIFT;

  aclpci->pci_gen = (speed == LINKSPEED_5_0_GB) ? 2 : 1;
  aclpci->pci_num_lanes = width;

  if (speed == LINKSPEED_2_5_GB) {
    printk("Link is operating at 2.5 GT/s with %d lanes. Need to retrain.\n", width);
  } else if (speed == LINKSPEED_5_0_GB) {
    printk("Link is operating at 5.0 GT/s with %d lanes.\n", width);
    if (width == 4) {
      INFO_PR("All is good!\n");
      return;
    } else {
      WARN_PR("Need to retrain.\n");
    }
  } else {
    WARN_PR("Not sure what's going on. Retraining.\n");
  }

retrain:
  /* Perform the training. */
  training = 1;
  timeout = 0;
  pci_read_config_word (upstream, upos + PCI_EXP_LNKCTL, &control);
  pci_write_config_word(upstream, upos + PCI_EXP_LNKCTL, control | RETRAIN_LINK_BIT);

  while (training && timeout < 50) {
    pci_read_config_word (upstream, upos + PCI_EXP_LNKSTA, &status);
    training = (status & TRAINING_IN_PROGRESS_BIT);
    msleep (1); /* 1 ms */
    ++timeout;
  }

  if(training) {
     WARN_PR("Error: Link training timed out.");
     WARN_PR("PCIe link not established.");
  } else {
     INFO_PR("Link training completed in %d ms.", timeout);
  }

  /* Verify that it's a 5 GT/s link now */
  pci_read_config_word (dev, pos + PCI_EXP_LNKSTA, &linkstat);
  pci_read_config_dword (upstream, upos + PCI_EXP_LNKCAP, &link_cap);
  speed = linkstat & PCI_EXP_LNKSTA_CLS;
  width = (linkstat & PCI_EXP_LNKSTA_NLW) >> PCI_EXP_LNKSTA_NLW_SHIFT;

  aclpci->pci_gen = (speed == LINKSPEED_5_0_GB) ? 2 : 1;
  aclpci->pci_num_lanes = width;

  if(speed == LINKSPEED_5_0_GB)
  {
    INFO_PR("Link operating at 5 GT/s with %d lanes\n", width);
	if(width != 4) {
		retrain_count++;
		if(retrain_count > 10) {
			WARN_PR("** WARNING:only %d lanes \n", width);
 			return;
		} else {
			goto retrain;
		}
	}
  }
  else
  {
    WARN_PR("** WARNING: Link training failed.  Link operating at 2.5 GT/s with %d lanes.\n", width);
  }

  return;
}

 
 

/**************************************************************************
 * Get PCI config
 **************************************************************************/ 

int pci_get_conf(struct skeleton *ske,void *arg)                  
{
	struct pci_dev* pDev;
	struct PCI_CONF *pci_config=arg;
	u32 pci_class_revision;
	
	DEBUG_PR( "  **********  %s ******* ",__FUNCTION__);

	pDev = ske->pci_dev;

	
	pci_read_config_word(pDev, PCI_VENDOR_ID,	&pci_config->vendor_id);
	pci_read_config_word(pDev, PCI_DEVICE_ID,	&pci_config->device_id);
	pci_read_config_word(pDev, PCI_COMMAND,		&pci_config->command);
	pci_read_config_word(pDev, PCI_STATUS,		&pci_config->status);
	pci_read_config_dword(pDev, PCI_CLASS_REVISION, &pci_class_revision);
	
	pci_config->revision_id = pci_class_revision & 0xFF;
	pci_config->class_code  = pci_class_revision >> 8;
	
	pci_read_config_byte(pDev,  PCI_CACHE_LINE_SIZE,&pci_config->cache_line);
	pci_read_config_byte(pDev,  PCI_LATENCY_TIMER,	&pci_config->latency_timer);
	pci_read_config_byte(pDev,  PCI_HEADER_TYPE,	&pci_config->header_type);
	pci_read_config_byte(pDev,  PCI_BIST,		&pci_config->bist);
	pci_read_config_dword(pDev, PCI_BASE_ADDRESS_0, &pci_config->base_addr_0);
	pci_read_config_dword(pDev, PCI_BASE_ADDRESS_1, &pci_config->base_addr_1);
	pci_read_config_dword(pDev, PCI_BASE_ADDRESS_2, &pci_config->base_addr_2);
	pci_read_config_dword(pDev, PCI_BASE_ADDRESS_3, &pci_config->base_addr_3);
	pci_read_config_dword(pDev, PCI_BASE_ADDRESS_4, &pci_config->base_addr_4);
	pci_read_config_dword(pDev, PCI_BASE_ADDRESS_5, &pci_config->base_addr_5);
	pci_read_config_dword(pDev, PCI_CARDBUS_CIS,	&pci_config->card_bus_cis);
	pci_read_config_word(pDev,  PCI_SUBSYSTEM_VENDOR_ID, &pci_config->sub_vendor_id);
	pci_read_config_word(pDev,  PCI_SUBSYSTEM_ID,	&pci_config->sub_device_id);
	pci_read_config_word(pDev,  PCI_ROM_ADDRESS,	&pci_config->rom_addr);
	pci_read_config_byte(pDev,  PCI_INTERRUPT_LINE,	&pci_config->interrupt_line);
	pci_read_config_byte(pDev,  PCI_INTERRUPT_PIN,	&pci_config->interrupt_pin);
	pci_read_config_byte(pDev,  PCI_MIN_GNT,	&pci_config->mix_gnt);
	pci_read_config_byte(pDev,  PCI_MAX_LAT,	&pci_config->max_lat);
	

	INFO_PR("---PCI---");
	INFO_PR("VENDOR_ID = 0x%x"		,pci_config->vendor_id);
	INFO_PR("DEVICE_ID = 0x%x"		,pci_config->device_id);
	INFO_PR("COMMAND = 0x%x"		,pci_config->command);
	INFO_PR("STATUS = 0x%x"		,pci_config->status);
	INFO_PR("CLASS_REVISION = 0x%x "	,pci_class_revision);
	INFO_PR("CACHE_LINE_SIZE = 0x%x"	,pci_config->cache_line);
	INFO_PR("LATENCY_TIMER = 0x%x"	,pci_config->latency_timer);
	INFO_PR("HEADER_TYPE = 0x%x"		,pci_config->header_type);
	INFO_PR("BIST = 0x%x"			,pci_config->bist);
	INFO_PR("BASE_ADDRESS_0 = 0x%x"	,pci_config->base_addr_0);
	INFO_PR("BASE_ADDRESS_1 = 0x%x"	,pci_config->base_addr_1);
	INFO_PR("BASE_ADDRESS_2 = 0x%x"	,pci_config->base_addr_2);
	INFO_PR("BASE_ADDRESS_3 = 0x%x"	,pci_config->base_addr_3);
	INFO_PR("BASE_ADDRESS_4 = 0x%x"	,pci_config->base_addr_4);
	INFO_PR("BASE_ADDRESS_5 = 0x%x"	,pci_config->base_addr_5);
	INFO_PR("CARDBUS_CIS = 0x%x"		,pci_config->card_bus_cis);
	INFO_PR("SUBSYSTEM_VENDOR_ID = 0x%x"	,pci_config->sub_vendor_id);
	INFO_PR("SUBSYSTEM_ID = 0x%x"		,pci_config->sub_device_id);
	INFO_PR("ROM_ADDRESS = 0x%x"		,pci_config->rom_addr);
	INFO_PR("INTERRUPT_LINE = 0x%x"	,pci_config->interrupt_line);
	INFO_PR("INTERRUPT_PIN = 0x%x"	,pci_config->interrupt_pin);
	INFO_PR("MIN_GNT = 0x%x"		,pci_config->mix_gnt);
	INFO_PR("MAX_LAT = 0x%x"		,pci_config->max_lat);


	DEBUG_PR(" %s Success",__FUNCTION__);
	
	return 0;
}

/**************************************************************************
 * pci_show_info
 **************************************************************************/ 

void pci_show_info(struct skeleton *ske,struct pci_dev *pci_dev)
{                                         

	struct PCI_CONF pci_config;
		
	INFO_PR(" --- PCI CONFIG --- ");	
	INFO_PR("vendor: 0x%x		",pci_dev->vendor);
	INFO_PR("device: 0x%x		",pci_dev->device);
	INFO_PR("subsystem_vendor:0x%x",pci_dev->subsystem_vendor);
	INFO_PR("subsystem_device:0x%x",pci_dev->subsystem_device);
	INFO_PR("class: 0x%x		",pci_dev->class);
	INFO_PR("revision: 0x%x	",pci_dev->revision);
	INFO_PR("pin: 0x%x		",pci_dev->pin);
	INFO_PR("irq: 0x%x		",pci_dev->irq);
	INFO_PR("bus: 0x%p		",pci_dev->bus);
	INFO_PR("devfn: %d		",pci_dev->devfn);

	INFO_PR("hdr_type: 0x%x	",pci_dev->hdr_type);
//	INFO_PR("pcie_type: 0x%x	",pci_dev->pcie_type);
	INFO_PR("rom_base_reg: 0x%x	",pci_dev->rom_base_reg);
	INFO_PR("dma_mask: 0x%x	",(int)pci_dev->dma_mask);

	INFO_PR("pm_cap: 0x%x		",pci_dev->pm_cap);
	INFO_PR("pme_support: 0x%x	",pci_dev->pme_support);
	INFO_PR("d1_support: %d	",pci_dev->d1_support);
	INFO_PR("d2_support: 0x%x	",pci_dev->d2_support);
	INFO_PR("no_d1d2: %d		",pci_dev->no_d1d2);

	INFO_PR("cfg_size: 0x%x	",pci_dev->cfg_size);
	INFO_PR("pme_support: 0x%x	",pci_dev->pme_support);
	INFO_PR("d1_support: %d	",pci_dev->d1_support);
	INFO_PR("d2_support: 0x%x	",pci_dev->d2_support);
	INFO_PR("no_d1d2: %d		",pci_dev->no_d1d2);
	INFO_PR("slot: 0x%p		",pci_dev->slot);
	
	
	pci_get_conf(ske,&pci_config);	
	
}


/********************************************************************************
 *
 * pci_scan_bars()
 *
 ********************************************************************************/


//static int __devinit pci_scan_bars(struct skeleton *ske, struct pci_dev *pci_dev)
static int pci_scan_bars(struct skeleton *ske, struct pci_dev *pci_dev)
{
	int i;
	for (i = 0; i < PCI_BAR_NUM; i++) {
		unsigned long bar_start = pci_resource_start(pci_dev, i);
		if (bar_start) {
			unsigned long bar_end = pci_resource_end(pci_dev, i);
			unsigned long bar_flags = pci_resource_flags(pci_dev, i);
			INFO_PR( "BAR%d 0x%08lx-0x%08lx flags 0x%08lx",
			  i, bar_start, bar_end, bar_flags);

		}
	}
	return 0;
}

/**
 * Unmap the BAR regions that had been mapped earlier using pci_map_bars()
 */
static void pci_unpci_map_bars(struct skeleton *ske, struct pci_dev *pci_dev)
{
	int i;
	for (i = 0; i < PCI_BAR_NUM; i++) {
	  /* is this BAR mapped? */
		if (ske->bar[i]) {
			/* unmap BAR */
			pci_iounmap(pci_dev, ske->bar[i]);
			ske->bar[i] = NULL;
		}
	}
}

/**
 * Map the device memory regions into kernel virtual address space after
 * verifying their sizes respect the minimum sizes needed, given by the
 * bar_min_len[] array.
 */
//static int __devinit pci_map_bars(struct skeleton *ske, struct pci_dev *pci_dev)
static int  pci_map_bars(struct skeleton *ske, struct pci_dev *pci_dev)
{
	int rc;
	int i;
	/* iterate through all the BARs */
	for (i = 0; i < PCI_BAR_NUM; i++) {
		unsigned long bar_start = pci_resource_start(pci_dev, i);
		unsigned long bar_end = pci_resource_end(pci_dev, i);
		unsigned long bar_length = bar_end - bar_start + 1;
		ske->bar[i] = NULL;		
		
		/* do not map, and skip, BARs with length 0 */
		if (!bar_min_len[i])
			continue;
		/* do not map BARs with address 0 */
		if(bar_min_len[i] == 0) {
			INFO_PR( "BAR #%d is len 0", i);
			continue;
		}
		if (!bar_start || !bar_end) {
			INFO_PR( "BAR #%d is not present?!", i);
			//rc = -1;
			//goto next1;
			continue;
		}


		bar_length = bar_end - bar_start + 1;
		/* BAR length is less than driver requires? */
		if (bar_length < bar_min_len[i]) {
			INFO_PR( "BAR #%d length = %lu bytes but driver "
			"requires at least %lu bytes",
			i, bar_length, bar_min_len[i]);
			rc = -1;
			goto fail;
		}
		/* map the device memory or IO region into kernel virtual
		 * address space */
		ske->bar[i] = pci_iomap(pci_dev, i, bar_min_len[i]);
		if (!ske->bar[i]) {
			INFO_PR( "Could not map BAR #%d.", i);
			rc = -1;
			goto fail;
		}
		INFO_PR( "BAR[%d] mapped at 0x%p with length 0x%lx(/0x%lx).", i,
		ske->bar[i], bar_min_len[i], bar_length);
		
		ske->bar_length[i] = bar_length;
//next1:
	}
	/* successfully mapped all required BAR regions */
	rc = 0;
	goto success;
fail:
	/* unmap any BARs that we did map */
	pci_unpci_map_bars(ske, pci_dev);
success:
	return rc;
}



/**************************************************************************
 * pci_init
 **************************************************************************/ 


int pci_init(struct pci_dev *pci_dev,struct skeleton *ske)
{
	int rc=0;
	
	DEBUG_PR( "  **********  %s ******* ",__FUNCTION__);	
	/* enable device */
	rc = pci_enable_device(pci_dev);
	if (rc) {
		ERR_PR( "pci_enable_device() failed");
		return -1;
	} else {
		ske->probe_status |= GOT_PCI_DEVICE;
	}
	DEBUG_PR( "  pci_enable_device ok ");	
	/* enable bus master capability on device */
	pci_set_master(pci_dev);	
	
	
	sprintf(ske->name, "pci_skeleton");
	INFO_PR("name=%s",ske->name);	
	/** XXX check for native or legacy PCIe endpoint? */
	rc = pci_request_regions(pci_dev, ske->name);
	/* could not request all regions? */
	if (rc) {
		/* assume device is in use (and do not disable it later!) */
		ske->in_use = 1;
		ERR_PR( "pci_request_regions err. %d",rc);
		return -1;
	}
	ske->probe_status |= GOT_REGIONS;	
	INFO_PR( "  pci_request_regions ok ");	
#if 1
	/* @todo For now, disable 64-bit, because I do not understand the implications (DAC!) */
	/* query for DMA transfer */
	/* @see Documentation/PCI/PCI-DMA-mapping.txt */
	if (!pci_set_dma_mask(pci_dev, DMA_BIT_MASK(64))) {
		pci_set_consistent_dma_mask(pci_dev, DMA_BIT_MASK(64));
		/* use 64-bit DMA */
		INFO_PR( "Using a 64-bit DMA mask.");
	} else
#endif
	if (!pci_set_dma_mask(pci_dev, DMA_BIT_MASK(32))) {
		INFO_PR( "Could not set 64-bit DMA mask.");
		pci_set_consistent_dma_mask(pci_dev, DMA_BIT_MASK(32));
		/* use 32-bit DMA */
		INFO_PR( "Using a 32-bit DMA mask.");
	} else {
		ERR_PR( "No suitable DMA possible.");
		/** @todo Choose proper error return code */
		rc = -1;
		return -1;
	}
	ske->probe_status |= GOT_PCI_DMA;
	/*  retrain */
	retrain_gen2(ske);	
	
	
	/* show BARs */
	pci_scan_bars(ske, pci_dev);
	/* map BARs */
	rc = pci_map_bars(ske, pci_dev);
	if (rc) {
		ERR_PR("\n pci_map_bars  err");
		return -1;
	} else {
		ske->probe_status |= GOT_PCI_BAR;	
	}

	/* perform DMA engines test */
	pci_set_drvdata(pci_dev, ske);
//	dev_set_drvdata(&pci_dev->dev, ske);
	INFO_PR( "probe(dev = 0x%p) where ske = 0x%p", pci_dev, ske);	
	
	pci_read_config_byte(pci_dev, PCI_REVISION_ID, &ske->revision);
	pci_read_config_word(pci_dev, PCI_VENDOR_ID , &ske->vendor);
	pci_read_config_word(pci_dev, PCI_DEVICE_ID , &ske->device);

	//INFO_PR( " FPGA VER=0x%x",hsrc_get_fpag_version(ske));

	
	return 0;
}

 
static int pci_probe(struct pci_dev *pdev, const struct pci_device_id *ent)
{
	/* The initial timings are chosen to be 720p60. */
	char cdev_name[16];
	int ret;
	DEBUG_PR("%s",__FUNCTION__);
	/* Enable PCI */
	ret = pci_enable_device(pdev);
	if (ret)
		return ret;
	ret = pci_set_dma_mask(pdev, DMA_BIT_MASK(32));
	if (ret) {
		dev_err(&pdev->dev, "no suitable DMA available.\n");
		goto disable_pci;
	}

	/* Allocate a new instance */
	skel = devm_kzalloc(&pdev->dev, sizeof(struct skeleton), GFP_KERNEL);
	if (!skel)
		return -ENOMEM;
	
	skel->probe_status = 0;
#if 0
	/* Allocate the interrupt */
	ret = devm_request_irq(&pdev->dev, pdev->irq,
			       pci_irq, 0, KBUILD_MODNAME, skel);
	if (ret) {
		dev_err(&pdev->dev, "request_irq failed\n");
		goto disable_pci;
	}
#endif	
	skel->pci_dev = pdev;
	
	if(pci_init(pdev,skel) != 0) {
		ret = -EINVAL;		
		goto err_pci;
	}

	
	skel->board = devcount++;	
	
	sprintf(cdev_name, "%s%d", ALTPCIE_CDEV_NAME,skel->board);	
	ret = skeleton_init_cdev(skel,cdev_name);
	
	if (ret) {
		goto err_cdev;
	}
	skel->probe_status |= GOT_CDEV;
	
	mutex_init(&skel->lock);
	DEBUG_PR("%s-end",__FUNCTION__);

	return 0;
err_cdev:
	devcount--;	
err_pci:
disable_pci:
	pci_disable_device(pdev);
	
	return ret;
}


static void pci_remove(struct pci_dev *pdev)
{
	DEBUG_PR("%s",__FUNCTION__);	

	if(skel->probe_status & GOT_CDEV) {	
		INFO_PR("skeleton_ioctl_unregister");
		skeleton_ioctl_unregister(skel);
	}
	
	if(skel->probe_status & GOT_PCI_BAR) {	
		INFO_PR( "pci_unpci_map_bars");		
		pci_unpci_map_bars(skel, pdev);
	}
	
	if(skel->probe_status & GOT_REGIONS) {	
		INFO_PR( "pci_release_regions");
		pci_release_regions(pdev);
	}

	if(skel->probe_status & GOT_PCI_DEVICE) {		
		INFO_PR( "pci_disable_device");		
		pci_disable_device(pdev);
	}
	devcount--;	
}

static struct pci_driver pci_driver = {
	.name = KBUILD_MODNAME,
	.probe = pci_probe,
	.remove = pci_remove,
	.id_table = pci_pci_tbl,
};

MODULE_DESCRIPTION("PCI Skeleton Driver");
MODULE_AUTHOR("Channing Lan");
MODULE_LICENSE("GPL v2");
MODULE_DEVICE_TABLE(pci, pci_pci_tbl);


module_pci_driver(pci_driver);