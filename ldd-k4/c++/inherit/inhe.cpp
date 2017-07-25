#include <iostream>
using namespace std;  

class A  
{  
public:  
    int m;  
    int n;  
public:  
    void function1()  
    {  
        cout<<"这是基类的普通函数function1\n";  
    }  
    void virtual function2()  
    {  
        cout<<"这是基类的虚函数function2\n";  
    }  
    static void function3()  
    {  
        cout<<"这是基类静态成员函数function3\n";  
    }  
};  
  
class B:public A  
{  
public:  
    void function1()  
    {  
        A::function1();  
    }  
    void  function2()  
    {  
        cout<<"这是派生类的函数function2\n";  
        A::function2();  
    }  
};  
  
  
int main()  
{  
    B b;  
    b.function1(); // (1)  
    b.function2(); // (2)  
    A::function3();// (3)  
  
    A *a;  
    a=new B;  
    a->function1(); // (4)   
    a->function2(); // (5)  
    a->function3(); // (6)
}
