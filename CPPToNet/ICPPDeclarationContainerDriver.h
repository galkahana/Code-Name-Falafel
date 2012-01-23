#pragma once
#include "EStatusCode.h"

#include <string>

class CPPElement;
class ICPPFieldDeclerator;
class ICPPFunctionPointerDeclerator;
class ICPPFunctionDefinitionDeclerator;

using namespace std;

class ICPPDeclarationContainerDriver
{
	public:

		virtual ~ICPPDeclarationContainerDriver(){}

		virtual Hummus::EStatusCode SetFlags(CPPElement* inType,bool inIsAuto,bool inIsRegister,bool inIsExtern,bool inIsConst,bool inIsVolatile, bool inIsStatic) = 0;
		virtual ICPPFieldDeclerator* AddFieldDeclerator(const string& inDecleratorName) = 0;
		virtual ICPPFunctionPointerDeclerator* AddFunctionPointerDeclarator(const string& inDecleratorName) = 0;
		virtual ICPPFunctionDefinitionDeclerator* AddFunctionDefinition(const string& inFunctionName) = 0;
		virtual bool VerifyDeclaratorStopper(const string& inTokenToExamine) = 0;

};