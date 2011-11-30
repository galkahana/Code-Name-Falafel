#include "CPPEnumerator.h"
#include "Trace.h"

CPPEnumeratorValue::CPPEnumeratorValue(const string& inEnumeratorValueName): CPPElement(inEnumeratorValueName,CPPElement::eCPPElementEnumeratorValue)
{

}


CPPEnumerator::CPPEnumerator(const string& inEnumeratorName): CPPElement(inEnumeratorName,CPPElement::eCPPElementEnumerator)
{
}

CPPEnumerator::~CPPEnumerator(void)
{
	CPPEnumeratorValueList::iterator it = mEnumeratorList.begin();

	for(; it != mEnumeratorList.end(); ++it)
		delete (*it);
}


CPPEnumeratorValue* CPPEnumerator::AddEnumeratorValue(const string& inEnumeratorValueName)
{
	if(mNamedEnumeratorValues.find(inEnumeratorValueName) != mNamedEnumeratorValues.end())
	{
		TRACE_LOG2("CPPEnumerator::AddEnumeratorValue, Error, enumerator value %s already exists in enumerator %s",inEnumeratorValueName.c_str(),Name.c_str());
		return NULL;
	}

	CPPEnumeratorValue* newElement = new CPPEnumeratorValue(inEnumeratorValueName);

	mEnumeratorList.push_back(newElement);
	mNamedEnumeratorValues.insert(StringToCPPEnumeratorValueMap::value_type(inEnumeratorValueName,newElement));

	return newElement;
}

CPPEnumeratorValue* CPPEnumerator::GetEnumeratorValue(const string& inEnumeratorValueName)
{
	StringToCPPEnumeratorValueMap::iterator it = mNamedEnumeratorValues.find(inEnumeratorValueName);

	return (it == mNamedEnumeratorValues.end()) ? NULL:it->second;
}

Hummus::SingleValueContainerIterator<CPPEnumeratorValueList> CPPEnumerator::GetEnumeratorValuesIterator()
{
	return Hummus::SingleValueContainerIterator<CPPEnumeratorValueList>(mEnumeratorList);
}