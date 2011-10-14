#pragma once

#include <string>
#include <utility>
using namespace std;

typedef pair<bool,string> BoolAndString;


class IPreprocessorTokenProvider
{
public:
	virtual ~IPreprocessorTokenProvider(){}

	virtual bool IsFinished() = 0;
	virtual BoolAndString GetNextToken() = 0;
	virtual BoolAndString GetNextNoSpaceEntity() = 0;
	virtual string GetStringTillEndOfLine() = 0;

};