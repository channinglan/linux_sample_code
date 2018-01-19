http://elinux.org/Device_drivers


Sample drivers

    LDT - Linux Driver Template - sample template of Linux device driver for learning and starting source for a custom driver. Implements UART char device driver for example. Uses following Linux facilities: module, platform driver, file operations (read/write, mmap, ioctl, blocking and nonblocking mode, polling), kfifo, completion, interrupt, tasklet, work, kthread, timer, misc device, proc fs, UART 0x3f8, HW loopback, SW loopback, ftracer. The code is in working condition and runs with test script.
    LDD3 - Samples for boot Linux Device Driver, 3rd edition, updated, compiled with kernel 3.2.0
        pci_skel.c - PCI skeleton
        sbull.c - simple block device
        scull.c - simple char device
        snull.c - simple network device
    Virtual Video driver, uses V4L2 - works
    mem2mem_testdev.c - virtual v4l2-mem2mem example device driver
    usb-skeleton.c - USB driver skeleton (can be compiled with trivial fix)
    skeletonfb.c - Frame Buffer device skeleton (can't be compiled)
    pcihp_skeleton.c - PCI Hot Plug Controller Skeleton Driver
    loopback.c - simple net_device implementing ifconfig lo
    gpio_driver - simple GPIO driver for Raspberry Pi model B+ ( not fully tested yet)