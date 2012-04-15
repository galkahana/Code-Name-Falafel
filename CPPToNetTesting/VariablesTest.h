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
													  ECPPPrimitiveType inPrimitiveType,
													  string inVariableName);
};

