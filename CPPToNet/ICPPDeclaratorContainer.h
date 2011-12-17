#pragma once
#include "EStatusCode.h"

#include <string>

class CPPElement;
class ICPPDeclerator;
class ICPPFunctionPointerDeclerator;

using namespace std;

class ICPPDeclaratorContainer
{
	public:

		virtual ~ICPPDeclaratorContainer(){}

		virtual Hummus::EStatusCode SetFlags(CPPElement* inType,bool inIsAuto,bool inIsRegister,bool inIsExtern,bool inIsConst,bool inIsVolatile, bool inIsStatic) = 0;
		virtual ICPPDeclerator* AddDeclarator(const string& inDecleratorName) = 0;
		virtual ICPPFunctionPointerDeclerator* AddFunctionPointerDeclarator(const string& inDecleratorName) = 0;
		virtual bool VerifyDeclaratorStopper(const string& inTokenToExamine) = 0;

};