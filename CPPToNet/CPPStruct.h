#pragma once

#include "AbstractClassOrStruct.h"
#include "UsedTypeOrExpression.h"
#include <list>
#include <map>
#include <utility>


class CPPStruct;
class CPPStructTemplateInstance;

using namespace std;
typedef list<CPPStruct*> CPPStructList;
typedef pair<CPPStructTemplateInstance*,bool> CPPStructTemplateInstanceAndBool;

typedef map<UsedTypeOrExpressionList,CPPStructTemplateInstance*,UsedTypeOrExpressionListLess> UsedTypeOrExpressionListToCPPStructTemplateInstanceMap;

class CPPStruct : public AbstractClassOrStruct
{
public:
	CPPStruct(const string& inName,
				bool inIsDefinition);
	// constructor for struct template
	CPPStruct(const string& inName,
			 const CPPElementList& inTemplateParameters,
			 bool inIsDefinition);
	// constructor for struct template specialization (partial or full)
	CPPStruct(const string& inName,
			 const CPPElementList& inTemplateParameters,
			 const UsedTypeOrExpressionList& inTemplateParametersAssignments,
			 bool inIsDefinition);
	~CPPStruct(void);

	// for tamplate classes
	void AddSpecialization(CPPStruct* inSpecialization); // these are references to the template specializations
	// if creates a new instance, will take ownership of the parametsrs. return value is the instance, and whether created a new one
	CPPStructTemplateInstanceAndBool QueryInstance(const UsedTypeOrExpressionList& inTemplateParametersAssignments);

private:

	UsedTypeOrExpressionListToCPPStructTemplateInstanceMap mInstances;

	virtual AbstractClassOrStruct* CreateNewSpecialization(	const CPPElementList& inTemplateParameters,
															const UsedTypeOrExpressionList& inTemplateParameterAssignments,
															bool inIsDefinition);
};

