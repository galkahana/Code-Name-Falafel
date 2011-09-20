#include "SimplePreprocessorValueIterator.h"
#include "Trace.h"

using namespace Hummus;

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

BoolAndString SimplePreprocessorValueIterator::GetNextNoSpaceEntity()
{
	// works the same as GetNextToken because the tokens/nospaceentity were determined in the iterator creation
	return GetNextToken();
}

string SimplePreprocessorValueIterator::GetStringTillEndOfLine()
{
	// sort of not supposed to get here for macros replacement, so have some default action	
	TRACE_LOG("SimplePreprocessorValueIterator::GetStringTillEndOfLine, unexpected call");
	return "";
}