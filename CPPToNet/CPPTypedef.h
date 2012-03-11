#pragma once

#include "CPPElement.h"

#include <list>
#include <string>

using namespace std;

class UsedTypeDescriptor;

class CPPTypedef : public CPPElement
{
public:
	CPPTypedef(const string& inTypedefName, 
				UsedTypeDescriptor* inTypeDescriptor);
	~CPPTypedef(void);

	bool IsEqual(CPPTypedef* inOther);
	bool IsLess(CPPTypedef* inOther);

	UsedTypeDescriptor* GetTypeDescriptor();

private:
	UsedTypeDescriptor* mTypeDescriptor;
};
