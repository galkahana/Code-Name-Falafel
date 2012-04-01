#include "CPPExpressionVariable.h"
#include "CPPElement.h"

CPPExpressionVariable::CPPExpressionVariable(const string& inVariableName,const StringList& inScopes):CPPExpression(CPPExpression::eCPPExpressionVariable)
{
	VariableName = inVariableName;
	Scopes = inScopes;
	mVariableElement = NULL;
}

CPPExpressionVariable::CPPExpressionVariable(CPPElement* inVariableElement):CPPExpression(CPPExpression::eCPPExpressionVariable)
{
	mVariableElement = inVariableElement;
}


CPPExpressionVariable::~CPPExpressionVariable(void)
{
}

BoolAndCPPPrimitiveValue CPPExpressionVariable::Evaluate(IExpressionEvaluationContext* inContext)
{
	// TODO. need to be determined by environment
	return BoolAndCPPPrimitiveValue(false,CPPPrimitiveValue());
}

bool CPPExpressionVariable::IsEqual(CPPExpression* inOther)
{
	if(inOther->Type != CPPExpression::eCPPExpressionVariable)
		return false;

	CPPExpressionVariable* otherVariable = (CPPExpressionVariable*)inOther;

	if(VariableName != otherVariable->VariableName)
		return false;

	if((mVariableElement == NULL) != (otherVariable->mVariableElement == NULL))
		return false;

	if(mVariableElement)
	{
		// in the context of an expression, variable name should be enough
		if(mVariableElement->Name != otherVariable->mVariableElement->Name)
			return false;
	}


	if(Scopes.size() != otherVariable->Scopes.size())
		return false;

	bool isEqual = true;
	StringList::const_iterator itScopes = Scopes.begin();
	StringList::const_iterator itScopesOther = otherVariable->Scopes.begin();
	for(; itScopes != Scopes.end() && isEqual; ++itScopes, ++itScopesOther)
		isEqual = (*itScopes == *itScopesOther);


	return isEqual;
}

bool CPPExpressionVariable::IsLess(CPPExpression* inOther)
{
	if(inOther->Type != CPPExpression::eCPPExpressionVariable)
		return CPPExpression::eCPPExpressionVariable < inOther->Type;

	CPPExpressionVariable* otherVariable = (CPPExpressionVariable*)inOther;

	if(VariableName != otherVariable->VariableName)
		return VariableName.compare(otherVariable->VariableName) < 0;

	if((mVariableElement == NULL) != (otherVariable->mVariableElement == NULL))
		return !mVariableElement;

	if(mVariableElement)
	{
		// in the context of an expression, variable name should be enough
		if(mVariableElement->Name != otherVariable->mVariableElement->Name)
			return mVariableElement->Name.compare(otherVariable->mVariableElement->Name) < 0;
	}

	if(Scopes.size() != otherVariable->Scopes.size())
		return Scopes.size() < otherVariable->Scopes.size();

	bool isEqual = true;
	bool isLess = false;
	StringList::const_iterator itScopes = Scopes.begin();
	StringList::const_iterator itScopesOther = otherVariable->Scopes.begin();
	for(; itScopes != Scopes.end() && isEqual; ++itScopes, ++itScopesOther)
	{
		isEqual = (*itScopes == *itScopesOther);
		if(!isEqual)
			isLess = (itScopes->compare(*itScopesOther) < 0);
	}

	return isLess;
}