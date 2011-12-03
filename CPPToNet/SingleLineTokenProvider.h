#pragma once

#include "ITokenProvider.h"
#include "PreProcessor.h"
#include "IPreprocessorListener.h"

class SingleLineTokenProvider : public ITokenProvider, public IPreprocessorListener
{
public:
	SingleLineTokenProvider(PreProcessor* inTokensSource);
	~SingleLineTokenProvider(void);

	// ITokenProvider implementation
	virtual BoolAndString GetNextToken();
	virtual void PutBackToken(const string& inToken);

	// IPreprocessorListener implementation
	virtual void OnNewLine(const string& inNewLineString);

	void Flush();


private:
	PreProcessor* mTokensSource;
	bool mNewLineEncountered;

	bool IsNewLineToken(const string& inToken);
};
