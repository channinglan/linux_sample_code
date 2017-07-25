#include <iostream>
#include <assert.h>
#include <string.h>

using namespace std;

class Complex
{
    double re,im;
public:
    Complex(double r,double i):re(r),im(i)
    {

    }
    Complex()
    {
        re = 0;
        im = 0;
    }
    Complex operator!();
    Complex operator+(const Complex &obj);
    //重载输入输出运算符，只能用友元函数
    friend ostream &operator<<(ostream &os,const Complex &c);
    friend istream &operator>>(istream &is,Complex &c);
};

Complex Complex::operator +(const Complex &obj)
{
    Complex temp;
    temp.re = re + obj.re;
    temp.im = im + obj.im;
    return temp;
}

Complex Complex::operator !()
{
    Complex temp;
    temp.re = -re;
    temp.im = -im;
    return temp;
}

ostream & operator<<(ostream &os,const Complex &c)
{
    os << c.re;
    if(c.im > 0)
        os << "+" << c.im << "i" << endl;
    else
        os << c.im << "i" << endl;
    return os;
}

istream & operator>>(istream &is,Complex &c)
{
    is >> c.re >> c.im;
    return is;
}

int main(int argc,char *argv[])
{
    Complex obj1(1,2),obj2(3,4);
    Complex obj3 = obj1 + !obj2;
    cout << obj3;
    cin >> obj3;
    cout << obj3;
    return 0;
}
