#include <iostream>
#include <exception>
using namespace std;

struct MyException : public exception
{
  //后面的const  throw()不是函数,这个东西叫异常规格说明,表示what函数可以抛出异常的类型,类型说明放到()里,这里面没有类型,就是声明这个函数不抛出异常
  const char * what () const throw ()
  {
    return "C++ Exception";
  }
};
 
int main()
{
  try
  {
    throw MyException();
  }
  catch(MyException& e)
  {
    std::cout << "MyException caught" << std::endl;
    std::cout << e.what() << std::endl;
  }
  catch(std::exception& e)
  {
    //其他的错误
  }
}
