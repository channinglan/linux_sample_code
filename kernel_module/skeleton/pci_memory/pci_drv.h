/*================================================
 *               ***************                 *
 *            **      \ | /      **              *
 *          *  ****   ****   ****** *            *
 *         * ---**---**-|-**--**--** *           *
 *        *     **  **  |  *  **  **  *          *
 *        *-----**--**--o-----****----*          *
 *        *     **  **  |     **      *          *
 *         * ---**---**-|--*--**---- *           *
 *          *  ****   ****   ****  *             *
 *            **      / | \     **               *
 *               **************                  *
 *       Copyright ICP Electronics (C) 2010      *
 *===============================================*/

#ifndef _ALTPCIE_DRV_H_
#define _ALTPCIE_DRV_H_


struct ALTPCIE_REG_INFO {
	unsigned int	bar;
	unsigned int	reg_add;
	unsigned int	reg_data;
};


struct ALTPCIE_PCI_CONF {
	unsigned short vendor_id;
	unsigned short device_id;
	unsigned short command;
	unsigned short status;
	unsigned int revision_id:8;
	unsigned int class_code:24;
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


struct bar_info {
	unsigned int	num;		
	unsigned int	length;	
};


struct mem_test_info {
	unsigned int	num;
	#define MEM_TEST_ALL 0xff
	unsigned int	offset;	
	unsigned int	length;	
};


struct ALTPCIE_INFO {
	unsigned int	fw_ver;
	unsigned int	fpga_ver;
	unsigned int	pci_num_lanes;
	unsigned int	pci_gen;	
	struct bar_info	bar[5];
};


#define ALTPCIE_IOC_MAGIC	0xc3
#define ALTPCIE_IOCG_GET_INFO	_IOR(ALTPCIE_IOC_MAGIC, 0, struct ALTPCIE_INFO)
#define ALTPCIE_IOCG_REG		_IOR(ALTPCIE_IOC_MAGIC, 1, struct ALTPCIE_REG_INFO)
#define ALTPCIE_IOCS_REG		_IOR(ALTPCIE_IOC_MAGIC, 2, struct ALTPCIE_REG_INFO)
#define ALTPCIE_IOCG_PCI_CONF	_IOR(ALTPCIE_IOC_MAGIC, 3, struct ALTPCIE_PCI_CONF)
#define ALTPCIE_IOCG_MEM_TEST	_IOW(ALTPCIE_IOC_MAGIC, 4, struct mem_test_info)


#endif
