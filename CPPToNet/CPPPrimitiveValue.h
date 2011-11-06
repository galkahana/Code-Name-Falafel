#pragma once

#include "CPPPrimitiveTypes.h"

struct CPPPrimitiveValue
{
	CPPPrimitiveValue(){mType = eCPPInt; mIntValue = 0;}

	ECPPPrimitiveType mType;
	union 
	{
		bool mBoolValue;
		char mCharValue;
		unsigned char mUCharValue;
		int mIntValue;
		unsigned mUIntValue;
		wchar_t mWCharValue;
		long mLongValue;
		unsigned long mULongValue;
		long long mLongLongValue;
		unsigned long long mULongLongValue;
	};

};
