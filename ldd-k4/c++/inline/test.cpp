/*test.cpp*/  
#include <iostream>  
#include "test.h"  
using std::cout;  
using std::endl;  
#if 0  
void test::print()  
{  
    cout << x << endl;  
}  
#endif

void test::display(int y)  
{  
    cout << x * y << endl;  
}  
