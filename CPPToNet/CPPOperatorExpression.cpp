#include "CPPOperatorExpression.h"
#include "PreProcessor.h"
#include "CPPExpressionSymbol.h"
#include "Trace.h"

CPPOperatorExpression::CPPOperatorExpression(CPPOperator* inOperator,PreProcessor* inSymbolsSource)
{
	mOperator = inOperator;
	mSymbolsSource = inSymbolsSource;
}

CPPOperatorExpression::~CPPOperatorExpression(void)
{
	delete mOperator;
	CPPExpressionList::iterator it = mOperands.begin();
	for(; it != mOperands.end(); ++it)
		delete *it;
	mOperands.clear();
}

void CPPOperatorExpression::PushOperand(CPPExpression* inExpression)
{
	mOperands.push_back(inExpression);
}

// operators command classes
class IUnaryOperatorCommand
{
public:
	virtual CPPValue Evaluate(ICPPType* inType, const CPPValue& inOperandValue) = 0 ;
};


class ExecuteOnesComplement : public IUnaryOperatorCommand
{

public:

	virtual CPPValue Evaluate(ICPPType* inType,const CPPValue& inOperandValue)
	{
		return inType->OnesComplement(inOperandValue);
	}

};
static ExecuteOnesComplement sOnesComplement;


class ExecuteNot : public IUnaryOperatorCommand
{

public:

	virtual CPPValue Evaluate(ICPPType* inType,const CPPValue& inOperandValue)
	{
		return inType->Not(inOperandValue);
	}

};
static ExecuteNot sNot;

class ExecuteUnaryMinus : public IUnaryOperatorCommand
{

public:

	virtual CPPValue Evaluate(ICPPType* inType,const CPPValue& inOperandValue)
	{
		return inType->UnaryMinus(inOperandValue);
	}

};
static ExecuteUnaryMinus sUnaryMinus;

class ExecuteUnaryPlus : public IUnaryOperatorCommand
{

public:

	virtual CPPValue Evaluate(ICPPType* inType,const CPPValue& inOperandValue)
	{
		return inType->UnaryPlus(inOperandValue);
	}

};
static ExecuteUnaryPlus sUnaryPlus;

// operators selectors
IUnaryOperatorCommand* CPPOperatorExpression::GetUnaryOperatorCommand(ECPPOperatorType inType)
{
	IUnaryOperatorCommand* result;

	switch(inType)
	{
		case eCPPOperatorOnesComplement:
			result = &sOnesComplement;
			break;
		case eCPPOperatorNot:
			result = &sNot;
			break;
		case eCPPOperatorUnaryMinus:
			result = &sUnaryMinus;
			break;
		case eCPPOperatorUnaryPlus:
			result = &sUnaryPlus;
			break;
		default:
			result = NULL;
			break;
	}

	return result;
}

ICPPType* CPPOperatorExpression::GetTypeClass(ECPPType inType)
{
	ICPPType* result;

	switch(inType)
	{
		case eCPPBool:
			result = &mBoolType;
			break;
		case eCPPChar:
			result = &mCharType;
			break;
		case eCPPUnsignedChar:
			result = &mUCharType;
			break;
		case eCPPInt:
			result = &mIntType;
			break;
		case eCPPUnsigned:
			result = &mUIntType;
			break;
		case eCPPWChar:
			result = &mCPPWCharType;
			break;
		case eCPPLong:
			result = &mLongType;
			break;
		case eCPPUnsignedLong:
			result = &mULongType;
			break;
		case eCPPLongLong:
			result = &mLongLongType;
			break;
		case eCPPUnsignedLongLong:
			result = &mULongLongType;
			break;
		default:
			result = NULL; // but then it shouldn't really happen
			break;
	}

	return result;
}

