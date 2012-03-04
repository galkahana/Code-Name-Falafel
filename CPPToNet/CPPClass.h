#pragma once

#include "AbstractClassOrStruct.h"
#include "UsedTypeOrExpression.h"
#include <list>
#include <map>
#include <utility>

class CPPClass;
class CPPClassTemplateInstance;

typedef list<CPPClass*> CPPClassList;
typedef pair<CPPClassTemplateInstance*,bool> CPPClassTemplateInstanceAndBool;

typedef map<UsedTypeOrExpressionList,CPPClassTemplateInstance*,UsedTypeOrExpressionListLess> UsedTypeOrExpressionListToCPPClassTemplateInstanceMap;

class CPPClass : public AbstractClassOrStruct
{
public:

	// constructor for class
	CPPClass(const string& inName,
				bool inIsDefinition);
	// constructor for class template
	CPPClass(const string& inName,
			 const CPPElementList& inTemplateParameters,
			 bool inIsDefinition);
	// constructor for class template specialization (partial or full)
	CPPClass(const string& inName,
			 const CPPElementList& inTemplateParameters,
			 const UsedTypeOrExpressionList& inTemplateParametersAssignments,
			 bool inIsDefinition);

	virtual ~CPPClass(void);

	// for tamplate classes
	void AddSpecialization(CPPClass* inSpecialization);
	// if creates a new instance, will take ownership of the parametsrs. return value is the instance, and whether created a new one
	CPPClassTemplateInstanceAndBool QueryInstance(const UsedTypeOrExpressionList& inTemplateParametersAssignments);

private:

	CPPClassList mSpecializations;
	UsedTypeOrExpressionListToCPPClassTemplateInstanceMap mInstances;
};

