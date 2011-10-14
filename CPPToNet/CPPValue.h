#pragma once

#include "CPPTypes.h"

struct CPPValue
{
	CPPValue(){mType = eCPPInt; mIntValue = 0;}

	ECPPType mType;
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
