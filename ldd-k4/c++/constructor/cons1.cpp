//通过派生类创建对象时必须要调用基类的构造函数，这是语法规定。也就是说，定义派生类构造函数时最好指明基类构造函数；如果不指明，就调用基类的默认构造函数（不带参数的构造函数）；如果没有默认构造函数，那么编译失败。
#include<iostream>
using namespace std;
//基类
class People{
protected:
  char *name;
  int age;
public:
  People();
  People(char*, int);
};
People::People(){
  this->name = "xxx";
  this->age = 0;
}
People::People(char *name, int age): name(name), age(age){}
//派生类
class Student: public People{
private:
  float score;
public:
  Student();
  Student(char*, int, float);
  void display();
};
Student::Student(){
  this->score = 0.0;
}
Student::Student(char *name, int age, float score): People(name, age){
  this->score = score;
}
void Student::display(){
  cout<<name<<"的年龄是"<<age<<"，成绩是"<<score<<endl;
}
int main(){
  Student stu1;
  stu1.display();
  Student stu2("小明", 16, 90.5);
  stu2.display();
  return 0;
}
