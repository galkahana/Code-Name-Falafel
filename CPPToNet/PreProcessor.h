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
#include "WindowsPath.h"

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
typedef set<ITokenProvider*> ITokenProviderSet;
typedef set<string> StringSet;
typedef map<ITokenProvider*,string> ITokenProviderToStringMap;
typedef pair<bool,BoolAndString> BoolAndBoolAndString;
typedef list<WindowsPath> WindowsPathList;
typedef pair<bool,bool> BoolAndBool;

class PreProcessor
{
public:
	PreProcessor(void);
	~PreProcessor(void);


	// Setup the preprocessor for reading.
	// provide the following:
	// 1. Stream to read the code from
	// 2. List of defined preprocessor definitions
	// 3. include folders list (ordered by priority, where the first folder is the first one to look at)
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
	string mSourceFileNameForMacro;
	ITokenProviderList mIncludeProvidersStack;
	WindowsPath mSourceFileFolderPath;
	WindowsPathList mIncludeFolders;
	StringSet mDontInclude;
	unsigned long mConditionalIteration;

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
	BoolAndBoolAndString HandlePredefinedMacros(const string& inToken);

	// this method is similar to GetNextToken, only it uses GetNextNoSpaceEntity, which does a more limited
	// tokenization - just of the next entity till the next space. this is used in the context of #include interpretation
	// which both forms ("XXXXX" <XXXXXXX>) don't follow regular tokenization rules.
	// Also, assuming that not in a define context
	BoolAndString GetNextNoSpaceEntity();

	// used in the context of define for reading the string till end of line, either from tokenizer, or from subordinate tokenizers
	string GetStringTillEndOfLine();

	BoolAndString FindFile(string inIncludeString,bool inIsDoubleQuoteSearch);
	void Reset();
	bool ModifyLineAndfile();
	void FlushTillEndOfLine();
	void SetNewFileAndLine(unsigned long inNewLineIndex,const string& inNewFileName);
	bool ImplementPragmaDirective();
	BoolAndBool EvaluateConstantExpression(const string& inConditionType);
	void SetupCondtionalIteration();
	void ResetConditionalIteration();
	bool InterpretConditionalTokenization(const string& inConditionalKeyword);
	bool InterpretConditionalStopper(const string& inConditionalKeyword);

};
