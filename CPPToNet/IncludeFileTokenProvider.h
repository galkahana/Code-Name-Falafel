#pragma once
#include "ITokenProvider.h"
#include "CPPTokenizer.h"
#include "PreTokenizerDecoder.h"
#include "InputFile.h"

class IncludeFileTokenProvider : public ITokenProvider
{
public:
	IncludeFileTokenProvider(const string& inIncludeFilePath);
	virtual ~IncludeFileTokenProvider(void);

	virtual bool IsFinished();
	virtual BoolAndString GetNextToken();

private:

	InputFile mInputFile;
	CPPTokenizer mTokenizer;
	PreTokenizerDecoder mPreTokenizationDecoder;

};
