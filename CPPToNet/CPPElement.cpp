#include "CPPElement.h"

CPPElement::CPPElement(const string& inName,ECPPElementType inType)
{
	Name = inName;
	Type = inType;
}

CPPElement::~CPPElement(void)
{
}

bool CPPElement::IsType()
{
	return Type < CPPElement::eCPPTypenames;
}
