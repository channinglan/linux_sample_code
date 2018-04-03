

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <unistd.h>
#include <inttypes.h>
#include "../pci_drv.h"


/* PCIE Device ID */
#define TARGET_DEVICE_ID_PCIE_A         (0x180cd686)
/* PCIE LINK Register */
#define PCIE_BOOT_FLAG                  (0x00000000)
#define PCIE_REMAP_BAR2_OFFSET          (0x000008AC)
#define PCIE_REMAP_BAR4_OFFSET          (0x000008B4)



#define DEBUG_PR(fmt, arg...)	printf("\n[DEB-B]<%s>" "" fmt, __func__,##arg);
#define INFO_PR(fmt, arg...)	printf("\n[INFO-B]<%s>" "" fmt, __func__,##arg);
#define WARN_PR(fmt, arg...)	printf("\n[WARN-B]<%s>" "" fmt, __func__,##arg);				
#define ERR_PR(fmt, arg...)		printf("\n[ERR-B]<%s>" "" fmt, __func__,##arg);		



struct device_info {
	int num;
	int fd;
	char name[20];	
	struct ALTPCIE_INFO binfo;	
};

static int total_dev=0;
static struct device_info dev[4];
const char optstring[] = "d:";



//----------------------------------------------------------------------
// MAIN
//----------------------------------------------------------------------
/* Data Buffer */
#define SRAM_DATA_MAX_SIZE              (0x00018000)
uint32_t data_buf[SRAM_DATA_MAX_SIZE / 4];
uint32_t temp_buf[SRAM_DATA_MAX_SIZE / 4];

