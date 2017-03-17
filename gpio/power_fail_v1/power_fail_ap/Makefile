CROSS_COMPILEPATH=../../../linux-devkit/sysroots/x86_64-arago-linux/usr/bin/arm-linux-gnueabihf-

CC = ${CROSS_COMPILEPATH}gcc


STRIP = ${CROSS_COMPILEPATH}strip
OBJ=power_fail_ap
all:
	$(CC) $(INCLUDE) $(OBJ).c -o $(OBJ)	
	$(STRIP) $(OBJ)

clean:
	rm -rf $(OBJ)
	rm -rf *.o *.bak
