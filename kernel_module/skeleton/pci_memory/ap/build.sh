#!/bin/sh


echo " ----- check OS version ------"
FIND_OS=0

if [ $FIND_OS -eq 0 ]; then
	#echo "check /etc/fedora-release"
	test -e /etc/fedora-release && FEDORA=1 || FEDORA=0
	if [ $FEDORA -eq 1 ]; then
		echo "check /etc/fedora-release"
		OS=$(awk '{print $1}' /etc/fedora-release | tr '[:upper:]' '[:lower:]')
		VERSION=$(awk '{print $3}' /etc/fedora-release)
		ARCH=$(uname -m | sed 's/x86_//;s/i[3-6]86/32/')
		FIND_OS=1
	fi
fi

if [ $FIND_OS -eq 0 ]; then
	#echo "check /etc/redhat-release"
	test -e /etc/redhat-release && CENTOS2=1 || CENTOS2=0
	if [ $CENTOS2 -eq 1 ]; then
    		OS=$(awk '{print $1}' /etc/redhat-release | tr '[:upper:]' '[:lower:]')
    		VERSION=$(awk '{print $4}' /etc/redhat-release)
    		ARCH=$(uname -m | sed 's/x86_//;s/i[3-6]86/32/')
    		FIND_OS=1
	fi
fi

if [ $FIND_OS -eq 0 ]; then
	#echo "check /etc/centos-release"
	test -e /etc/centos-release && CENTOS=1 || CENTOS=0
	if [ $CENTOS -eq 1 ]; then
    		OS=$(awk '{print $1}' /etc/centos-release | tr '[:upper:]' '[:lower:]')
    		VERSION=$(awk '{print $3}' /etc/centos-release)
    		ARCH=$(uname -m | sed 's/x86_//;s/i[3-6]86/32/')
    		FIND_OS=1
	fi
fi

if [ $FIND_OS -eq 0 ]; then
	#echo "check /etc/lsb-release"
	test -e /etc/lsb-release && UBUNTU=1 || UBUNTU=0
	if [ "$UBUNTU" -eq "1" ]; then
		#for ubuntu use,/etc/lsb-release
		OS=$(awk '/DISTRIB_ID=/' /etc/*-release | sed 's/DISTRIB_ID=//' | tr '[:upper:]' '[:lower:]')
		ARCH=$(uname -m | sed 's/x86_//;s/i[3-6]86/32/')
		VERSION=$(awk '/DISTRIB_RELEASE=/' /etc/*-release | sed 's/DISTRIB_RELEASE=//' | sed 's/[.]0/./')

		if [ -z "$OS" ]; then
    			OS=$(awk '{print $1}' /etc/*-release | tr '[:upper:]' '[:lower:]')
		fi

		if [ -z "$VERSION" ]; then
    			VERSION=$(awk '{print $3}' /etc/*-release)
		fi
    		FIND_OS=1
	fi
fi

if [ $FIND_OS -eq 0 ]; then
	ARCH=$(uname -m)
	OS=$(uname -n)
	VERSION=$(uname -r)
fi


echo $OS-$VERSION-$ARCH




if [ "$ARCH" -eq "64" ]; then
	echo "x64"
	cp -rf libieihsrcaes64.so /usr/local/lib/libieihsrcaes.so
	cp -rf libieihsrcaes64.a libieihsrcaes.a		
else
	echo "x32"
	cp -rf libieihsrcaes32.so /usr/local/lib/libieihsrcaes.so
	cp -rf libieihsrcaes32.a libieihsrcaes.a		
fi

file /usr/local/lib/libieihsrcaes.so
file libieihsrcaes.a

make