int main(int argc, char	*argv[])
{
	int fd,dev_sel,run_dev;
	int opt = 0;
	char deviceName[256];
	int lenDeviceName;
	int c,i=0,bar_num=0;
    FILE *fp = NULL;
    uint8_t *mmap_remap[6] = {MAP_FAILED};
    uint8_t *buf;
    uint32_t bus, dev, ssid;
    uint64_t bar_phys[6];
    uint32_t bar_size[6];
    char *str;	
    uint32_t prev_remap = 0xFFFFFFFF;
    uint32_t mmap_remap_size;
    uint32_t mmap_memrw_size;
    uint32_t status_reg;
    uint32_t config_reg;
    uint32_t write_reg_data;
    struct stat st;
	
	printf("\n============== Build %s %s ==============\n",__DATE__,__TIME__);

	

    /* open proc */
    fp = fopen("/proc/bus/pci/devices", "r");
    if (fp == NULL) {
        fprintf(stderr, "open error: /proc/bus/pci/devices\n");
        goto EXIT;
    }

    str = (char *)temp_buf;

	printf("search device \n");
		
    /* search device */
    while (fgets(str, 1024, fp) != NULL) {
        sscanf(str, "%x %x %*x %" SCNx64 " %" SCNx64 " %" SCNx64 " %" SCNx64 " %" SCNx64 " %" SCNx64 " %*x %x %x %x %x %x %x", &bus, &dev, &bar_phys[0], &bar_phys[1], &bar_phys[2], &bar_phys[3], &bar_phys[4], &bar_phys[5], &bar_size[0], &bar_size[1], &bar_size[2], &bar_size[3], &bar_size[4], &bar_size[5]);
        if (dev == TARGET_DEVICE_ID_PCIE_A) {
			
        printf("%s\n",str);			
        printf("BAR0: 0x%016" PRIX64 " (0x%08X)\n", bar_phys[0], bar_size[0]);
        printf("BAR1: 0x%016" PRIX64 " (0x%08X)\n", bar_phys[1], bar_size[1]);
        printf("BAR2: 0x%016" PRIX64 " (0x%08X)\n", bar_phys[2], bar_size[2]);
        printf("BAR3: 0x%016" PRIX64 " (0x%08X)\n", bar_phys[3], bar_size[3]);
        printf("BAR4: 0x%016" PRIX64 " (0x%08X)\n", bar_phys[4], bar_size[4]);
        printf("BAR5: 0x%016" PRIX64 " (0x%08X)\n", bar_phys[5], bar_size[5]);			
	printf(" bus=0x%x,dev=0x%x\n",bus,dev);			
                printf("%d %02x:%02x.%x ", i, (bus >> 8) & 0xFF, (bus >> 3) & 0x1F, bus & 0x07);
                printf("\n");
                i++;
				break;
        }
    }

    fclose(fp);
    fp = NULL;	


	printf(" bus=0x%x,dev=0x%x\n",bus,dev);

    sprintf(str, "/proc/bus/pci/%02x/%02x.%x", (bus >> 8) & 0xFF, (bus >> 3) & 0x1F, bus & 0x07);
    if (stat(str, &st) != 0) {
        sprintf(str, "/proc/bus/pci/0000:%02x/%02x.%x", (bus >> 8) & 0xFF, (bus >> 3) & 0x1F, bus & 0x07);
    }

	printf(" open proc \n");
    /* open proc */
    fp = fopen(str, "rb");
    if (fp == NULL) {
        fprintf(stderr, "open error: %s\n", str);
        goto EXIT;
    }

    buf = (uint8_t *)data_buf;

	printf(" read config \n");
    /* read config */
    if (fread(buf, 1, 64, fp) != 64) {
        fprintf(stderr, "read error: %s\n", str);
        goto EXIT;
    }

    fclose(fp);
    fp = NULL;

	
    /* get subsystem id and subsystem vendor id */
    ssid = (buf[0x2D] << 24) | (buf[0x2C] << 16) | (buf[0x2F] << 8) | buf[0x2E];
	printf(" get subsystem id and subsystem vendor id 0x%x\n",ssid);
	
	
	printf(" check command register \n");	
    /* check command register */
    if ((buf[0x04] & 0x02) == 0) {
        /* check enable file */
        sprintf(str, "echo 1 > /sys/bus/pci/devices/0000:%02x:%02x.%x/enable", (bus >> 8) & 0xFF, (bus >> 3) & 0x1F, bus & 0x07);
        if (stat(str + 9, &st) != 0) {
            strcat(str, "d");
        }

        /* pci_enable_device() */
        if (system(str) < 0) {
            fprintf(stderr, "error: %s\n", str);
            goto EXIT;
        }
    }

	printf(" open memory \n");	
    /* open memory */
    fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (fd < 0) {
        fprintf(stderr, "open error: /dev/mem\n");
        goto EXIT;
    }

    mmap_remap_size = (uint32_t)bar_size[0];
    mmap_memrw_size = (uint32_t)bar_size[2];	
	
	
	for(bar_num=0;bar_num<6;bar_num++) {
		printf("\nBAR%d: 0x%016" PRIX64 " (0x%08X)\n",bar_num, bar_phys[bar_num], bar_size[bar_num]);
		mmap_remap[bar_num] = MAP_FAILED;
		if(bar_size[bar_num]) {
            /* mmap BAR0 */
            mmap_remap[bar_num] = mmap(NULL, bar_size[bar_num], PROT_READ | PROT_WRITE, MAP_SHARED, fd, (off_t)(bar_phys[bar_num] & ~15ULL));
            if (mmap_remap[bar_num] == MAP_FAILED) {
                fprintf(stderr, "mmap error: BAR%d\n",bar_num);
                goto EXIT;
            }

			for(i=0;i<0x100;i+=4) {
				if((i%0x10)==0) {
					printf("\n0x%4x:",i);
				}
				printf(" 0x%8x",*((volatile uint32_t *)(mmap_remap[bar_num] + i)));			
			}
		}
		printf("\n");
	}
	 goto EXIT;
select_dev:	

//	fd = dev[run_dev].fd;
	
	if (fd < 0) {
		fprintf(stderr,"open");
		return -1;
	}
	//GetInfo(fd,dev_sel);
	for(;;) {
		int ch;
		int isExit = 0;

		//show_menu(fd,run_dev);
		scanf("%d", &ch);
		switch(ch) {								
		case 98:
			goto select_dev;
		case 99:
			isExit = 1;
			break;																				
		default:
			printf("Input error!(\'%c\',0x%x)\n", ch, ch);
			break;
		}


		if (isExit) {
			break;
		}
	}

EXIT:
	
	return 0;
}

	
	
	