BoolAndCPPValue CPPOperatorExpression::Evaluate()
{
	// implementations of operators. evaluation is per operator, 'mfraid.
	BoolAndCPPValue result(false,CPPValue());


	switch(mOperator->Type)
	{
		case eCPPOperatorDefined:
			result = EvaluateDefined();
			break;
	
		// unary operators
		case eCPPOperatorOnesComplement:
		case eCPPOperatorNot:
		case eCPPOperatorUnaryMinus:
		case eCPPOperatorUnaryPlus:
			{
				BoolAndCPPValue operandValue = mOperands.back()->Evaluate();
				if(!operandValue.first)
				{
					TRACE_LOG("CPPOperatorExpression::Evaluate, error in evaluating operand for unary operator");
					break;
				}

				result.second = GetUnaryOperatorCommand(mOperator->Type)->Evaluate(GetTypeClass(operandValue.second.mType),operandValue.second);
				result.first = true;

				break;
			}			
		// binary operators
		case eCPPOperatorMultiplication:
		case eCPPOperatorDivision:
		case eCPPOperatorModulus:
		case eCPPOperatorAddition:
		case eCPPOperatorSubtraction:

		// comparison
		case eCPPOperatorLessThan:
		case eCPPOperatorGreaterThan:
		case eCPPOperatorLessThanOrEqual:
		case eCPPOperatorGreaterThanOrEqual:
		case eCPPOperatorEquality:
		case eCPPOperatorInequality:

		// bitwise operators
		case eCPPOperatorBitwiseAnd:
		case eCPPOperatorBitwiseOr:
		case eCPPOperatorExclusiveOr:

			{
				BoolAndCPPValue leftOperand = mOperands.front()->Evaluate();
				BoolAndCPPValue rightOperand = mOperands.back()->Evaluate();

				if(!leftOperand.first || !rightOperand.first)
				{
					TRACE_LOG("CPPOperatorExpression::Evaluate, error in evaluating operands for binary operator");
					break;
				}

				result.second = EvaluateBinaryOperator(mOperator->Type,leftOperand.second,rightOperand.second);
				result.first = true;

				break;
			}

		case eCPPOperatorLeftByteShift:
		case eCPPOperatorRightByteShift:
			{
				// shift commands don't require same operand type on both sides, as normal arithmetics do.
				// instead of impelementing the whole slew of possible conversions i'll take note that
				// the value on the right is always positive. so i'll cast it always to unsigned long long...and implement
				// the shift always with this as type on the right.

				BoolAndCPPValue leftOperand = mOperands.front()->Evaluate();
				BoolAndCPPValue rightOperand = mOperands.back()->Evaluate();

				if(!leftOperand.first || !rightOperand.first)
				{
					TRACE_LOG("CPPOperatorExpression::Evaluate, error in evaluating operands for shift operator");
					break;
				}

				result.second = EvaluateShiftOperator(mOperator->Type,leftOperand.second,rightOperand.second);
				result.first = true;

				break;
			}

		case eCPPOperatorLogicalAnd:
			{
				BoolAndCPPValue leftOperand = mOperands.front()->Evaluate();
				if(!leftOperand.first)
				{
					TRACE_LOG("CPPOperatorExpression::Evaluate, error in evaluating left operand for logical and");
					break;
				}

				// evaluate first operand. if false, stop here and return false (which is the operand value)
				if(!GetTypeClass(leftOperand.second.mType)->ToBool(leftOperand.second).mBoolValue)
				{
					result = leftOperand;
					break;
				}

				// otherwise, the value is excatly the 2nd operand
				BoolAndCPPValue rightOperand = mOperands.back()->Evaluate();
				if(!rightOperand.first)
				{
					TRACE_LOG("CPPOperatorExpression::Evaluate, error in evaluating right operand for logical and");
					break;
				}	

				result = rightOperand;
				break;
			}

			case eCPPOperatorLogicalOr:
			{
				BoolAndCPPValue leftOperand = mOperands.front()->Evaluate();
				if(!leftOperand.first)
				{
					TRACE_LOG("CPPOperatorExpression::Evaluate, error in evaluating left operand for logical or");
					break;
				}

				// evaluate first operand. if true, stop here and return true (which is the operand value)
				if(GetTypeClass(leftOperand.second.mType)->ToBool(leftOperand.second).mBoolValue)
				{
					result = leftOperand;
					break;
				}

				// otherwise, the value is excatly the 2nd operand
				BoolAndCPPValue rightOperand = mOperands.back()->Evaluate();
				if(!rightOperand.first)
				{
					TRACE_LOG("CPPOperatorExpression::Evaluate, error in evaluating right operand for logical or");
					break;
				}	

				result = rightOperand;
				break;
			}

			case eCPPOperatorConditional:
			{
				CPPExpressionList::iterator it = mOperands.begin();

				// evaluate first. if true return 2nd oprand evaluation, otherwise reutrn 3rd operand evaluations
				BoolAndCPPValue conditionOperand = (*it)->Evaluate();
				if(!conditionOperand.first)
				{
					TRACE_LOG("CPPOperatorExpression::Evaluate, error in evaluating condition part of conditional operator");
					break;
				}

				++it;
				if(GetTypeClass(conditionOperand.second.mType)->ToBool(conditionOperand.second).mBoolValue)
				{
					result = (*it)->Evaluate();
				}
				else
				{
					++it;
					result = (*it)->Evaluate();
				}
				break;
			}
	}

	return result;
}

