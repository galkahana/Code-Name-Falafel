#pragma once

#include "CPPElement.h"
#include "SingleValueContainerIterator.h"
#include "ICPPDeclerator.h"

#include <list>

using namespace std;

class CPPTypedef : public CPPElement, public ICPPDeclerator
{
public:
	CPPTypedef(	const string& inTypedefName,
				CPPElement* inSubordinateType, 
				bool inIsConst,
				bool inIsVolatile);
	~CPPTypedef(void);

	// ICPPDeclerator implementation
	virtual void AppendModifiers(const DeclaratorModifierList& inModifiers);
	virtual void AddSubscript();

	Hummus::SingleValueContainerIterator<DeclaratorModifierList> GetModifiersListIterator();
	unsigned long GetSubscriptCount();
	CPPElement* GetSubordinateType();

	bool IsConst;
	bool IsVolatile;

private:
	DeclaratorModifierList mModifiers;
	unsigned long mSubscriptCount;
	CPPElement* mSubordinateType;
};
