#pragma once

#include "CPPElement.h"
#include "DeclaratorModifier.h"
#include "SingleValueContainerIterator.h"

#include <list>
#include <string>

using namespace std;

typedef list<DeclaratorModifier> DeclaratorModifierList;

class CPPVariable : public CPPElement
{
public:
	CPPVariable(CPPElement* inType, 
				const string& inVariableName,
				bool inIsAuto,
				bool inIsRegister,
				bool inIsExtern,
				bool inIsConst,
				bool inIsVolatile);
	~CPPVariable(void);


	void PushModifier(const DeclaratorModifier& inModifier);
	void AppendModifiers(const DeclaratorModifierList& inModifiers);
	void AddSubscript();

	Hummus::SingleValueContainerIterator<DeclaratorModifierList> GetModifiersListIterator();
	unsigned long GetSubscriptCount();
	CPPElement* GetType();


	bool IsAuto;
	bool IsRegister;
	bool IsExtern;
	bool IsConst;
	bool IsVolatile;

private:
	DeclaratorModifierList mModifiers;
	unsigned long mSubscriptCount;
	CPPElement* mType;
};
