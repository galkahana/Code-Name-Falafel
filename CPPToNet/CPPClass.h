#pragma once

#include "AbstractClassOrStruct.h"

class CPPClass : public AbstractClassOrStruct
{
public:



	CPPClass(const string& inName,
				bool inIsDefinition);
	virtual ~CPPClass(void);
};

