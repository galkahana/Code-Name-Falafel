#include "EmptyTest.h"
#include "TestsRunner.h"
#include "HeaderUnit.h"

#include "InputByteArrayStream.h"
#include "CPPStatementsParser.h"
#include "CPPNamespace.h"

#include <iostream>

using namespace Hummus;

EmptyTest::EmptyTest(void)
{
}


EmptyTest::~EmptyTest(void)
{
}

EStatusCode EmptyTest::Run()
{
	CPPStatementsParser parser;
	Byte empty;
	InputByteArrayStream emptySource(&empty,0);

	EStatusCodeAndHeaderUnit result = parser.Parse(&emptySource,"empty",StringToStringMap(),StringList());

	if(result.first != eSuccess)
	{
		cout<<"EmptyTest::Run, failed to parse empty source\n";
		return eFailure;
	}

	HeaderUnit* parsedTree = result.second;
	CPPNamespace* globalNamespace = parsedTree->GetGlobalNamespace();
	
	delete parsedTree;
	return eSuccess;
}

ADD_CATEGORIZED_TEST(EmptyTest,"Parser")