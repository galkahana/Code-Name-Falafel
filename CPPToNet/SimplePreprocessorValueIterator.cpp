#include "SimplePreprocessorValueIterator.h"

SimplePreprocessorValueIterator::SimplePreprocessorValueIterator(const StringList& inTokenStrings):mTokenStrings(inTokenStrings)
{
	mItTokenStrings = mTokenStrings.begin();
}

SimplePreprocessorValueIterator::~SimplePreprocessorValueIterator(void)
{
}


bool SimplePreprocessorValueIterator::IsFinished()
{
	return mItTokenStrings == mTokenStrings.end();
}

BoolAndString SimplePreprocessorValueIterator::GetNextToken()
{
	if(mItTokenStrings == mTokenStrings.end())
		return BoolAndString(false,"");
	
	BoolAndString result(true,*mItTokenStrings);
	++mItTokenStrings;
	return result;
}
