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

bool CPPTypedef::IsEqual(CPPTypedef* inOther)
{
	return mTypeDescriptor->IsEqual(inOther->mTypeDescriptor);
}

bool CPPTypedef::IsLess(CPPTypedef* inOther)
{
	return mTypeDescriptor->IsLess(inOther->mTypeDescriptor);
}