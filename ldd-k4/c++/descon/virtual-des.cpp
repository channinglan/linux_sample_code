#include<iostream>
using namespace std;
class ClxBase{
public:
    ClxBase() {};
    virtual ~ClxBase() {cout << "Output from the destructor of class ClxBase!" << endl;};
    virtual void DoSomething() { cout << "Do something in class ClxBase!" << endl; };
};

class ClxDerived : public ClxBase{
public:
    ClxDerived() {};
    ~ClxDerived() { cout << "Output from the destructor of class ClxDerived!" << endl; };
    void DoSomething() { cout << "Do something in class ClxDerived!" << endl; };
};

int main(){  
	//虚析构函数，它所存在的意义：基类的指针指向派生类对象，用基类的指针删除派生类对象
	ClxBase *p =  new ClxDerived;
	p->DoSomething();
	delete p;
	return 0;
}  
