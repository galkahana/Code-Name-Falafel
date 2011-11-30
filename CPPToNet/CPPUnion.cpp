#include "CPPUnion.h"
#include "CPPVariable.h"
#include "Trace.h"

CPPUnion::CPPUnion(const string& inName):CPPElement(inName,CPPElement::eCPPElementUnion)
{
}

CPPUnion::~CPPUnion(void)
{
	StringToCPPVariableMap::iterator it = mVariables.begin();

	for(; it != mVariables.end();++it)
		delete it->second;
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
									  CPPElement* inType,
									  bool inIsAuto,
									  bool inIsRegister,
									  bool inIsExtern,
									  bool inIsConst,
									  bool inIsVolatile)
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
		CPPVariable* aVariable = new CPPVariable(inType,inVariableName,inIsAuto,inIsRegister,inIsExtern,inIsConst,inIsVolatile);

		mVariables.insert(StringToCPPVariableMap::value_type(inVariableName,aVariable));
		
		return aVariable;
	}		
}