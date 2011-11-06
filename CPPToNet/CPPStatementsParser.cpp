#include "CPPStatementsParser.h"
#include "HeaderUnit.h"
#include "Trace.h"
#include "CPPNamespace.h"

using namespace Hummus;

CPPStatementsParser::CPPStatementsParser(void):mUnnamedSequance(0)
{
}

CPPStatementsParser::~CPPStatementsParser(void)
{
}


EStatusCodeAndHeaderUnit CPPStatementsParser::Parse(IByteReader* inSourceStream,
												 const string& inSourceFileName,
												 const StringToStringMap& inPreprocessorDefinitions,
												 const StringList& inIncludeFolders)
{
	

	// setup 
	mTokensSource.Setup(inSourceStream,inSourceFileName,inPreprocessorDefinitions,inIncludeFolders);
	
	// parsing
	return ParseUnit();
}


EStatusCodeAndHeaderUnit CPPStatementsParser::ParseUnit()
{
	EStatusCodeAndBool statementParsingResult;
	HeaderUnit* result = new HeaderUnit();
	mWorkingUnit = result;

	// take care of global namespace and starting an initial local context
	mDefinitionContextStack.push_back(result->GetGlobalNamespace());
	StartLocalContext();

	do
	{
		statementParsingResult = ParseStatement(result);
	}while(statementParsingResult.first == eSuccess && statementParsingResult.second);

	// finalize environment
	mDefinitionContextStack.clear();
	EndLocalContext();

	if(eSuccess == statementParsingResult.first)
	{
		return EStatusCodeAndHeaderUnit(eSuccess,result);
	}
	else
	{
		delete result;
		return EStatusCodeAndHeaderUnit(eFailure,NULL);
	}
}

EStatusCodeAndBool CPPStatementsParser::ParseStatement(HeaderUnit* inUnitModel)
{
	EStatusCode status = eSuccess;
	BoolAndString tokenizerResult = GetNextToken();

	// currently no treatment of errors in the tokenizer....need to add that....
	if(!tokenizerResult.first)
		return EStatusCodeAndBool(eSuccess,false);
	
	// now for the interesting part, based on the first token determine what kind of statement this is
	// and parse accordingly.

	do
	{

		if(tokenizerResult.second == "namespace")
		{
			status = ParseNamespaceDeclaration();
		}
		else if(tokenizerResult.second == "using")
		{
			tokenizerResult = GetNextToken();
			if(!tokenizerResult.first)
			{
				status = eFailure;
				TRACE_LOG("CPPStatementsParser::ParseStatement, unexpected end of file while looking for next token after 'using' keyword");
				break;
			}

			if(tokenizerResult.second == "namespace")
				status = ParseUsingDirective();
			else
			{
				PutBackToken(tokenizerResult.second);
				status = ParseUsingDeclaration();
			}
		}
		else if(tokenizerResult.second == "class")
		{
			status = ParseClassDeclaration();
		}
		else if(tokenizerResult.second == "struct")
		{
			status = ParseStructDeclaration();
		}
		else if(tokenizerResult.second == "union")
		{
			status = ParseUnionDeclaration();
		}
		else if(tokenizerResult.second == "typedef")
		{
			status = ParseTypedefDeclaration();
		}
		else if(tokenizerResult.second == "enum")
		{
			status = ParseEnumeratorDeclaration();
		}
		else if(tokenizerResult.second == "template")
		{
			status = ParseTemplateDeclaration();
		}
		else if(tokenizerResult.second == "}")
		{
			// namespace closer (note that namespace must always contain at least one namespace for the global namespace)
			if(mDefinitionContextStack.size() > 1)
			{
				mDefinitionContextStack.pop_back();
				status = eSuccess;
			}
			else
			{
				TRACE_LOG("CPPStatementsParser::ParseStatement, syntax error, unexpected block closer '}'");
				status = eFailure;
			}
		}
		else
		{
			// in addition to the above statements there are also:
			// labeled statements, block statements, selection statements, iteration statements, 
			// jump statements, all occuring in or as function defintions...which i don't care about
			// and don't expect to have at the interesting level of the header file. as such
			// i'm ignoring skipping them. if in the future i'll see that for some reason ppl use 
			// them and that this usage is allowed in C++, i'll write code to skip these statements.

			// this means that now we are left with three types of statements:
			// function declarations(and/or definitions), variables declarations and expression statements (like function calls or assignments)
			// only the first two types interest me (and i'm not really sure i should expect expression statements at the first level...but let's keep them just in case
		
			// so now i have to identify what i got. deferring function declarations from variable declarations is easy, due to the usage of
			// paranthesis after the name of the function/variable if this is a function. prior to that both kinds of
			// declarations look exactly the same. after...it's a matter of what is identified.
			
			// but how do i know that this is not actually an expression statements? easy. what begins a function/variable declaration
			// are some specific specifiers (like "static") and then a type name...which should be known to be a type name...meaning i should
			// already have it in the state. so..if this is the case...i know that i have a function/variable declarations...otherwise assume
			// that this is an expression statements. which i can simply skip till the semicolon. i could also just parse the thing but then what's the point.
			// another method of realizing whether this is an expression statements (just describing for fun) is that it will start with something that
			// should be an operand, or unary operators + operand. which means it will refer to a known variable, or simply have primitives.
			// but then again, i can just use the first method...

			// Determine if we see here a declaration of function or variable.

			// TODO

			status = eFailure;
		}

	}while(false);

	return EStatusCodeAndBool(status,true);
}

