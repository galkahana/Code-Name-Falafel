#include "VariablesTest.h"
#include "TestsRunner.h"

#include "Trace.h"
#include "HeaderUnit.h"
#include "InputFile.h"
#include "CPPStatementsParser.h"
#include "CPPNamespace.h"
#include "CPPVariable.h"
#include "CPPPrimitive.h"

#include <iostream>

using namespace Hummus;

const string scSamplesBasePath = "C:\\CPPToNetTests\\TestMaterials\\statements\\";
const string scResultBasePath = "C:\\CPPToNetTests\\";

VariablesTest::VariablesTest(void)
{
}


VariablesTest::~VariablesTest(void)
{
}


EStatusCode VariablesTest::Run()
{

	CPPStatementsParser parser;
	InputFile inputFile;
	EStatusCode status = eSuccess;

	Hummus::Singleton<Hummus::Trace>::GetInstance()->SetLogSettings(scResultBasePath + "variablesLog.txt",true,true);

	inputFile.OpenFile(scSamplesBasePath + "variables.h");

	EStatusCodeAndHeaderUnit result = parser.Parse(
										inputFile.GetInputStream(),
										inputFile.GetFilePath(),
										StringToStringMap(),
										StringList()
									);

	if(result.first != eSuccess)
	{
		cout<<"VariablesTest::Run, failed to parse empty source\n";
		return eFailure;
	}

	HeaderUnit* parsedTree = result.second;
	CPPNamespace* globalNamespace = parsedTree->GetGlobalNamespace();

	do
	{
		status = VerifySimplePrimitiveVariables(globalNamespace);
		if(status != eSuccess)
		{
			cout<<"VariablesTest::Run, failed simple variables definition\n";
		}
	}
	while(false);
	
	delete parsedTree;

	Hummus::Singleton<Hummus::Trace>::Reset();
	return status;

}

EStatusCode VariablesTest::VerifySimplePrimitiveVariables(CPPNamespace* inVariablesContainer)
{
	EStatusCode status = eSuccess;
	do
	{
		status = VerifySimpleVariableExistance(inVariablesContainer,eCPPBool,"aBool");
		if(status != eSuccess)
			break;
		status = VerifySimpleVariableExistance(inVariablesContainer,eCPPChar,"aChar");
		if(status != eSuccess)
			break;
		status = VerifySimpleVariableExistance(inVariablesContainer,eCPPUnsignedChar,"aUChar");
		if(status != eSuccess)
			break;
		status = VerifySimpleVariableExistance(inVariablesContainer,eCPPShort,"aShort");
		if(status != eSuccess)
			break;
		status = VerifySimpleVariableExistance(inVariablesContainer,eCPPUnsignedShort,"aUShort");
		if(status != eSuccess)
			break;
		status = VerifySimpleVariableExistance(inVariablesContainer,eCPPUnsignedShort,"aUShort");
		if(status != eSuccess)
			break;
		status = VerifySimpleVariableExistance(inVariablesContainer,eCPPInt,"anInt");
		if(status != eSuccess)
			break;
		status = VerifySimpleVariableExistance(inVariablesContainer,eCPPUnsigned,"anUnsigned");
		if(status != eSuccess)
			break;
		status = VerifySimpleVariableExistance(inVariablesContainer,eCPPWChar,"aWChar_t");
		if(status != eSuccess)
			break;
		status = VerifySimpleVariableExistance(inVariablesContainer,eCPPLong,"aLong");
		if(status != eSuccess)
			break;
		status = VerifySimpleVariableExistance(inVariablesContainer,eCPPUnsignedLong,"aULong");
		if(status != eSuccess)
			break;
		status = VerifySimpleVariableExistance(inVariablesContainer,eCPPLongLong,"aLongLong");
		if(status != eSuccess)
			break;
		status = VerifySimpleVariableExistance(inVariablesContainer,eCPPUnsignedLongLong,"aULongLong");
		if(status != eSuccess)
			break;
		status = VerifySimpleVariableExistance(inVariablesContainer,eCPPVoid,"aPVoid");
		if(status != eSuccess)
			break;
		status = VerifySimpleVariableExistance(inVariablesContainer,eCPPFloat,"aFloat");
		if(status != eSuccess)
			break;
		status = VerifySimpleVariableExistance(inVariablesContainer,eCPPDouble,"aDouble");
		if(status != eSuccess)
			break;
		status = VerifySimpleVariableExistance(inVariablesContainer,eCPPLongDouble,"aLongDouble");
		if(status != eSuccess)
			break;
	}while(false);

	return status;
}

EStatusCode VariablesTest::VerifySimpleVariableExistance(CPPNamespace* inVariablesContainer,
										  ECPPPrimitiveType inPrimitiveType,
										  string inVariableName)
{
	CPPVariable* aVariable = inVariablesContainer->GetVariable(inVariableName);
	if(!aVariable ||
		!aVariable->GetTypeDescriptor()->GetFieldDescriptor() ||
		aVariable->GetTypeDescriptor()->GetFieldDescriptor()->GetType()->Type != CPPElement::eCPPElementPrimitive)
	{
		cout<<"VariablesTest::VerifySimpleVariableExistance, could not find variable of appropriate type for "<<inVariableName<<"\n";
		return eFailure;
	}

	CPPPrimitive* aPrimitive = (CPPPrimitive*)(aVariable->GetTypeDescriptor()->GetFieldDescriptor()->GetType());

	if(aPrimitive->PrimitiveType != inPrimitiveType)
	{
		cout<<"VariablesTest::VerifySimpleVariableExistance, variable "<<inVariableName<<" has type "<<
			scPrimitivesLabels[aPrimitive->PrimitiveType]<<", but supposed to have type "<<
			scPrimitivesLabels[inPrimitiveType]<<"\n";
		return eFailure;
	}

	return eSuccess;
}


ADD_CATEGORIZED_TEST(VariablesTest,"Parser")