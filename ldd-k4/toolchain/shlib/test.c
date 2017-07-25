#include "test.h"
 
int xyz = 4;
 
int foo() {
	xyz++;
	return xyz;
}
 
int foo2() {
	return xyz * 2;
}
