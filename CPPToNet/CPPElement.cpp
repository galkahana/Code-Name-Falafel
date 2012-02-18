#include "CPPElement.h"

string CPPElement::TypeLabels[eCPPElementTotalCount] = 
{
		"eCPPElementPrimitive",
		"eCPPElementEnumerator",
		"eCPPElementUnion",
		"eCPPElementTypedef",
		"eCPPElementClass",
		"eCPPElementStruct",
		"eCPPTypenames",
		"eCPPElementNamespace",
		"eCPPElementEnumeratorValue",
		"eCPPElementVariable",
		"eCPPElementFunction",
};

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

