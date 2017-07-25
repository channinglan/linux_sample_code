all: hello arm-mmap

hello: hello.S
	arm-linux-gnueabi-as -o hello.o hello.S
	arm-linux-gnueabi-ld -o hello hello.o
arm-mmap: arm-mmap.S
	arm-linux-gnueabi-as -o arm-mmap.o arm-mmap.S
	arm-linux-gnueabi-ld -o arm-mmap arm-mmap.o

clean:
	rm *.o hello arm-mmap

