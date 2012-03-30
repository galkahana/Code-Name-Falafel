#pragma once

#include "ITokenProvider.h"

class BracketsConditionalTokenProvider : public ITokenProvider
{
public:
	BracketsConditionalTokenProvider(ITokenProvider* inProvider);
	~BracketsConditionalTokenProvider(void);

	// ITokenProvider implementation
	virtual BoolAndString GetNextToken();
	virtual void PutBackToken(const string& inToken);

private:
	ITokenProvider* mProvider;
	unsigned long mParanthesisLevel;
};
