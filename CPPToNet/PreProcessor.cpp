#include "PreProcessor.h"
#include "DefineIdentifierDefinition.h"
#include "Trace.h"
#include "ITokenProvider.h"
#include "BoxingBase.h"
#include "IncludeFileTokenProvider.h"
#include "WindowsFileSystem.h"
#include "SafeBufferMacrosDefs.h"

#include <ctime>

using namespace Hummus;

PreProcessor::PreProcessor(void)
{
}

PreProcessor::~PreProcessor(void)
{
	Reset();
}

void PreProcessor::Reset()
{
	StringToDefineIdentifierDefinitionMap::iterator it = mDefines.begin();

	for(; it != mDefines.end(); ++it)
		delete it->second;
	mDefines.clear();

	ITokenProviderList::iterator itTokenProviders = mTokenSubcontructors.begin();
	for(; itTokenProviders != mTokenSubcontructors.end();++itTokenProviders)
		delete *itTokenProviders;
	mTokenSubcontructors.clear();

	mIncludeProvidersStack.clear();
	mDefines.clear();
	mIncludeFolders.clear();
	mDontInclude.clear();
	mConditionalIteration = 0;
}

void PreProcessor::Setup(IByteReader* inSourceStream,
						 const string& inSourceFileName,
					     const StringList& inPreprocessorDefinitions,
						 const StringList& inIncludeFolders)
{
	Reset();

	mPreTokenizationDecoder.SetStream(inSourceStream);
	mTokenizer.SetReadStream(&mPreTokenizationDecoder);

	StringList::const_iterator itDefs = inPreprocessorDefinitions.begin();
	
	for(; itDefs != inPreprocessorDefinitions.end(); ++itDefs)
		mDefines.insert(StringToDefineIdentifierDefinitionMap::value_type(*itDefs,new DefineIdentifierDefinition));

	mSourceFileName = inSourceFileName;
	mSourceFileNameForMacro = inSourceFileName;
	mSourceFileFolderPath = WindowsPath(mSourceFileName).GetFolder();

	StringList::const_iterator itFolders = inIncludeFolders.begin();
	for(; itFolders != inIncludeFolders.end(); ++ itFolders)
		mIncludeFolders.push_back(WindowsPath(*itFolders));
		
}

