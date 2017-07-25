#define MAX (1024*1024)
#define MIN (MAX/2048)

int main()
{
	volatile int a;
#ifdef MULTIPLE
	a = MAX * MIN;
#else
	a = MAX / MIN;
#endif
	(void)a;
	return 0;
}
