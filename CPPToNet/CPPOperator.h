#pragma once

enum ECPPOperatorType
{
	eCPPOperatorDefined,
	eCPPOperatorOnesComplement,
	eCPPOperatorNot,
	eCPPOperatorUnaryMinus,
	eCPPOperatorUnaryPlus,
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

class CPPOperator
{
public:
	CPPOperator();
	CPPOperator(ECPPOperatorType inType);
	~CPPOperator(void);

	ECPPOperatorType Type;

	bool Precedes(const CPPOperator& inOther) const;
	bool Precedes(CPPOperator* inOther) const;
};
