#include<iostream>
using namespace std;
//基类
class People{
protected:
  char *name;
  int age;
public:
  People(char*, int);
};
People::People(char *name, int age): name(name), age(age){}
//派生类
class Student: public People{
private:
  float score;
public:
  Student(char*, int, float);
  void display();
};
//调用了基类的构造函数
// Student::Student(char *name, int age, float score): People(name, age), score(score){}
Student::Student(char *name, int age, float score): People(name, age){
  this->score = score;
}
void Student::display(){
  cout<<name<<"的年龄是"<<age<<"，成绩是"<<score<<endl;
}
int main(){
  Student stu("小明", 16, 90.5);
  stu.display();
  return 0;
}
