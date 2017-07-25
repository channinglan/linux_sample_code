#include <iostream>

using namespace std;

class B;
class A;
void Show( A& , B& );

class B
{
	private:
		int tt;
		friend class A;
		friend void Show( A& , B& );

	public:
		B( int temp = 100):tt ( temp ){}
};

class A
{
	private:
		int value;
		friend void Show( A& , B& );

	public:
		A(int temp = 200 ):value ( temp ){}

		void Show( B &b )
		{
			cout << value << endl;
			cout << b.tt << endl; 
		}
};

void Show( A& a, B& b )
{
	cout << a.value << endl;
	cout << b .tt << endl;
}

int main()
{
	A a;
	B b;
	a.Show( b );
	Show( a, b );

	return 0;
}

