#pragma once
#include "CPPElement.h"
#include "ICPPElementsContainer.h"
#include "ITemplateWithDefault.h"
#include <string>
#include <map>


class CPPTemplateTypename;
class UsedTypeDescriptor;
struct TypedParameter;

using namespace std;

typedef map<string,CPPTemplateTypename*> StringToCPPTemplateTypenameMap;

class CPPTemplateTypename : public CPPElement, public ICPPElementsContainer, public ITemplateWithDefault
{
public:
	CPPTemplateTypename(const string& inTypename,unsigned long inParameterIndex);
	virtual ~CPPTemplateTypename(void);

	// ICPPElementsContainer implementation
	virtual CPPElementList FindElements(const string& inElementName);

	// ITemplateWithDefault implementation
	virtual void SetDefaultTypename(TypedParameter* inDefaultType);

	UsedTypeDescriptor* GetDefault();

	unsigned long GetParameterIndex();

	// checks for equivalency. which is the position in the parent parameters list. and that's all.
	bool IsEqual(CPPTemplateTypename* inOther);
	bool IsLess(CPPTemplateTypename* inOther);
private:

	TypedParameter* mDefaultType;
	unsigned long mParameterIndex;


	StringToCPPTemplateTypenameMap mTypenames;
};

