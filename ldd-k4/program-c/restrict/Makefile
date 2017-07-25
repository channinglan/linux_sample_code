all: c99 c89 gnu

c99: restrict-exam.c
	gcc -o c99 restrict-exam.c  -O3 -std=c99
	gcc -o memcpy memcpy.c  -std=c99

c89: restrict-exam.c
	gcc -o c89 restrict-exam.c  -std=c89 -pedantic -pedantic-errors

gnu: restrict-exam.c
	gcc -o c89 restrict-exam.c  -std=gnu89

clean:
	rm c99 c89 gnu
