#include <iostream>
#include <string>
using namespace std;
class CBase {
    string id;
    string name;
public:
    CBase(string s1, string s2) : name(s1), id("base-"+s2) { }
    void show() {
        cout << "name: " << name << endl;
        cout << "id: " << id << endl;
    }
};
class CDerive : public CBase {
    string id;
    int age;
public:
    CDerive(string s1, int a, string s2) : age(a), id("DERIVE-"+s2), CBase(s1, s2) { }
    void show() {
        cout << "id: " << id << endl;
        cout << "age: " << age << endl;
        //派生类的内部调用基类的方法
        CBase::show();
        //show();    //如果误写成这样，将不断地重复循环地调用自身(即死循环)。
    }
};
int main ( )
{
    CDerive d("d", 3, "8503026");    //派生类的实例
    d.show();    //运行的是CDerive类的show()函数
    cout << "-------------"<< endl;
    //类的外部调用基类的方法
    d.CBase::show();    //运行的是CBase类的show()函数
    cout << "-------------"<< endl;
    CBase b("d", "8503026");    //基类当作普通类直接产生的实例
    b.show();    //运行的是CBase类的show()函数
    return 0;
}