BoolAndString PreProcessor::GetNextToken()
{
	BoolAndString tokenizerResult;
	
	// preprocessor implementations are mostly implemented by employing subcontructors
	// that get tokens according to the matching macro, ifdef section or whatnot.
	if(DetermineIfHasActiveSubcontructor())
		mTokenSubcontructors.back()->GetNextToken();
	else
		tokenizerResult = mTokenizer.GetNextToken();
	
	if(!tokenizerResult.first)
		return tokenizerResult;

	// first thing to realize is whether we have an defined identifier here. if this is the case, need to stop and switch context
	// to idetnfier
	StringToDefineIdentifierDefinitionMap::iterator itDefine = mDefines.find(tokenizerResult.second);
	if(itDefine != mDefines.end())
	{
		if(mUsedDefines.find(tokenizerResult.second) != mUsedDefines.end())
		{
			TRACE_LOG1("PreProcessor::GetNextToken, circular macro usage identified, ingoring macro replacement. identifier = %s",tokenizerResult.second.c_str());
		}
		else
		{
			// hurrah! define symbol. move control to a symbol definition
			ITokenProvider* provider = itDefine->second->CreateTokenProvider(this);
			if(!provider)
				return BoolAndString(false,"");
			mTokenSubcontructors.push_back(provider);
			mUsedDefines.insert(tokenizerResult.second);
			mIdentifierTokens.insert(ITokenProviderToStringMap::value_type(provider,tokenizerResult.second));
			return GetNextToken();
		}
	}

	// now let's try some predefined macros
	BoolAndBoolAndString predefinedMacrosResult = HandlePredefinedMacros(tokenizerResult.second);
	if(predefinedMacrosResult.first)
	{
		return predefinedMacrosResult.second;
	}
	else if(IsNewLineToken(tokenizerResult.second)) // now, regular cases and preprocessor commands
	{
		// skip newlines. not interesting for higher levels
		return GetNextToken();
	}
	else if(tokenizerResult.second == "#define")
	{
		bool status = DefineIdentifierReplacement();
		if(!status)
			return BoolAndString(false,"");
		return GetNextToken();
	}
	else if(tokenizerResult.second == "#undef")
	{
		bool status = UndefIdentifierReplacement();
		if(!status)
			return BoolAndString(false,"");
		return GetNextToken();
	}
	else if(tokenizerResult.second == "#error")
	{
		return FirePreprocessorError();
	}
	else if(tokenizerResult.second == "#incude")
	{
		bool status = IncludeFile();
		if(!status)
			return BoolAndString(false,"");
		return GetNextToken();
	}
	else if(tokenizerResult.second == "#line")
	{
		bool status = ModifyLineAndfile();
		if(!status)
			return BoolAndString(false,"");
		return GetNextToken();
	}
	else if(tokenizerResult.second == "#")
	{
		// the empty pre-processor node. just skip it
		return GetNextToken();
	}
	else if(tokenizerResult.second == "#pragma")
	{
		bool status = ImplementPragmaDirective();
		if(!status)
			return BoolAndString(false,"");
		return GetNextToken();
	}
	else if(tokenizerResult.second == "#if" || tokenizerResult.second == "#ifdef" || tokenizerResult.second == "#ifndef")
	{
		bool status = InterpretConditionalTokenization(tokenizerResult.second);
		if(!status)
			return BoolAndString(false,"");
		return GetNextToken();
	}
	else if(tokenizerResult.second == "#else" || tokenizerResult.second == "#elif" || tokenizerResult.second == "#endif")
	{
		bool status = InterpretConditionalStopper(tokenizerResult.second);
		if(!status)
			return BoolAndString(false,"");
		return GetNextToken();
	}
	else if(tokenizerResult.second.at(0) == '#')
	{
		// starts with #, meaning an unimplemented directive
		TRACE_LOG1("PreProcessor::GetNextToken, unimplemented directive %s, skipping",tokenizerResult.second.c_str());
		FlushTillEndOfLine();
		return GetNextToken();
	}
	else 
	{
		return tokenizerResult;
	}
}

BoolAndBoolAndString PreProcessor::HandlePredefinedMacros(const string& inToken)
{
	if(inToken == "__DATE__")
	{
		return BoolAndBoolAndString(true,GetDateMacroToken());
	}
	else if(inToken == "__FILE__")
	{
		return BoolAndBoolAndString(true,GetFileMacroToken());
	}
	else if(inToken == "__LINE__")
	{
		return BoolAndBoolAndString(true,GetLineMacroToken());
	}
	else if(inToken == "__STDC__")
	{
		return BoolAndBoolAndString(true,GetSTDCMacroToken());
	}
	else if(inToken == "__TIME__")
	{
		return BoolAndBoolAndString(true,GetTimeMacroToken());
	}
	else if(inToken == "__TIMESTAMP__")
	{
		return BoolAndBoolAndString(true,GetTimeStampMacroToken());
	}
	else
		return BoolAndBoolAndString(false,BoolAndString(false,""));
}

bool PreProcessor::IsNewLineToken(const string& inToken)
{
	// new line can be: \r, \n or \r\n. and is the only kind of token to have these chars
	return inToken.at(0) == '\r' || inToken.at(0) == '\n';
}

