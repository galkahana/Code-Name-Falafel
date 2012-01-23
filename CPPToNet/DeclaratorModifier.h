#pragma once
#include <list>

using namespace std;

struct DeclaratorModifier
{
	enum EDeclaratorModifier
	{
		eDeclaratorModifierPointer,
		eDeclaratorModifierReference
	};

	DeclaratorModifier(){Modifier = eDeclaratorModifierPointer;IsConst = false;IsVolatile = false;}

	bool IsEqual(const DeclaratorModifier& inOther)
	{
		if(Modifier != inOther.Modifier)
			return false;

		if(IsConst != inOther.IsConst)
			return false;

		return IsVolatile == inOther.IsVolatile;
	}

	EDeclaratorModifier Modifier;
	bool IsConst;
	bool IsVolatile;
};


typedef list<DeclaratorModifier> DeclaratorModifierList;
