/*
   Source File : PreProcessor.h


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


   C++ preprocessor. implements preprocessing instructions implemenetations and macros replacement.
*/
#pragma once

#include "CPPTokenizer.h"
#include "PreTokenizerDecoder.h"

#include <string>
#include <list>
#include <utility>
#include <map>
#include <set>

namespace Hummus
{
	class IByteReader;
}

class DefineIdentifierDefinition;
class ITokenProvider;

using namespace std;

typedef list<string> StringList;
typedef pair<bool,string> BoolAndString;
typedef map<string,DefineIdentifierDefinition*> StringToDefineIdentifierDefinitionMap;
typedef list<ITokenProvider*> ITokenProviderList;
typedef set<string> StringSet;
typedef map<ITokenProvider*,string> ITokenProviderToStringMap;

class PreProcessor
{
public:
	PreProcessor(void);
	~PreProcessor(void);


	// Setup the preprocessor for reading.
	// provide the following:
	// 1. Stream to read the code from
	// 2. List of defined preprocessor definitions
	// 3. include folders list
	void Setup(Hummus::IByteReader* inSourceStream,
			   const string& inSourceFileName,
			   const StringList& inPreprocessorDefinitions,
			   const StringList& inIncludeFolders);

	// Get the next available token. The proprocessor retrieves processed tokens
	// after having considered macros replacement and preprocessor instructions
	BoolAndString GetNextToken();

	// call this when changing underlying stream position
	void ResetReadState();

private:
	CPPTokenizer mTokenizer;
	PreTokenizerDecoder mPreTokenizationDecoder;
	StringToDefineIdentifierDefinitionMap mDefines;
	ITokenProviderList mTokenSubcontructors;
	StringSet mUsedDefines;
	ITokenProviderToStringMap mIdentifierTokens;
	string mSourceFileName;

	bool IsNewLineToken(const string& inToken);
	bool DefineIdentifierReplacement();
	bool UndefIdentifierReplacement();
	bool DetermineIfHasActiveSubcontructor();
	BoolAndString GetDateMacroToken();
	BoolAndString GetFileMacroToken();
	BoolAndString GetLineMacroToken();
	BoolAndString GetSTDCMacroToken();
	BoolAndString GetTimeMacroToken();
	BoolAndString GetTimeStampMacroToken();
	BoolAndString FirePreprocessorError();
	bool IncludeFile();
	void SkipToNextLine();

};
