#include "IncludeFileTokenProvider.h"

IncludeFileTokenProvider::IncludeFileTokenProvider(const string& inFilePath)
{
	mInputFile.OpenFile(inFilePath);

	mPreTokenizationDecoder.SetStream(mInputFile.GetInputStream());
	mTokenizer.SetReadStream(&mPreTokenizationDecoder);

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




