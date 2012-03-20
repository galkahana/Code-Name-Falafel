#pragma once

enum ECPPOperatorType
{
	eCPPOperatorDefined,
	eCPPOperatorSubscript,
	eCPPOperatorPostfixIncrement,
	eCPPOperatorPostfixDecrement,
	eCPPOperatorSizeof,
	eCPPOperatorPrefixIncrement,
	eCPPOperatorPrefixDecrement,
	eCPPOperatorOnesComplement,
	eCPPOperatorNot,
	eCPPOperatorUnaryMinus,
	eCPPOperatorUnaryPlus,
	eCPPOperatorAddressOf,
	eCPPOperatorIndirection,
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
	eCPPOperatorUndefined
};

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
