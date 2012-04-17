#pragma once

#ifndef NULL
#define NULL 0
#endif

/*
	Primitives
*/
bool aBool;
char aChar;
unsigned char aUChar;
short aShort;
unsigned short aUShort;
int anInt;
unsigned anUnsigned;
wchar_t aWChar_t;
long aLong;
unsigned long aULong;
long long aLongLong;
unsigned long long aULongLong;
void* aPVoid;
float aFloat;
double aDouble;
long double aLongDouble;


// Storage
static const int aStaticConstInt = 0;
static int aStaticInt;
const int* aConstIntPointer = NULL;
volatile float aVolatileFloat;
extern unsigned short anExternUShort;


// Pointers, arrays and special initialization
int anIntArray[2];
int anIntDoubleArray[2][5];
int* anIntPointerArray[7];
static int& anIntReference = anInt;
int** anIntPointerToPointer;
char* const aConstCharPointer = NULL;
char* volatile aVolatileCharPointer;
int anIntArrayWithInitialization[10] = { 9, 8, 4, 6, 5, 6, 3, 5, 6, 11 };
int anIntTwoDepthArray[2][4] = { {1, 2, 3, 4}, {5, 6, 7, 8} };
char aString[] = "aString is legit";
char anotherString[5] = "abcd";
char moarString[4] = { 'a', 'b', 'c', 'd' };
int anIntInitializeSpecial(2);
int aComplexInitializer = aStaticConstInt * 5 + 6;






