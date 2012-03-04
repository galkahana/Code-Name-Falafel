#include "CPPStructTemplateInstance.h"
#include "CPPStruct.h"

CPPStructTemplateInstance::CPPStructTemplateInstance(const string& inName,CPPStruct* inTemplate,const UsedTypeOrExpressionList& inInstanceParameters):CPPElement(inName,CPPElement::eCPPElementStructTemplateInstance)
{
	mSourceTamplate = inTemplate;
	mInstanceParameters = inInstanceParameters;
}


CPPStructTemplateInstance::~CPPStructTemplateInstance(void)
{
	UsedTypeOrExpressionList::iterator it = mInstanceParameters.begin();
	for(; it != mInstanceParameters.end(); ++it)
		delete *it;
}

CPPElementList CPPStructTemplateInstance::FindElements(const string& inElementName)
{
	// here there's a bit of a foul. obviously, what should be done is that the right instance is created
	// and through the parameters the real types are intepreted. not gonna do this. for now.
	// and so, i'll use the template itself, in hope that for most usages of this project, it will suffice
	return mSourceTamplate->FindElements(inElementName);
}

CPPStruct* CPPStructTemplateInstance::GetSourceTemplate()
{
	return mSourceTamplate;
}

UsedTypeOrExpressionList& CPPStructTemplateInstance::GetInstanceParameters()
{
	return mInstanceParameters;
}
