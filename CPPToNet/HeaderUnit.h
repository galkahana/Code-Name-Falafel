#pragma once

#include "CPPNamespace.h"

class HeaderUnit
{
public:
	HeaderUnit(void);
	~HeaderUnit(void);

	CPPNamespace* GetGlobalNamespace();

private:

	CPPNamespace mGlobalNamespace;
};
