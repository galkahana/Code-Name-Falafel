#include "CPPTemplateTypename.h"
#include "TypedParameter.h"

CPPTemplateTypename::CPPTemplateTypename(const string& inTypename):CPPElement(inTypename,CPPElement::eCPPElemeentTemplateTypename)
{
	mDefaultType = NULL;
}


CPPTemplateTypename::~CPPTemplateTypename(void)
{
	delete mDefaultType;
}

UsedTypeDescriptor* CPPTemplateTypename::GetDefault()
{
	return mDefaultType ? mDefaultType->Type : NULL;
}
