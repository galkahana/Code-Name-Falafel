#pragma once

#include "CPPValue.h"

#include <utility>

using namespace std;

typedef pair<bool,CPPValue> BoolAndCPPValue;

class CPPExpression
{
public:
	virtual ~CPPExpression(void){}

	virtual BoolAndCPPValue Evaluate() = 0;
};
