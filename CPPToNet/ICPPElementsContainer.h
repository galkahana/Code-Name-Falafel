#pragma once

#include <string>
#include <list>

class CPPElement;

using namespace std;

typedef list<CPPElement*> CPPElementList;

class ICPPElementsContainer
{
public:
	virtual ~ICPPElementsContainer(){}

	virtual CPPElementList FindElements(const string& inElementName) = 0;

};