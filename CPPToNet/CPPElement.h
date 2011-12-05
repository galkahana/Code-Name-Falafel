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
		eCPPElementNamespace,
		eCPPElementEnumerator,
		eCPPElementUnion,
		eCPPElementTypedef,
		eCPPTypenames, // just a marker for typenames
		// values
		eCPPElementEnumeratorValue,
		eCPPElementVariable
	};


	CPPElement(const string& inName,ECPPElementType inType);
	~CPPElement(void);

	// returns true if this element is a type definition (as oppose to an actual instance - function, variable)
	bool IsType();


	ECPPElementType Type;
	string Name;

};
