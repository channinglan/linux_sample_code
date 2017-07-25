#include <iostream>
#include <string>
using namespace std;
class stuff {
    string name;
    int age;
public:
    stuff(string n, int a):name(n),age(a) { }
    string getName() {
        return name;
    }
    int getAge() {
        return age;
    }
    operator int() {     //stuff →　int
        return age;
    }
    operator string() {  //stuff →　string
        return name;
    }
};
int main ( )
{
    stuff st("C语言中文网", 3);
    string m_name = st; //stuff →　string
    int m_age = st;     //stuff →　int
    cout << m_name << endl;
    cout << m_age << endl;
    return 0;
}
