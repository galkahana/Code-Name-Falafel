#pragma once
#include "CPPExpression.h"
#include "CPPCharType.h"
#include "CPPBoolType.h"
#include "CPPUnsignedCharType.h"
#include "CPPIntType.h"
#include "CPPUnsignedIntType.h"
#include "CPPLongType.h"
#include "CPPUnsignedLongType.h"
#include "CPPLongLongType.h"
#include "CPPULongLongType.h"
#include "CPPWCharType.h"
#include "CPPOperator.h"

#include <list>

using namespace std;

class PreProcessor;
class IUnaryOperatorCommand;
class IBinaryOperatorCommand;
class ICPPPrimitiveType;

typedef list<CPPExpression*> CPPExpressionList;

class CPPOperatorExpression : public CPPExpression
{
public:
	CPPOperatorExpression(CPPOperator* inOperator);
	virtual ~CPPOperatorExpression(void);

	void PushOperand(CPPExpression* inExpression);

	virtual BoolAndCPPPrimitiveValue Evaluate(IExpressionEvaluationContext* inContext);
	virtual bool IsEqual(CPPExpression* inExpression);
	virtual bool IsLess(CPPExpression* inExpression);

private:
	CPPOperator* mOperator;
	CPPExpressionList mOperands;

	// primitive types
	CPPBoolType mBoolType;
	CPPCharType mCharType;
	CPPUnsignedCharType mUCharType;
	CPPIntType mIntType;
	CPPUnsignedIntType mUIntType;
	CPPWCharType mCPPWCharType;
	CPPLongType mLongType;
	CPPUnsignedLongType mULongType;
	CPPLongLongType mLongLongType;
	CPPULongLongType mULongLongType;

	IUnaryOperatorCommand* GetUnaryOperatorCommand(ECPPOperatorType inType);
	ICPPPrimitiveType* GetTypeClass(ECPPPrimitiveType inType);
	CPPPrimitiveValue EvaluateBinaryOperator(ECPPOperatorType inOperatorType,const CPPPrimitiveValue& inLeftOperand, const CPPPrimitiveValue& inRightOperand);
	IBinaryOperatorCommand* GetBinaryOperatorCommand(ECPPOperatorType inType);
	CPPPrimitiveValue EvaluateShiftOperator(ECPPOperatorType inOperatorType,const CPPPrimitiveValue& inLeftOperand, const CPPPrimitiveValue& inRightOperand);

};
