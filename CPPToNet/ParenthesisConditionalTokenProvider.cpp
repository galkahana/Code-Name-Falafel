#include "ParenthesisConditionalTokenProvider.h"

ParenthesisConditionalTokenProvider::ParenthesisConditionalTokenProvider(ITokenProvider* inProvider)
{
	mProvider = inProvider;
	mParanthesisLevel = 1;
}

ParenthesisConditionalTokenProvider::~ParenthesisConditionalTokenProvider(void)
{
}

BoolAndString ParenthesisConditionalTokenProvider::GetNextToken()
{
	if(0 == mParanthesisLevel)
		return BoolAndString(false,"");

	BoolAndString tokenResult = mProvider->GetNextToken();

	if(!tokenResult.first)
		return BoolAndString(false,"");

	if(tokenResult.second == ")")
		--mParanthesisLevel;

	if(tokenResult.second == "(")
		++mParanthesisLevel;

	if(0 == mParanthesisLevel)
		return BoolAndString(false,"");
	else
		return tokenResult;

}

void ParenthesisConditionalTokenProvider::PutBackToken(const string& inToken)
{
	mProvider->PutBackToken(inToken);
}