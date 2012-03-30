#pragma once

enum ECPPOperatorType
{
	eCPPOperatorDefined,
	eCPPOperatorMemberSelectionObject,
	eCPPOperatorMemberSelectionPointer,
	eCPPOperatorSubscript,
	eCPPOperatorFunctionCall,
	eCPPOperatorPostfixIncrement,
	eCPPOperatorPostfixDecrement,
	eCPPOperatorTypeID,
	eCPPOperatorConstCast,
	eCPPOperatorDynamicCast,
	eCPPOperatorReinterpretCast,
	eCPPOperatorStaticCast,
	eCPPOperatorSizeof,
	eCPPOperatorPrefixIncrement,
	eCPPOperatorPrefixDecrement,
	eCPPOperatorOnesComplement,
	eCPPOperatorNot,
	eCPPOperatorUnaryMinus,
	eCPPOperatorUnaryPlus,
	eCPPOperatorAddressOf,
	eCPPOperatorIndirection,
	eCPPOperatorNew,
	eCPPOperatorDelete,
	eCPPOperatorDeleteArray,
	eCPPOperatorCasting,
	eCPPOperatorMultiplication,
	eCPPOperatorDivision,
	eCPPOperatorModulus,
	eCPPOperatorAddition,
	eCPPOperatorSubtraction,
	eCPPOperatorLeftByteShift,
	eCPPOperatorRightByteShift,
	eCPPOperatorLessThan,
	eCPPOperatorGreaterThan,
	eCPPOperatorLessThanOrEqual,
	eCPPOperatorGreaterThanOrEqual,
	eCPPOperatorEquality,
	eCPPOperatorInequality,
	eCPPOperatorBitwiseAnd,
	eCPPOperatorExclusiveOr,
	eCPPOperatorBitwiseOr,
	eCPPOperatorLogicalAnd,
	eCPPOperatorLogicalOr,
	eCPPOperatorConditional,
	eCPPOperatorConditionalSecond,
	eCPPOperatorAssigment,
	eCPPOperatorMultiplicationAssigment,
	eCPPOperatorDivisionAssigment,
	eCPPOperatorModulusAssigment,
	eCPPOperatorAdditionAssigment,
	eCPPOperatorSubstractionAssigment,
	eCPPOperatorLeftShiftAssigment,
	eCPPOperatorRightShiftAssigment,
	eCPPOperatorBitwiseAndAssigment,
	eCPPOperatorBitwiseOrAssigment,
	eCPPOperatorBitwiseExclusiveOrAssigment,
	eCPPOperatorThrow,
	eCPPOperatorUndefined
};

// i'm excluding comma (,), pointer to member (.* and ->*) and scope resolution (::). things should be fine

class CPPExpression;

class CPPOperator
{
public:
	CPPOperator();
	CPPOperator(ECPPOperatorType inType);
	CPPOperator(ECPPOperatorType inType,CPPExpression* inParameter);
	~CPPOperator(void);

	ECPPOperatorType Type;
	CPPExpression* Parameter;

	bool Precedes(const CPPOperator& inOther) const;
	bool Precedes(CPPOperator* inOther) const;
};
