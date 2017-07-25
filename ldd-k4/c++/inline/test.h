/*test.h*/  
#ifndef TEST_H  
#define TEST_H  
  
#include <iostream>  
using std::cout;  
using std::endl;  
  
class test  
{  
public:  
    test():x(10){}  
    inline void print();  
    void display (int y);  
private:  
    int x;  
};  
  
void test::print()  
{  
    cout << x << endl;  
}  
  
#endif  
