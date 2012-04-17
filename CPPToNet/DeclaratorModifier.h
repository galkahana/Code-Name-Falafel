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
	DeclaratorModifier(EDeclaratorModifier inModifier,bool inConst, bool inVolatile)
	{
		Modifier = inModifier;
		IsConst = inConst;
		IsVolatile = inVolatile;
	}

	bool IsEqual(const DeclaratorModifier& inOther) const
	{
		if(Modifier != inOther.Modifier)
			return false;

		if(IsConst != inOther.IsConst)
			return false;

		return IsVolatile == inOther.IsVolatile;
	}

	bool IsLess(const DeclaratorModifier& inOther)
	{
		if(Modifier != inOther.Modifier)
			return Modifier < inOther.Modifier;

		if(IsConst != inOther.IsConst)
			return !IsConst;

		if(IsVolatile == inOther.IsVolatile)
			return false;
		else
			return !IsVolatile;
	}

	EDeclaratorModifier Modifier;
	bool IsConst;
	bool IsVolatile;
};


typedef list<DeclaratorModifier> DeclaratorModifierList;
