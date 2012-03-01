#include "CPPTemplateTypename.h"
#include "TypedParameter.h"

CPPTemplateTypename::CPPTemplateTypename(const string& inTypename,unsigned long inParameterIndex):CPPElement(inTypename,CPPElement::eCPPElemeentTemplateTypename)
{
	mDefaultType = NULL;
	mParameterIndex = inParameterIndex;
}


CPPTemplateTypename::~CPPTemplateTypename(void)
{
	delete mDefaultType;
	StringToCPPTemplateTypenameMap::iterator it = mTypenames.begin();
	for(; it != mTypenames.end(); ++it)
		delete it->second;
}

void CPPTemplateTypename::SetDefaultTypename(TypedParameter* inDefaultType)
{
	mDefaultType = inDefaultType;
}


UsedTypeDescriptor* CPPTemplateTypename::GetDefault()
{
	return mDefaultType ? mDefaultType->Type : NULL;
}

unsigned long CPPTemplateTypename::GetParameterIndex()
{
	return mParameterIndex;
}

bool CPPTemplateTypename::IsEqual(CPPTemplateTypename* inOther)
{
	return mParameterIndex == inOther->GetParameterIndex();
}

CPPElementList CPPTemplateTypename::FindElements(const string& inElementName)
{
	// if not found...just add a new one :)
	StringToCPPTemplateTypenameMap::iterator it = mTypenames.find(inElementName);

	if(it == mTypenames.end())
		it = mTypenames.insert(StringToCPPTemplateTypenameMap::value_type(inElementName,new CPPTemplateTypename(inElementName,0))).first;
	CPPElementList result;
	result.push_back(it->second);
	return result;
}