BoolAndCPPValue CPPOperatorExpression::EvaluateDefined()
{
	// function, accepting a single ExressionSymbol operand
	CPPValue value;
	value.mType = eCPPBool;
	value.mBoolValue = mSymbolsSource->IsSymbolDefined(((CPPExpressionSymbol*)mOperands.back())->mSymbol);

	return BoolAndCPPValue(true, value);
}

class IBinaryOperatorCommand
{
public:
	virtual CPPValue Evaluate(ICPPType* inType, const CPPValue& inLeftOperandValue,const CPPValue& inRightOperandValue) = 0;
};

CPPValue CPPOperatorExpression::EvaluateBinaryOperator(ECPPOperatorType inOperatorType,const CPPValue& inLeftOperand, const CPPValue& inRightOperand)
{
	// easy case, both types are the same.
	if(inLeftOperand.mType == inRightOperand.mType)
		return GetBinaryOperatorCommand(inOperatorType)->Evaluate(GetTypeClass(inLeftOperand.mType),inLeftOperand,inRightOperand);

	// not the same types. start by promoting. following CPP rules as discussed in MSDN, but at this point only with integers. note that there are no rules for long longs,
	// so i'm inferring from the rules about long

	// If either operand is of type unsigned long long, the other operand is converted to unsigned long long
	if(inLeftOperand.mType == eCPPUnsignedLongLong)
		return GetBinaryOperatorCommand(inOperatorType)->Evaluate(
															GetTypeClass(eCPPUnsignedLongLong),
															inLeftOperand,
															GetTypeClass(inRightOperand.mType)->ToULongLong(inRightOperand));

	if(inRightOperand.mType == eCPPUnsignedLongLong)
		return GetBinaryOperatorCommand(inOperatorType)->Evaluate(
															GetTypeClass(eCPPUnsignedLongLong),
															GetTypeClass(inLeftOperand.mType)->ToULongLong(inLeftOperand),
															inRightOperand);

	// If either operand is of type long long and the other of type unsigned long, both operands are converted to type unsigned long long
	if( (inLeftOperand.mType == eCPPLongLong && inRightOperand.mType == eCPPUnsignedLong) ||
		(inRightOperand.mType == eCPPLongLong && inLeftOperand.mType == eCPPUnsignedLong) )
		return GetBinaryOperatorCommand(inOperatorType)->Evaluate(
															GetTypeClass(eCPPUnsignedLongLong),
															GetTypeClass(inLeftOperand.mType)->ToULongLong(inLeftOperand),
															GetTypeClass(inRightOperand.mType)->ToULongLong(inRightOperand));

	// If either operand is of type unsigned long, the other operand is converted to type unsigned long.
	if(inLeftOperand.mType == eCPPUnsignedLong)
		return GetBinaryOperatorCommand(inOperatorType)->Evaluate(
															GetTypeClass(eCPPUnsignedLong),
															inLeftOperand,
															GetTypeClass(inRightOperand.mType)->ToULong(inRightOperand));

	if(inRightOperand.mType == eCPPUnsignedLong)
		return GetBinaryOperatorCommand(inOperatorType)->Evaluate(
															GetTypeClass(eCPPUnsignedLong),
															GetTypeClass(inLeftOperand.mType)->ToULong(inLeftOperand),
															inRightOperand);



	// If either operand is of type long and the other of type unsigned int, both operands are converted to type unsigned long
	if( (inLeftOperand.mType == eCPPLong && inRightOperand.mType == eCPPUnsigned) ||
		(inRightOperand.mType == eCPPLong && inLeftOperand.mType == eCPPUnsigned) )
		return GetBinaryOperatorCommand(inOperatorType)->Evaluate(
															GetTypeClass(eCPPUnsignedLong),
															GetTypeClass(inLeftOperand.mType)->ToULong(inLeftOperand),
															GetTypeClass(inRightOperand.mType)->ToULong(inRightOperand));



	// If either operand is of type long, the other operand is converted to type long
	if(inLeftOperand.mType == eCPPLong)
		return GetBinaryOperatorCommand(inOperatorType)->Evaluate(
															GetTypeClass(eCPPLong),
															inLeftOperand,
															GetTypeClass(inRightOperand.mType)->ToLong(inRightOperand));

	if(inRightOperand.mType == eCPPLong)
		return GetBinaryOperatorCommand(inOperatorType)->Evaluate(
															GetTypeClass(eCPPLong),
															GetTypeClass(inLeftOperand.mType)->ToLong(inLeftOperand),
															inRightOperand);



	// If either operand is of type unsigned int, the other operand is converted to type unsigned int
		if(inLeftOperand.mType == eCPPUnsigned)
		return GetBinaryOperatorCommand(inOperatorType)->Evaluate(
															GetTypeClass(eCPPUnsigned),
															inLeftOperand,
															GetTypeClass(inRightOperand.mType)->ToUnsignedInt(inRightOperand));

	if(inRightOperand.mType == eCPPUnsigned)
		return GetBinaryOperatorCommand(inOperatorType)->Evaluate(
															GetTypeClass(eCPPUnsigned),
															GetTypeClass(inLeftOperand.mType)->ToUnsignedInt(inLeftOperand),
															inRightOperand);

	// If none of the preceding conditions are met, both operands are converted to type int
	return GetBinaryOperatorCommand(inOperatorType)->Evaluate(
														GetTypeClass(eCPPInt),
														GetTypeClass(inLeftOperand.mType)->ToInt(inLeftOperand),
														GetTypeClass(inRightOperand.mType)->ToInt(inRightOperand));

}



