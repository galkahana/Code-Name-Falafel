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
		cout<<"VariablesTest::Run, failed to parse source\n";
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
		status = VerifySimpleVariableExistance(inVariablesContainer,"aBool",eCPPBool);
		if(status != eSuccess)
			break;
		status = VerifySimpleVariableExistance(inVariablesContainer,"aChar",eCPPChar);
		if(status != eSuccess)
			break;
		status = VerifySimpleVariableExistance(inVariablesContainer,"aUChar",eCPPUnsignedChar);
		if(status != eSuccess)
			break;
		status = VerifySimpleVariableExistance(inVariablesContainer,"aShort",eCPPShort);
		if(status != eSuccess)
			break;
		status = VerifySimpleVariableExistance(inVariablesContainer,"aUShort",eCPPUnsignedShort);
		if(status != eSuccess)
			break;
		status = VerifySimpleVariableExistance(inVariablesContainer,"aUShort",eCPPUnsignedShort);
		if(status != eSuccess)
			break;
		status = VerifySimpleVariableExistance(inVariablesContainer,"anInt",eCPPInt);
		if(status != eSuccess)
			break;
		status = VerifySimpleVariableExistance(inVariablesContainer,"anUnsigned",eCPPUnsigned);
		if(status != eSuccess)
			break;
		status = VerifySimpleVariableExistance(inVariablesContainer,"aWChar_t",eCPPWChar);
		if(status != eSuccess)
			break;
		status = VerifySimpleVariableExistance(inVariablesContainer,"aLong",eCPPLong);
		if(status != eSuccess)
			break;
		status = VerifySimpleVariableExistance(inVariablesContainer,"aULong",eCPPUnsignedLong);
		if(status != eSuccess)
			break;
		status = VerifySimpleVariableExistance(inVariablesContainer,"aLongLong",eCPPLongLong);
		if(status != eSuccess)
			break;
		status = VerifySimpleVariableExistance(inVariablesContainer,"aULongLong",eCPPUnsignedLongLong);
		if(status != eSuccess)
			break;
		status = VerifySimpleVariableExistance(inVariablesContainer,"aPVoid",eCPPVoid);
		if(status != eSuccess)
			break;
		status = VerifySimpleVariableExistance(inVariablesContainer,"aFloat",eCPPFloat);
		if(status != eSuccess)
			break;
		status = VerifySimpleVariableExistance(inVariablesContainer,"aDouble",eCPPDouble);
		if(status != eSuccess)
			break;
		status = VerifySimpleVariableExistance(inVariablesContainer,"aLongDouble",eCPPLongDouble);
		if(status != eSuccess)
			break;
		status = VerifyStorageVariableExistance(inVariablesContainer,"aStaticConstInt",eCPPInt,false,true,true,false,false,false);
		if(status != eSuccess)
			break;
		status = VerifyStorageVariableExistance(inVariablesContainer,"aStaticInt",eCPPInt,false,true,false,false,false,false);
		if(status != eSuccess)
			break;
		status = VerifyStorageVariableExistance(inVariablesContainer,"aConstIntPointer",eCPPInt,false,false,true,false,false,false);
		if(status != eSuccess)
			break;
		status = VerifyStorageVariableExistance(inVariablesContainer,"aVolatileFloat",eCPPFloat,false,false,false,true,false,false);
		if(status != eSuccess)
			break;
		status = VerifyStorageVariableExistance(inVariablesContainer,"anExternUShort",eCPPUnsignedShort,false,false,false,false,false,true);
		if(status != eSuccess)
			break;
	}while(false);

	return status;
}

EStatusCode VariablesTest::VerifySimpleVariableExistance(CPPNamespace* inVariablesContainer,
										  string inVariableName,
										  ECPPPrimitiveType inPrimitiveType)
{
	return VerifyStorageVariableExistance(inVariablesContainer,inVariableName,inPrimitiveType,false,false,false,false,false,false);
}

Hummus::EStatusCode VariablesTest::VerifyStorageVariableExistance(CPPNamespace* inVariablesContainer,
											string inVariableName,
											ECPPPrimitiveType inPrimitiveType,
											bool inAuto,
											bool inStatic,
											bool inConst,
											bool inVolatile,
											bool inRegister,
											bool inExtern)
{
	CPPVariable* aVariable = inVariablesContainer->GetVariable(inVariableName);
	if(!aVariable ||
		!aVariable->GetTypeDescriptor()->GetFieldDescriptor() ||
		aVariable->GetTypeDescriptor()->GetFieldDescriptor()->GetType()->Type != CPPElement::eCPPElementPrimitive)
	{
		cout<<"VariablesTest::VerifySimpleVariableExistance, could not find variable of appropriate type for "<<inVariableName<<"\n";
		return eFailure;
	}

	FieldTypeDescriptor* fieldTypeDescriptor = aVariable->GetTypeDescriptor()->GetFieldDescriptor();

	CPPPrimitive* aPrimitive = (CPPPrimitive*)(fieldTypeDescriptor->GetType());

	if(aPrimitive->PrimitiveType != inPrimitiveType)
	{
		cout<<"VariablesTest::VerifySimpleVariableExistance, variable "<<inVariableName<<" has type "<<
			scPrimitivesLabels[aPrimitive->PrimitiveType]<<", but supposed to have type "<<
			scPrimitivesLabels[inPrimitiveType]<<"\n";
		return eFailure;
	}

	if(fieldTypeDescriptor->IsAuto != inAuto)
	{
		cout<<"VariablesTest::VerifySimpleVariableExistance, Auto supposed to be "<<inAuto<<" but is "<<fieldTypeDescriptor->IsAuto<<"\n";
		return eFailure;
	}

	if(fieldTypeDescriptor->IsStatic != inStatic)
	{
		cout<<"VariablesTest::VerifySimpleVariableExistance, Static supposed to be "<<inStatic<<" but is "<<fieldTypeDescriptor->IsStatic<<"\n";
		return eFailure;
	}

	if(fieldTypeDescriptor->IsConst != inConst)
	{
		cout<<"VariablesTest::VerifySimpleVariableExistance, Const supposed to be "<<inConst<<" but is "<<fieldTypeDescriptor->IsConst<<"\n";
		return eFailure;
	}

	if(fieldTypeDescriptor->IsVolatile != inVolatile)
	{
		cout<<"VariablesTest::VerifySimpleVariableExistance, Volatile supposed to be "<<inVolatile<<" but is "<<fieldTypeDescriptor->IsVolatile<<"\n";
		return eFailure;
	}

	if(fieldTypeDescriptor->IsRegister != inRegister)
	{
		cout<<"VariablesTest::VerifySimpleVariableExistance, Register supposed to be "<<inRegister<<" but is "<<fieldTypeDescriptor->IsRegister<<"\n";
		return eFailure;
	}

	if(fieldTypeDescriptor->IsExtern != inExtern)
	{
		cout<<"VariablesTest::VerifySimpleVariableExistance, Extern supposed to be "<<inExtern<<" but is "<<fieldTypeDescriptor->IsExtern<<"\n";
		return eFailure;
	}
	return eSuccess;
}

ADD_CATEGORIZED_TEST(VariablesTest,"Parser")