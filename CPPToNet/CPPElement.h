#pragma once

#include <string>

using namespace std;

class CPPElement
{
public:
	enum ECPPElementType
	{
		// typenames
		eCPPElementPrimitive,
		eCPPElementEnumerator,
		eCPPElementUnion,
		eCPPElementTypedef,
		eCPPElementClass,
		eCPPElementStruct,
		eCPPElemeentTemplateTypename,
		eCPPElemeentTemplateTemplateParameter,
		eCPPElementClassTemplateInstance,
		eCPPElementStructTemplateInstance,
		eCPPTypenames, // just a marker for typenames
		// values
		eCPPElementNamespace,
		eCPPElementEnumeratorValue,
		eCPPElementVariable,
		eCPPElementFunction, // can be also function template
		eCPPElementTemplateValue,
		eCPPElementTotalCount
	};

	static string TypeLabels[eCPPElementTotalCount];

	CPPElement(const string& inName,ECPPElementType inType);
	~CPPElement(void);

	// returns true if this element is a type definition (as oppose to an actual instance - function, variable)
	bool IsType();


	ECPPElementType Type;
	string Name;

};
