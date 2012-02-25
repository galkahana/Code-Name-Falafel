#include "CPPTemplateTemplateParameter.h"


CPPTemplateTemplateParameter::CPPTemplateTemplateParameter(const string& inName,StringToCPPElementMap& inTemplateParameters):CPPElement(inName,CPPElement::eCPPElemeentTemplateTemplateParameter)
{
	mTemplateParameters = inTemplateParameters;
}


CPPTemplateTemplateParameter::~CPPTemplateTemplateParameter(void)
{
	StringToCPPElementMap::iterator it = mTemplateParameters.begin();
	for(; it != mTemplateParameters.end(); ++it)
		delete it->second;
}

Hummus::MapIterator<StringToCPPElementMap> CPPTemplateTemplateParameter::GetParametersIterator()
{
	return Hummus::MapIterator<StringToCPPElementMap>(mTemplateParameters);
}
