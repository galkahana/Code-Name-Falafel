#pragma once

#include <string>
#include <utility>

using namespace std;

typedef pair<bool,string> BoolAndString;

class ITokenProvider
{
public:
	virtual ~ITokenProvider(){}

	virtual BoolAndString GetNextToken() = 0;
	virtual void PutBackToken(const string& inToken) = 0;
};