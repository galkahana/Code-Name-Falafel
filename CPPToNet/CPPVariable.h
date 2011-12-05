#pragma once

#include "CPPElement.h"
#include "SingleValueContainerIterator.h"
#include "ICPPDeclerator.h"

#include <list>

using namespace std;

class CPPVariable : public CPPElement, public ICPPDeclerator
{
public:
	CPPVariable(CPPElement* inType, 
				const string& inVariableName,
				bool inIsAuto,
				bool inIsRegister,
				bool inIsExtern,
				bool inIsConst,
				bool inIsVolatile,
				bool inIsStatic);
	~CPPVariable(void);

	// ICPPDeclerator implementation
	virtual void AppendModifiers(const DeclaratorModifierList& inModifiers);
	virtual void AddSubscript();

	Hummus::SingleValueContainerIterator<DeclaratorModifierList> GetModifiersListIterator();
	unsigned long GetSubscriptCount();
	CPPElement* GetType();


	bool IsAuto;
	bool IsRegister;
	bool IsExtern;
	bool IsConst;
	bool IsVolatile;
	bool IsStatic;

private:
	DeclaratorModifierList mModifiers;
	unsigned long mSubscriptCount;
	CPPElement* mType;
};
