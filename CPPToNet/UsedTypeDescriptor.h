#pragma once
#include "DeclaratorModifier.h"
#include "SingleValueContainerIterator.h"
#include "ICPPDeclerator.h"
#include "ICPPFunctionPointerDeclerator.h"
#include <list>

using namespace std;


class CPPElement;
class UsedTypeDescriptor;

typedef list<DeclaratorModifier> DeclaratorModifierList;


class FieldTypeDescriptor : public ICPPDeclerator
{
public:

	FieldTypeDescriptor(CPPElement* inType, 
				bool inIsAuto,
				bool inIsRegister,
				bool inIsExtern,
				bool inIsConst,
				bool inIsVolatile,
				bool inIsStatic);

	bool IsAuto;
	bool IsRegister;
	bool IsExtern;
	bool IsConst;
	bool IsVolatile;
	bool IsStatic;


	// ICPPDeclerator implemenetation
	virtual void AppendModifiers(const DeclaratorModifierList& inModifiers);
	virtual void AddSubscript();

	Hummus::SingleValueContainerIterator<DeclaratorModifierList> GetModifiersListIterator();
	unsigned long GetSubscriptCount();
	CPPElement* GetType();

private:
	DeclaratorModifierList mModifiers;
	unsigned long mSubscriptCount;
	CPPElement* mType;

};

struct FunctionParameter
{
	FunctionParameter();
	~FunctionParameter();

	string Name;
	UsedTypeDescriptor* Type;
};

typedef list<FunctionParameter*> FunctionParameterList;

class FunctionPointerTypeDescriptor : public ICPPFunctionPointerDeclerator
{
public:
	FunctionPointerTypeDescriptor(UsedTypeDescriptor* inReturnType);
	~FunctionPointerTypeDescriptor();

	virtual void SetFunctionPointerType(ICPPFunctionPointerDeclerator::EFunctionPointerType inFunctionPointerType);
	virtual void AppendModifiersForReturnType(const DeclaratorModifierList& inModifiers);
	virtual void SetHasElipsis();
	virtual FunctionParameter* CreateParameter(const string& inParameterName,
												UsedTypeDescriptor* inParameterType);

	ICPPFunctionPointerDeclerator::EFunctionPointerType GetPointerType(); 
	UsedTypeDescriptor* GetReturnType();
	Hummus::SingleValueContainerIterator<FunctionParameterList> GetParametersListIterator();
	bool HasElipsis();
	

private:
	ICPPFunctionPointerDeclerator::EFunctionPointerType mPointerType;
	UsedTypeDescriptor* mReturnType;
	FunctionParameterList mDeclaredParameters;
	bool mHasElipsis;
};

class UsedTypeDescriptor
{
public:

	// Field oriented constructor
	UsedTypeDescriptor(CPPElement* inType, 
				bool inIsAuto,
				bool inIsRegister,
				bool inIsExtern,
				bool inIsConst,
				bool inIsVolatile,
				bool inIsStatic);
	// Function pointer constructor
	UsedTypeDescriptor(UsedTypeDescriptor* inReturnType);

	~UsedTypeDescriptor(void);


	enum EUsedType
	{
		eUsedTypeField,
		eUsedTypeFunctionPointer,
		eUsedTypeUndefined
	};

	EUsedType GetUsedType();

	FieldTypeDescriptor* GetFieldDescriptor();
	FunctionPointerTypeDescriptor* GetFunctionPointerDescriptor();

private:
	EUsedType mUsedType;

	FieldTypeDescriptor* mFieldDescriptor;
	FunctionPointerTypeDescriptor* mFunctionPointerDescriptor;

};
