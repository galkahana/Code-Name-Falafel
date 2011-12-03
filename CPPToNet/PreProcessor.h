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
#include "ITokenProvider.h"
#include "IExpressionEvaluationContext.h"

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
class IPreprocessorTokenProvider;
struct CPPPrimitiveValue;
class IPreprocessorListener;

using namespace std;

typedef list<string> StringList;
typedef pair<bool,string> BoolAndString;
typedef map<string,DefineIdentifierDefinition*> StringToDefineIdentifierDefinitionMap;
typedef list<IPreprocessorTokenProvider*> IPreprocessorTokenProviderList;
typedef set<IPreprocessorTokenProvider*> IPreprocessorTokenProviderSet;
typedef set<string> StringSet;
typedef map<IPreprocessorTokenProvider*,string> IPreprocessorTokenProviderToStringMap;
typedef pair<bool,BoolAndString> BoolAndBoolAndString;
typedef list<WindowsPath> WindowsPathList;
typedef pair<bool,bool> BoolAndBool;
typedef set<IPreprocessorListener*> IPreprocessorListenerSet;
typedef map<string,string> StringToStringMap;

class PreProcessor : public ITokenProvider, public IExpressionEvaluationContext
{
public:
	PreProcessor(void);
	~PreProcessor(void);


	// Setup the preprocessor for reading.
	// provide the following:
	// 1. Stream to read the code from
	// 2. List of defined preprocessor definitions mapped to their value (pass empty string value for no value)
	// 3. include folders list (ordered by priority, where the first folder is the first one to look at)
	void Setup(Hummus::IByteReader* inSourceStream,
			   const string& inSourceFileName,
			   const StringToStringMap& inPreprocessorDefinitions,
			   const StringList& inIncludeFolders);

	// ITokenProvider implementation

	// Get the next available token. The proprocessor retrieves processed tokens
	// after having considered macros replacement and preprocessor instructions
	virtual BoolAndString GetNextToken();
	virtual void PutBackToken(const string& inToken);

	// IExpressionEvaluationContext implementation
	virtual EStatusCodeAndBool IsPreprocessorSymbolDefined(const string& inSymbol);

	// call this when changing underlying stream position
	void ResetReadState();

	// Internal, don't call (runs token getting but avoids macro replacement)
	BoolAndString GetNextTokenNoMacroReplacement();
	// Internal, don't call (retrieves GetNextToken with options to get also newlines as tokens)
	BoolAndString GetNextToken(bool inSkipNewLines);


	void AddListener(IPreprocessorListener* inListener);
	void RemoveListener(IPreprocessorListener* inListener);
private:
	CPPTokenizer mTokenizer;
	PreTokenizerDecoder mPreTokenizationDecoder;
	StringToDefineIdentifierDefinitionMap mDefines;
	IPreprocessorTokenProviderList mTokenSubcontructors;
	StringSet mUsedDefines;
	IPreprocessorTokenProviderToStringMap mIdentifierTokens;
	string mSourceFileName;
	string mSourceFileNameForMacro;
	IPreprocessorTokenProviderList mIncludeProvidersStack;
	WindowsPath mSourceFileFolderPath;
	WindowsPathList mIncludeFolders;
	StringSet mDontInclude;
	unsigned long mConditionalIteration;
	IPreprocessorListenerSet mListeners;
	bool mSkippingConditionals;
	StringList mTokensStack;
	bool mNoMacroReplacemeentState;
	unsigned long mParanthesisLevel;
	bool mNotifyNewLines;
	bool mEncounteredNewLineWhileNotNotifying;


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
	void DetermineContinuedNoReplaceement(const string& inToken);

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
	bool GetAsBoolean(const CPPPrimitiveValue& inValue);
	void FireNewLine(const string& inNewLineString);
	void StopNewLineNotification();
	void StartNewLineNotification();

};
