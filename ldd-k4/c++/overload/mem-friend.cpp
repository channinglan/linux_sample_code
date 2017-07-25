class A
{
public:
 A(int d):data(d){}
 A operator+(A&);//成员函数
 A operator-(A&);
 A operator*(A&);
 A operator/(A&);
 A operator%(A&);
 friend A operator+(A&,A&);//友元函数
 friend A operator-(A&,A&);
 friend A operator*(A&,A&);
 friend A operator/(A&,A&);
 friend A operator%(A&,A&);
private:
 int data;
};
//成员函数的形式
A A::operator+(A &a)
{
 return A(data+a.data);
}
A A::operator-(A &a)
{
 return A(data-a.data);
}
A A::operator*(A &a)
{
 return A(data*a.data);
}
A A::operator/(A &a)
{
 return A(data/a.data);
}
A A::operator%(A &a)
{
 return A(data%a.data);
}
//友元函数的形式
A operator+(A &a1,A &a2)
{
 return A(a1.data+a2.data);
}
A operator-(A &a1,A &a2)
{
 return A(a1.data-a2.data);
}
A operator*(A &a1,A &a2)
{
 return A(a1.data*a2.data);
}
A operator/(A &a1,A &a2)
{
 return A(a1.data/a2.data);
}
A operator%(A &a1,A &a2)
{
 return A(a1.data%a2.data);
}
//然后我们就可以对类的对象进行+、-、*、/了。
int main(void)
{
 A a1(1),a2(2),a3(3);
// a1=a2+a3;
 //或者
 a1=a2.operator+(a3);
 return 0;
}
