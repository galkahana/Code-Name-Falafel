#pragma once

#include "CPPElement.h"
#include "SingleValueContainerIterator.h"
#include "ICPPElementsContainer.h"
#include "ITemplateWithDefault.h"

#include <map>
#include <string>
#include <list>


using namespace std;

class CPPTemplateTypename;
class UsedTypeDescriptor;
struct TypedParameter;

typedef list<CPPElement*> CPPElementList;
typedef map<string,CPPTemplateTypename*> StringToCPPTemplateTypenameMap;

class CPPTemplateTemplateParameter : public CPPElement, public ICPPElementsContainer, public ITemplateWithDefault
{
public:
	CPPTemplateTemplateParameter(const string& inName,unsigned long inParameterIndex,CPPElementList& inTemplateParameters);
	virtual ~CPPTemplateTemplateParameter(void);

	// ICPPElementsContainer implementation
	virtual CPPElementList FindElements(const string& inElementName);

	// ITemplateWithDefault implementation
	virtual void SetDefaultTypename(TypedParameter* inDefaultType);

	UsedTypeDescriptor* GetDefault();

	Hummus::SingleValueContainerIterator<CPPElementList> GetParametersIterator();
	CPPElementList& GetTemplateParameters();

	unsigned long GetParameterIndex();

	// checks for equivalency. which is the position in the parent parameters list, and the equivalency of the template parameters
	bool IsEqual(CPPTemplateTemplateParameter* inOther);
	bool IsLess(CPPTemplateTemplateParameter* inOther);

private:
	CPPElementList mTemplateParameters;
	unsigned long mParameterIndex;

	TypedParameter* mDefaultType;


	StringToCPPTemplateTypenameMap mTypenames;
};

