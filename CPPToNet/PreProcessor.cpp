#include "PreProcessor.h"
#include "DefineIdentifierDefinition.h"
#include "Trace.h"
#include "ITokenProvider.h"
#include "BoxingBase.h"
#include "IncludeFileTokenProvider.h"

using namespace Hummus;

PreProcessor::PreProcessor(void)
{
}

PreProcessor::~PreProcessor(void)
{
	StringToDefineIdentifierDefinitionMap::iterator it = mDefines.begin();

	for(; it != mDefines.end(); ++it)
		delete it->second;
	mDefines.clear();

	ITokenProviderList::iterator itTokenProviders = mTokenSubcontructors.begin();
	for(; itTokenProviders != mTokenSubcontructors.end();++itTokenProviders)
		delete *itTokenProviders;
	mTokenSubcontructors.clear();
}

void PreProcessor::Setup(IByteReader* inSourceStream,
						 const string& inSourceFileName,
					     const StringList& inPreprocessorDefinitions,
						 const StringList& inIncludeFolders)
{
	mPreTokenizationDecoder.SetStream(inSourceStream);
	mTokenizer.SetReadStream(&mPreTokenizationDecoder);

	StringList::const_iterator itDefs = inPreprocessorDefinitions.begin();
	
	for(; itDefs != inPreprocessorDefinitions.end(); ++itDefs)
		mDefines.insert(StringToDefineIdentifierDefinitionMap::value_type(*itDefs,new DefineIdentifierDefinition));

	mSourceFileName = inSourceFileName;

	// TBD on inIncludeFolders
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
	if(tokenizerResult.second == "__DATE__")
	{
		return GetDateMacroToken();
	}
	else if(tokenizerResult.second == "__FILE__")
	{
		return GetFileMacroToken();
	}
	else if(tokenizerResult.second == "__LINE__")
	{
		return GetLineMacroToken();
	}
	else if(tokenizerResult.second == "__STDC__")
	{
		return GetSTDCMacroToken();
	}
	else if(tokenizerResult.second == "__TIME__")
	{
		return GetTimeMacroToken();
	}
	else if(tokenizerResult.second == "__TIMESTAMP__")
	{
		return GetTimeStampMacroToken();
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
		readResult = mTokenizer.GetNextToken();
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
				readResult = mTokenizer.GetNextToken();
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

		// now read the replacement token strings
		bool foundEndLine = false;

		while(!foundEndLine)
		{
			readResult = mTokenizer.GetNextToken();
			if(!readResult.first)
			{
				TRACE_LOG("PreProcessor::DefineIdentifierReplacement, Error in reading token for identifier token strings");
				status = false;
				break;
			}
			if(IsNewLineToken(readResult.second))
				foundEndLine = true;
			else
				newDefine->PushTokenString(readResult.second);		
		}
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

bool PreProcessor::UndefIdentifierReplacement()
{
	// k. this is simple. next identifier should be the symbol and that's it. flush the rest of the tokens till end line
	BoolAndString tokenResult = mTokenizer.GetNextToken();

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
		tokenResult = mTokenizer.GetNextToken();
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
			ITokenProviderToStringMap::iterator it = mIdentifierTokens.find(mTokenSubcontructors.back());
			if(it != mIdentifierTokens.end())
			{
				mUsedDefines.erase(it->second);
				mIdentifierTokens.erase(it);
			}
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
	return BoolAndString(true,mSourceFileName);
}

BoolAndString PreProcessor::GetLineMacroToken()
{
	return BoolAndString(true,Long(mPreTokenizationDecoder.GetCurrentLineIndex()).ToString());
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
	BoolAndString bufferTokenReader = mTokenizer.GetNextToken();

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
				bufferTokenReader = mTokenizer.GetNextToken();
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
	BoolAndString bufferTokenReader = mTokenizer.GetNextToken();

	if(!bufferTokenReader.first)
	{
		TRACE_LOG("PreProcessor::IncludeFile, cannot read next token in #include preprocessor command");
		return false;
	}

	// Gal: OOPS! gotta read a < as a single token after #include!!!!
	string fileName;

	if(bufferTokenReader.second.size() > 1 && 
		((bufferTokenReader.second.at(0) == '\"' && bufferTokenReader.second.at(bufferTokenReader.second.size() - 1) == '\"') ||
		 (bufferTokenReader.second.at(0) ==  '<' && bufferTokenReader.second.at(bufferTokenReader.second.size() - 1) == '>')))
		 // must be either <XXXXXXX> or "XXXXXXXX"
	{
		// string type include
		fileName = bufferTokenReader.second.substr(1,bufferTokenReader.second.length()-2);
		SkipToNextLine();
	}
	else
	{
		TRACE_LOG1("PreProcessor::IncludeFile, unrecognized token after #include - %s",bufferTokenReader.second.c_str());
		return false;
	}

	BoolAndString fileFindResult = FindFile(fileName,bufferTokenReader.second.at(0) == '\"');
	if(!fileFindResult.first)
	{
		TRACE_LOG1("PreProcessor::IncludeFile, unable to find include file. file name = %s",fileName.c_str());
		return false;
	}

	mTokenSubcontructors.push_back(new IncludeFileTokenProvider(fileFindResult.second));
	return true;
}

void PreProcessor::SkipToNextLine()
{
	bool foundEndLine = false;
	BoolAndString readResult;

	while(!foundEndLine)
	{
		readResult = mTokenizer.GetNextToken();
		if(!readResult.first)
			break;
		if(IsNewLineToken(readResult.second))
			foundEndLine = true;
	}
}