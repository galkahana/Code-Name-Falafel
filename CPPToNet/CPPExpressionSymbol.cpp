#include "CPPExpressionSymbol.h"

CPPExpressionSymbol::CPPExpressionSymbol(const string& inSymbol)
{
	mSymbol = inSymbol;
}

CPPExpressionSymbol::~CPPExpressionSymbol(void)
{
}


BoolAndCPPPrimitiveValue CPPExpressionSymbol::Evaluate()
{
	return BoolAndCPPPrimitiveValue(false,CPPPrimitiveValue());
}