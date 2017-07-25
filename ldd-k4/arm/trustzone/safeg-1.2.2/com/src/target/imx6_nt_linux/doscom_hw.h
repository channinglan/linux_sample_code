/*
 *  @(#) $Id: doscom_hw.h 724 2015-03-11 09:57:33Z ertl-honda $
 */
#ifndef DOSCOM_HW
#define DOSCOM_HW

#include <linux/ioctl.h>
#include <stdbool.h>
#include <stdint.h>

// 'j' 00-3F   linux/joystick.h
#define DOSCOM_IOCTL_CMD_INIT           _IO('j',  0x40)
#define DOSCOM_IOCTL_CMD_NOTIFY         _IOW('j', 0x41, int)
#define DOSCOM_IOCTL_CMD_WAIT           _IOW('j', 0x42, int)
#define DOSCOM_IOCTL_CMD_REGFIFOCH      _IOW('j', 0x44, int)
#define DOSCOM_IOCTL_CMD_GETFIFOCHMASK  _IOW('j', 0x45, int)
#define DOSCOM_IOCTL_CMD_INITFIFO       _IOW('j', 0x46, int)
#endif /* DOSCOM_HW */
