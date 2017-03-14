
CROSS_COMPILEPATH=/home/share/PXM/ti-processor-sdk-linux-am57xx-evm-03.00.00.04

CC = ${CROSS_COMPILEPATH}/linux-devkit/sysroots/x86_64-arago-linux/usr/bin/arm-linux-gnueabihf-gcc


STRIP = ${CROSS_COMPILEPATH}/linux-devkit/sysroots/x86_64-arago-linux/usr/bin/arm-linux-gnueabihf-strip
OBJ=gpio
all:
	$(CC) $(INCLUDE) $(OBJ).c -o $(OBJ)	
	$(STRIP) $(OBJ)

clean:
	rm -rf $(OBJ)
	rm -rf *.o 
