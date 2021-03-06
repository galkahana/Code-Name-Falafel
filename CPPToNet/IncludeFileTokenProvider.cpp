#include "IncludeFileTokenProvider.h"
#include "WindowsFileSystem.h"

IncludeFileTokenProvider::IncludeFileTokenProvider(const string& inFilePath)
{
	mInputFile.OpenFile(inFilePath);

	mPreTokenizationDecoder.SetStream(mInputFile.GetInputStream());
	mTokenizer.SetReadStream(&mPreTokenizationDecoder);
	WindowsFileSystem fileSystem;

	mIncludeFileFolder = WindowsPath(inFilePath).GetFolder();
	mSourceFileName = inFilePath;
	mSourceFileNameForMacro = inFilePath;

}

IncludeFileTokenProvider::~IncludeFileTokenProvider(void)
{
	mInputFile.CloseFile();
}

bool IncludeFileTokenProvider::IsFinished()
{
	return !mInputFile.GetInputStream()->NotEnded();	// hopefully good enough...
}

BoolAndString IncludeFileTokenProvider::GetNextToken()
{
	return mTokenizer.GetNextToken();
}

BoolAndString IncludeFileTokenProvider::GetNextNoSpaceEntity()
{
	return mTokenizer.GetNextNoSpaceEntity();
}

string IncludeFileTokenProvider::GetStringTillEndOfLine()
{
	return mTokenizer.GetStringTillEndOfLine();
}

const WindowsPath& IncludeFileTokenProvider::GetIncludeFileFolder() const
{
	return mIncludeFileFolder;
}

const string& IncludeFileTokenProvider::GetSourceFileName() const
{
	return mSourceFileName;
}

const string& IncludeFileTokenProvider::GetSourceFileNameForMacro() const
{
	return mSourceFileNameForMacro;
}

unsigned long IncludeFileTokenProvider::GetCurrentLineIndex() const
{
	return mPreTokenizationDecoder.GetCurrentLineIndex();
}


void IncludeFileTokenProvider::SetNewFileAndLine(unsigned long inNewLineIndex,const string& inNewFileName)
{
	if(inNewFileName.size() > 0)
		mSourceFileNameForMacro = inNewFileName;
	mPreTokenizationDecoder.SetNewLineIndex(inNewLineIndex);
}
