#pragma once

#include "ITestUnit.h"
#include "CPPPrimitiveTypes.h"
#include "UsedTypeDescriptor.h"
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
													  const string& inVariableName,
													  ECPPPrimitiveType inPrimitiveType);
	Hummus::EStatusCode VerifySpecialStorageVariables(CPPNamespace* inVariablesContainer);
	Hummus::EStatusCode VerifyStorageVariableExistance(CPPNamespace* inVariablesContainer,
												const string& inVariableName,
												ECPPPrimitiveType inPrimitiveType,
												bool inAuto,
												bool inStatic,
												bool inConst,
												bool inVolatile,
												bool inRegister,
												bool inExtern);
	Hummus::EStatusCode VerifyPointerArraysAndInitializers(CPPNamespace* inVariablesContainer);
	bool IsEqualFieldType(CPPNamespace* inVariablesContainer,
						  const string& inVariableName,
						  const FieldTypeDescriptor& inCompareType);
};

