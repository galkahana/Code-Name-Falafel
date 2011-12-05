#include "CPPVariable.h"

CPPVariable::CPPVariable(CPPElement* inType, 
						 const string& inVariableName,
						 bool inIsAuto,
						 bool inIsRegister,
						 bool inIsExtern,
						 bool inIsConst,
						 bool inIsVolatile,
						 bool inIsStatic):CPPElement(inVariableName,CPPElement::eCPPElementVariable)
{
	mType = inType;
	mSubscriptCount = 0;
	IsAuto = inIsAuto;
	IsRegister = inIsRegister;
	IsExtern = inIsExtern;
	IsConst = inIsConst;
	IsVolatile = inIsVolatile;
	IsStatic = inIsStatic;
}

CPPVariable::~CPPVariable(void)
{
}


void CPPVariable::AppendModifiers(const DeclaratorModifierList& inModifiers)
{
	mModifiers.insert(mModifiers.end(),inModifiers.begin(),inModifiers.end());
}

void CPPVariable::AddSubscript()
{
	++mSubscriptCount;
}

Hummus::SingleValueContainerIterator<DeclaratorModifierList> CPPVariable::GetModifiersListIterator()
{
	return Hummus::SingleValueContainerIterator<DeclaratorModifierList>(mModifiers);
}

unsigned long CPPVariable::GetSubscriptCount()
{
	return mSubscriptCount;
}

CPPElement* CPPVariable::GetType()
{
	return mType;
}

