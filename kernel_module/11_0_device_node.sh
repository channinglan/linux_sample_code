#!/bin/sh

#�blinux kernel 2.6.x���ѤFsysfs�A�g�ѳo�˪�file-system�i�H�i�Duser-space�t�Φ����Ǹ˸m�A��user-space���{���N�i�H�ʺA���b/dev���U���ͬ۹�����device node�C
#
#device node�G
#�b/sys���U���@�ǦW��"dev"���ɮסA�N�O�]�t�Ӹ˸m��major/minor number�A
#�Ҧ���block device���i�H�b/sys/block/*/dev�M/sys/block/*/*/dev���C
#�Ҧ���char device���i�H�b/sys/bus/*/devices/*/dev�M/sys/class/*/*/dev���C



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