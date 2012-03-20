#include "TokenProviderStateRecovery.h"


TokenProviderStateRecovery::TokenProviderStateRecovery(void)
{
	mTokenProvider = NULL;
}


TokenProviderStateRecovery::TokenProviderStateRecovery(ITokenProvider* inTokenProvider)
{
	Setup(inTokenProvider);
}


TokenProviderStateRecovery::~TokenProviderStateRecovery(void)
{
}

void TokenProviderStateRecovery::Setup(ITokenProvider* inTokenProvider)
{
	mTokenProvider = inTokenProvider;
}

void TokenProviderStateRecovery::TakeTokenSnapshot()
{
	mTokenStateRecovery.push_back(TokenProviderState());
}

void TokenProviderStateRecovery::CancelSnapshot()
{
	mTokenStateRecovery.pop_back();
}

void TokenProviderStateRecovery::RevertToTopSnapshot()
{
	// remove one items from the stack and use it for getting back to the snapshot it represents
	TokenStateList tokenState = mTokenStateRecovery.back().State;
	mTokenStateRecovery.pop_back();

	TokenStateList::reverse_iterator it = tokenState.rbegin();

	for(; it != tokenState.rend(); ++it)
	{
		if(it->IsPut)
		{
			// if was a put action, so reverse it by getting (which should always succeed, so no need to check)
			GetNextToken();
		}
		else
		{
			// if it was a get action, put back
			PutBackToken(it->Token);
		}
	}

}

BoolAndString TokenProviderStateRecovery::GetNextToken()
{
	if(mTokenStateRecovery.size() > 0)
	{
		BoolAndString token = mTokenProvider->GetNextToken();
		if(token.first)
		{
			TokenProviderStateList::iterator it = mTokenStateRecovery.begin();
			for(; it != mTokenStateRecovery.end(); ++it)
				it->State.push_back(TokenState(token.second,false));
		}
		return token;
	}
	else
		return mTokenProvider->GetNextToken();
}

void TokenProviderStateRecovery::PutBackToken(const string& inToken)
{
	if(mTokenStateRecovery.size() > 0)
	{
		TokenProviderStateList::iterator it = mTokenStateRecovery.begin();
		for(; it != mTokenStateRecovery.end(); ++it)
			it->State.push_back(TokenState(inToken,true));
	}
	mTokenProvider->PutBackToken(inToken);
}