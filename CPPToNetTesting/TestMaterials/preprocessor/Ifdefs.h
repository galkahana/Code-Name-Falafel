#pragma once

#define SYMBOL1 1
#define SYMBOL2 1

#ifdef SYMBOL1
static const char* scTest1 = "testSymbol1";
#endif

#ifdef SYMBOL_NOT_DEFINED
static const char* scTest2 = "Should not be defined";
#endif

class MyClass
{
public:
	MyClass()
	{
#ifndef SYMBOL1
		mA = 2;
#else
		mA = SYMBOL1;
#endif
#ifdef SYMBOL_NOT_DEFINED
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
#ifdef PREDEFINED_SYMBOL
	printf("testing");
#else
	printf("should not appear");
#endif
	return inA + inB;
}