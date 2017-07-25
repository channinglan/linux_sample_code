#include <iostream>
using namespace std;

void test(int &a)
{
	cout << &a << " " << a << endl;
}

int main(void)
{
	int a=1;
	cout << &a << " " << a << endl;
	test(a);
	return 0;
}
