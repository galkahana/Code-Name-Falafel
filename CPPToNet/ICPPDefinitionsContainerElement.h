#pragma once

#include "EStatusCode.h"
#include "ICPPVariablesContainerElement.h"
#include <string>

class CPPElement;
class CPPEnumerator;
class CPPEnumeratorValue;
class CPPUnion;

using namespace std;

class ICPPDefinitionsContainerElement : public ICPPVariablesContainerElement
{
public:
	virtual ~ICPPDefinitionsContainerElement(){};

	virtual Hummus::EStatusCode DefineAlias(const string& inAlias,CPPElement* inNewElement) = 0;
	
	// enumrators
	virtual CPPEnumerator* CreateEnumerator(const string& inEnumeratorName) = 0;
	virtual CPPEnumeratorValue* CreateEnumeratorValue(CPPEnumerator* inEnumerator,const string& inEnumeratorValueName) = 0;

	// unions
	virtual CPPUnion* CreateUnion(const string& inUnionName) = 0;

};