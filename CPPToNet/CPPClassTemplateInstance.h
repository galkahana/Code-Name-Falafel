#pragma once
#include "CPPElement.h"
#include "ICPPElementsContainer.h"
#include "UsedTypeOrExpression.h"
#include <string>

class CPPClass;

using namespace std;

class CPPClassTemplateInstance : public CPPElement, public ICPPElementsContainer
{
public:
	CPPClassTemplateInstance(const string& inName,CPPClass* inTemplate,const UsedTypeOrExpressionList& inInstanceParameters);
	virtual ~CPPClassTemplateInstance(void);

	// ICPPElementsContainer implementation
	virtual CPPElementList FindElements(const string& inElementName);

	CPPClass* GetSourceTemplate();
	UsedTypeOrExpressionList& GetInstanceParameters();

private:

	CPPClass* mSourceTamplate;
	UsedTypeOrExpressionList mInstanceParameters;

};

