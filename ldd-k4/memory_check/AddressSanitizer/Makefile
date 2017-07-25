all: off-by-one-error use-after-free

use-after-free: use-after-free.c
	gcc-4.9 -fsanitize=address -O0 -fno-omit-frame-pointer -g -o $@ $^
off-by-one-error: off-by-one-error.c
	gcc-4.9 -fsanitize=address -O0 -fno-omit-frame-pointer -g -o $@ $^
