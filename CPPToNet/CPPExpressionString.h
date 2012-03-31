#pragma once
#include "CPPExpression.h"

#include <string>

using namespace std;

class CPPExpressionString : public CPPExpression
{
public:
	CPPExpressionString(void);
	~CPPExpressionString(void);

	CPPExpressionString(const wstring& inWideString);
	CPPExpressionString(const string& inString);

	virtual BoolAndCPPPrimitiveValue Evaluate(IExpressionEvaluationContext* inContext);
	virtual bool IsEqual(CPPExpression* inOther);
	virtual bool IsLess(CPPExpression* inOther);

private:

	bool mIsWideString;
	wstring mWStringValue;
	string mStringValue;
};

