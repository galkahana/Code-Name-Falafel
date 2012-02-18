#pragma once

#include "AbstractClassOrStruct.h"

class CPPStruct : public AbstractClassOrStruct
{
public:
	CPPStruct(const string& inName,
				bool inIsDefinition);
	~CPPStruct(void);
};

