#include "ParenthesisConditionalTokenProvider.h"

ParenthesisConditionalTokenProvider::ParenthesisConditionalTokenProvider(IPreprocessorConditionTokenProvider* inProvider)
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

BoolAndString ParenthesisConditionalTokenProvider::GetNextTokenNoMacroReplacement()
{
	if(0 == mParanthesisLevel)
		return BoolAndString(false,"");

	BoolAndString tokenResult = mProvider->GetNextTokenNoMacroReplacement();

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