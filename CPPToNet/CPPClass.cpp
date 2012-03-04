#include "CPPClass.h"
#include "CPPClassTemplateInstance.h"

CPPClass::CPPClass(const string& inName,
					bool inIsDefinition):AbstractClassOrStruct(inName,inIsDefinition,eCPPElementClass,eAbstractClassOrStructAccessLevelPrivate)
{
}

CPPClass::CPPClass(const string& inName,
					 const CPPElementList& inTemplateParameters,
					 bool inIsDefinition):AbstractClassOrStruct(inName,inIsDefinition,eCPPElementClass,eAbstractClassOrStructAccessLevelPrivate,inTemplateParameters)
{

}

CPPClass::CPPClass(const string& inName,
			const CPPElementList& inTemplateParameters,
			const UsedTypeOrExpressionList& inTemplateParametersAssignments,
			bool inIsDefinition):AbstractClassOrStruct(inName,inIsDefinition,eCPPElementClass,eAbstractClassOrStructAccessLevelPrivate,inTemplateParameters,inTemplateParametersAssignments)
{

}


CPPClass::~CPPClass()
{

	// note that specializations are not owned by the templates, while instances are
	UsedTypeOrExpressionListToCPPClassTemplateInstanceMap::iterator itInstances = mInstances.begin();

	for(; itInstances != mInstances.end(); ++itInstances)
		delete itInstances->second;
}

void CPPClass::AddSpecialization(CPPClass* inSpecialization)
{
	mSpecializations.push_back(inSpecialization);
}

CPPClassTemplateInstanceAndBool CPPClass::QueryInstance(const UsedTypeOrExpressionList& inTemplateParametersAssignments)
{
	CPPClassTemplateInstanceAndBool result;

	UsedTypeOrExpressionListToCPPClassTemplateInstanceMap::iterator it = mInstances.find(inTemplateParametersAssignments);

	if(it == mInstances.end())
	{
		result.first = mInstances.insert(
							UsedTypeOrExpressionListToCPPClassTemplateInstanceMap::value_type(
																								inTemplateParametersAssignments,
																								new CPPClassTemplateInstance(Name,this,inTemplateParametersAssignments)
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