BoolAndString CPPStatementsParser::GetNextToken()
{
	if(mTokensStack.size() == 0)
	{
		return mTokensSource.GetNextToken();
	}
	else
	{
		string nextToken = mTokensStack.back();
		mTokensStack.pop_back();
		return BoolAndString(true,nextToken);
	}
}

void CPPStatementsParser::PutBackToken(const string& inToken)
{
	mTokensStack.push_back(inToken);
}


EStatusCode CPPStatementsParser::ParseNamespaceDeclaration()
{
	BoolAndString tokenizerResult = GetNextToken();

	if(!tokenizerResult.first)
	{
		TRACE_LOG("CPPStatementsParser::ParseNamespaceDeclaration, syntax error, namespace declared but no identifier provided as name");
		return eFailure;
	}

	// unnamed namespace case
	if(tokenizerResult.second == "{")
	{
		// namespace { }
		if(((CPPElement*)mDefinitionContextStack.back())->Type != CPPElement::eCPPElementNamespace)
		{
			TRACE_LOG("CPPStatementsParser::ParseNamespaceDeclaration, syntax error, current context is not namespace, cannot define an unnamed namespace");
			return eFailure;
		}

		CPPNamespace* newNamespace = ((CPPNamespace*)mDefinitionContextStack.back())->CreateNewNamespace(mUnnamedSequance.ToString());

		if(!newNamespace)
			return eFailure;

		mDefinitionContextStack.push_back(newNamespace);
		mUnnamedSequance = (long)mUnnamedSequance + 1;
		
	}
	else
	{


		string namespaceName = tokenizerResult.second;

		// next char can be either a block opener (regular case) or equality (alias)
		tokenizerResult = GetNextToken(); 
		if(!tokenizerResult.first)
		{
			TRACE_LOG("CPPStatementsParser::ParseNamespaceDeclaration, syntax error, no block opener after namespace declaration");
			return eFailure;
		}
		
		if(tokenizerResult.second == "=")
		{
			// namespace XXXA = XXX;

			tokenizerResult = GetNextToken(); 
			if(!tokenizerResult.first)
			{
				TRACE_LOG("CPPStatementsParser::ParseNamespaceDeclaration, syntax error, no namespace name for alias namespace definition");
				return eFailure;
			}

			CPPElement* anElement = FindUnqualifiedElement(tokenizerResult.second);
			CPPNamespace* aNamespace = (anElement ? (anElement->Type == CPPElement::eCPPElementNamespace ? (CPPNamespace*)anElement:NULL): NULL);

			if(!aNamespace)
			{
				TRACE_LOG1("CPPStatementsParser::ParseNamespaceDeclaration, unrecognized namespace name for alias - %s", tokenizerResult.second.c_str());
				return eFailure;
			}

			if(((CPPElement*)mDefinitionContextStack.back())->Type != CPPElement::eCPPElementNamespace)
			{
				TRACE_LOG("CPPStatementsParser::ParseNamespaceDeclaration, syntax error, current context is not namespace, cannot define a namespace alias");
				return eFailure;
			}

			if(((CPPNamespace*)mDefinitionContextStack.back())->CreateNewNamespaceAlias(namespaceName,aNamespace) != eSuccess)
				return eFailure;				

			return SkipSemiColon();
		}
		else if(tokenizerResult.second != "{")
		{
			TRACE_LOG1("CPPStatementsParser::ParseNamespaceDeclaration, syntax error, expected block opener '{' after namespace decalaration, instead recieved - %s",tokenizerResult.second.c_str());
			return eFailure;
		}
		else
		{
			// namespace XXXA { }
			if(((CPPElement*)mDefinitionContextStack.back())->Type != CPPElement::eCPPElementNamespace)
			{
				TRACE_LOG("CPPStatementsParser::ParseNamespaceDeclaration, syntax error, current context is not namespace, cannot define a namespace");
				return eFailure;
			}

			CPPNamespace* newNamespace = ((CPPNamespace*)mDefinitionContextStack.back())->CreateNewNamespace(mUnnamedSequance.ToString());

			if(!newNamespace)
				return eFailure;


			mDefinitionContextStack.push_back(newNamespace);
		}
	}
	return eSuccess;
}

