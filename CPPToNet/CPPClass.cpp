#include "CPPClass.h"

CPPClass::CPPClass(const string& inName,
					bool inIsDefinition):AbstractClassOrStruct(inName,inIsDefinition,eCPPElementClass,eAbstractClassOrStructAccessLevelPrivate)
{
}

CPPClass::~CPPClass()
{

}