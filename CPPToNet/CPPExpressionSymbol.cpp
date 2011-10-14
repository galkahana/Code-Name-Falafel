#include "CPPExpressionSymbol.h"

CPPExpressionSymbol::CPPExpressionSymbol(const string& inSymbol)
{
	mSymbol = inSymbol;
}

CPPExpressionSymbol::~CPPExpressionSymbol(void)
{
}


BoolAndCPPValue CPPExpressionSymbol::Evaluate()
{
	return BoolAndCPPValue(false,CPPValue());
}