class ExecuteMultiplication : public IBinaryOperatorCommand
{

public:

	virtual CPPValue Evaluate(ICPPType* inType, const CPPValue& inLeftOperandValue,const CPPValue& inRightOperandValue)
	{
		return inType->Multiply(inLeftOperandValue,inRightOperandValue);
	}

};

static ExecuteMultiplication sMultiplication;

class ExecuteDivision : public IBinaryOperatorCommand
{

public:

	virtual CPPValue Evaluate(ICPPType* inType, const CPPValue& inLeftOperandValue,const CPPValue& inRightOperandValue)
	{
		return inType->Divide(inLeftOperandValue,inRightOperandValue);
	}

};

static ExecuteDivision sDivision;


class ExecuteModulus : public IBinaryOperatorCommand
{

public:

	virtual CPPValue Evaluate(ICPPType* inType, const CPPValue& inLeftOperandValue,const CPPValue& inRightOperandValue)
	{
		return inType->Modulus(inLeftOperandValue,inRightOperandValue);
	}

};

static ExecuteModulus sModulus;

class ExecuteAddition : public IBinaryOperatorCommand
{

public:

	virtual CPPValue Evaluate(ICPPType* inType, const CPPValue& inLeftOperandValue,const CPPValue& inRightOperandValue)
	{
		return inType->Add(inLeftOperandValue,inRightOperandValue);
	}

};

static ExecuteAddition sAddition;

class ExecuteSubtraction : public IBinaryOperatorCommand
{

public:

	virtual CPPValue Evaluate(ICPPType* inType, const CPPValue& inLeftOperandValue,const CPPValue& inRightOperandValue)
	{
		return inType->Subtract(inLeftOperandValue,inRightOperandValue);
	}

};

static ExecuteSubtraction sSubtraction;

class ExecuteLeftByteShift : public IBinaryOperatorCommand
{

public:

	virtual CPPValue Evaluate(ICPPType* inType, const CPPValue& inLeftOperandValue,const CPPValue& inRightOperandValue)
	{
		return inType->LeftByteShift(inLeftOperandValue,inRightOperandValue);
	}

};

static ExecuteLeftByteShift sLeftByteShift;

class ExecuteRightByteShift : public IBinaryOperatorCommand
{

public:

	virtual CPPValue Evaluate(ICPPType* inType, const CPPValue& inLeftOperandValue,const CPPValue& inRightOperandValue)
	{
		return inType->RightByteShift(inLeftOperandValue,inRightOperandValue);
	}

};

static ExecuteRightByteShift sRightByteShift;

class ExecuteLessThan : public IBinaryOperatorCommand
{

public:

	virtual CPPValue Evaluate(ICPPType* inType, const CPPValue& inLeftOperandValue,const CPPValue& inRightOperandValue)
	{
		return inType->LessThan(inLeftOperandValue,inRightOperandValue);
	}

};

static ExecuteLessThan sLessThan;

class ExecuteGreaterThan : public IBinaryOperatorCommand
{

public:

	virtual CPPValue Evaluate(ICPPType* inType, const CPPValue& inLeftOperandValue,const CPPValue& inRightOperandValue)
	{
		return inType->GreaterThan(inLeftOperandValue,inRightOperandValue);
	}

};

static ExecuteGreaterThan sGreaterThan;

class ExecutesLessThanOrEqual : public IBinaryOperatorCommand
{

public:

	virtual CPPValue Evaluate(ICPPType* inType, const CPPValue& inLeftOperandValue,const CPPValue& inRightOperandValue)
	{
		return inType->LessThanOrEqual(inLeftOperandValue,inRightOperandValue);
	}

};

