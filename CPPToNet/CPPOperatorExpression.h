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
class ICPPType;

typedef list<CPPExpression*> CPPExpressionList;

class CPPOperatorExpression : public CPPExpression
{
public:
	CPPOperatorExpression(CPPOperator* inOperator,PreProcessor* inSymbolsSource);
	virtual ~CPPOperatorExpression(void);

	void PushOperand(CPPExpression* inExpression);

	virtual BoolAndCPPValue Evaluate();

private:
	CPPOperator* mOperator;
	CPPExpressionList mOperands;
	PreProcessor* mSymbolsSource;


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

	BoolAndCPPValue EvaluateDefined();

	IUnaryOperatorCommand* GetUnaryOperatorCommand(ECPPOperatorType inType);
	ICPPType* GetTypeClass(ECPPType inType);
	CPPValue EvaluateBinaryOperator(ECPPOperatorType inOperatorType,const CPPValue& inLeftOperand, const CPPValue& inRightOperand);
	IBinaryOperatorCommand* GetBinaryOperatorCommand(ECPPOperatorType inType);
	CPPValue EvaluateShiftOperator(ECPPOperatorType inOperatorType,const CPPValue& inLeftOperand, const CPPValue& inRightOperand);

};
