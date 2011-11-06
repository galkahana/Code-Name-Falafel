#pragma once

#include <string>

using namespace std;

class CPPElement
{
public:
	enum ECPPElementType
	{
		eCPPElementNamespace
	};


	CPPElement(const string& inName,ECPPElementType inType);
	~CPPElement(void);


	ECPPElementType Type;
	string Name;

};
