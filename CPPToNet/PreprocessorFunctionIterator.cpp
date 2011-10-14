#include "PreprocessorFunctionIterator.h"
#include "Trace.h"

#include <sstream>

using namespace Hummus;

PreprocessorFunctionIterator::PreprocessorFunctionIterator(void)
{
}

PreprocessorFunctionIterator::~PreprocessorFunctionIterator(void)
{
}


StringList& PreprocessorFunctionIterator::CreateNewParameter(const string& inParameterName)
{
	StringToStringListMap::iterator it = mParameters.insert(StringToStringListMap::value_type(inParameterName,StringList())).first;
	return it->second;
}

static const string scQuote = "'";
static const string scDoubleQuote = "\"";
void PreprocessorFunctionIterator::ComposeExpression(const StringList& inOriginalTokens)
{
	// here just create the new token string
	StringList::const_iterator it = inOriginalTokens.begin();
	bool stringizeNext = false;
	bool charizeNext = false;
	bool concatNext = false;

	for(; it != inOriginalTokens.end(); ++it)
	{
		if(*it == "#")
		{
			stringizeNext = true;
			continue;
		}
		else if(*it == "#@")
		{
			charizeNext = true;
			continue;
		}
		else if(*it == "##")
		{
			concatNext = true;
			continue;
		}

		StringToStringListMap::iterator itParameter = mParameters.find(*it);
		if(itParameter == mParameters.end())
		{
			if(stringizeNext)
			{
				mTokens.push_back(scDoubleQuote + *it + scDoubleQuote);
				stringizeNext = false;
			} else if(charizeNext)
			{
				mTokens.push_back(scQuote + *it + scQuote);
				charizeNext = false;
			}
			else if(concatNext)
			{
				// take already placed token and concat with next one
				if(mTokens.size() > 0)
					mTokens.back() = mTokens.back() + *it;
				else // humff, error
					mTokens.push_back(*it);
				concatNext = false;
			}
			else
				mTokens.push_back(*it);
		}
		else
		{
			if(stringizeNext)
			{
				mTokens.push_back(ConcatToSingleString(itParameter->second,scDoubleQuote));
				stringizeNext = false;
			}
			else if(charizeNext)
			{
				mTokens.push_back(ConcatToSingleString(itParameter->second,scQuote));
				stringizeNext = false;
			}
			else if(concatNext)
			{
				StringList::iterator itParameterValue = itParameter->second.begin();
				if(itParameterValue != itParameter->second.end())
				{
					if(mTokens.size() > 0)
						mTokens.back() = mTokens.back() + *itParameterValue;
					else // humff, error
						mTokens.push_back(*itParameterValue);
					++itParameterValue;

					for(; itParameterValue != itParameter->second.end();++itParameterValue)
						mTokens.push_back(*itParameterValue);
				}
				concatNext = false;
			}
			else
			{
				StringList::iterator itParameterValue = itParameter->second.begin();
				for(; itParameterValue != itParameter->second.end();++itParameterValue)
					mTokens.push_back(*itParameterValue);
			}
		}
	}

	mItTokenStrings = mTokens.begin();
}

string PreprocessorFunctionIterator::ConcatToSingleString(const StringList& inStrings,const string& inSurroundingText)
{
	stringstream aStream;
	aStream<<inSurroundingText;
	StringList::const_iterator it = inStrings.begin();
	for(; it != inStrings.end();++it)
		aStream<<*it;					
	aStream<<inSurroundingText;
	return aStream.str();
}

bool PreprocessorFunctionIterator::IsFinished()
{
	return mItTokenStrings == mTokens.end();
}

BoolAndString PreprocessorFunctionIterator::GetNextToken()
{
	if(mItTokenStrings == mTokens.end())
		return BoolAndString(false,"");
	
	BoolAndString result(true,*mItTokenStrings);
	++mItTokenStrings;
	return result;
}

BoolAndString PreprocessorFunctionIterator::GetNextNoSpaceEntity()
{
	// works the same as GetNextToken because the tokens/nospaceentity were determined in the iterator creation
	return GetNextToken();
}

string PreprocessorFunctionIterator::GetStringTillEndOfLine()
{
	// sort of not supposed to get here for macros replacement, so have some default action	
	TRACE_LOG("PreprocessorFunctionIterator::GetStringTillEndOfLine, unexpected call");
	return "";
}