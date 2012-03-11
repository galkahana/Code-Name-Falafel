#include "CPPTemplateValue.h"
#include "TypedParameter.h"
#include "UsedTypeDescriptor.h"


CPPTemplateValue::CPPTemplateValue(TypedParameter* inTypeOfValue,unsigned long inParameterIndex):CPPElement(inTypeOfValue->Name,CPPElement::eCPPElementTemplateValue)
{
	mTypeOfValue = inTypeOfValue;
	mParameterIndex = inParameterIndex;
}


CPPTemplateValue::~CPPTemplateValue(void)
{
	delete mTypeOfValue;
}

UsedTypeDescriptor* CPPTemplateValue::GetTypeOfValue()
{
	return mTypeOfValue->Type;
}

unsigned long CPPTemplateValue::GetParameterIndex()
{
	return mParameterIndex;
}

bool CPPTemplateValue::IsEqual(CPPTemplateValue* inOther)
{
	if(mParameterIndex != inOther->GetParameterIndex())
		return false;

	return mTypeOfValue->Type->IsEqual(inOther->GetTypeOfValue());
}

bool CPPTemplateValue::IsLess(CPPTemplateValue* inOther)
{
	if(mParameterIndex != inOther->GetParameterIndex())
		return mParameterIndex < inOther->GetParameterIndex();

	return mTypeOfValue->Type->IsLess(inOther->GetTypeOfValue());
}
