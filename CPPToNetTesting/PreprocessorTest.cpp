#include "PreprocessorTest.h"
#include "PreProcessor.h"
#include "TestsRunner.h"
#include "SimpleFileComparer.h"
#include "IByteWriterWithPosition.h"
#include "InputFile.h"
#include "OutputFile.h"

#include <iostream>

using namespace Hummus;

const string scSamplesBasePath = "C:\\CPPToNetTests\\TestMaterials\\preprocessor\\";
const string scResultsBasePath = "C:\\CPPToNetTests\\";

PreprocessorTest::PreprocessorTest(void)
{
	mCurrentStream = NULL;
}

PreprocessorTest::~PreprocessorTest(void)
{
}

EStatusCode PreprocessorTest::Run()
{
	EStatusCode status = eSuccess;

	// simple test (no pre preprocessor)
	if(RunTest("Simple",
						scSamplesBasePath + "SimplePreprocessor.h", 
						scResultsBasePath + "SimplePreprocessorResult.h",
						scSamplesBasePath + "SimplePreprocessorCompare.h") != eSuccess)
		status = eFailure;

	// include (including not found)
	if(RunTest("Include",
						scSamplesBasePath + "SimpleWithInclude.h", 
						scResultsBasePath + "SimpleWithIncludeResult.h",
						scSamplesBasePath + "SimpleWithIncludeCompare.h") != eSuccess)
		status = eFailure;

	// this one has an include command for an non found file. processor should stop at point of include
	if(RunTest("IncludeFail",
						scSamplesBasePath + "SimpleWithIncludeFail.h", 
						scResultsBasePath + "SimpleWithIncludeFailResult.h",
						scSamplesBasePath + "SimpleWithIncludeFailCompare.h") != eSuccess)
		status = eFailure;

	// include + pragma once
	if(RunTest("Pragma",
						scSamplesBasePath + "SimpleWithIncludePragma.h", 
						scResultsBasePath + "SimpleWithIncludePragmaResult.h",
						scSamplesBasePath + "SimpleWithIncludePragmaCompare.h") != eSuccess)
		status = eFailure;

	// macro definition - simple constants
	// using predefined macros - line, file
	if(RunTest("Simple Defines",
						scSamplesBasePath + "SimpleDefines.h", 
						scResultsBasePath + "SimpleDefinesResult.h",
						scSamplesBasePath + "SimpleDefinesCompare.h") != eSuccess)
		status = eFailure;
	// macro definition - functions and nested defines
	if(RunTest("Function Defines",
						scSamplesBasePath + "FunctionDefines.h", 
						scResultsBasePath + "FunctionDefinesResult.h",
						scSamplesBasePath + "FunctionDefinesCompare.h") != eSuccess)
		status = eFailure;

	// ifdef/ifndef/else/endif with defined and predefined symbols
	if(RunTest("IfDef",
						scSamplesBasePath + "Ifdefs.h", 
						scResultsBasePath + "IfdefsResult.h",
						scSamplesBasePath + "IfdefsCompare.h") != eSuccess)
		status = eFailure;


	// if/elif/else/endif with defined and predefined symbols (same as ifdef/ifndef but with the defined operator)
	if(RunTest("IfDefWithIf",
						scSamplesBasePath + "IfdefsWithIf.h", 
						scResultsBasePath + "IfdefsWithIfResult.h",
						scSamplesBasePath + "IfdefsCompare.h") != eSuccess)
		status = eFailure;

	// if/elif/else/endif - simple conditions (simple compares and bools and ints)
	if(RunTest("IfDefWithIf Simple Compare",
						scSamplesBasePath + "IfdefsWithISimpleCompares.h", 
						scResultsBasePath + "IfdefsWithISimpleComparesResult.h",
						scSamplesBasePath + "IfdefsCompare.h") != eSuccess)
		status = eFailure;

	// if/elif/else/endif - arithmetic complexers
	if(RunTest("IfDefWithIf Complex Compare",
						scSamplesBasePath + "IfdefsWithComplexCompare.h", 
						scResultsBasePath + "IfdefsWithComplexCompareResult.h",
						scSamplesBasePath + "IfdefsCompare.h") != eSuccess)
		status = eFailure;
	



	return status;
}

EStatusCode PreprocessorTest::RunTest(const string& inName, const string& inOriginalFile, const string& inOutputFile, const string& inComparisonFile)
{
	EStatusCode status = eSuccess;

	StringList preprocessorDefinitions;
	StringList includeFolders;

	includeFolders.push_back(scSamplesBasePath);
	preprocessorDefinitions.push_back("PREDEFINED_SYMBOL");

	InputFile sourceFile;
	sourceFile.OpenFile(inOriginalFile);

	OutputFile outputFile;
	outputFile.OpenFile(inOutputFile);
	mCurrentStream = outputFile.GetOutputStream();

	PreProcessor preProcessor;

	preProcessor.Setup(sourceFile.GetInputStream(),inOriginalFile,preprocessorDefinitions,includeFolders);

	preProcessor.AddListener(this);

	mStartRow = true;
	BoolAndString tokenizerResult = preProcessor.GetNextToken();
	while(tokenizerResult.first)
	{
		if(!mStartRow)
			mCurrentStream->Write((const Byte*)"  ",2); // 2 spaces, so we can clearly distinct tokens
		mCurrentStream->Write((const Byte*)tokenizerResult.second.c_str(),tokenizerResult.second.size());

		mStartRow = false;

		tokenizerResult = preProcessor.GetNextToken();
	}

	sourceFile.CloseFile();
	outputFile.CloseFile();

	mCurrentStream = NULL;
	
	SimpleFileComparer comparer;

	if(!comparer.Same(inOutputFile,inComparisonFile))
	{
		cout<<"TokenizerTest::Run, failed in test named "<<inName<<". see result in "<<inOutputFile<<" and compare with the required result in "<<inComparisonFile<<"\n";
		status = eFailure;
	}

	return status;	

}

void PreprocessorTest::OnNewLine(const string& inNewLineString)
{
	mStartRow = true;
	if(mCurrentStream)
		mCurrentStream->Write((const Byte*)inNewLineString.c_str(),inNewLineString.size());
}

ADD_CATEGORIZED_TEST(PreprocessorTest,"Parser")