bool PreProcessor::DefineIdentifierReplacement()
{
	BoolAndString readResult;
	string identifier;
	DefineIdentifierDefinition* newDefine = new DefineIdentifierDefinition();
	bool status = true;

	do
	{
		// first token is identifier name. note that if this is a function definition
		// it will look like identifier(<optionally first parameter><optionally also )>
		readResult = GetNextToken();
		if(!readResult.first)
		{
			TRACE_LOG("PreProcessor::DefineIdentifierReplacement, Error in reading token");
			status = false;
			break;
		}

		string::size_type parametersPosition = readResult.second.find('(');
		if(parametersPosition == string::npos)
		{
			// simple identifier
			identifier = readResult.second;
		}
		else
		{
			bool foundEndParameters = false;

			// function definition, grab identifier
			identifier = readResult.second.substr(0,parametersPosition);

			// read function parameters

			// here is treatment of a case where the first parameter (and possibly also the closing paranthesis) are
			// attached to the identifier token
			if(readResult.second.length() > parametersPosition + 1)
			{
				string::size_type endParametersPosition = readResult.second.find(')',parametersPosition + 1);
				if(string::npos == endParametersPosition)
				{
					newDefine->PushParameter(readResult.second.substr(parametersPosition + 1));
				}
				else
				{
					foundEndParameters = true;
					if(parametersPosition + 1 != endParametersPosition) 
						newDefine->PushParameter(readResult.second.substr(parametersPosition + 1,endParametersPosition - parametersPosition - 1));
				}
			}

			while(!foundEndParameters)
			{
				readResult = GetNextToken();
				if(readResult.first)
				{
					TRACE_LOG("PreProcessor::DefineIdentifierReplacement, Error in reading token for identifier parameters");
					status = false;
					break;
				}
				if(readResult.second == ")")
					foundEndParameters = true;
				else
					newDefine->PushParameter(readResult.second);
			}
			if(!status)
				break;
		}

		// now grab string till end of line (do not intepret yet, because dependent on reading context
		newDefine->SetTokenStrings(GetStringTillEndOfLine());
		
	}while(false);

	if(!status)
	{
		delete newDefine;
		return false;
	}

	// now let's make a define out of this...
	mDefines.insert(StringToDefineIdentifierDefinitionMap::value_type(identifier,newDefine));
	return true;
}

string PreProcessor::GetStringTillEndOfLine()
{
	BoolAndString tokenizerResult;
	
	// preprocessor implementations are mostly implemented by employing subcontructors
	// that get tokens according to the matching macro, ifdef section or whatnot.
	if(DetermineIfHasActiveSubcontructor())
		return mTokenSubcontructors.back()->GetStringTillEndOfLine();
	else
		return mTokenizer.GetStringTillEndOfLine();

}

bool PreProcessor::UndefIdentifierReplacement()
{
	// k. this is simple. next identifier should be the symbol and that's it. flush the rest of the tokens till end line
	BoolAndString tokenResult = GetNextToken();

	if(!tokenResult.first)
	{
		TRACE_LOG("PreProcessor::UndefIdentifierReplacement, Error in reading token for #undef identifier");
		return false;
	}

	StringToDefineIdentifierDefinitionMap::iterator it = mDefines.find(tokenResult.second);
	if(it != mDefines.end())
	{
		delete it->second;
		mDefines.erase(it);
	}

	// now flush everything till end of line
	FlushTillEndOfLine();
	return true;
}

void PreProcessor::FlushTillEndOfLine()
{
	BoolAndString tokenResult;
	bool foundEndLine = false;

	while(!foundEndLine)
	{
		tokenResult = GetNextToken();
		if(!tokenResult.first || IsNewLineToken(tokenResult.second)) 
			foundEndLine = true;
	}
}

bool PreProcessor::DetermineIfHasActiveSubcontructor()
{
	bool hasActive = false;

	while(mTokenSubcontructors.size() > 0 && !hasActive)
	{
		if(mTokenSubcontructors.back()->IsFinished())
		{
			// see if this is a macro token provider, if so remove related identifier, to allow further usage of it
			ITokenProviderToStringMap::iterator itIdentifier = mIdentifierTokens.find(mTokenSubcontructors.back());
			if(itIdentifier != mIdentifierTokens.end())
			{
				mUsedDefines.erase(itIdentifier->second);
				mIdentifierTokens.erase(itIdentifier);
			}
			if(mIncludeProvidersStack.size() > 0 && mTokenSubcontructors.back() == mIncludeProvidersStack.back())
				mIncludeProvidersStack.pop_back();
			delete mTokenSubcontructors.back();
			mTokenSubcontructors.pop_back();
		}
		else
			hasActive = true;
	}
	return hasActive;
}

