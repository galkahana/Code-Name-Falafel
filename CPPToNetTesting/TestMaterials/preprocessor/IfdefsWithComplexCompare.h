#pragma once

#define SYMBOL1 1
#define SYMBOL2 1

#if 2 + 5 * 3 == 17
static const char* scTest1 = "testSymbol1";
#endif

#if 2 + 5 * 3 + 1 != 18
static const char* scTest2 = "Should not be defined";
#endif

class MyClass
{
public:
	MyClass()
	{
#if 1 << 1 + 2 * 3 != 128
		mA = 2;
#elif 1 + -2 == -1
		mA = SYMBOL1;
#endif
#if (defined(SYMBOL1) ? SYMBOL1 : 0)  == 0
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
#if (2 * 3 == 6) || (2 * 3 == 5)
	printf("testing");
#else
	printf("should not appear");
#endif
	return inA + inB;
}