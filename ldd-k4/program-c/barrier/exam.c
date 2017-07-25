#define barrier() __asm__ __volatile__("": : :"memory")

int A, B;

void foo()
{
	A = B + 1;
//	barrier();
	B = 0;
}

