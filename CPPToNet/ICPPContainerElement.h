#pragma once

#include "EStatusCode.h"
#include <string>

class CPPElement;

using namespace std;

class ICPPContainerElement
{
public:
	virtual ~ICPPContainerElement(){};

	virtual CPPElement* FindElement(const string& inElementName) = 0;
	virtual Hummus::EStatusCode DefineAlias(const string& inAlias,CPPElement* inNewElement) = 0;
};