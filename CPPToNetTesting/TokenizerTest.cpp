#include "TokenizerTest.h"
#include "TestsRunner.h"
#include "CPPTokenizer.h"
#include "PreTokenizerDecoder.h"
#include "InputFile.h"
#include "OutputFile.h"
#include "IByteWriterWithPosition.h"
#include "SimpleFileComparer.h"

#include <iostream>

using namespace Hummus;

TokenizerTest::TokenizerTest(void)
{
}

TokenizerTest::~TokenizerTest(void)
{
}

const string scSamplesBasePath = "C:\\CPPToNetTests\\TestMaterials\\";
const string scResultsBasePath = "C:\\CPPToNetTests\\";

EStatusCode TokenizerTest::Run()
{

	EStatusCode status = eSuccess;

	InputFile encodedFile;
	PreTokenizerDecoder decoder;
	CPPTokenizer tokenizer;

	encodedFile.OpenFile(scSamplesBasePath + "PreTokenizerDecoderSample.h");
	decoder.SetStream(encodedFile.GetInputStream());
	tokenizer.SetReadStream(&decoder);


	OutputFile tokensOutput;
	tokensOutput.OpenFile(scResultsBasePath + "PreTokenizerDecoderSampleTokenized.h");

	BoolAndString tokenizerResult = tokenizer.GetNextToken();
	bool startRow = true;
	while(tokenizerResult.first)
	{
		if(!startRow && !IsNewLine(tokenizerResult.second))
			tokensOutput.GetOutputStream()->Write((const Byte*)"  ",2);
		tokensOutput.GetOutputStream()->Write((const Byte*)tokenizerResult.second.c_str(),tokenizerResult.second.size());

		startRow = IsNewLine(tokenizerResult.second);

		tokenizerResult = tokenizer.GetNextToken();
	}

	encodedFile.CloseFile();
	tokensOutput.CloseFile();

	
	SimpleFileComparer comparer;

	if(!comparer.Same(scSamplesBasePath + "PreTokenizerDecoderSampleTokenizedCompare.h",scResultsBasePath + "PreTokenizerDecoderSampleTokenized.h"))
	{
		cout<<"TokenizerTest::Run, failed in testing tokenizing. see result in "<<scResultsBasePath<<"PreTokenizerDecoderSampleTokenizedCompare.h\n";
		status = eFailure;
	}

	return status;


}

bool TokenizerTest::IsNewLine(const string& inToken)
{
	return inToken.at(0) == '\r' || inToken.at(0) == '\n';
}

ADD_CATEGORIZED_TEST(TokenizerTest,"Parser")