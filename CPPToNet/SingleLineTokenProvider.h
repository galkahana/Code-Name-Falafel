#pragma once

#include "IPreprocessorConditionTokenProvider.h"

class PreProcessor;

class SingleLineTokenProvider : public IPreprocessorConditionTokenProvider
{
public:
	SingleLineTokenProvider(PreProcessor* inPreProcessor);
	~SingleLineTokenProvider(void);


	virtual BoolAndString GetNextToken();
	virtual BoolAndString GetNextTokenNoMacroReplacement();

private:
	PreProcessor* mPreProcessor;
	bool mNewLineEncountered;

	bool IsNewLineToken(const string& inToken);
};
