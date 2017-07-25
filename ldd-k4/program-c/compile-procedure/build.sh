arm-linux-gnueabihf-gcc -E hello.c -o hello.i
arm-linux-gnueabihf-gcc -S hello.i -o hello.s
arm-linux-gnueabihf-gcc -c hello.c -o hello.o
arm-linux-gnueabihf-gcc -o hello hello.o
