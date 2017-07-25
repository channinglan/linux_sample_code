#include <stdlib.h>
#include <assert.h>

void *memcpy(void *restrict dest, const void *restrict src, size_t n)
{
        int i;
        const char *s = src;
        char *d = dest;

	assert(dest && src);
	assert((src + n <= dest) || (dest + n <= src));

        for (i = 0; i < n; i++)
                d[i] = s[i];
        return dest;
}

int main(int argc, char **argv)
{
	char p[100];
	memcpy(p, p + 1, 2);
	memcpy(p + 1, p, 2);
	memcpy(p + 2, p, 2);
	return 0;
}

