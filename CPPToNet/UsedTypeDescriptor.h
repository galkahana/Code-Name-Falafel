#pragma once
#include "DeclaratorModifier.h"
#include "TypedParameter.h"
#include "SingleValueContainerIterator.h"
#include "ICPPFieldDeclerator.h"
#include "ICPPFunctionPointerDeclerator.h"
#include <list>

using namespace std;


class CPPElement;
class UsedTypeDescriptor;

typedef list<DeclaratorModifier> DeclaratorModifierList;


class FieldTypeDescriptor
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


	void AppendModifier(const DeclaratorModifier& inModifier);
	void AppendModifiers(const DeclaratorModifierList& inModifiers);
	void AddSubscript();
	void RemoveSubscript();

	size_t GetModifiersCount();
	Hummus::SingleValueContainerIterator<DeclaratorModifierList> GetModifiersListIterator();
	unsigned long GetSubscriptCount();
	CPPElement* GetType();

	bool IsEqual(FieldTypeDescriptor* inOtherDescriptor);
	FieldTypeDescriptor* Clone();

private:
	DeclaratorModifierList mModifiers;
	unsigned long mSubscriptCount;
	CPPElement* mType;

};

class FunctionPointerTypeDescriptor
{
public:
	FunctionPointerTypeDescriptor(UsedTypeDescriptor* inReturnType);
	~FunctionPointerTypeDescriptor();

	void SetFunctionPointerType(ICPPFunctionPointerDeclerator::EFunctionPointerType inFunctionPointerType);
	void AppendModifiersForFunctionPointerReturnType(const DeclaratorModifierList& inModifiers);
	void SetFunctionPointerHasElipsis();
	TypedParameter* CreateParameter(const string& inParameterName, UsedTypeDescriptor* inParameterType);

	ICPPFunctionPointerDeclerator::EFunctionPointerType GetPointerType(); 
	UsedTypeDescriptor* GetReturnType();
	Hummus::SingleValueContainerIterator<TypedParameterList> GetParametersListIterator();
	bool HasElipsis();
	
	bool IsEqual(FunctionPointerTypeDescriptor* inOther);
	FunctionPointerTypeDescriptor* Clone();

private:
	ICPPFunctionPointerDeclerator::EFunctionPointerType mPointerType;
	UsedTypeDescriptor* mReturnType;
	TypedParameterList mDeclaredParameters;
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

	bool IsEqual(UsedTypeDescriptor* inOtherUsedType);

	UsedTypeDescriptor* Clone();

private:
	UsedTypeDescriptor();

	EUsedType mUsedType;

	FieldTypeDescriptor* mFieldDescriptor;
	FunctionPointerTypeDescriptor* mFunctionPointerDescriptor;

};
