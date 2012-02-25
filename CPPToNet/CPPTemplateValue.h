#pragma once

#include "CPPElement.h"

struct TypedParameter;
class UsedTypeDescriptor;

class CPPTemplateValue : public CPPElement
{
public:
	CPPTemplateValue(TypedParameter* inTypeOfValue);
	virtual ~CPPTemplateValue(void);

	 UsedTypeDescriptor* GetTypeOfValue();

private:
	TypedParameter* mTypeOfValue;

};

