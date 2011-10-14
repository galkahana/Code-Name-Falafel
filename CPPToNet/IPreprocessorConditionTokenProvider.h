#pragma once

#include <string>
#include <utility>
using namespace std;

typedef pair<bool,string> BoolAndString;


class IPreprocessorConditionTokenProvider
{
public:
	virtual ~IPreprocessorConditionTokenProvider(){}

	virtual BoolAndString GetNextToken() = 0;
	virtual BoolAndString GetNextTokenNoMacroReplacement() = 0;

};