void PreProcessor::ResetReadState()
{
	mTokenizer.ResetReadState();
}

const char* scMonthsShortNames[] = {"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};

BoolAndString PreProcessor::GetDateMacroToken()
{
	// Supposed to be compilation time...well...use the current time.
	// format is Mmm dd yyyy
	time_t currentTime;
	tm structuredLocalTime;

	time(&currentTime);
	SAFE_LOCAL_TIME(structuredLocalTime,currentTime);
	
	char aString[12];

	SAFE_SPRINTF_3(aString,12,"%s %2d %4d",scMonthsShortNames[structuredLocalTime.tm_mon],structuredLocalTime.tm_mday,structuredLocalTime.tm_year + 1900);
				 
	return BoolAndString(true,aString); 
}

BoolAndString PreProcessor::GetFileMacroToken()
{
	return BoolAndString(true,
			mIncludeProvidersStack.size() > 0 ? 
				((IncludeFileTokenProvider*)mIncludeProvidersStack.back())->GetSourceFileNameForMacro() :
				mSourceFileNameForMacro);
}

BoolAndString PreProcessor::GetLineMacroToken()
{
	return BoolAndString(true,Long(
			mIncludeProvidersStack.size() > 0 ? 
				((IncludeFileTokenProvider*)mIncludeProvidersStack.back())->GetCurrentLineIndex() :
				mPreTokenizationDecoder.GetCurrentLineIndex()
			).ToString());
}

BoolAndString PreProcessor::GetSTDCMacroToken()
{
#ifdef __STDC__
	return BoolAndString(true,#__STDC__);
#else
	return GetNextToken(); // Gal, Should i be kind enough to issue a warning?
#endif
}

BoolAndString PreProcessor::GetTimeMacroToken()
{
	// like in date, i'm using current time here
	// format is hh:mm:ss

	
	time_t currentTime;
	tm structuredLocalTime;

	time(&currentTime);
	SAFE_LOCAL_TIME(structuredLocalTime,currentTime);
	
	char aString[9];

	SAFE_SPRINTF_3(aString,9,"%2d:%2d:%2d",structuredLocalTime.tm_hour,structuredLocalTime.tm_min,structuredLocalTime.tm_sec);
				 
	return BoolAndString(true,aString); 
}

// tm_wday

const char* scDaysShortNames[] = { "Sun","Mon","Tue","Wed","Thu","Fri","Sat"};


BoolAndString PreProcessor::GetTimeStampMacroToken()
{
	// Get last save time of the file at hand (consider includes!)
	// formatted as Ddd Mmm Date hh:mm:ss yyyy

	tm structuredTime;

	WindowsFileSystem fileSystem;

	structuredTime = fileSystem.GetFileModificationTime(mSourceFileName).second;

	char aString[25];

	SAFE_SPRINTF_7(aString,25,"%s %s %2d %2d:%2d:%2d %4d",scDaysShortNames[structuredTime.tm_wday],
														  scMonthsShortNames[structuredTime.tm_mon],
														  structuredTime.tm_mday,
														  structuredTime.tm_hour,
														  structuredTime.tm_min,
														  structuredTime.tm_sec,
														  structuredTime.tm_year + 1900);
		

	return BoolAndString(true,aString);
}

BoolAndString PreProcessor::FirePreprocessorError()
{
	// k. this means error situation, should return error message as a string. also trace it
	BoolAndString bufferTokenReader = GetNextToken();

	if(bufferTokenReader.first)
	{
		bool foundEndLine = IsNewLineToken(bufferTokenReader.second);
		
		if(foundEndLine)
		{
			return BoolAndString(false,"");
		}
		else
		{
			stringstream bufferStream;
			bufferStream<<bufferTokenReader.second;

			while(!foundEndLine)
			{
				bufferTokenReader = GetNextToken();
				if(!bufferTokenReader.first) // a "failure" here is not really a failure - it may be the file end
					return BoolAndString(false,"");
				if(IsNewLineToken(bufferTokenReader.second))
				{
					foundEndLine = true;
				}
				else
				{
					bufferStream<<" "<<bufferTokenReader.second;
				}
			}
			TRACE_LOG1("PreProcessor::FirePreprocessorError, Preprocessor error command encountered. message is : %s",bufferStream.str().c_str());

			return BoolAndString(false,bufferStream.str());
		}
	}
	else
		return bufferTokenReader;
}

bool PreProcessor::IncludeFile()
{
	// To read the file name one must read the content after the #include command, that will be either "XXXXX" or <XXXXX>.
	// note that regualr syntax rules don't follow here. for example, if the file path/name is surrounded by double quotes, it doesn't make it a string.
	// the pre-processor should simply skip the spaces, get to eitehr < or " and then read anything past that until a matching > or ". then skip till end of line
	// whatever read is the file path
	BoolAndString fileNameToken = GetNextNoSpaceEntity();
	
	if(!fileNameToken.first)
	{
		TRACE_LOG("PreProcessor::IncludeFile, unable to get file name token entity for #include");
		return false;
	}

	if(fileNameToken.second.size() < 2 || 
		(fileNameToken.second.at(0) == '<' && fileNameToken.second.at(fileNameToken.second.length() - 1) != '>') ||
		(fileNameToken.second.at(0) == '\"' && fileNameToken.second.at(fileNameToken.second.length() - 1) != '\"'))
	{
		TRACE_LOG1("PreProcessor::IncludeFile, syntax error with file name token for #include - %s",fileNameToken.second.c_str());
		return false;
	}

	BoolAndString fileFindResult = FindFile(fileNameToken.second.substr(1,fileNameToken.second.length()-2),fileNameToken.second.at(0) == '\"');
	if(!fileFindResult.first)
	{
		TRACE_LOG1("PreProcessor::IncludeFile, unable to find include file. file name = %s",fileNameToken.second.c_str());
		return false;
	}

	// make sure to include only if didn't include yet, where #pragma once was used
	if(mDontInclude.find(fileFindResult.second) == mDontInclude.end())
	{

		mTokenSubcontructors.push_back(new IncludeFileTokenProvider(fileFindResult.second));

		// register include tokenizers for searching purposes
		mIncludeProvidersStack.push_back(mTokenSubcontructors.back());
	}
	return true;
}

BoolAndString PreProcessor::GetNextNoSpaceEntity()
{
	BoolAndString tokenizerResult;
	
	// preprocessor implementations are mostly implemented by employing subcontructors
	// that get tokens according to the matching macro, ifdef section or whatnot.
	if(DetermineIfHasActiveSubcontructor())
		mTokenSubcontructors.back()->GetNextNoSpaceEntity();
	else
		tokenizerResult = mTokenizer.GetNextNoSpaceEntity();
	
	if(!tokenizerResult.first)
		return tokenizerResult;

	// first thing to realize is whether we have an defined identifier here. if this is the case, need to stop and switch context
	// to idetnfier
	StringToDefineIdentifierDefinitionMap::iterator itDefine = mDefines.find(tokenizerResult.second);
	if(itDefine != mDefines.end())
	{
		if(mUsedDefines.find(tokenizerResult.second) != mUsedDefines.end())
		{
			TRACE_LOG1("PreProcessor::GetNextToken, circular macro usage identified, ingoring macro replacement. identifier = %s",tokenizerResult.second.c_str());
		}
		else
		{
			// hurrah! define symbol. move control to a symbol definition
			ITokenProvider* provider = itDefine->second->CreateNoSpaceEntityProvider(this);
			if(!provider)
				return BoolAndString(false,"");
			mTokenSubcontructors.push_back(provider);
			mUsedDefines.insert(tokenizerResult.second);
			mIdentifierTokens.insert(ITokenProviderToStringMap::value_type(provider,tokenizerResult.second));
			return GetNextNoSpaceEntity();
		}
	}

	// now let's try some predefined macros
	BoolAndBoolAndString predefinedMacrosResult = HandlePredefinedMacros(tokenizerResult.second);
	if(predefinedMacrosResult.first)
		return predefinedMacrosResult.second;
	else
		return tokenizerResult;

	// note that many other cases that are checked in GetNextToken are not checked here. this is because they just don't make sense here,
	// and should therefore trigger errors, which they will if used as literal tokens
}


BoolAndString PreProcessor::FindFile(string inIncludeString,bool inIsDoubleQuoteSearch)
{
	/* search order is this:
		for double quotes search in:
			1. Same directory as file that includes the #include statement
			2. continue with other files in the current include context
		then, or for angle brackets start from here:
			3. the input directories list
	*/

	ITokenProviderList::reverse_iterator itIncludeStack = mIncludeProvidersStack.rbegin();
	WindowsPath filePath(inIncludeString);
	WindowsFileSystem fileSystem;
	BoolAndString result(false,"");

	if(filePath.IsAbsolute())
	{
		return BoolAndString(fileSystem.FileExists(filePath),inIncludeString);
	}

	do
	{
		if(inIsDoubleQuoteSearch)
		{
			for(; itIncludeStack != mIncludeProvidersStack.rend() && !result.first;++itIncludeStack)
				result = fileSystem.GetExistingFilePath(((IncludeFileTokenProvider*)*itIncludeStack)->GetIncludeFileFolder(),filePath);
			if(result.first)
				break;

			result = fileSystem.GetExistingFilePath(mSourceFileFolderPath,filePath);
			if(result.first)
				break;
		}

		WindowsPathList::iterator itIncludes = mIncludeFolders.begin();
		for(; itIncludes != mIncludeFolders.end() && !result.first;++itIncludes)
			result = fileSystem.GetExistingFilePath(*itIncludes,filePath);

	} while (false);

	return result;
}

bool PreProcessor::ModifyLineAndfile()
{
	// get the values of line and file
	bool status = false;
	long newLineIndex;
	string newFileName;

	do
	{
		// get the first value, which is mandatory, it needs to be a line number, simply...a positive, and should start with a digit
		BoolAndString tokenResult = GetNextToken();

		if(!tokenResult.first)
		{
			TRACE_LOG("PreProcessor::ModifyLineAndfile, Error in reading token for #line identifier");
			break;
		}

		if(tokenResult.second.at(0) < '0' || tokenResult.second.at(0) > '9')
		{
			TRACE_LOG1("PreProcessor::ModifyLineAndfile, first parameter for line is not a number, %s. should be a line number",tokenResult.second.c_str());
			break;
		}

		// parse the first toke, which is the line number
		newLineIndex = Long(tokenResult.second);

		// k. new param, file name. note that this is (like #include) is not a regular string, but a filename
		tokenResult = GetNextNoSpaceEntity();
		if(!tokenResult.first || IsNewLineToken(tokenResult.second))
		{
			status = true;
			break; // done here
		}
		
		if(tokenResult.second.at(0) != '\"' || tokenResult.second.size() < 2 || tokenResult.second.at(tokenResult.second.length()-1) != '\"')
		{
			TRACE_LOG1("PreProcessor::ModifyLineAndfile, syntax error in second parameter for #line. should be a filename in double quotes, %s",tokenResult.second.c_str());
			break;
		}

		newFileName = tokenResult.second.substr(1,tokenResult.second.length()-2);

		status = true;
	}while(false);

	if(status) // that's right - will ignore empty file name
		SetNewFileAndLine((unsigned long)newLineIndex,newFileName);
	return status;
}

void PreProcessor::SetNewFileAndLine(unsigned long inNewLineIndex,const string& inNewFileName)
{
	// first, find the right location to set the file name on.

	if(mIncludeProvidersStack.size() > 0)
	{
		((IncludeFileTokenProvider*)mIncludeProvidersStack.back())->SetNewFileAndLine(inNewLineIndex,inNewFileName);
	}
	else
	{
		if(inNewFileName.size() > 0)
			mSourceFileNameForMacro = inNewFileName;
		mPreTokenizationDecoder.SetNewLineIndex(inNewLineIndex);
	}
}

bool PreProcessor::ImplementPragmaDirective()
{
	BoolAndString tokenResult = GetNextToken();

	if(!tokenResult.first)
	{
		TRACE_LOG("PreProcessor::ImplementPragmaDirective, unable to read pragma type");
		return false;
	}

	if(tokenResult.second == "once")
	{
		mDontInclude.insert(mIncludeProvidersStack.size() > 0 ? 
				((IncludeFileTokenProvider*)mIncludeProvidersStack.back())->GetSourceFileName() :
				mSourceFileName);
	}
	else
		TRACE_LOG1("PreProcessor::ImplementPragmaDirective, unimplemented pragma %s. The only supported pragma is 'once'",tokenResult.second.c_str());

	FlushTillEndOfLine();
	return true;
}

bool PreProcessor::InterpretConditionalTokenization(const string& inConditionalKeyword)
{
	BoolAndBool evaluateResult = EvaluateConstantExpression(inConditionalKeyword);

	if(!evaluateResult.first)
	{
		TRACE_LOG1(
			"PreProcessor::InterpretConditionalTokenization, unable to evaluate condition for %s. stopping", 
			inConditionalKeyword.c_str());
		return false;
	}

	// condition evaluated to true, stop here
	if(evaluateResult.second)
	{
		SetupCondtionalIteration();
		return true;
	}

	// condition evaluate to false, continue till true condition is found or endif reached
	bool foundTrue = false;
	bool status = true;

	while(!foundTrue && status)
	{
		BoolAndString tokenizerResult = GetNextToken();

		if(!tokenizerResult.first)
		{
			status = false;
			TRACE_LOG("PreProcessor::InterpretConditionalTokenization, unexpected read error, unable to tokenized skipped condition");
			break;
		}

		if(tokenizerResult.second == "#else")
		{
			foundTrue = true;
		}
		else if(tokenizerResult.second == "#elif")
		{
			evaluateResult = EvaluateConstantExpression(inConditionalKeyword);
			if(!evaluateResult.first)
			{
				TRACE_LOG("PreProcessor::InterpretConditionalTokenization, unable to evaluate condition for #elif. stopping");
				status = false;
			}

			if(evaluateResult.second)
				foundTrue = true;
		}
		else if(tokenizerResult.second == "#endif")
		{
			foundTrue = false;
			ResetConditionalIteration();
			break;
		}
	}

	if(status && foundTrue)
		SetupCondtionalIteration();
	return status;
}

void PreProcessor::SetupCondtionalIteration()
{
	++mConditionalIteration;
}

void PreProcessor::ResetConditionalIteration()
{
	--mConditionalIteration;
}

bool PreProcessor::InterpretConditionalStopper(const string& inConditionalKeyword)
{
	// if not while in condtional scope, then we have an unexpected encounter. this means error
	if(mConditionalIteration == 0)
	{
		TRACE_LOG1("PreProcessor::InterpretConditionalStopper, unexpected condtional keyword while not in conditional state - %s",
					inConditionalKeyword.c_str());
		return false;
	}
	
	bool status = true;

	do
	{
		// if reached #elif or #else, skip tokens till endif
		if(inConditionalKeyword == "#elif" || inConditionalKeyword == "else")
		{
			bool foundEndif = false;

			while(!foundEndif && status)
			{
				BoolAndString tokenizerResult = GetNextToken();

				if(!tokenizerResult.first)
				{
					status = false;
					TRACE_LOG("PreProcessor::InterpretConditionalStopper, unexpected read error, unable to tokenized skipped condition");
					break;
				}	

				if(tokenizerResult.second == "#endif")
					foundEndif = true;
			}
		}
		if(!status)
			break;

		// so now should be past #endif [either skipped, or reached]
		ResetConditionalIteration();

	}while(false);
	return status;
}


BoolAndBool PreProcessor::EvaluateConstantExpression(const string& inConditionType)
{
	// TODO, Gal

	return BoolAndBool(false,false);
}