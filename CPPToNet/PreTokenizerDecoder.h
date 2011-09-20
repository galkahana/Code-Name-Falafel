/*
   Source File : PreTokenizerDecoder.cpp


   Copyright 2011 Gal Kahana Falafel

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.


   pre tokenizer takes care of preparing for tokenizer work. essentially - taking care of \ (slash) line separator usage (making it one line).
   Trigraphs would have been treated here if i would bother with this silly language feature. i might sometime...but honest..i doubt it

*/
#pragma once

#include "IByteReader.h"
#include "EStatusCode.h"

#include <list>

using namespace Hummus;
using namespace std;

typedef list<Byte> ByteList;

class PreTokenizerDecoder : public IByteReader
{
public:
	PreTokenizerDecoder();
	// constrcutor. accepts underlying method. does not take ownership
	PreTokenizerDecoder(IByteReader* inSourceStream);
	~PreTokenizerDecoder(void);

	void SetStream(IByteReader* inSourceStream);

	virtual LongBufferSizeType Read(Byte* inBuffer,LongBufferSizeType inBufferSize);
	virtual bool NotEnded();


	unsigned long GetCurrentLineIndex() const;

private:

	IByteReader* mSourceStream;
	ByteList mReadCharsBuffer;
	unsigned long mCurrentLineIndex;
	bool mCarriegeReturnEncountered;

	EStatusCode GetNextByte(Byte& outByte);

};
