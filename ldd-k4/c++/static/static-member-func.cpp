#include <iostream>
#include <cstdlib>
using namespace std;
//静态成员函数一般在公共程序中用的比较多。这样的程序与类自身关系不大，只是归类性质放在某个类中
class Integer {
public:
    static int atoi(const char *s) {
        return ::atoi(s);
    }
    static float atof(const char *s) {
        return ::atof(s);
    }
};
int main ( )
{
    int x = Integer::atoi("322");
    float y = Integer::atof("3.14");
    cout << "x = " << x << "  y = " << y << endl;
    return 0;
}
