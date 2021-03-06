#pragma once

#include "CPPElement.h"

#include <string>
#include <set>

class ITokenProvider;
struct TypedParameter;

using namespace std;

typedef set<CPPElement::ECPPElementType> ECPPElementTypeSet;

class ITypeParserHelper
{
public:

	virtual ~ITypeParserHelper(){}

	// Checks if the next tokens form typename decription. should rollback to state prior to parsing - if parsing
	virtual bool IsAboutToParseType(ITokenProvider* inTokenProvider) = 0;
	// Parse a type with next tokens. use inTypeDelimeter to realize that parsing should stop. expected to consume the inTypeDelimeter character
	virtual TypedParameter* ParseType(ITokenProvider* inTokenProvider,const string& inTypeDelimeter) = 0;
	// Parse a type for new. parse only what's compatible for new. stop (and don't consume) after finished parsing the type, with possible block being the initializing call
	virtual TypedParameter* ParseTypeForNew(ITokenProvider* inTokenProvider) = 0;
	// Parse the next element (probably variable, function). return as used parameter
	virtual CPPElement* ParseTypename(ITokenProvider* inTokenProvider) = 0;
	virtual CPPElement* ParseTypename(ITokenProvider* inTokenProvider,ECPPElementTypeSet inOfTypes) = 0;

};