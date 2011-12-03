#include "SingleLineTokenProvider.h"

SingleLineTokenProvider::SingleLineTokenProvider(PreProcessor* inTokensSource)
{
	mTokensSource = inTokensSource;
	if(mTokensSource)
		mTokensSource->AddListener(this);
	mNewLineEncountered = false;
}

SingleLineTokenProvider::~SingleLineTokenProvider(void)
{
	mTokensSource->RemoveListener(this);
}


BoolAndString SingleLineTokenProvider::GetNextToken()
{
	if(mNewLineEncountered)
		return BoolAndString(false,"");

	BoolAndString result = mTokensSource->GetNextToken();

	if(mNewLineEncountered) // will be modified through event
	{
		if(result.first)
			mTokensSource->PutBackToken(result.second);
		return BoolAndString(false,"");
	}
	else
		return result;
}

void SingleLineTokenProvider::PutBackToken(const string& inToken)
{
	mTokensSource->PutBackToken(inToken);
}

void SingleLineTokenProvider::OnNewLine(const string& inNewLineString)
{
	mNewLineEncountered = true;
}

void SingleLineTokenProvider::Flush()
{
	BoolAndString result;

	while(!mNewLineEncountered)
	{
		BoolAndString result = mTokensSource->GetNextToken();
		if(!result.first)
			break;

		if(mNewLineEncountered)
		{
			mTokensSource->PutBackToken(result.second);
			break;
		}
	}
}
