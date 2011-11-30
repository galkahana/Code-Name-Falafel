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
	CPPEnumerator(const string& inEnumeratorName);
	~CPPEnumerator(void);

	CPPEnumeratorValue* AddEnumeratorValue(const string& inEnumeratorValueName);

	CPPEnumeratorValue* GetEnumeratorValue(const string& inEnumeratorValueName);
	Hummus::SingleValueContainerIterator<CPPEnumeratorValueList> GetEnumeratorValuesIterator();

private:

	StringToCPPEnumeratorValueMap mNamedEnumeratorValues;
	CPPEnumeratorValueList mEnumeratorList;

};
