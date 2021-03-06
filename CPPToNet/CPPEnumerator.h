#pragma once

#include "CPPElement.h"
#include "SingleValueContainerIterator.h"

#include <map>
#include <list>

using namespace std;

class CPPEnumeratorValue : public CPPElement
{
public:
	CPPEnumeratorValue(const string& inEnumeratorValueName);

};

typedef map<string,CPPEnumeratorValue*> StringToCPPEnumeratorValueMap;
typedef list<CPPEnumeratorValue*> CPPEnumeratorValueList;

class CPPEnumerator : public CPPElement
{
public:
	CPPEnumerator(const string& inEnumeratorName,bool inIsDefinition);
	~CPPEnumerator(void);

	CPPEnumeratorValue* AddEnumeratorValue(const string& inEnumeratorValueName);

	CPPEnumeratorValue* GetEnumeratorValue(const string& inEnumeratorValueName);
	Hummus::SingleValueContainerIterator<CPPEnumeratorValueList> GetEnumeratorValuesIterator();

	bool IsEqual();
		
	
	bool IsDefinition();

	// update enumerator flag, when function defintion is found
	void SetIsDefinition();
private:

	StringToCPPEnumeratorValueMap mNamedEnumeratorValues;
	CPPEnumeratorValueList mEnumeratorList;
	bool mIsDefinition;

};
