#pragma once

#include "EStatusCode.h"
#include <string>

using namespace std;

class UsedTypeDescriptor;

class ICPPParametersContainer
{
public:
	virtual ~ICPPParametersContainer(){}

	virtual Hummus::EStatusCode CreateParameter(const string& inParameterName,
											    UsedTypeDescriptor* inParameterType) = 0;

};