#pragma once

#include "CPPElement.h"

struct TypedParameter;
class UsedTypeDescriptor;

class CPPTemplateValue : public CPPElement
{
public:
	CPPTemplateValue(TypedParameter* inTypeOfValue,unsigned long inParameterIndex);
	virtual ~CPPTemplateValue(void);

	 UsedTypeDescriptor* GetTypeOfValue();

	unsigned long GetParameterIndex();

	// checks for equivalency. which is the position in the parent parameters list, and the equality of the pointed type
	bool IsEqual(CPPTemplateValue* inOther);

private:
	TypedParameter* mTypeOfValue;
	unsigned long mParameterIndex;
};

