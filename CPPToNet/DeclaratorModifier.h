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

	EDeclaratorModifier Modifier;
	bool IsConst;
	bool IsVolatile;
};


typedef list<DeclaratorModifier> DeclaratorModifierList;
