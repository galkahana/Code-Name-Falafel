#include "CPPExpressionTypename.h"
#include "TypedParameter.h"
#include "UsedTypeDescriptor.h"


CPPExpressionTypename::CPPExpressionTypename(void):CPPExpression(CPPExpression::eCPPExpressionTypename)
{
	mTypename = NULL;
}

CPPExpressionTypename::CPPExpressionTypename(TypedParameter* inTypename):CPPExpression(CPPExpression::eCPPExpressionTypename)
{
	mTypename = inTypename;
}


CPPExpressionTypename::~CPPExpressionTypename(void)
{
	delete mTypename;
}

UsedTypeDescriptor* CPPExpressionTypename::GetType()
{
	return mTypename ? mTypename->Type : NULL;
}

BoolAndCPPPrimitiveValue CPPExpressionTypename::Evaluate(IExpressionEvaluationContext* inContext)
{
	// should never be evaluated. makes sense only in context of other expressions that does not require evaluate to primitive
	return BoolAndCPPPrimitiveValue(false,CPPPrimitiveValue());
}

bool CPPExpressionTypename::IsEqual(CPPExpression* inOther)
{
	if(inOther->Type != Type)
		return false;

	return mTypename->Type->IsEqual(((CPPExpressionTypename*)inOther)->GetType());
}

bool CPPExpressionTypename::IsLess(CPPExpression* inOther)
{
	if(inOther->Type != Type)
		return Type < inOther->Type;

	return mTypename->Type->IsLess(((CPPExpressionTypename*)inOther)->GetType());
}