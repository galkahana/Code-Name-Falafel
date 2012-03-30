#include "CPPExpressionNewOperator.h"
#include "TypedParameter.h"
#include "UsedTypeDescriptor.h"

CPPExpressionNewOperator::CPPExpressionNewOperator(TypedParameter* inNewOfType,const CPPExpressionList& inInitializerParameters):CPPExpression(CPPExpression::eCPPExpressionOperatorNew)
{
	NewOfType = inNewOfType;
	InitializerParameters = inInitializerParameters;
}


CPPExpressionNewOperator::~CPPExpressionNewOperator(void)
{
	delete NewOfType;

	CPPExpressionList::iterator	it = InitializerParameters.begin();
	for(; it != InitializerParameters.end(); ++it)
		delete *it;
}

BoolAndCPPPrimitiveValue CPPExpressionNewOperator::Evaluate(IExpressionEvaluationContext* inContext)
{
	// TODO. need to be determined by environment
	return BoolAndCPPPrimitiveValue(false,CPPPrimitiveValue());
}

bool CPPExpressionNewOperator::IsEqual(CPPExpression* inOther)
{
	if(inOther->Type != CPPExpression::eCPPExpressionOperatorNew)
		return false;

	CPPExpressionNewOperator* otherNew = (CPPExpressionNewOperator*)inOther;

	if(!NewOfType->Type->IsEqual(otherNew->NewOfType->Type))
		return false;


	if(InitializerParameters.size() != otherNew->InitializerParameters.size())
		return false;

	bool result = true;
	CPPExpressionList::const_iterator itParameters = InitializerParameters.begin();
	CPPExpressionList::const_iterator itParametersOther = otherNew->InitializerParameters.begin();
	for(; itParameters != InitializerParameters.end() && result; ++itParameters,++itParametersOther)
		result = ((*itParameters)->IsEqual(*itParametersOther));

	return result;

}

bool CPPExpressionNewOperator::IsLess(CPPExpression* inOther)
{
	if(inOther->Type != CPPExpression::eCPPExpressionOperatorNew)
		return  CPPExpression::eCPPExpressionOperatorNew < inOther->Type;


	CPPExpressionNewOperator* otherNew = (CPPExpressionNewOperator*)inOther;

	if(!NewOfType->Type->IsEqual(otherNew->NewOfType->Type))
		return NewOfType->Type->IsLess(otherNew->NewOfType->Type);

	bool isEqual = true;
	bool isLess = false;

	if(InitializerParameters.size() != otherNew->InitializerParameters.size())
		return InitializerParameters.size() < otherNew->InitializerParameters.size();

	CPPExpressionList::const_iterator itParameters = InitializerParameters.begin();
	CPPExpressionList::const_iterator itParametersOther = otherNew->InitializerParameters.begin();
	for(; itParameters != InitializerParameters.end() && isEqual; ++itParameters,++itParametersOther)
	{
		isEqual = ((*itParameters)->IsEqual(*itParametersOther));
		if(!isEqual)
			isLess = ((*itParameters)->IsLess(*itParametersOther));
	}

	return isLess;
}