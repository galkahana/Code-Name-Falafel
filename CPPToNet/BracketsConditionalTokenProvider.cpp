#include "BracketsConditionalTokenProvider.h"

BracketsConditionalTokenProvider::BracketsConditionalTokenProvider(ITokenProvider* inProvider)
{
	mProvider = inProvider;
	mParanthesisLevel = 1;
}

BracketsConditionalTokenProvider::~BracketsConditionalTokenProvider(void)
{
}

BoolAndString BracketsConditionalTokenProvider::GetNextToken()
{
	if(0 == mParanthesisLevel)
		return BoolAndString(false,"");

	BoolAndString tokenResult = mProvider->GetNextToken();

	if(!tokenResult.first)
		return BoolAndString(false,"");

	if(tokenResult.second == "]")
		--mParanthesisLevel;

	if(tokenResult.second == "[")
		++mParanthesisLevel;

	if(0 == mParanthesisLevel)
		return BoolAndString(false,"");
	else
		return tokenResult;

}

void BracketsConditionalTokenProvider::PutBackToken(const string& inToken)
{
	// better implementation is probably to implement ones own stack, but we can manage
	// here with adding/substracting proparly. need to reverse the logic, basically

	if(inToken == "]")
		++mParanthesisLevel;

	if(inToken == "[")
		--mParanthesisLevel;


	mProvider->PutBackToken(inToken);
}