#pragma once
#include "IPreprocessorTokenProvider.h"
#include "CPPTokenizer.h"
#include "PreTokenizerDecoder.h"
#include "InputFile.h"
#include "WindowsPath.h"

class IncludeFileTokenProvider : public IPreprocessorTokenProvider
{
public:
	IncludeFileTokenProvider(const string& inIncludeFilePath);
	virtual ~IncludeFileTokenProvider(void);

	virtual bool IsFinished();
	virtual BoolAndString GetNextToken();
	virtual BoolAndString GetNextNoSpaceEntity();
	virtual string GetStringTillEndOfLine();

	const WindowsPath& GetIncludeFileFolder() const;
	const string& GetSourceFileName() const;
	const string& GetSourceFileNameForMacro() const;
	unsigned long GetCurrentLineIndex() const;

	void SetNewFileAndLine(unsigned long inNewLineIndex,const string& inNewFileName);

private:

	InputFile mInputFile;
	CPPTokenizer mTokenizer;
	PreTokenizerDecoder mPreTokenizationDecoder;
	WindowsPath mIncludeFileFolder;
	string mSourceFileName;
	string mSourceFileNameForMacro;

};
