#pragma once

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