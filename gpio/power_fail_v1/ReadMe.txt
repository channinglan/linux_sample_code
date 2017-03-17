
Device Tree

Because  need to use the gpio2_23, you need to remove the original settings

//        USER1 {
//            label = "Up";
//            linux,code = <KEY_UP>;
//            gpios = <&gpio2 23 GPIO_ACTIVE_LOW>;
//        };



insmod power_fail.ko
[  194.569832] pxm_io_int_init_module
[  194.573551] pxm_io_int_config
[  194.576539] gpio_to_irq(55)
[  194.579418] Mapped int 94
[  194.582189] gpio pin is high
[  194.585080] IEI GPIO INFO [ver: 0.0.1] enable successfully!

root@am57xx-evm:/run/media/mmcblk0p1# ./power_fail_ap
[  198.772066] GPIO_SET_PID 0x634
[  198.775138] pid = 1588
1.  TEST SIG
99. Exit Program
Please enter the choice>> received value 1	<--- power_fail detect
1.  TEST SIG
99. Exit Program
Please enter the choice>> received value 2	<--- power_fail detect
1.  TEST SIG
99. Exit Program
Please enter the choice>> received value 3	<--- power_fail detect
1.  TEST SIG
99. Exit Program
Please enter the choice>>
