#include "CPPTemplateValue.h"
#include "TypedParameter.h"


CPPTemplateValue::CPPTemplateValue(TypedParameter* inTypeOfValue):CPPElement(inTypeOfValue->Name,CPPElement::eCPPElementTemplateValue)
{
	mTypeOfValue = inTypeOfValue;
}


CPPTemplateValue::~CPPTemplateValue(void)
{
	delete mTypeOfValue;
}

UsedTypeDescriptor* CPPTemplateValue::GetTypeOfValue()
{
	return mTypeOfValue->Type;
}