#include "CPPTypedef.h"

CPPTypedef::CPPTypedef(const string& inTypedefName,
					   CPPElement* inSubordinateType, 
					   bool inIsConst,
					   bool inIsVolatile):CPPElement(inTypedefName,CPPElement::eCPPElementTypedef)
{
	IsConst = inIsConst;
	IsVolatile = inIsVolatile;
	mSubordinateType = inSubordinateType;
	mSubscriptCount = 0;
}

CPPTypedef::~CPPTypedef(void)
{
}


void CPPTypedef::AppendModifiers(const DeclaratorModifierList& inModifiers)
{
	mModifiers.insert(mModifiers.end(),inModifiers.begin(),inModifiers.end());
}

void CPPTypedef::AddSubscript()
{
	++mSubscriptCount;
}

Hummus::SingleValueContainerIterator<DeclaratorModifierList> CPPTypedef::GetModifiersListIterator()
{
	return Hummus::SingleValueContainerIterator<DeclaratorModifierList>(mModifiers);
}

unsigned long CPPTypedef::GetSubscriptCount()
{
	return mSubscriptCount;
}

CPPElement* CPPTypedef::GetSubordinateType()
{
	return mSubordinateType;
}

