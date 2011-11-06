#include "HeaderUnit.h"

HeaderUnit::HeaderUnit(void):mGlobalNamespace("")
{
}

HeaderUnit::~HeaderUnit(void)
{
}


CPPNamespace* HeaderUnit::GetGlobalNamespace()
{
	return &mGlobalNamespace;
}