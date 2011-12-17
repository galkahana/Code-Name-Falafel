#include "CPPTypedef.h"
#include "UsedTypeDescriptor.h"

CPPTypedef::CPPTypedef(const string& inVariableName,
						 UsedTypeDescriptor* inTypeDescriptor):CPPElement(inVariableName,CPPElement::eCPPElementTypedef)
{
	mTypeDescriptor = inTypeDescriptor;
}

CPPTypedef::~CPPTypedef(void)
{
	delete mTypeDescriptor;
}

UsedTypeDescriptor* CPPTypedef::GetTypeDescriptor()
{
	return mTypeDescriptor;
}