#include "DefineIdentifierDefinition.h"
#include "SimplePreprocessorValueIterator.h"
#include "PreprocessorFunctionIterator.h"
#include "PreProcessor.h"
#include "Trace.h"

DefineIdentifierDefinition::DefineIdentifierDefinition(void)
{
}

DefineIdentifierDefinition::~DefineIdentifierDefinition(void)
{
}

void DefineIdentifierDefinition::PushParameter(const string& inParameter)
{
	mParameters.push_back(inParameter);
}

void DefineIdentifierDefinition::PushTokenString(const string& inTokenString)
{
	mTokenStrings.push_back(inTokenString);
}

ITokenProvider* DefineIdentifierDefinition::CreateTokenProvider(PreProcessor* inTokenSource)
{
	if(mParameters.size() == 0)
	{
		return new SimplePreprocessorValueIterator(mTokenStrings);
	}
	else
	{
		// first make sure the next token is a function opener
		BoolAndString readToken = inTokenSource->GetNextToken();
		if(!readToken.first || readToken.second != "(")
		{
			TRACE_LOG("DefineIdentifierDefinition::CreateTokenProvider, can't read openening paranthesis for macro call");
			return NULL;
		}

		bool statusOK = true;
		bool stopEncountered = false;
		PreprocessorFunctionIterator* iterator = new PreprocessorFunctionIterator();

		StringList::iterator itParameters = mParameters.begin();
		// loop parameters. for each scan for the next parameter in the list
		for(; itParameters != mParameters.end() && statusOK; ++itParameters)
		{
			if(stopEncountered)
			{
				// problem!
				TRACE_LOG("DefineIdentifierDefinition::CreateTokenProvider, unexpected end of parameters list. too few parameters passed");
				statusOK = false;
				break;
			}
			if(*itParameters == "...") // variadic macro!
			{
				StringList& tokensForParameter = iterator->CreateNewParameter("__VA_ARGS__");
				while(statusOK && !stopEncountered)
				{
					statusOK = ScanParameter(tokensForParameter,inTokenSource,stopEncountered);
					if(!stopEncountered)
						tokensForParameter.push_back(",");
				}
				
			}
			else
			{
				StringList& tokensForParameter = iterator->CreateNewParameter(*itParameters);
				statusOK = ScanParameter(tokensForParameter,inTokenSource,stopEncountered);
			}
		}

		do
		{
			if(!statusOK)	
				break;

			// make sure we're at the end
			if(!stopEncountered)
			{
				TRACE_LOG("DefineIdentifierDefinition::CreateTokenProvider, should have encountered end of parameters list. this means that macro call has too many parameters");
				statusOK = false;
				break;
			}

			// instruct iterator to now compose the full expression, with interpreted parameters
			iterator->ComposeExpression(mTokenStrings);
		}while(false);

		if(!statusOK)
		{
			delete iterator;
			return NULL;
		}
		else
			return iterator;
			
	}
		
}

bool DefineIdentifierDefinition::ScanParameter(StringList& inTokens,PreProcessor* inTokenSource,bool& outStopEncountered)
{
	// ok. i should grab here one expression, which end is ",". pay attention that there "," may be part
	// of {},[],(),<> in which case it does not count

	int nestingLevel = 0;
	bool statusOK = true;
	bool foundEnd = false;
	outStopEncountered = false;

	while(statusOK && !foundEnd)
	{
		BoolAndString tokenResult = inTokenSource->GetNextToken();
		if(!tokenResult.first)
		{
			statusOK = false;
			TRACE_LOG("DefineIdentifierDefinition::ScanParameter, unexpected end of read, while scanning parameter values");
			break;
		}

		if(tokenResult.second == "," && 0 == nestingLevel)
		{
			foundEnd = true;
			break;
		}

		if(tokenResult.second == "(" || tokenResult.second == "{" || tokenResult.second == "[" || tokenResult.second == "<")
			++nestingLevel;

		if(tokenResult.second == ")" || tokenResult.second == "}" || tokenResult.second == "]" || tokenResult.second == ">")
		{
			--nestingLevel;
			if(nestingLevel < 0)
			{
				if(tokenResult.second == ")")
				{
					// this marks the end of the last parameter
					outStopEncountered = true;
					foundEnd = true;
					break;
				}
				else
				{
					statusOK = false;
					TRACE_LOG("DefineIdentifierDefinition::ScanParameter, unexpected ending token. probably syntax error. not balanced },> or ]");
					break;
				}

			}
		}
		inTokens.push_back(tokenResult.second);
	}

	return statusOK;
}
