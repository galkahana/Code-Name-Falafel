/*
   Source File : CPPTokenizer.cpp


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


   C++ tokenizer, works to separate individual syntactic entities. returns punctuation marks and operators, identifiers, number
   all as uninterpreted strings
*/
#pragma once

#include "IOBasicTypes.h"
#include "EStatusCode.h"

#include <string>
#include <list>
#include <utility>

namespace Hummus
{
	class IByteReader;
}

using namespace std;
typedef pair<bool,string> BoolAndString;
typedef list<Hummus::Byte> ByteList;

class CPPTokenizer
{
public:
	CPPTokenizer(void);
	~CPPTokenizer(void);

	// Assign the stream to read from (does not take ownership of the stream)
	void SetReadStream(Hummus::IByteReader* inSourceStream);

	// Get the next avialable C++ token. return result returns whether
	// token retreive was successful and the token. Token retrieval may be unsuccesful if
	// the end of file was reached and no token was recorded, or if read failure occured.
	// tokens may be:
	// 1. Identifiers separated from other content with space or punctuation marks
	// 2. Complete Comments
	// 3. Complete Strings, Chars
	// 4. Numbers 
	// 5. Operators, or what is suspected as operator (for instance >> may be shift right, but could turn out as template closure). # signs will be returned togather with their later directive. 
	// 6. Grouping punctuation marks - [,],{,},?,(,)
	// 7. newlines. yes. i'm having newlines as tokens. This is for the sake of the preprocessor implementations of line or when to stop preprocessor commands
	// each of these function as a separate syntactic entity - where and equivelent program would be to write the tokens as is with spaces between them.
	// This should allow the later parser to easily get their meaning through going over tokens and considering the various options.
	BoolAndString GetNextToken();

	// call this when changing underlying stream position
	void ResetReadState();

private:
	Hummus::IByteReader* mStream;

	
	bool mNotEncounteredTokensForLine;
	bool mNextTokenCanIncludeParanthesis; 
	ByteList mTokenBuffer;

	// failure in GetNextByteForToken actually marks a true read failure, if you checked end of file before calling it...
	Hummus::EStatusCode GetNextByteForToken(Hummus::Byte& outByte);
	void SkipTillToken();
	bool IsNonNewlineWhiteSpace(Hummus::Byte inCharacter);
	bool IsWhiteSpace(Hummus::Byte inCharacter);
	void SaveTokenBuffer(Hummus::Byte inToSave);
	bool IsEntityBreaker(Hummus::Byte inCharacter,bool inCanIncludeParanthesis);
	void PutBackToken(const string& inToken);


};
