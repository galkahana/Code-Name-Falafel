#include "CPPStruct.h"


CPPStruct::CPPStruct(const string& inName,
					bool inIsDefinition):AbstractClassOrStruct(inName,inIsDefinition,eCPPElementStruct,eAbstractClassOrStructAccessLevelPublic)
{
}

CPPStruct::~CPPStruct()
{

}