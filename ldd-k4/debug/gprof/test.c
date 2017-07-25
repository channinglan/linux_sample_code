#include "stdio.h"
#include "stdlib.h"

void a()
{
        volatile int i=20000000;
        while(i--);
}


void c()
{
        volatile int i=40000000;
        while(i--);
}

int b()
{
        volatile int i=50000000;
        while(i--);

        a();
        c();
        return 0;
}

int main()
{
        printf(" main() function()\n");
        b();
}
