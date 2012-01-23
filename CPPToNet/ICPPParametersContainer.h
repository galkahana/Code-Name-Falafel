#pragma once

#include <string>

using namespace std;

struct FunctionParameter;
class UsedTypeDescriptor;

class ICPPParametersContainer
{
public:
	virtual ~ICPPParametersContainer(){}

	virtual FunctionParameter* CreateParameter(const string& inParameterName,
										 UsedTypeDescriptor* inParameterType) = 0;

};