#include "CPPTemplateTemplateParameter.h"
#include "CPPTemplateTypename.h"
#include "CPPTemplateValue.h"

CPPTemplateTemplateParameter::CPPTemplateTemplateParameter(const string& inName,unsigned long inParameterIndex,CPPElementList& inTemplateParameters):CPPElement(inName,CPPElement::eCPPElemeentTemplateTemplateParameter)
{
	mTemplateParameters = inTemplateParameters;
	mParameterIndex = inParameterIndex;
}


CPPTemplateTemplateParameter::~CPPTemplateTemplateParameter(void)
{
	CPPElementList::iterator itParameters = mTemplateParameters.begin();
	for(; itParameters != mTemplateParameters.end(); ++itParameters)
		delete *itParameters;

	StringToCPPTemplateTypenameMap::iterator itTypenames = mTypenames.begin();
	for(; itTypenames != mTypenames.end(); ++itTypenames)
		delete itTypenames->second;
}

Hummus::SingleValueContainerIterator<CPPElementList> CPPTemplateTemplateParameter::GetParametersIterator()
{
	return Hummus::SingleValueContainerIterator<CPPElementList>(mTemplateParameters);
}

CPPElementList& CPPTemplateTemplateParameter::GetTemplateParameters()
{
	return mTemplateParameters;
}

unsigned long CPPTemplateTemplateParameter::GetParameterIndex()
{
	return mParameterIndex;
}

bool CPPTemplateTemplateParameter::IsEqual(CPPTemplateTemplateParameter* inOther)
{
	if(mParameterIndex != inOther->GetParameterIndex())
		return false;


	if(mTemplateParameters.size() != inOther->GetTemplateParameters().size())
		return false;

	CPPElementList::const_iterator itThis = mTemplateParameters.begin();
	CPPElementList::const_iterator itOther = inOther->GetTemplateParameters().begin();
	bool isEqual = true;

	for(; itThis != mTemplateParameters.end() && isEqual; ++itThis,++itOther)
	{
		if((*itThis)->Type != (*itOther)->Type)
		{
			isEqual = false;
			break;
		}

		// can only be one of the three
		switch((*itThis)->Type)
		{
			case CPPElement::eCPPElemeentTemplateTemplateParameter:
				isEqual	= ((CPPTemplateTemplateParameter*)(*itThis))->IsEqual((CPPTemplateTemplateParameter*)(*itOther));
				break;
			case CPPElement::eCPPElemeentTemplateTypename:
				isEqual	= ((CPPTemplateTypename*)(*itThis))->IsEqual((CPPTemplateTypename*)(*itOther));
				break;
			case CPPElement::eCPPElementTemplateValue:
				isEqual	= ((CPPTemplateValue*)(*itThis))->IsEqual((CPPTemplateValue*)(*itOther));
				break;
		}
	}

	return isEqual;
}

CPPElementList CPPTemplateTemplateParameter::FindElements(const string& inElementName)
{
	// if not found...just add a new one :)
	StringToCPPTemplateTypenameMap::iterator it = mTypenames.find(inElementName);

	if(it == mTypenames.end())
		it = mTypenames.insert(StringToCPPTemplateTypenameMap::value_type(inElementName,new CPPTemplateTypename(inElementName,0))).first;
	CPPElementList result;
	result.push_back(it->second);
	return result;
}