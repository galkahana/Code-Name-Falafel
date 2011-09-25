#include "PreTokenizerDecoder.h"

PreTokenizerDecoder::PreTokenizerDecoder()
{
}

PreTokenizerDecoder::PreTokenizerDecoder(IByteReader* inSourceStream)
{
	mSourceStream = inSourceStream;
	mCurrentLineIndex = 0;
	mCarriegeReturnEncountered = false;
}

PreTokenizerDecoder::~PreTokenizerDecoder(void)
{
}

void PreTokenizerDecoder::SetStream(IByteReader* inSourceStream)
{
	mSourceStream = inSourceStream;
	mReadCharsBuffer.clear();
	mCurrentLineIndex = 0;
	mCarriegeReturnEncountered = false;
}

LongBufferSizeType PreTokenizerDecoder::Read(Byte* inBuffer,LongBufferSizeType inBufferSize)
{
	LongBufferSizeType readCount = 0;
	Byte buffer;

	while(NotEnded() && readCount < inBufferSize)
	{
		if(GetNextByte(buffer) != eSuccess)
			break;
		
		// skip slashes with endliners after them
		if('\\' == buffer && NotEnded())
		{
			mCarriegeReturnEncountered = false;

			if(GetNextByte(buffer) != eSuccess)
				break;
			if(buffer != 0xA || buffer != 0xD)
			{
				// place both chars...
				*inBuffer = '\\';
				++inBuffer;
				++readCount;

				if(readCount < inBufferSize)
				{
					*inBuffer = buffer;
					++inBuffer;
					++readCount;
				}
				else
					mReadCharsBuffer.push_back(buffer);
			}
			else
			{
				++mCurrentLineIndex; // advance line index, also in lines that are continued

				// note that when skipping, i will possibly still place one character in the output buffer, in case of \0D and something which is not 0A. this
				// is fine becuase the buffer requires at least one more.

				// check out if this is 0D0A sequance, which in case the 0A should be skipped as well
				if(0xD == buffer && NotEnded())
				{
					if(GetNextByte(buffer) != eSuccess)
						break;
					if(buffer != 0xA)
					{
						*inBuffer = buffer;
						++inBuffer;
						++readCount;						
					}
				}
			}
		}
		else
		{
			if(0xA == buffer && !mCarriegeReturnEncountered)
			{
				++mCurrentLineIndex;
				mCarriegeReturnEncountered = false;
			}
			else if(0xD == buffer)
			{
				++mCurrentLineIndex;
				mCarriegeReturnEncountered = true; // this indication is now up, so that if next read is 0xA it will counted as an 0D0A pair, which counts as one line
			}
			else
				mCarriegeReturnEncountered = false;

			*inBuffer = buffer;
			++inBuffer;
			++readCount;
		}
	}

	return readCount;
}

bool PreTokenizerDecoder::NotEnded()
{
	return mSourceStream->NotEnded() || mReadCharsBuffer.size() >0;
}

EStatusCode PreTokenizerDecoder::GetNextByte(Byte& outByte)
{
	if(mReadCharsBuffer.size() > 0)
	{
		outByte = mReadCharsBuffer.front();
		mReadCharsBuffer.pop_front();
		return eSuccess;
	}
	else
		return (mSourceStream->Read(&outByte,1) != 1) ? eFailure:eSuccess;
}

unsigned long PreTokenizerDecoder::GetCurrentLineIndex() const
{
	return mCurrentLineIndex;
}


void PreTokenizerDecoder::SetNewLineIndex(unsigned long inNewLineIndex)
{
	mCurrentLineIndex = inNewLineIndex;
}
