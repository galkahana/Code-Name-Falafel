#include "CPPOperator.h"

CPPOperator::CPPOperator()
{
	Type = eCPPOperatorUndefined;
}

CPPOperator::CPPOperator(ECPPOperatorType inType)
{
	Type = inType;
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