static ExecutesLessThanOrEqual sLessThanOrEqual;

class ExecutesGreaterThanOrEqual : public IBinaryOperatorCommand
{

public:

	virtual CPPValue Evaluate(ICPPType* inType, const CPPValue& inLeftOperandValue,const CPPValue& inRightOperandValue)
	{
		return inType->GreaterThanOrEqual(inLeftOperandValue,inRightOperandValue);
	}

};

static ExecutesGreaterThanOrEqual sGreaterThanOrEqual;

class ExecutesEquality : public IBinaryOperatorCommand
{

public:

	virtual CPPValue Evaluate(ICPPType* inType, const CPPValue& inLeftOperandValue,const CPPValue& inRightOperandValue)
	{
		return inType->Equal(inLeftOperandValue,inRightOperandValue);
	}

};

static ExecutesEquality sEquality;

class ExecutesInequality : public IBinaryOperatorCommand
{

public:

	virtual CPPValue Evaluate(ICPPType* inType, const CPPValue& inLeftOperandValue,const CPPValue& inRightOperandValue)
	{
		return inType->NotEqual(inLeftOperandValue,inRightOperandValue);
	}

};

static ExecutesInequality sInequality;

class ExecutesBitwiseAnd : public IBinaryOperatorCommand
{

public:

	virtual CPPValue Evaluate(ICPPType* inType, const CPPValue& inLeftOperandValue,const CPPValue& inRightOperandValue)
	{
		return inType->BitwiseAnd(inLeftOperandValue,inRightOperandValue);
	}

};

static ExecutesBitwiseAnd sBitwiseAnd;

class ExecutesBitwiseOr : public IBinaryOperatorCommand
{

public:

	virtual CPPValue Evaluate(ICPPType* inType, const CPPValue& inLeftOperandValue,const CPPValue& inRightOperandValue)
	{
		return inType->BitwiseOr(inLeftOperandValue,inRightOperandValue);
	}

};

static ExecutesBitwiseOr sBitwiseOr;

class ExecutesExclusiveOr : public IBinaryOperatorCommand
{

public:

	virtual CPPValue Evaluate(ICPPType* inType, const CPPValue& inLeftOperandValue,const CPPValue& inRightOperandValue)
	{
		return inType->ExclusiveOr(inLeftOperandValue,inRightOperandValue);
	}

};

static ExecutesExclusiveOr sExclusiveOr;

IBinaryOperatorCommand* CPPOperatorExpression::GetBinaryOperatorCommand(ECPPOperatorType inType)
{
	IBinaryOperatorCommand* result;

	switch(inType)
	{
		case eCPPOperatorMultiplication:
			result = &sMultiplication;
			break;
		case eCPPOperatorDivision:
			result = &sDivision;
			break;
		case eCPPOperatorModulus:
			result = &sModulus;
			break;
		case eCPPOperatorAddition:
			result = &sAddition;
			break;
		case eCPPOperatorSubtraction:
			result = &sSubtraction;
			break;
		case eCPPOperatorLeftByteShift:
			result = &sLeftByteShift;
			break;
		case eCPPOperatorRightByteShift:
			result = &sRightByteShift;
			break;
		case eCPPOperatorLessThan:
			result = &sLessThan;
			break;
		case eCPPOperatorGreaterThan:
			result = &sGreaterThan;
			break;
		case eCPPOperatorLessThanOrEqual:
			result = &sLessThanOrEqual;
			break;
		case eCPPOperatorGreaterThanOrEqual:
			result = &sGreaterThanOrEqual;
			break;
		case eCPPOperatorEquality:
			result = &sEquality;
			break;
		case eCPPOperatorInequality:
			result = &sInequality;
			break;
		case eCPPOperatorBitwiseAnd:
			result = &sBitwiseAnd;
			break;
		case eCPPOperatorBitwiseOr:
			result = &sBitwiseOr;
			break;
		case eCPPOperatorExclusiveOr:
			result = &sExclusiveOr;
			break;
		default:
			result = NULL;
			break;
	}

	return result;	
}


CPPValue CPPOperatorExpression::EvaluateShiftOperator(ECPPOperatorType inOperatorType,const CPPValue& inLeftOperand, const CPPValue& inRightOperand)
{
	// make  the 2nd operand unsigned long long, so we're always providing the same type to shift action...so i can write less code.
	
	return GetBinaryOperatorCommand(inOperatorType)->Evaluate(GetTypeClass(inLeftOperand.mType),inLeftOperand,GetTypeClass(inRightOperand.mType)->ToULongLong(inRightOperand));
}