EStatusCode CPPStatementsParser::ParseUsingDirective()
{
	// next token should be the namespace to be used
	BoolAndString tokenizerResult = GetNextToken(); 
	if(!tokenizerResult.first || tokenizerResult.second == ";")
	{
		TRACE_LOG("CPPStatementsParser::ParseUsingDirective, syntax error, using directive does not provide a namespace name");
		return eFailure;
	}

	CPPElement* anElement = FindUnqualifiedElement(tokenizerResult.second);
	CPPNamespace* aNamespace = (anElement ? (anElement->Type == CPPElement::eCPPElementNamespace ? (CPPNamespace*)anElement:NULL): NULL);

	if(!aNamespace)
	{
		TRACE_LOG1("CPPStatementsParser::ParseUsingDirective, unrecognized namespace name - %s", tokenizerResult.second.c_str());
		return eFailure;
	}

	AddNamespaceToUnqualifiedSearch(aNamespace);

	// and skip also the final semi-colon
	return SkipSemiColon();
}

Hummus::EStatusCode CPPStatementsParser::SkipSemiColon()
{
	BoolAndString tokenizerResult = GetNextToken(); 
	if(!tokenizerResult.first)
	{
		TRACE_LOG("CPPStatementsParser::SkipSemiColon, syntax error, expected Semi Colon and couldn't read further");
		return eFailure;
	}

	if(tokenizerResult.second != ";")
	{
		TRACE_LOG1("CPPStatementsParser::SkipSemiColon, syntax error, expected Semi Colon and found - %s",tokenizerResult.second.c_str());
		return eFailure;
	}


	return eSuccess;
}

void CPPStatementsParser::AddNamespaceToUnqualifiedSearch(CPPNamespace* inNamespace)
{
	//that's actually an easy one :)
	mLocalContext.back().mUsedNamespaces.insert(inNamespace);
}

void CPPStatementsParser::StartLocalContext()
{
	mLocalContext.push_back(LocalContext());
}

void CPPStatementsParser::EndLocalContext()
{
	if(mLocalContext.size() > 0) 
		mLocalContext.pop_back();

	// i'm using the local contexts as utility for this class.. expecting not to have failures here.
}

EStatusCode CPPStatementsParser::ParseUsingDeclaration()
{
	// In top level, using declaration can refer only to a qualified namespace type (and not to a class type)
	// it's of the form - using [<namespace name>]::<typename>, and it adds to either a current defined namespace or the global namespace, the type as a synonym

	// get qualifiied object
	CPPElement* anElement = GetElementFromCurrentLocation();
	if(!anElement)
	{
		TRACE_LOG("CPPStatementsParser::ParseUsingDeclaration, error in using declaration, could not find designated type");
		return eFailure;
	}

	// define it in this namespace (either local or global namespace)
	return mDefinitionContextStack.back()->DefineAlias(anElement->Name,anElement);
}

