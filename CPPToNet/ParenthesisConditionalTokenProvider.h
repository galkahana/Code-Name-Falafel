#pragma once

#include "ITokenProvider.h"

class ParenthesisConditionalTokenProvider : public ITokenProvider
{
public:
	ParenthesisConditionalTokenProvider(ITokenProvider* inProvider);
	~ParenthesisConditionalTokenProvider(void);

	// ITokenProvider implementation
	virtual BoolAndString GetNextToken();
	virtual void PutBackToken(const string& inToken);

private:
	ITokenProvider* mProvider;
	unsigned long mParanthesisLevel;
};
