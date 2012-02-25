#pragma once

#include "CPPElement.h"
#include "MapIterator.h"

#include <string>
#include <map>


using namespace std;


typedef map<string,CPPElement*> StringToCPPElementMap;

class CPPTemplateTemplateParameter : public CPPElement
{
public:
	CPPTemplateTemplateParameter(const string& inName,StringToCPPElementMap& inTemplateParameters);
	virtual ~CPPTemplateTemplateParameter(void);

	Hummus::MapIterator<StringToCPPElementMap> GetParametersIterator();

private:
	StringToCPPElementMap mTemplateParameters;
};

