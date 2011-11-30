#pragma once

#include <string>
#include <utility>
#include "EStatusCode.h"

using namespace std;

typedef pair<Hummus::EStatusCode,bool> EStatusCodeAndBool;

class IExpressionEvaluationContext
{
public:
	virtual ~IExpressionEvaluationContext(){};

	// return value is a pair, where the first is status denoting validity of the question, and the second is wheher the symbol is defined
	// only preprocessor stage is supposed to answer with a valid response
	virtual EStatusCodeAndBool IsPreprocessorSymbolDefined(const string& inSymbol) = 0;
};