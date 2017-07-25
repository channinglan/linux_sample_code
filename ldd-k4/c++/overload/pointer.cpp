#include <iostream>  
using namespace std;  
  
struct Date  
{  
    int y;  
    int m;  
    int d;  
  
};  
  
  
class DatePtr  
{  
    Date* ptr;  
public:  
    //如果创建DataPtr对象时使用参数，那么传进去的指针也就成为了DataPtr的成员ptr的值了  
    DatePtr(Date* ptr = NULL):ptr(ptr)  
     {  
       
     }  
  
  
    //重载了*号  
    Date& operator*()  
    {  
        return *ptr;  //取了ptr的内容后将其返回  
    }  
  
    //重载了箭头号  
    Date* operator->()  
    {  
        return ptr;  
    }  
  
    //重载了前加加号  
    DatePtr& operator++()  
    {  
        ptr++;  
        return *this;  
    }  
  
  
    //重载了中括号  
    Date& operator[](int index)  
    {  
        return *(ptr+index);  
    }  
};  
int main()  
{  
    //要多多见识一下这些赋值的方式，蛋疼的考试  
    //struct  
    Date d = {2012,9,27};  
    //DatePtr p(&d);  
    //class  
    DatePtr p = &d;  
    (*p).y = 2013;  //相当于p-> = 2013;，但又能这么写 p.operator*().y = 2013  
    p->m = 10;  //p.operator->()->m = 10;  
    cout << d.y << '-' << d.m << '-' << d.d << endl;   
    Date ds[3] = {  
        {2012,9,27},  
        {2012,9,30},  
        {2012,12,21}  
    };  
    //赋的其实是地址  
    DatePtr p2 = ds;  
    for (int i = 0; i<3; i++)  
    {  
    //  cout << p2->y << '-' << p2->m  
    //            << '-' << p2->d << endl;  
    //            ++p2;  
        cout << p2[i].y << '-' << p2[i].m  
                  << '-' << p2[i].d << endl;  
    }  
}  
