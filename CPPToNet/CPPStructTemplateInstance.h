#pragma once
#include "CPPElement.h"
#include "ICPPElementsContainer.h"
#include "UsedTypeOrExpression.h"
#include <string>

class CPPStruct;

using namespace std;

class CPPStructTemplateInstance : public CPPElement, public ICPPElementsContainer
{
public:
	CPPStructTemplateInstance(const string& inName,CPPStruct* inTemplate,const UsedTypeOrExpressionList& inInstanceParameters);
	virtual ~CPPStructTemplateInstance(void);

	// ICPPElementsContainer implementation
	virtual CPPElementList FindElements(const string& inElementName);

	CPPStruct* GetSourceTemplate();
	UsedTypeOrExpressionList& GetInstanceParameters();

private:

	CPPStruct* mSourceTamplate;
	UsedTypeOrExpressionList mInstanceParameters;

};

