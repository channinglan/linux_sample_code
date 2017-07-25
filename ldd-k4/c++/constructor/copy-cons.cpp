#include <iostream>
#include <string>
using namespace std;
class stuff {
    string name;
    int age;
public:
    stuff(string n, int a):name(n),age(a)
    {
        cout << "构造函数  " << name << age << endl;
    }
    string getName() {
        return name;
    }
    int getAge() {
        return age;
    }
    stuff& operator =(stuff& x);    //赋值运算符重载
    stuff(stuff& x):name(x.name),age(20)    //拷贝构造函数重载
    {
        cout << "拷贝构造函数  " << name << age << endl;
    }
};
stuff& stuff::operator =(stuff& x)
{
    age = x.age;
    cout << "赋值运算符  " << name << age << endl;
    return *this;
}
int main ( )
{
    stuff st("st", 3);     //调用通常的构造函数
    stuff st1("st1", 2);   //调用通常的构造函数
    st1 = st;         //因为不产生新的实例，所以调用的是赋值运算符
    stuff st2 = st;   //因为产生新的实例，所以调用的是拷贝构造函数
    cout << st.getName() << st.getAge() << endl;
    cout << st1.getName() << st1.getAge() << endl;
    cout << st2.getName() << st2.getAge() << endl;
    return 0;
}
