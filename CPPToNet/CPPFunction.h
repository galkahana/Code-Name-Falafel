#pragma once

#include "CPPElement.h"
#include "SingleValueContainerIterator.h"
#include "TypedParameter.h"

class UsedTypeDescriptor;

class CPPFunction : public CPPElement
{
public:
	CPPFunction(const string& inFunctionName,
				bool inIsVirtual,
				bool inIsStatic,
				UsedTypeDescriptor* inReturnType,
				const TypedParameterList& inParametersList,
				bool inHasElipsis,
				bool inIsPure,
				bool inIsDefinition);
	~CPPFunction(void);


	UsedTypeDescriptor* GetReturnType();
	TypedParameterList& GetDeclaredParameterList();
	Hummus::SingleValueContainerIterator<TypedParameterList> GetParametersListIterator();
	bool HasElipsis();
	bool IsDefinition();
	bool IsVirtual();
	bool IsStatic();
	bool IsPure();

	// update function flag, when function defintion is found
	void SetIsDefinition();

private:

	UsedTypeDescriptor* mReturnType;
	TypedParameterList mDeclaredParameters;
	bool mHasElipsis;
	bool mIsDefinition;
	bool mIsVirtual;
	bool mIsStatic;
	bool mIsPure;

};
