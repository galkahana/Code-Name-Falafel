#include "PreProcessor.h"
#include "DefineIdentifierDefinition.h"
#include "Trace.h"
#include "ITokenProvider.h"
#include "BoxingBase.h"
#include "IncludeFileTokenProvider.h"
#include "WindowsFileSystem.h"

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
	bool foundEndLine = false;

	while(!foundEndLine)
	{
		tokenResult = GetNextToken();
		if(!tokenResult.first) // a "failure" here is not really a failure - it may be the file end
			return false;
		if(IsNewLineToken(tokenResult.second)) 
			foundEndLine = true;
	}
	return true;
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

BoolAndString PreProcessor::GetDateMacroToken()
{
	return BoolAndString(true,""); // Gal, Todo
}

BoolAndString PreProcessor::GetFileMacroToken()
{
	return BoolAndString(true,
			mIncludeProvidersStack.size() > 0 ? 
				((IncludeFileTokenProvider*)mIncludeProvidersStack.back())->GetSourceFileName() :
				mSourceFileName);
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
	return BoolAndString(true,""); // Gal, Todo
}

BoolAndString PreProcessor::GetTimeStampMacroToken()
{
	return BoolAndString(true,""); // Gal, Todo
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

	mTokenSubcontructors.push_back(new IncludeFileTokenProvider(fileFindResult.second));

	// register include tokenizers for searching purposes
	mIncludeProvidersStack.push_back(mTokenSubcontructors.back());
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

void PreProcessor::SkipToNextLine()
{
	bool foundEndLine = false;
	BoolAndString readResult;

	while(!foundEndLine)
	{
		readResult = GetNextToken();
		if(!readResult.first)
			break;
		if(IsNewLineToken(readResult.second))
			foundEndLine = true;
	}
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