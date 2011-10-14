#include "PreTokenizerDecoderTest.h"
#include "InputFile.h"
#include "OutputFile.h"
#include "PreTokenizerDecoder.h"
#include "OutputStreamTraits.h"
#include "IByteWriterWithPosition.h"
#include "SimpleFileComparer.h"
#include "TestsRunner.h"

#include <iostream>

using namespace Hummus;

PreTokenizerDecoderTest::PreTokenizerDecoderTest(void)
{
}

PreTokenizerDecoderTest::~PreTokenizerDecoderTest(void)
{
}

const string scSamplesBasePath = "C:\\CPPToNetTests\\TestMaterials\\";
const string scResultsBasePath = "C:\\CPPToNetTests\\";

EStatusCode PreTokenizerDecoderTest::Run()
{
	EStatusCode status = eSuccess;


	// simple reading of sample file, writing and comparing.

	InputFile encodedFile;
	encodedFile.OpenFile(scSamplesBasePath + "PreTokenizerDecoderSample.h");

	OutputFile decodedFileOutput;
	decodedFileOutput.OpenFile(scResultsBasePath + "PreTokenizerDecoderSampleDecoded.h");

	OutputStreamTraits traits(decodedFileOutput.GetOutputStream());

	PreTokenizerDecoder decoder;

	decoder.SetStream(encodedFile.GetInputStream());

	traits.CopyToOutputStream(&decoder);
	encodedFile.CloseFile();
	decodedFileOutput.CloseFile();

	SimpleFileComparer comparer;

	if(!comparer.Same(scSamplesBasePath + "PreTokenizerDecoderSampleCompare.h",scResultsBasePath + "PreTokenizerDecoderSampleDecoded.h"))
	{
		cout<<"PreTokenizerDecoderTest::Run, failed in testing pre tokenizers. see result in "<<scResultsBasePath<<"PreTokenizerDecoderSampleDecoded.h\n";
		status = eFailure;
	}

	return status;
}

ADD_CATEGORIZED_TEST(PreTokenizerDecoderTest,"Parser")