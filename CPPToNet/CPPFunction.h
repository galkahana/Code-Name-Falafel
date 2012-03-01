#pragma once

#include "CPPElement.h"
#include "SingleValueContainerIterator.h"
#include "MapIterator.h"
#include "TypedParameter.h"
#include "UsedTypeOrExpression.h"

#include <list>
#include <string>

class UsedTypeDescriptor;

/*
	Represents both a function and a function template.
	The difference in them is with the existance of template parameters

*/

using namespace std;

typedef list<CPPElement*> CPPElementList;

class CPPFunction : public CPPElement
{
public:
	// constructor for function
	CPPFunction(const string& inFunctionName,
				bool inIsVirtual,
				bool inIsStatic,
				UsedTypeDescriptor* inReturnType,
				const TypedParameterList& inParametersList,
				bool inHasElipsis,
				bool inIsPure,
				bool inIsDefinition);

	//constructor for function template
	CPPFunction(const string& inFunctionName,
				bool inIsVirtual,
				bool inIsStatic,
				UsedTypeDescriptor* inReturnType,
				const TypedParameterList& inParametersList,
				bool inHasElipsis,
				bool inIsPure,
				bool inIsDefinition,
				const CPPElementList& inTemplateParameters);
	// constructor for function template specialization
	CPPFunction(const string& inFunctionName,
				bool inIsVirtual,
				bool inIsStatic,
				UsedTypeDescriptor* inReturnType,
				const TypedParameterList& inParametersList,
				bool inHasElipsis,
				bool inIsPure,
				bool inIsDefinition,
				const CPPElementList& inTemplateParameters,
				const UsedTypeOrExpressionList& inTemplateSpecializationParameters);

	// constructor for function template instantiation
	CPPFunction(const string& inFunctionName,
				bool inIsVirtual,
				bool inIsStatic,
				UsedTypeDescriptor* inReturnType,
				const TypedParameterList& inParametersList,
				bool inHasElipsis,
				bool inIsPure,
				bool inIsDefinition,
				const UsedTypeOrExpressionList& inTemplateSpecializationParameters);
	~CPPFunction(void);


	UsedTypeDescriptor* GetReturnType();
	TypedParameterList& GetDeclaredParameterList();
	Hummus::SingleValueContainerIterator<TypedParameterList> GetParametersListIterator();
	bool HasElipsis();
	bool IsDefinition();
	bool IsVirtual();
	bool IsStatic();
	bool IsPure();

	// tempalte functions functionalities
	bool IsFunctionTemplate();
	bool IsTemplateInsantiation();
	Hummus::SingleValueContainerIterator<CPPElementList> GetFunctionTemplateParametersIterator();
	CPPElementList& GetFunctionTemplateParameters();
	Hummus::SingleValueContainerIterator<UsedTypeOrExpressionList> GetFunctionTemplateSpecializationParametersIterator();
	UsedTypeOrExpressionList& GetFunctionTemplateSpecializationParametrs();

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
	bool mIsTemplate;
	bool mIsTemplateInsantiation;
	CPPElementList mTemplateParameters;
	UsedTypeOrExpressionList mTemplateSpecializationParameters;

};
