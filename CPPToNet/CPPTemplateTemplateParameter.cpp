#include "CPPTemplateTemplateParameter.h"
#include "CPPTemplateTypename.h"
#include "CPPTemplateValue.h"
#include "TypedParameter.h"

CPPTemplateTemplateParameter::CPPTemplateTemplateParameter(const string& inName,unsigned long inParameterIndex,CPPElementList& inTemplateParameters):CPPElement(inName,CPPElement::eCPPElemeentTemplateTemplateParameter)
{
	mDefaultType = NULL;
	mTemplateParameters = inTemplateParameters;
	mParameterIndex = inParameterIndex;
}


CPPTemplateTemplateParameter::~CPPTemplateTemplateParameter(void)
{
	delete mDefaultType;
	CPPElementList::iterator itParameters = mTemplateParameters.begin();
	for(; itParameters != mTemplateParameters.end(); ++itParameters)
		delete *itParameters;

	StringToCPPTemplateTypenameMap::iterator itTypenames = mTypenames.begin();
	for(; itTypenames != mTypenames.end(); ++itTypenames)
		delete itTypenames->second;
}

void CPPTemplateTemplateParameter::SetDefaultTypename(TypedParameter* inDefaultType)
{
	mDefaultType = inDefaultType;
}


UsedTypeDescriptor* CPPTemplateTemplateParameter::GetDefault()
{
	return mDefaultType ? mDefaultType->Type : NULL;
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

bool CPPTemplateTemplateParameter::IsLess(CPPTemplateTemplateParameter* inOther)
{
	if(mParameterIndex != inOther->GetParameterIndex())
		return mParameterIndex < inOther->GetParameterIndex();


	if(mTemplateParameters.size() != inOther->GetTemplateParameters().size())
		return mTemplateParameters.size() < inOther->GetTemplateParameters().size();

	CPPElementList::const_iterator itThis = mTemplateParameters.begin();
	CPPElementList::const_iterator itOther = inOther->GetTemplateParameters().begin();
	bool isEqual = true;
	bool isLess = false;

	for(; itThis != mTemplateParameters.end() && isEqual; ++itThis,++itOther)
	{
		if((*itThis)->Type != (*itOther)->Type)
		{
			isEqual = false;
			isLess = ((*itThis)->Type < (*itOther)->Type);
			break;
		}

		// can only be one of the three
		switch((*itThis)->Type)
		{
			case CPPElement::eCPPElemeentTemplateTemplateParameter:
				isEqual	= ((CPPTemplateTemplateParameter*)(*itThis))->IsEqual((CPPTemplateTemplateParameter*)(*itOther));
				if(!isEqual)
					isLess = ((CPPTemplateTemplateParameter*)(*itThis))->IsLess((CPPTemplateTemplateParameter*)(*itOther));
				break;
			case CPPElement::eCPPElemeentTemplateTypename:
				isEqual	= ((CPPTemplateTypename*)(*itThis))->IsEqual((CPPTemplateTypename*)(*itOther));
				if(!isEqual)
					isLess = ((CPPTemplateTypename*)(*itThis))->IsLess((CPPTemplateTypename*)(*itOther));
				break;
			case CPPElement::eCPPElementTemplateValue:
				isEqual	= ((CPPTemplateValue*)(*itThis))->IsEqual((CPPTemplateValue*)(*itOther));
				if(!isEqual)
					isLess = ((CPPTemplateValue*)(*itThis))->IsLess((CPPTemplateValue*)(*itOther));
				break;
		}
	}

	return isLess;
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