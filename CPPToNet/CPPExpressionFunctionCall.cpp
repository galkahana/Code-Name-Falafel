#include "CPPExpressionFunctionCall.h"

CPPExpressionFunctionCall::CPPExpressionFunctionCall(const string& inFunctionName,const StringList& inScopes,const CPPExpressionList& inParameters):CPPExpression(CPPExpression::eCPPExpressionFunctionCall)
{
	FunctionName = inFunctionName;
	Scopes = inScopes;
	Parameters = inParameters;
}

CPPExpressionFunctionCall::~CPPExpressionFunctionCall(void)
{
}

BoolAndCPPPrimitiveValue CPPExpressionFunctionCall::Evaluate(IExpressionEvaluationContext* inContext)
{
	// TODO. need to be determined by environment
	return BoolAndCPPPrimitiveValue(false,CPPPrimitiveValue());
}

bool CPPExpressionFunctionCall::IsEqual(CPPExpression* inOther)
{
	if(inOther->Type != CPPExpression::eCPPExpressionFunctionCall)
		return false;


	CPPExpressionFunctionCall* otherCall = (CPPExpressionFunctionCall*)inOther;
	if(FunctionName != otherCall->FunctionName)
		return false;

	if(Scopes.size() != otherCall->Scopes.size())
		return false;

	StringList::const_iterator itScopes = Scopes.begin();
	StringList::const_iterator itScopesOther = otherCall->Scopes.begin();

	bool result = true;
	for(; itScopes != Scopes.end() && result; ++itScopes,++itScopesOther)
		result = (*itScopes == *itScopesOther);
	if(!result)
		return false;

	if(Parameters.size() != otherCall->Parameters.size())
		return false;

	CPPExpressionList::const_iterator itParameters = Parameters.begin();
	CPPExpressionList::const_iterator itParametersOther = otherCall->Parameters.begin();
	for(; itParameters != Parameters.end() && result; ++itParameters,++itParametersOther)
		result = ((*itParameters)->IsEqual(*itParametersOther));

	return result;
}

bool CPPExpressionFunctionCall::IsLess(CPPExpression* inOther)
{
	if(inOther->Type != CPPExpression::eCPPExpressionFunctionCall)
		return inOther->Type < CPPExpression::eCPPExpressionFunctionCall;


	CPPExpressionFunctionCall* otherCall = (CPPExpressionFunctionCall*)inOther;
	if(FunctionName != otherCall->FunctionName)
		return FunctionName.compare(otherCall->FunctionName) < 0;

	if(Scopes.size() != otherCall->Scopes.size())
		return Scopes.size() < otherCall->Scopes.size();

	StringList::const_iterator itScopes = Scopes.begin();
	StringList::const_iterator itScopesOther = otherCall->Scopes.begin();

	bool isEqual = true;
	bool isLess = false;
	for(; itScopes != Scopes.end() && isEqual; ++itScopes,++itScopesOther)
	{
		isEqual = (*itScopes == *itScopesOther);
		if(!isEqual)
			isLess = (itScopes->compare(*itScopesOther) < 0);
	}
	if(!isEqual)
		return isLess;

	if(Parameters.size() != otherCall->Parameters.size())
		return Parameters.size() < otherCall->Parameters.size();

	CPPExpressionList::const_iterator itParameters = Parameters.begin();
	CPPExpressionList::const_iterator itParametersOther = otherCall->Parameters.begin();
	for(; itParameters != Parameters.end() && isEqual; ++itParameters,++itParametersOther)
	{
		isEqual = ((*itParameters)->IsEqual(*itParametersOther));
		if(!isEqual)
			isLess = ((*itParameters)->IsLess(*itParametersOther));
	}

	return isLess;
}
