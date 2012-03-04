#include "CPPStruct.h"
#include "CPPStructTemplateInstance.h"

CPPStruct::CPPStruct(const string& inName,
					bool inIsDefinition):AbstractClassOrStruct(inName,inIsDefinition,eCPPElementStruct,eAbstractClassOrStructAccessLevelPublic)
{
}

CPPStruct::CPPStruct(const string& inName,
					 const CPPElementList& inTemplateParameters,
					 bool inIsDefinition):AbstractClassOrStruct(inName,inIsDefinition,eCPPElementStruct,eAbstractClassOrStructAccessLevelPublic,inTemplateParameters)
{

}

CPPStruct::CPPStruct(const string& inName,
			const CPPElementList& inTemplateParameters,
			const UsedTypeOrExpressionList& inTemplateParametersAssignments,
			bool inIsDefinition):AbstractClassOrStruct(inName,inIsDefinition,eCPPElementStruct,eAbstractClassOrStructAccessLevelPublic,inTemplateParameters,inTemplateParametersAssignments)
{

}

CPPStruct::~CPPStruct()
{
	// note that specializations are not owned by the templates, while instances are
	UsedTypeOrExpressionListToCPPStructTemplateInstanceMap::iterator itInstances = mInstances.begin();

	for(; itInstances != mInstances.end(); ++itInstances)
		delete itInstances->second;
}

void CPPStruct::AddSpecialization(CPPStruct* inSpecialization)
{
	mSpecializations.push_back(inSpecialization);
}

CPPStructTemplateInstanceAndBool CPPStruct::QueryInstance(const UsedTypeOrExpressionList& inTemplateParametersAssignments)
{
	CPPStructTemplateInstanceAndBool result;

	UsedTypeOrExpressionListToCPPStructTemplateInstanceMap::iterator it = mInstances.find(inTemplateParametersAssignments);

	if(it == mInstances.end())
	{
		result.first = mInstances.insert(
							UsedTypeOrExpressionListToCPPStructTemplateInstanceMap::value_type(
																								inTemplateParametersAssignments,
																								new CPPStructTemplateInstance(Name,this,inTemplateParametersAssignments)
																							 )).first->second;
		result.second = false;
	}
	else
	{
		result.first = it->second;
		result.second = false;
	}
	return result;
}