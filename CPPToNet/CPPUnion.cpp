#include "CPPUnion.h"
#include "CPPVariable.h"
#include "Trace.h"

CPPUnion::CPPUnion(const string& inName,bool inIsDefinition):CPPElement(inName,CPPElement::eCPPElementUnion)
{
	mIsDefinition = inIsDefinition;
}

CPPUnion::~CPPUnion(void)
{
	StringToCPPVariableMap::iterator it = mVariables.begin();

	for(; it != mVariables.end();++it)
		delete it->second;
}

CPPElementList CPPUnion::FindElements(const string& inElementName)
{
	CPPElementList result;

	CPPElement* anElement = FindElement(inElementName);
	if(anElement)
		result.push_back(anElement);

	return result;
}

CPPElement* CPPUnion::FindElement(const string& inElementName)
{
	StringToCPPVariableMap::iterator it = mVariables.find(inElementName);

	if(it == mVariables.end())
		return NULL;
	else
		return it->second;
}

CPPVariable* CPPUnion::CreateVariable(const string& inVariableName,
									  UsedTypeDescriptor* inTypeDescriptor)
{
	CPPElement* existingElement = FindElement(inVariableName);

	if(existingElement)
	{
		TRACE_LOG1("CPPUnion::CreateVariable, syntax error, variable value is already defined for %s",
					inVariableName.c_str());
		return NULL;
	}
	else
	{
		CPPVariable* aVariable = new CPPVariable(inVariableName,inTypeDescriptor);

		mVariables.insert(StringToCPPVariableMap::value_type(inVariableName,aVariable));
		
		return aVariable;
	}		
}

CPPFunction* CPPUnion::CreateFunction(	const string& inFunctionName,
										bool inIsVirtual,
										bool inIsStatic,
										UsedTypeDescriptor* inReturnType,
										const TypedParameterList& inParametersList,
										bool inHasElipsis,
										bool inIsPure,
										const UsedTypeOrExpressionList& inTemplateSpecializationList,
										bool inIsTemplateInstantiation,
										bool inIsDefinition)
{
	TRACE_LOG("CPPUnion::CreateFunction, syntax error, cannot define a function in a union");
	return NULL;
}

CPPFunction* CPPUnion::CreateFunctionTemplate(const string& inFunctionName,
									bool inIsVirtual,
									bool inIsStatic,
									UsedTypeDescriptor* inReturnType,
									const TypedParameterList& inParametersList,
									bool inHasElipsis,
									bool inIsPure,
									bool inIsDefinition,
									const CPPElementList& inTemplateParameters,
									const UsedTypeOrExpressionList& inTemplateSpecializationList)
{
	TRACE_LOG("CPPUnion::CreateFunctionTemplate, syntax error, cannot define a function template in a union");
	return NULL;
}

bool CPPUnion::IsDefinition()
{
	return mIsDefinition;
}

void CPPUnion::SetIsDefinition()
{
	mIsDefinition = true;
}
