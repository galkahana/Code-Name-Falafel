#pragma once

#include "CPPPrimitiveValue.h"

#include <utility>

using namespace std;

typedef pair<bool,CPPPrimitiveValue> BoolAndCPPPrimitiveValue;

class CPPExpression
{
public:
	virtual ~CPPExpression(void){}

	virtual BoolAndCPPPrimitiveValue Evaluate() = 0;
};
