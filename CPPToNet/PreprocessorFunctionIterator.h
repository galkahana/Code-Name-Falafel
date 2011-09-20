#pragma once
#include "ITokenProvider.h"

#include <string>
#include <list>
#include <map>

using namespace std;

typedef list<string> StringList;
typedef map<string,StringList> StringToStringListMap;

class PreprocessorFunctionIterator : public ITokenProvider
{
public:
	PreprocessorFunctionIterator(void);
	~PreprocessorFunctionIterator(void);

	// preparation methods for the iterator
	StringList& CreateNewParameter(const string& inParameterName);
	void ComposeExpression(const StringList& inOriginalTokens);

	// this is the actual iterator
	virtual bool IsFinished();
	virtual BoolAndString GetNextToken();
	virtual BoolAndString GetNextNoSpaceEntity();
	virtual string GetStringTillEndOfLine(); 


private:

	StringToStringListMap mParameters;
	StringList mTokens;
	StringList::iterator mItTokenStrings;

	string ConcatToSingleString(const StringList& inStrings,const string& inSurroundingText);

};
