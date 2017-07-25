all:
	gcc -g -fPIC -c -o test.o test.c
	gcc -g -shared -Wl,-soname,libtest.so.1 -o libtest.so.1.0 test.o -lc
	ln -s libtest.so.1.0 libtest.so.1
	ln -s libtest.so.1 libtest.so
	gcc -g -I. -L. main.c -o main -ltest
tar:
	tar zcvf main.tar.gz main.c test.c test.h
clean:
	rm -f *.so* *.o main
.PHONY: all tar clean
