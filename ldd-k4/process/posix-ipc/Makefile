all: posix-shm-w posix-shm-r posix-sem-w posix-sem-r

posix-shm-w: posix-shm-w.c
	gcc -g -o posix-shm-w posix-shm-w.c -lrt

posix-shm-r: posix-shm-r.c
	gcc -g -o posix-shm-r posix-shm-r.c -lrt

posix-sem-w: posix-sem-w.c
	gcc -g -o posix-sem-w posix-sem-w.c -lrt -pthread

posix-sem-r: posix-sem-r.c
	gcc -g -o posix-sem-r posix-sem-r.c -lrt -pthread

