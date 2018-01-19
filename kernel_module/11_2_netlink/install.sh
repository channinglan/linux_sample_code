#!/bin/bash


CURDIR=`pwd`
echo "Your current directory is $CURDIR. This is where the sigusb_suite software will be installed..."
A=`whoami`

RULES_NAME=89-sigusb.rules

create_udev_rules() {
	echo 'KERNEL=="*", SUBSYSTEM=="usb", ENV{DEVTYPE}=="usb_device", ACTION=="add", ATTR{idVendor}=="2933",ATTR{idProduct}=="0755", MODE="666", TAG="XXX_DEV_ID", RUN+="'"/usr/local/bin/sig_renumerate.sh A"'"' >> ${RULES_NAME}
	echo 'KERNEL=="*", SUBSYSTEM=="usb", ENV{DEVTYPE}=="usb_device", ACTION=="remove", TAG=="XXX_DEV_ID", RUN+="'"/usr/local/bin/sig_renumerate.sh R"'"' >> ${RULES_NAME}

}

if [ $A != 'root' ]; then
   echo "Warn:$A have to be root to run this script"
#   exit 1;
fi

echo  "create_udev_rules"
create_udev_rules
cp configs/sigusb.conf /etc/
cp ${RULES_NAME} /etc/udev/rules.d/


cp $CURDIR/configs/sig_renumerate.sh /usr/local/bin
chmod 777 /usr/local/bin/sig_renumerate.sh

# Set the sigusb_ROOT variable to point to the current directory.
echo "" >> /etc/profile
echo "# USB Suite: Root directory" >> /etc/profile
echo "export SIGUSB_ROOT=$CURDIR" >> /etc/profile
echo "" >> /etc/profile

#sudo service udev restart
