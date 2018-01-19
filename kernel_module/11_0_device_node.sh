#!/bin/sh

#在linux kernel 2.6.x提供了sysfs，經由這樣的file-system可以告訴user-space系統有哪些裝置，而user-space的程式就可以動態的在/dev底下產生相對應的device node。
#
#device node：
#在/sys底下有一些名為"dev"的檔案，就是包含該裝置的major/minor number，
#所有的block device都可以在/sys/block/*/dev和/sys/block/*/*/dev找到。
#所有的char device都可以在/sys/bus/*/devices/*/dev和/sys/class/*/*/dev找到。



# Block Device
echo "list Block Device"
#ls /sys/block/*/dev
#ls /sys/block/*/*/dev
echo "------------------"
for i in /sys/block/*/dev /sys/block/*/*/dev
do
    if [ -f $i ]
    then
        MAJOR=$(sed 's/:.*//' < $i)
        MINOR=$(sed 's/.*://' < $i)
        DEVNAME=$(echo $i | sed -e 's@/dev@@' -e 's@.*/@@')
        echo "mknod /dev/$DEVNAME b $MAJOR $MINOR"
    fi
done

# Char Device
echo "list Char Device"
#ls /sys/bus/*/devices/*/dev
#ls /sys/class/*/*/dev
echo "------------------"
for i in /sys/bus/*/devices/*/dev /sys/class/*/*/dev
do
    if [ -f $i ]
    then
        MAJOR=$(sed 's/:.*//' < $i)
        MINOR=$(sed 's/.*://' < $i)
        DEVNAME=$(echo $i | sed -e 's@/dev@@' -e 's@.*/@@')
        echo "mknod /dev/$DEVNAME c $MAJOR $MINOR"
    fi
done