CPPElement* CPPStatementsParser::GetElementFromCurrentLocation()
{
	// a name can be either:
	// XXX
	// [::]YYY[::AAA::ZZZ...]::XXX
	CPPElement* anElement = NULL;

	BoolAndString firstToken = GetNextToken();
	do
	{
		if(!firstToken.first)
		{
			TRACE_LOG("CPPStatementsParser::GetElementFromCurrentLocation, no symbol in current location");
			break;
		}

		BoolAndString secondToken = GetNextToken();

		// differ qualified from non-qualified states
		if(firstToken.second == "::" || (secondToken.first && secondToken.second == "::"))
		{
			// qualified
			anElement = (firstToken.second == "::") ? mWorkingUnit->GetGlobalNamespace() : FindUnqualifiedElement(firstToken.second);
			BoolAndString token = (firstToken.second == "::") ? secondToken : GetNextToken(); // token should now have the thing next to ::
			
			while(anElement && token.first)
			{
				if(IsTypenamesContainer(anElement))
				{
					// k. now look the element named in token inside it
					anElement = ((ICPPContainerElement*)anElement)->FindElement(token.second);
					if(!anElement)
					{
						TRACE_LOG1("CPPStatementsParser::GetElementFromCurrentLocation, element not found, %s",token.second.c_str());
					}
					else
					{
						// found element, now look for next tokens, and see if they are following qualifications
						token = GetNextToken();
						if(!token.first)
						{
							// done
							break;
						}

						if(token.second != "::")
						{
							PutBackToken(token.second);
							// also done
							break;
						}

						// nother qualification, continue loop
						token = GetNextToken();
					}
				}
				else
				{
					TRACE_LOG1("CPPStatementsParser::GetElementFromCurrentLocation, element is qualifying another element, but is not a container of types, %s",anElement->Name.c_str());
					anElement = NULL;
				}
			}
		}
		else
		{
			// non qualified. get current context (currently only namespace), and search the element in it
			anElement = FindUnqualifiedElement(firstToken.second);
			if(secondToken.first)
				PutBackToken(secondToken.second);
		}

	}while(false);

	return anElement;
}

CPPElement* CPPStatementsParser::FindUnqualifiedElement(const string& inElementName)
{
	// first go through the current stack (back to front)
	ICPPContainerElementList::reverse_iterator itStack =  mDefinitionContextStack.rbegin();
	CPPElement* anElement = NULL;

	for(; itStack != mDefinitionContextStack.rend() && !anElement; ++itStack)
		anElement = (*itStack)->FindElement(inElementName);

	if(anElement)
		return anElement;

	// then go for the specifically searched names (those added by "using" statements). adhere to contexts.
	LocalContextList::reverse_iterator itContext = mLocalContext.rbegin();
	for(; itContext != mLocalContext.rend() && !anElement; ++ itContext)
	{
		CPPNamespaceSet::iterator itSet = itContext->mUsedNamespaces.begin();
		for(; itSet != itContext->mUsedNamespaces.end() && !anElement; ++itSet)
			anElement = (*itSet)->FindElement(inElementName);
	}
	return anElement;	
}

bool CPPStatementsParser::IsTypenamesContainer(CPPElement* inElement)
{
	return inElement->Type == CPPElement::eCPPElementNamespace;
}

EStatusCode CPPStatementsParser::ParseEnumeratorDeclaration()
{
	// TODO

	return eFailure;
}

EStatusCode CPPStatementsParser::ParseUnionDeclaration()
{
	// TODO

	return eFailure;
}

EStatusCode CPPStatementsParser::ParseTypedefDeclaration()
{
	// TODO

	return eFailure;
}

// now after having played initial games with types give a shot at functions parsing

EStatusCode CPPStatementsParser::ParseClassDeclaration()
{
	// TODO

	return eFailure;
}

EStatusCode CPPStatementsParser::ParseStructDeclaration()
{
	// TODO

	return eFailure;
}

EStatusCode CPPStatementsParser::ParseTemplateDeclaration()
{
	// TODO

	return eFailure;
}