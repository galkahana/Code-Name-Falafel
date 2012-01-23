#pragma once

#include "CPPElement.h"
#include "SingleValueContainerIterator.h"
#include "FunctionParameter.h"

class UsedTypeDescriptor;

class CPPFunction : public CPPElement
{
public:
	CPPFunction(const string& inFunctionName,
				UsedTypeDescriptor* inReturnType,
				const FunctionParameterList& inParametersList,
				bool inHasElipsis,
				bool inIsDefinition);
	~CPPFunction(void);


	UsedTypeDescriptor* GetReturnType();
	FunctionParameterList& GetDeclaredParameterList();
	Hummus::SingleValueContainerIterator<FunctionParameterList> GetParametersListIterator();
	bool HasElipsis();
	bool IsDefinition();

	// update function flag, when function defintion is found
	void SetIsDefinition();

private:

	UsedTypeDescriptor* mReturnType;
	FunctionParameterList mDeclaredParameters;
	bool mHasElipsis;
	bool mIsDefinition;

};
