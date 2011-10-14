#pragma once

#define MY_CONST 5
#define MY_CONST_FUNC(X,Y) X + Y + MY_CONST
#define MY_CONST_FUNC1(X) L##X


class MyClass
{
public:
	MyClass() {mA = MY_CONST; mB = MY_CONST + 1;}


private:
	int mA;
	char* mB;

};

#define MY_CONST2 MY_CONST_FUNC1("WELL THEN")


int MyFunc(int inA,int inB)
{
	printf(MY_CONST2);
	return MY_CONST_FUNC(inA,inB);
}