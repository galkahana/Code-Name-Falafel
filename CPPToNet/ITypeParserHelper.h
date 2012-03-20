#pragma once

#include <string>

class ITokenProvider;
struct TypedParameter;

using namespace std;

class ITypeParserHelper
{
public:

	virtual ~ITypeParserHelper(){}

	// Checks if the next tokens form typename decription. should rollback to state prior to parsing - if parsing
	virtual bool IsAboutToParseType(ITokenProvider* inTokenProvider) = 0;
	// Parse a type with next tokens. use inTypeDelimeter to realize that parsing should stop. expected to consume the inTypeDelimeter character
	virtual TypedParameter* ParseType(ITokenProvider* inTokenProvider,const string& inTypeDelimeter) = 0;

};