#pragma once

#define SYMBOL1 1
#define SYMBOL2 1

#if SYMBOL1 > 0
static const char* scTest1 = "testSymbol1";
#endif

#if SYMBOL1 < 0
static const char* scTest2 = "Should not be defined";
#endif

class MyClass
{
public:
	MyClass()
	{
#if 5 * 2 == 11
		mA = 2;
#elif 5 * 2 == 10
		mA = SYMBOL1;
#endif
#if false
	mB = scTest2;
#else
	mB = scTest1;
#endif
	}


private:
	int mA;
	char* mB;

};

int MyFunc(int inA,int inB)
{
#if true
	printf("testing");
#else
	printf("should not appear");
#endif
	return inA + inB;
}