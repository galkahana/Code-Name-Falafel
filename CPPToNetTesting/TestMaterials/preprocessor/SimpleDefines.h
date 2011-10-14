#pragma once

#define MY_CONST 5

#define MY_CONST1 "R"

class MyClass
{
public:
	MyClass() {mA = MY_CONST; mB = MY_CONST1;}


private:
	int mA;
	char* mB;

};

#define MY_CONST2 "WELL THEN"


int MyFunc(int inA,int inB)
{
	printf("%s %d", __FILE__, __LINE__);
	printf(MY_CONST2);
	return inA + inB;
}