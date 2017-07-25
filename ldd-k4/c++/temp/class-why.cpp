#include <iostream>

using namespace std;
template <class numtype>
class Compare
{
public :
   Compare(numtype a,numtype b)
   {
     x=a;
     y=b;
   }
   numtype max()
   {
     return (x>y)?x:y;
   }
   numtype min()
   {
     return (x<y)?x:y;
   }
private :
   numtype x,y;
};

#if 0
template <class numtype>
numtype Compare<numtype>::max()
{
   return (x>y)?x:y;
}
#endif
int main() 
{
	Compare<double> cmp(2.3, 5.8);
	cout << cmp.min() << endl;
}
