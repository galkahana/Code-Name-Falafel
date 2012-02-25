#include "CPPElement.h"

string CPPElement::TypeLabels[eCPPElementTotalCount] = 
{
		"eCPPElementPrimitive",
		"eCPPElementEnumerator",
		"eCPPElementUnion",
		"eCPPElementTypedef",
		"eCPPElementClass",
		"eCPPElementStruct",
		"eCPPElemeentTemplateTypename",
		"eCPPElemeentTemplateTemplateParameter",
		"eCPPTypenames",
		"eCPPElementNamespace",
		"eCPPElementEnumeratorValue",
		"eCPPElementVariable",
		"eCPPElementFunction",
		"eCPPElementTemplateValue"
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

