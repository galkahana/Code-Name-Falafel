#pragma once
#include "ITokenProvider.h"
#include <string>
#include <list>

using namespace std;

struct TokenState
{
	TokenState(string inToken,bool inIsPut){Token = inToken;IsPut = inIsPut;}

	string Token;
	bool IsPut;
};

typedef list<TokenState> TokenStateList;

struct TokenProviderState
{
	TokenStateList State;
};

typedef list<TokenProviderState> TokenProviderStateList;

class TokenProviderStateRecovery : public ITokenProvider
{
public:
	TokenProviderStateRecovery(void);
	TokenProviderStateRecovery(ITokenProvider* inTokenProvider);
	~TokenProviderStateRecovery(void);

	void Setup(ITokenProvider* inTokenProvider);

	void TakeTokenSnapshot();
	void CancelSnapshot();
	void RevertToTopSnapshot();

	virtual BoolAndString GetNextToken();
	virtual void PutBackToken(const string& inToken);

private:

	TokenProviderStateList mTokenStateRecovery;
	ITokenProvider* mTokenProvider;


};

