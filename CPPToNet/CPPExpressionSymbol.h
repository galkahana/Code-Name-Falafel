#pragma once
#include "CPPExpression.h"

#include <string>

using namespace std;

class CPPExpressionSymbol : public CPPExpression
{
public:
	CPPExpressionSymbol(const string& inSymbol);
	~CPPExpressionSymbol(void);

	// evaluate for symbol always fails, because symbol is only used in the context of defined, which uses it explicitly, otherwise it's just illegal.
	virtual BoolAndCPPValue Evaluate();

	string mSymbol;

};
