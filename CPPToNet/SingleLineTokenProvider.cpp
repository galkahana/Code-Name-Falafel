#include "SingleLineTokenProvider.h"
#include "PreProcessor.h"

SingleLineTokenProvider::SingleLineTokenProvider(PreProcessor* inPreProcessor)
{
	mPreProcessor = inPreProcessor;
	mNewLineEncountered = false;
}

SingleLineTokenProvider::~SingleLineTokenProvider(void)
{
}


BoolAndString SingleLineTokenProvider::GetNextToken()
{
	if(mNewLineEncountered)
		return BoolAndString(false,"");

	BoolAndString result = mPreProcessor->GetNextToken(false);

	if(result.first && IsNewLineToken(result.second))
	{
		mNewLineEncountered = true;
		return BoolAndString(false,"");
	}
	else
		return result;
}

BoolAndString SingleLineTokenProvider::GetNextTokenNoMacroReplacement()
{
	if(mNewLineEncountered)
		return BoolAndString(false,"");

	BoolAndString result = mPreProcessor->GetNextTokenNoMacroReplacement();

	if(result.first && IsNewLineToken(result.second))
	{
		mNewLineEncountered = true;
		return BoolAndString(false,"");
	}
	else
		return result;
}

bool SingleLineTokenProvider::IsNewLineToken(const string& inToken)
{
	// new line can be: \r, \n or \r\n. and is the only kind of token to have these chars
	return inToken.at(0) == '\r' || inToken.at(0) == '\n';
}