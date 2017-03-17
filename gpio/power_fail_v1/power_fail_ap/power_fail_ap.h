#ifndef POWER_FAIL_H
#define POWER_FAIL_H


#define SIG_TEST 44	// we choose 44 as our signal number (real-time signals are in the range of 33 to 64)


typedef struct {
	unsigned int user_pid;	
} gpio_params;

/* Use 'g' as magic number */
#define IOC_MAGIC	'g'

#define GPIO_SET_PID		_IOW(IOC_MAGIC, 0x01,unsigned int)
#define GPIO_SEND_SIG_TEST	_IO(IOC_MAGIC, 0x02)

#define GPIO_GROUP0	0 //GPIO registers set '0' (GPIO0_* registers) are for GPIO pins 0 through 31.
#define GPIO_GROUP1	1 //GPIO registers set '1' (GPIO1_* registers) are for GPIO pins 32 through 63.
#define GPIO_GROUP2	2 //GPIO registers set '2' (GPIO2_* registers) are for GPIO pins 64 through 95.

#define GPIO0		1
#define GPIO1		2
#define GPIO2		3
#define GPIO3		4
#define GPIO4		5
#define GPIO5		6
#define GPIO6		7
#define GPIO7		8
#define GPIO8		9
#define GPIO9		10
#define GPIO10		11
#define GPIO11		12
#define GPIO12		13
#define GPIO13		14
#define GPIO14		15
#define GPIO15		16
#define GPIO16		17
#define GPIO17		18
#define GPIO18		19
#define GPIO19		20
#define GPIO20		21
#define GPIO21		22
#define GPIO22		23
#define GPIO23		24
#define GPIO24		25
#define GPIO25		26
#define GPIO26		27
#define GPIO27		28
#define GPIO28		29
#define GPIO29		30
#define GPIO30		31
#define GPIO31		32	

#define BIT0		(0x00000001)
#define BIT1		(0x00000002)
#define BIT2		(0x00000004)
#define BIT3		(0x00000008)
#define BIT4		(0x00000010)
#define BIT5		(0x00000020)
#define BIT6		(0x00000040)
#define BIT7		(0x00000080)
#define BIT8		(0x00000100)
#define BIT9		(0x00000200)
#define BIT10		(0x00000400)
#define BIT11		(0x00000800)
#define BIT12		(0x00001000)
#define BIT13		(0x00002000)
#define BIT14		(0x00004000)
#define BIT15		(0x00008000)
#define BIT16		(0x00010000)
#define BIT17		(0x00020000)
#define BIT18		(0x00040000)
#define BIT19		(0x00080000)
#define BIT20		(0x00100000)
#define BIT21		(0x00200000)
#define BIT22		(0x00400000)
#define BIT23		(0x00800000)
#define BIT24		(0x01000000)
#define BIT25		(0x02000000)
#define BIT26		(0x04000000)
#define BIT27		(0x08000000)
#define BIT28		(0x10000000)
#define BIT29		(0x20000000)
#define BIT30		(0x40000000)
#define BIT31		(0x80000000)

#endif //POWER_FAIL_H
