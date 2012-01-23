#pragma once

#include "CPPElement.h"

class UsedTypeDescriptor;

class CPPVariable : public CPPElement
{
public:
	CPPVariable(const string& inVariableName, 
				UsedTypeDescriptor* inTypeDescriptor);
	~CPPVariable(void);

	UsedTypeDescriptor* GetTypeDescriptor();

private:
	UsedTypeDescriptor* mTypeDescriptor;
};
