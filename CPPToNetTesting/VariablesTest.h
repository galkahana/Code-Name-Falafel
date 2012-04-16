#pragma once

#include "ITestUnit.h"
#include "CPPPrimitiveTypes.h"
#include <string>

class CPPNamespace;

using namespace std;

class VariablesTest : public ITestUnit
{
public:
	VariablesTest(void);
	~VariablesTest(void);

	// ITestUnit interface
	virtual Hummus::EStatusCode Run();

private:
	Hummus::EStatusCode VerifySimplePrimitiveVariables(CPPNamespace* inVariablesContainer);
	Hummus::EStatusCode VerifySimpleVariableExistance(CPPNamespace* inVariablesContainer,
													  string inVariableName,
													  ECPPPrimitiveType inPrimitiveType);
	Hummus::EStatusCode VerifyStorageVariableExistance(CPPNamespace* inVariablesContainer,
												string inVariableName,
												ECPPPrimitiveType inPrimitiveType,
												bool inAuto,
												bool inStatic,
												bool inConst,
												bool inVolatile,
												bool inRegister,
												bool inExtern);
};

