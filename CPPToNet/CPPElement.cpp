#include "CPPElement.h"

string CPPElement::TypeLabels[eCPPElementTotalCount] = 
{
		"eCPPElementPrimitive",
		"eCPPElementNamespace",
		"eCPPElementClass",
		"eCPPElementEnumerator",
		"eCPPElementUnion",
		"eCPPElementTypedef",
		"eCPPTypenames", 
		"eCPPElementEnumeratorValue",
		"eCPPElementVariable",
		"eCPPElementFunction"
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

