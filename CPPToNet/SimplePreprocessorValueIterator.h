#pragma once

#include "ITokenProvider.h"

#include <string>
#include <list>

using namespace std;

typedef list<string> StringList;

class SimplePreprocessorValueIterator : public ITokenProvider
{
public:
	SimplePreprocessorValueIterator(const StringList& inTokenStrings);
	virtual ~SimplePreprocessorValueIterator(void);

	virtual bool IsFinished();
	virtual BoolAndString GetNextToken();

private:

	const StringList& mTokenStrings;
	StringList::const_iterator mItTokenStrings;
};
