#pragma once
#include "CPPElement.h"

class UsedTypeDescriptor;
struct TypedParameter;

class CPPTemplateTypename : public CPPElement
{
public:
	CPPTemplateTypename(const string& inTypename);
	virtual ~CPPTemplateTypename(void);

	void SetDefaultTypename(TypedParameter* inDefaultType);
	UsedTypeDescriptor* GetDefault();

private:

	TypedParameter* mDefaultType;

};

