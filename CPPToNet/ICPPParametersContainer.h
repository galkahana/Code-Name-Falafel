#pragma once

#include <string>

using namespace std;

struct TypedParameter;
class UsedTypeDescriptor;

class ICPPParametersContainer
{
public:
	virtual ~ICPPParametersContainer(){}

	virtual TypedParameter* CreateParameter(const string& inParameterName,
										 UsedTypeDescriptor* inParameterType) = 0;

};