/*
   Source File : InputStreamSkipperStream.cpp


   Copyright 2011 Gal Kahana Hummus

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.

   
*/
#include "InputStreamSkipperStream.h"

using namespace Hummus;

InputStreamSkipperStream::InputStreamSkipperStream(void)
{
	mStream = NULL;
}

InputStreamSkipperStream::~InputStreamSkipperStream(void)
{
	if(mStream != NULL)
		delete mStream;
}

InputStreamSkipperStream::InputStreamSkipperStream(IByteReader* inSourceStream)
{
	Assign(inSourceStream);
}


void InputStreamSkipperStream::Assign(IByteReader* inSourceStream)
{
	mStream = inSourceStream;
	mAmountRead = 0;
}

LongBufferSizeType InputStreamSkipperStream::Read(Byte* inBuffer,LongBufferSizeType inBufferSize)
{
	LongBufferSizeType readThisTime = mStream->Read(inBuffer,inBufferSize);
	mAmountRead+=readThisTime;

	return readThisTime;
}

bool InputStreamSkipperStream::NotEnded()
{
	return mStream->NotEnded();
}


bool InputStreamSkipperStream::CanSkipTo(LongFilePositionType inPositionInStream)
{
	return mAmountRead <= inPositionInStream;
}

void InputStreamSkipperStream::SkipTo(LongFilePositionType inPositionInStream)
{
	if(!CanSkipTo(inPositionInStream))
		return;

	SkipBy(inPositionInStream-mAmountRead);
}

// will skip by, or hit EOF
void InputStreamSkipperStream::SkipBy(LongFilePositionType inAmountToSkipBy)
{
	Byte buffer;

	while(NotEnded() && inAmountToSkipBy>0)
	{
		Read(&buffer,1);
		--inAmountToSkipBy;
	}

}

void InputStreamSkipperStream::Reset()
{
	mAmountRead = 0;
}

LongFilePositionType InputStreamSkipperStream::GetCurrentPosition()
{
	return mAmountRead;
}