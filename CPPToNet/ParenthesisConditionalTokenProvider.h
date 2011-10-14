#pragma once

#include "IPreprocessorConditionTokenProvider.h"

class ParenthesisConditionalTokenProvider : public IPreprocessorConditionTokenProvider
{
public:
	ParenthesisConditionalTokenProvider(IPreprocessorConditionTokenProvider* inProvider);
	~ParenthesisConditionalTokenProvider(void);


	virtual BoolAndString GetNextToken();
	virtual BoolAndString GetNextTokenNoMacroReplacement();

private:
	IPreprocessorConditionTokenProvider* mProvider;
	unsigned long mParanthesisLevel;
};
