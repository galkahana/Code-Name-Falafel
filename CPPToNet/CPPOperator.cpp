#include "CPPOperator.h"
#include "CPPElement.h"

CPPOperator::CPPOperator()
{
	Type = eCPPOperatorUndefined;
	Parameter = NULL;
}

CPPOperator::CPPOperator(ECPPOperatorType inType)
{
	Type = inType;
	Parameter = NULL;
}

CPPOperator::CPPOperator(ECPPOperatorType inType,CPPExpression* inParameter)
{
	Type = inType;
	Parameter = inParameter;
}


CPPOperator::~CPPOperator(void)
{
}

bool CPPOperator::Precedes(const CPPOperator& inOther) const
{
	return Type < inOther.Type;
}

bool CPPOperator::Precedes(CPPOperator* inOther) const
{
	return Type < inOther->Type;
}
