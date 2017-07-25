#include <iostream>
#include <string>
using namespace std;
class CBase {
    string name;
public:
    CBase(string s) : name(s) {
        cout << "BASE: " << name << endl;
    }
    ~CBase() {
        cout << "~BASE" << endl;
    }
};
class CDerive : public CBase {
    int age;
public:
    CDerive(string s, int a) : CBase(s), age(a) {
        cout << "DERIVE: " << age << endl;
    }
    ~CDerive() {
        cout << "~DERIVE" << endl;
    }
};
class CSon : public CDerive {
    string id;
public:
    CSon(string s1, int a, string s2) : CDerive(s1, a), id(s2) {
        cout << "SON: " << id << endl;
    }
    ~CSon() {
        cout << "~SON" << endl;
    }
};
int main ( )
{
    CSon s("小雅", 27, "8503026");
    return 0;
}
