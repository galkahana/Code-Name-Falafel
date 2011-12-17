#include "CPPVariable.h"
#include "UsedTypeDescriptor.h"

CPPVariable::CPPVariable(const string& inVariableName,
						 UsedTypeDescriptor* inTypeDescriptor):CPPElement(inVariableName,CPPElement::eCPPElementVariable)
{
	mTypeDescriptor = inTypeDescriptor;
}

CPPVariable::~CPPVariable(void)
{
	delete mTypeDescriptor;
}

UsedTypeDescriptor* CPPVariable::GetTypeDescriptor()
{
	return mTypeDescriptor;
}