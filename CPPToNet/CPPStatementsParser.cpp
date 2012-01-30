#include "CPPStatementsParser.h"
#include "HeaderUnit.h"
#include "Trace.h"
#include "CPPNamespace.h"
#include "CPPEnumerator.h"
#include "CPPExpressionParser.h"
#include "CPPExpression.h"
#include "CPPVariable.h"
#include "CPPUnion.h"
#include "CPPPrimitive.h"
#include "ICPPVariablesContainerElement.h"
#include "ICPPDefinitionsContainerElement.h"
#include "CPPPrimitiveTypes.h"
#include "ICPPFieldDeclerator.h"
#include "ICPPDeclarationContainerDriver.h"
#include "CPPTypedef.h"
#include "UsedTypeDescriptor.h"
#include "ICPPParametersContainer.h"
#include "CPPFunction.h"
#include "ICPPDeclarationContainerDriver.h"
#include "ICPPFunctionDefinitionDeclerator.h"
#include "DecleratorAsVariableContainer.h"
#include "DecleratorAsParametersContainer.h"
#include "DecleratorAsTypedefContainer.h"
#include "FunctionPointerReturnTypeDeclerator.h"

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

	// setup the primitives as members of the global namespace
	SetupPrimitiveTypes();

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
		result = NULL;
		return EStatusCodeAndHeaderUnit(eFailure,result);
	}
}

void CPPStatementsParser::SetupPrimitiveTypes()
{
	// iterate primitive enums and create a type for each
	int i = eCPPBool;
	for(; i < eCPPPrimitivesMax; ++i)
	{
		CPPPrimitive* newPrimitive = new CPPPrimitive(scPrimitivesLabels[(int)i],(ECPPPrimitiveType)i);
		mWorkingUnit->GetGlobalNamespace()->AppendPrimitive(scPrimitivesLabels[(int)i],newPrimitive);
	}
	
}

EStatusCodeAndBool CPPStatementsParser::ParseStatement(HeaderUnit* inUnitModel)
{
	EStatusCode status = eSuccess;
	BoolAndString tokenizerResult = mTokensSource.GetNextToken();

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
			tokenizerResult = mTokensSource.GetNextToken();
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
				mTokensSource.PutBackToken(tokenizerResult.second);
				status = ParseUsingDeclaration();
			}
		}
		else if(tokenizerResult.second == "enum")
		{
			status = ParseEnumeratorDeclaration();
		}
		else if(tokenizerResult.second == "union")
		{
			status = ParseUnionDeclaration();
		}
		else if(tokenizerResult.second == "typedef")
		{
			status = ParseTypedefDeclaration();
		}
		else if(tokenizerResult.second == "class")
		{
			status = ParseClassDeclaration();
		}
		else if(tokenizerResult.second == "struct")
		{
			status = ParseStructDeclaration();
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

			// this means that now we are left with 2 types of statements:
			// function declarations(and/or definitions) and variables declarations
		
			// so now i have to identify what i got. deferring function declarations from variable declarations is easy, due to the usage of
			// paranthesis at the name of the function/variable if this is a function. prior to that both kinds of
			// declarations look exactly the same. after...it's a matter of what is identified.

			DecleratorAsVariableContainer namespaceDefinitionContainerDriver(mDefinitionContextStack.back());

			status =  ParseGenericDeclerationStatement(&namespaceDefinitionContainerDriver);
			if(status != eSuccess)
				TRACE_LOG("CPPStatementsParser::ParseStatement, failed in parsing variable or function definition");
		}

	}while(false);

	return EStatusCodeAndBool(status,true);
}

EStatusCode CPPStatementsParser::ParseNamespaceDeclaration()
{
	BoolAndString tokenizerResult = mTokensSource.GetNextToken();

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

		CPPNamespace* newNamespace = ((CPPNamespace*)mDefinitionContextStack.back())->CreateNewNamespace(GetNewUnnamedName());

		if(!newNamespace)
			return eFailure;

		mDefinitionContextStack.push_back(newNamespace);		
	}
	else
	{


		string namespaceName = tokenizerResult.second;

		// next char can be either a block opener (regular case) or equality (alias)
		tokenizerResult = mTokensSource.GetNextToken(); 
		if(!tokenizerResult.first)
		{
			TRACE_LOG("CPPStatementsParser::ParseNamespaceDeclaration, syntax error, no block opener after namespace declaration");
			return eFailure;
		}
		
		if(tokenizerResult.second == "=")
		{
			// namespace XXXA = XXX;

			tokenizerResult = mTokensSource.GetNextToken(); 
			if(!tokenizerResult.first)
			{
				TRACE_LOG("CPPStatementsParser::ParseNamespaceDeclaration, syntax error, no namespace name for alias namespace definition");
				return eFailure;
			}

			CPPNamespace* aNamespace = (CPPNamespace*)FindUnqualifiedElement(tokenizerResult.second,CPPElement::eCPPElementNamespace);
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

string CPPStatementsParser::GetNewUnnamedName()
{
	string result = mUnnamedSequance.ToString();
	mUnnamedSequance = (long)mUnnamedSequance + 1;
	return result;
}

EStatusCode CPPStatementsParser::ParseUsingDirective()
{
	// next token should be the namespace to be used
	BoolAndString tokenizerResult = mTokensSource.GetNextToken(); 
	if(!tokenizerResult.first || tokenizerResult.second == ";")
	{
		TRACE_LOG("CPPStatementsParser::ParseUsingDirective, syntax error, using directive does not provide a namespace name");
		return eFailure;
	}

	CPPNamespace* aNamespace = (CPPNamespace*)FindUnqualifiedElement(tokenizerResult.second,CPPElement::eCPPElementNamespace);

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
	BoolAndString tokenizerResult = mTokensSource.GetNextToken(); 
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
	CPPElement* anElement = GetElementFromCurrentLocation(false);
	if(!anElement)
	{
		TRACE_LOG("CPPStatementsParser::ParseUsingDeclaration, error in using declaration, could not find designated type");
		return eFailure;
	}

	// define it in this namespace (either local or global namespace)
	return mDefinitionContextStack.back()->DefineAlias(anElement->Name,anElement);
}

// will return either NULL or an element, if and only if there's just ONE element of this name
CPPElement* CPPStatementsParser::FindElement(ICPPDefinitionsContainerElement* inContainer, const string& inElementName)
{
	CPPElementList elementsList = inContainer->FindElements(inElementName);
	if(elementsList.size() == 0)
	{
		TRACE_LOG1("CPPStatementsParser::FindElement, no elements exist with the name %s",inElementName.c_str());
		return NULL;
	}
	else if(elementsList.size() > 1)
	{
		TRACE_LOG1("CPPStatementsParser::FindElement, ambiguity. multiple elements exist with the name %s",inElementName.c_str());
		return NULL;
	}
	else
		return elementsList.front();
}

// will return either NULL or an element, if and only if there's just ONE element of this name and of this type
CPPElement* CPPStatementsParser::FindElement(ICPPDefinitionsContainerElement* inContainer,const string& inElementName,CPPElement::ECPPElementType inOfType)
{
	CPPElementList elementsList = inContainer->FindElements(inElementName);
	CPPElementList::iterator it = elementsList.begin();
	CPPElement* foundElement = NULL;
	
	for(; it != elementsList.end(); ++it)
	{
		if((*it)->Type == inOfType)
		{
			if(foundElement)
			{
				TRACE_LOG1("CPPStatementsParser::FindElement, ambiguity. multiple elements exist with the name %s",inElementName.c_str());
				foundElement = NULL;
				break;
			}
			else
			{
				foundElement = *it;
			}
		}
	}

	if(!foundElement)
		TRACE_LOG1("CPPStatementsParser::FindElement,unable to find a compatible element with the name %s",inElementName.c_str());
	return foundElement;
}

CPPElement* CPPStatementsParser::FindElement(ICPPDefinitionsContainerElement* inContainer,const string& inElementName,ECPPElementTypeSet inOfTypes)
{
	CPPElementList elementsList = inContainer->FindElements(inElementName);
	CPPElementList::iterator it = elementsList.begin();
	CPPElement* foundElement = NULL;
	
	for(; it != elementsList.end(); ++it)
	{
		if(inOfTypes.find((*it)->Type) != inOfTypes.end())
		{
			if(foundElement)
			{
				TRACE_LOG1("CPPStatementsParser::FindElement, ambiguity. multiple elements exist with the name %s",inElementName.c_str());
				foundElement = NULL;
				break;
			}
			else
			{
				foundElement = *it;
			}
		}
	}

	if(!foundElement)
		TRACE_LOG1("CPPStatementsParser::FindElement,unable to find a compatible element with the name %s",inElementName.c_str());
	return foundElement;
}


CPPElement* CPPStatementsParser::GetElementFromCurrentLocation(bool inRequireType)
{
	// a name can be either:
	// XXX
	// [::]YYY[::AAA::ZZZ...]::XXX
	CPPElement* anElement = NULL;
	ECPPElementTypeSet typesetWithNamespace,typeset;

	if(inRequireType)
	{
		typeset.insert(CPPElement::eCPPElementPrimitive);
		typeset.insert(CPPElement::eCPPElementEnumerator);
		typeset.insert(CPPElement::eCPPElementUnion);
		typeset.insert(CPPElement::eCPPElementTypedef);
		typesetWithNamespace = typeset;
		typesetWithNamespace.insert(CPPElement::eCPPElementNamespace);
	}

	BoolAndString firstToken = mTokensSource.GetNextToken();
	do
	{
		if(!firstToken.first)
		{
			TRACE_LOG("CPPStatementsParser::GetElementFromCurrentLocation, no symbol in current location");
			break;
		}

		BoolAndString secondToken = mTokensSource.GetNextToken();

		// differ qualified from non-qualified states
		if(firstToken.second == "::" || (secondToken.first && secondToken.second == "::"))
		{
			ECPPElementTypeSet typeSet;
			// get any type that is a container
			typeSet.insert(CPPElement::eCPPElementNamespace);

			// qualified
			anElement = (firstToken.second == "::") ? mWorkingUnit->GetGlobalNamespace() : FindUnqualifiedElement(firstToken.second,CPPElement::eCPPElementNamespace);
			BoolAndString token = (firstToken.second == "::") ? secondToken : mTokensSource.GetNextToken(); // token should now have the thing next to ::
			
			while(anElement && token.first)
			{
				// k. now look the element named in token inside it
				if(inRequireType)
					anElement = FindElement((ICPPDefinitionsContainerElement*)anElement,token.second,typesetWithNamespace);
				else
					anElement = FindElement((ICPPDefinitionsContainerElement*)anElement,token.second);
				if(!anElement)
				{
					TRACE_LOG1("CPPStatementsParser::GetElementFromCurrentLocation, element not found, %s",token.second.c_str());
				}
				else
				{
					// found element, now look for next tokens, and see if they are following qualifications
					token = mTokensSource.GetNextToken();
					if(!token.first)
					{
						// done
						break;
					}

					if(token.second != "::")
					{
						mTokensSource.PutBackToken(token.second);
						// also done
						break;
					}

					// nother qualification, continue loop
					token = mTokensSource.GetNextToken();
				}
			}
		}
		else
		{
			string elementName = firstToken.second;

			if(secondToken.first)
			{
				// consider some primitive cases (primitives are defined in global namespace, so will be found
				// by the below. just make sure that the names are right
				if("unsigned" == elementName)
				{
					// options: ,unsigned long, unsigned long long, unsigned int->unsigned,unsigned, unsigned char, unsigned short
					
					// unsigned long, unsigned long long
					if("long" == secondToken.second)
					{
						BoolAndString thirdToken = mTokensSource.GetNextToken();
						// unsigned long long
						if(thirdToken.first && "long" == thirdToken.second)
						{
							elementName = "unsigend long long";
						}
						else
						{
							elementName = "unsigend long";
							// unsigned long
							if(thirdToken.first)
								mTokensSource.PutBackToken(thirdToken.second);
						}
					} // unsigned int
					else if("int" == secondToken.second)
					{
						elementName = "unsigend";
					} // unsigned char
					else if("char" == secondToken.second)
					{
						elementName = "unsigend char";
					} // unsigned short
					else if("short" == secondToken.second)
					{
						elementName = "unsigend short";
					}
					else
					{
						// unsigned
						mTokensSource.PutBackToken(secondToken.second);
					}

				}
				else if("long" == elementName)
				{
					// options: long,long int->long, long long, long double
					if("long" == secondToken.second)
					{
						elementName = "long long";
					}
					else if("int" == secondToken.second)
					{
						elementName = "long";
					}
					else if("double" == secondToken.second)
					{
						elementName = "long double";
					}
					else
					{
						// long
						mTokensSource.PutBackToken(secondToken.second);
					}

				}
				else if("short" == elementName)
				{
					// options: short, short int->short
					if("int" == secondToken.second)
					{
						elementName = "short";
					}
					else
					{
						// short
						mTokensSource.PutBackToken(secondToken.second);
					}
				}
				else
				{
					mTokensSource.PutBackToken(secondToken.second);
				}
			}
			
			// get current context (currently only namespace), and search the element in it
			if(inRequireType)
				anElement = FindUnqualifiedElement(elementName,typeset);
			else
				anElement = FindUnqualifiedElement(elementName);

		}

	}while(false);

	return anElement;
}

CPPElement* CPPStatementsParser::FindUnqualifiedElement(const string& inElementName,ECPPElementTypeSet inOfTypes)
{
	// first go through the current stack (back to front)
	ICPPDefinitionsContainerElementList::reverse_iterator itStack =  mDefinitionContextStack.rbegin();
	CPPElement* anElement = NULL;

	for(; itStack != mDefinitionContextStack.rend() && !anElement; ++itStack)
		anElement = FindElement(*itStack,inElementName,inOfTypes);

	if(anElement)
		return anElement;

	// then go for the specifically searched names (those added by "using" statements). adhere to contexts.
	LocalContextList::reverse_iterator itContext = mLocalContext.rbegin();
	for(; itContext != mLocalContext.rend() && !anElement; ++ itContext)
	{
		CPPNamespaceSet::iterator itSet = itContext->mUsedNamespaces.begin();
		for(; itSet != itContext->mUsedNamespaces.end() && !anElement; ++itSet)
			anElement = FindElement(*itSet,inElementName,inOfTypes);
	}
	return anElement;	
}


CPPElement* CPPStatementsParser::FindUnqualifiedElement(const string& inElementName,CPPElement::ECPPElementType inOfType)
{
	// first go through the current stack (back to front)
	ICPPDefinitionsContainerElementList::reverse_iterator itStack =  mDefinitionContextStack.rbegin();
	CPPElement* anElement = NULL;

	for(; itStack != mDefinitionContextStack.rend() && !anElement; ++itStack)
		anElement = FindElement(*itStack,inElementName,inOfType);

	if(anElement)
		return anElement;

	// then go for the specifically searched names (those added by "using" statements). adhere to contexts.
	LocalContextList::reverse_iterator itContext = mLocalContext.rbegin();
	for(; itContext != mLocalContext.rend() && !anElement; ++ itContext)
	{
		CPPNamespaceSet::iterator itSet = itContext->mUsedNamespaces.begin();
		for(; itSet != itContext->mUsedNamespaces.end() && !anElement; ++itSet)
			anElement = FindElement(*itSet,inElementName,inOfType);
	}
	return anElement;	
}

CPPElement* CPPStatementsParser::FindUnqualifiedElement(const string& inElementName)
{
	// first go through the current stack (back to front)
	ICPPDefinitionsContainerElementList::reverse_iterator itStack =  mDefinitionContextStack.rbegin();
	CPPElement* anElement = NULL;

	for(; itStack != mDefinitionContextStack.rend() && !anElement; ++itStack)
		anElement = FindElement(*itStack,inElementName);

	if(anElement)
		return anElement;

	// then go for the specifically searched names (those added by "using" statements). adhere to contexts.
	LocalContextList::reverse_iterator itContext = mLocalContext.rbegin();
	for(; itContext != mLocalContext.rend() && !anElement; ++ itContext)
	{
		CPPNamespaceSet::iterator itSet = itContext->mUsedNamespaces.begin();
		for(; itSet != itContext->mUsedNamespaces.end() && !anElement; ++itSet)
			anElement = FindElement(*itSet,inElementName);
	}
	return anElement;	
}

bool CPPStatementsParser::IsTypenamesContainer(CPPElement* inElement)
{
	return inElement->Type == CPPElement::eCPPElementNamespace;
}

EStatusCode CPPStatementsParser::ParseEnumeratorDeclaration()
{
	/*	
		enum [tag] [: type] {enum-list} [declarator];
		enum tag declarator;
	*/

	BoolAndString token = mTokensSource.GetNextToken();
	EStatusCode status = eFailure;
	CPPEnumerator* anEnumerator = NULL;

	do
	{
		if(!token.first)
		{
			TRACE_LOG("CPPStatementsParser::ParseEnumeratorDeclaration, synatx error, unable to read past 'enum' keyword");
			break;
		}

		string enumeratorName = ((token.second == ":" || token.second == "{") ? GetNewUnnamedName():token.second);

		if(token.second != ":" && token.second != "{")
		{
			token = mTokensSource.GetNextToken();
			if(!token.first)
			{
				TRACE_LOG("CPPStatementsParser::ParseEnumeratorDeclaration, synatx error, not found keys list for enumartor");
				break;
			}
		}

		// so now we are surely past the enum tag. now should be either ":", "{" or declarator
		if(token.second == ":" || token.second == "{")
		{
			// enumerator definition case. define the enumerator

			anEnumerator = mDefinitionContextStack.back()->CreateEnumerator(enumeratorName,true);
			if(!anEnumerator)
				break;

			if(token.second == ":")
			{
				// case of tag integer type...boring. skip
				token = mTokensSource.GetNextToken();
				if(!token.first)
				{
					TRACE_LOG("CPPStatementsParser::ParseEnumeratorDeclaration, synatx error, enumerator underflying type not defined");
					break;
				}
				token = mTokensSource.GetNextToken();
				if(!token.first)
				{
					TRACE_LOG("CPPStatementsParser::ParseEnumeratorDeclaration, synatx error, not found keys list for enumartor");
					break;
				}
			}

			// now should have "{"
			
			do
			{
				token = mTokensSource.GetNextToken();
				if(!token.first)
				{
					TRACE_LOG("CPPStatementsParser::ParseEnumeratorDeclaration, synatx error, unexpected end in enumrator values list");
					break;
				}

				if(token.second == "}")
					break;

				if(!mDefinitionContextStack.back()->CreateEnumeratorValue(anEnumerator,token.second))
					break;
				
				token = mTokensSource.GetNextToken();
				if(!token.first)
				{
					TRACE_LOG("CPPStatementsParser::ParseEnumeratorDeclaration, synatx error, unexpected end in enumrator values list");
					break;
				}

				if(token.second == "=") // got an assignment here. skip it
				{
					if(SkipConstantExpression() != eSuccess)
					{
						TRACE_LOG("CPPStatementsParser::ParseEnumeratorDeclaration, synatx error, problem in value expression definition for enumerator");
						break;
					}

					token = mTokensSource.GetNextToken(); // get next token which should now be either } or ,
					if(!token.first)
					{
						TRACE_LOG("CPPStatementsParser::ParseEnumeratorDeclaration, synatx error, unexpected end in enumrator values list");
						break;
					}
				}
			}while(token.second == ",");
			
			// here check that it's true and "}"
			if(!token.first || token.second != "}")
				break;

			// get next token to be in either semicolon or declarators
			token = mTokensSource.GetNextToken();
			if(!token.first)
			{
				TRACE_LOG("CPPStatementsParser::ParseEnumeratorDeclaration, synatx error,unexpected end of expression. should be either semicolor or declarators");
				break;
			}		

			// define variables if any exists
			if(token.second != ";")
			{
				// variables definition. put back token and parse declarators normally
				mTokensSource.PutBackToken(token.second);

				DecleratorAsVariableContainer variablesDefinitionDriver(mDefinitionContextStack.back());
				variablesDefinitionDriver.SetFlags(anEnumerator,false,false,false,false,false,false);

				if(ParseDeclarators(&variablesDefinitionDriver) != eSuccess) // will consume the semicolon as well
				{
					TRACE_LOG1("CPPStatementsParser::ParseEnumeratorDeclaration, failed to parse declarators for enumerator %s",enumeratorName.c_str());
					break;
				}
			}

		}
		else
		{
			if(token.second == ";")
			{
				// Just a type declaration. create it if required
				anEnumerator = mDefinitionContextStack.back()->CreateEnumerator(enumeratorName,false);
				if(!anEnumerator)
					break;

			}
			else
			{

				// variable declaration of that enum type. this means that the enumerator should already be defiend [name can be qualified at this point?]
				anEnumerator = (CPPEnumerator*)FindUnqualifiedElement(enumeratorName,CPPElement::eCPPElementEnumerator);
				if(!anEnumerator)
				{
					TRACE_LOG1("CPPStatementsParser::ParseEnumeratorDeclaration, enumerator %s not found in enumerator variable definition.",enumeratorName.c_str());
					break;
				}

				// variables definition. put back token and parse declarators normally
				mTokensSource.PutBackToken(token.second);

				DecleratorAsVariableContainer variablesDefinitionDriver(mDefinitionContextStack.back());
				variablesDefinitionDriver.SetFlags(anEnumerator,false,false,false,false,false,false);

				if(ParseDeclarators(&variablesDefinitionDriver) != eSuccess) // will consume the semicolon as well
				{
					TRACE_LOG1("CPPStatementsParser::ParseEnumeratorDeclaration, failed to parse declarators for enumerator %s",enumeratorName.c_str());
					break;
				}
			}
		}


		status = eSuccess;
	}while(false);	

	return status;
}

EStatusCode CPPStatementsParser::SkipConstantExpression()
{
	CPPExpressionParser expressionParser;

	// parse, but just for the sake of skipping
	BoolAndCPPExpression result = expressionParser.ParseExpression(&mTokensSource);

	if(result.first)
	{
		delete result.second;
		return eSuccess;
	}
	else
		return eFailure;
}

EStatusCodeAndBool CPPStatementsParser::ParseFieldOrFunction(ICPPDeclarationContainerDriver* inContainer,const DeclaratorModifierList& inFieldModifiersList)
{
	ICPPFieldDeclerator* aDeclarator;
	EStatusCodeAndBool result(eSuccess,false);

	BoolAndString token = mTokensSource.GetNextToken();	
	if(!token.first)
	{
		TRACE_LOG("CPPStatementsParser::ParseAndDefineField, tokens not found when trying to parse for fields");
		result.first = eFailure;
		return result;
	}

	do
	{
		result.second = inContainer->VerifyDeclaratorStopper(token.second);
		if(result.second || token.second == "[")
		{
			// unnamed variable case with or without subscripts, continue as in a variable
			aDeclarator = inContainer->AddFieldDeclerator("");
			if(!aDeclarator)
			{
				TRACE_LOG("CPPStatementsParser::ParseAndDefineField, unable to create an unnamed declerator");
				result.first = eFailure;
				break;
			}
			aDeclarator->AppendModifiers(inFieldModifiersList);

			
			if(token.second == "[")
			{
				mTokensSource.PutBackToken(token.second);
				result = ParseFieldDefinition(aDeclarator);
			}
			else
				result.first = aDeclarator->FinalizeFieldDefinition();
			break;
		}

		// else, named declerator. may be a field or a function
		string decleratorName = token.second;
		
		// At this point we it might be that there will be a parenthesis - "(" - in which case this is a function declaration (and could also be a definition)
		// or a regular field definition, in which case it will be either a subscript or initialization (otherwise the parsing would be done for this variable)
		token = mTokensSource.GetNextToken();
		if(!token.first)
		{
			TRACE_LOG("CPPStatementsParser::ParseAndDefineField, unexpected end, should have semicolon, initializor, comma or subscript marks");
			result.first = eFailure;
			break;
		}

		result.second = inContainer->VerifyDeclaratorStopper(token.second);
		if(result.second)
		{
			// ok. must be a variable, with no continued parsing required
			aDeclarator = inContainer->AddFieldDeclerator(decleratorName);
			if(!aDeclarator)
			{
				TRACE_LOG1("CPPStatementsParser::ParseAndDefineField, unable to create a declarator with the name %s.",decleratorName.c_str());
				result.first = eFailure;
				break;
			}
			aDeclarator->AppendModifiers(inFieldModifiersList);
			result.first = aDeclarator->FinalizeFieldDefinition();
			break;
		}

		if(token.second == "(")
		{
			// oh well. this is a function definition. go parse it someplace else
			result = ParseFunctionDefinition(inContainer,inFieldModifiersList,decleratorName);
			if(result.first != eSuccess)
			{
				TRACE_LOG("CPPStatementsParser::ParseAndDefineField, failed to parse a function. fail");
				break;
			}
		}
		else
		{
			// ok. must be a variable, with continued parsing required
			aDeclarator = inContainer->AddFieldDeclerator(decleratorName);
			if(!aDeclarator)
			{
				TRACE_LOG1("CPPStatementsParser::ParseAndDefineField, unable to create a declarator with the name %s.",decleratorName.c_str());
				result.first = eFailure;
				break;
			}
			aDeclarator->AppendModifiers(inFieldModifiersList);
			mTokensSource.PutBackToken(token.second);
			result = ParseFieldDefinition(aDeclarator);
		}
	}while(false);

	return result;

}

EStatusCodeAndBool CPPStatementsParser::ParseFieldDefinition(ICPPFieldDeclerator* inFieldDeclerator)
{
	EStatusCodeAndBool result(eSuccess,false);
	BoolAndString token;

	do
	{
		// Continuing parsing of a field, by checking out for subscripts and initializers
		token = mTokensSource.GetNextToken();

		// subscripts
		while(token.first && token.second == "[")
		{
			inFieldDeclerator->AddSubscript();

			token = mTokensSource.GetNextToken();
			if(!token.first)
			{
				TRACE_LOG("CPPStatementsParser::ParseAndDefineField, unexpected end, expected an expression or closing bracket ']'");
				result.first = eFailure;
				break;
			}

			if(token.second != "]")
			{
				mTokensSource.PutBackToken(token.second);
				result.first = SkipConstantExpression();
				if(result.first != eSuccess)
					break;

				token = mTokensSource.GetNextToken();
				if(!token.first || token.second != "]")
				{
					TRACE_LOG("CPPStatementsParser::ParseAndDefineField, unexpected end or wrong token, expected closing bracket ']'");
					result.first = eFailure;
					break;
				}
			}
			token = mTokensSource.GetNextToken();
		}
		if(result.first != eSuccess)
			break;

		// k. done with subscripts, now initializers skipping
		if(token.second == "=")
		{
			result.first = SkipInitializer();
			if(result.first != eSuccess)
				break;
		}
		else
			mTokensSource.PutBackToken(token.second);
		result.first = inFieldDeclerator->FinalizeFieldDefinition();
	}while(false);

	return result;
}

EStatusCode CPPStatementsParser::SkipInitializer()
{
	EStatusCode status = eSuccess;

	do
	{

		BoolAndString token = mTokensSource.GetNextToken();
		
		if(!token.first)
		{
			TRACE_LOG("CPPStatementsParser::SkipInitalizer, expecting a token and found nothing");
			status = eFailure;
			break;
		}

		if(token.second == "{")
			status = SkipBlock();
		else
			status = SkipConstantExpression();
	}while(false);

	return status;
}

EStatusCode CPPStatementsParser::SkipBlock()
{
	// assuming block already started
	EStatusCode status = eSuccess;
	int blockLevel = 1;
	BoolAndString token;

	while(eSuccess == status && blockLevel > 0)
	{
		token = mTokensSource.GetNextToken();

		if(!token.first)
		{
			TRACE_LOG("CPPStatementsParser::SkipBlock, unxpected end of reading, while skipping block");
			status = eFailure;
		}
		else
		{
			if(token.second == "}")
				--blockLevel;
			else if(token.second == "{")
				++blockLevel;
		}
	}
	return status;
}

EStatusCodeAndBool CPPStatementsParser::ParseFunctionDefinition(ICPPDeclarationContainerDriver* inContainer,const DeclaratorModifierList& inReturnTypeModifiersList,const string& inFunctionName)
{
	EStatusCodeAndBool result(eSuccess,false);
	BoolAndString token;

	// At this point, the parser should be after having parsed the opening paranthesis for the function.

	do
	{
		ICPPFunctionDefinitionDeclerator* functionDeclerator = inContainer->AddFunctionDefinition(inFunctionName);
		if(!functionDeclerator)
		{
			TRACE_LOG1("CPPStatementsParser::ParseFunctionDefinition, unable to create a function declarator with the name %s.",inFunctionName.c_str());
			result.first = eFailure;
			break;
		}
		functionDeclerator->AppendModifiersForFunctionDefinitionReturnType(inReturnTypeModifiersList);

		token = mTokensSource.GetNextToken();	
		
		DecleratorAsParametersContainer parametersContainer(functionDeclerator->GetParametersContainerForFunctionDefinition());

		// Now for parameters parsing
		while(token.first && token.second != ")" &&  eSuccess == result.first)
		{
			if(token.second == "...")
			{
				functionDeclerator->SetFunctionDefinitionHasElipsis();
				token = mTokensSource.GetNextToken();
				if(!token.first)
				{
					TRACE_LOG("CPPStatementsParser::ParseFunctionDefinition, unexpected no tokens after elipsis. expected )");
					result.first = eFailure;
				}
				else if(token.second != ")")
				{
					TRACE_LOG1("CPPStatementsParser::ParseFunctionDefinition, unexpected token after elipsis. expected ), got %s",token.second.c_str());
					result.first = eFailure;
				}
				break;
			}
			else
			{
				result.first = ParseGenericDeclerationStatement(&parametersContainer);
				if(result.first != eSuccess)
				{
					TRACE_LOG("CPPStatementsParser::ParseFunctionDefinition, failed to parase function pointer paremeter");
					break;
				}
				
				if(parametersContainer.FoundStop())	
					break;

				parametersContainer.Reset();
				token = mTokensSource.GetNextToken();
			}
		}

		if(!token.first || result.first != eSuccess)
			break;

		result.second = inContainer->VerifyDeclaratorStopper(token.second);
		if(result.second)
		{
			// done here. expression not continued.
			result.first = functionDeclerator->FinalizeFunctionDefinition(false);
			break;
		}

		// if continued, then it must be a block start, for an actual function definition. skip, and note in the function definition object
		if(token.second == "{")
		{
			result.first = SkipBlock();
			if(result.first != eSuccess)
			{
				TRACE_LOG("CPPStatementsParser::ParseFunctionDefinition, failed to skip function definition");
				break;
			}
			result.first = functionDeclerator->FinalizeFunctionDefinition(true);
			
			// this is a pacularity of function defintion, which when there's a function definition always ends in "}", and not in the
			// regular delcearator stopper (normally a semi-colon). hence, mark the ending token as consumed.
			result.second = true;
			break;
		}
		
		// if it wasn't a block, fail
		TRACE_LOG1("CPPStatementsParser::ParseFunctionDefinition, expected either expression termination or function definition (block). instead got %s",token.second.c_str());
		result.first = eFailure;

	}while(false);

	return result;
}


EStatusCodeAndBool CPPStatementsParser::ParseFunctionPointerOrFunction(ICPPDeclarationContainerDriver* inContainer,const DeclaratorModifierList& inReturnTypeModifiersList)
{
	// function pointers look like this:
	// T (name)(params)
	// or
	// T (*name)(params)
	// or
	// T (&name)(params)
	// at this point we already have the "(" part consumed, so now there should be 
	// either "*" or "&" or the name
	// this function consumes tokens until function pointer decleration finishes. it doesn't look further for the ending characeter or something. which is good,
	// cause its parent function should consume it. hence the second parameter of the result here is always remaining "false"

	// note that this could also be a case of a function that returns a function pointer. This is a slightly complex definitions which looks like this:
	// function pointer return value (*function name(function params))(function pointer params) 
	
	EStatusCodeAndBool result(eSuccess,false);
	ICPPFunctionPointerDeclerator::EFunctionPointerType pointerType;
	string decleratorName;
	ICPPFunctionDefinitionDeclerator* functionDeclerator = NULL; // will also serve as indication for this being a function or not
	FunctionPointerReturnTypeDeclerator returnTypeDeclerator;

	BoolAndString token = mTokensSource.GetNextToken();	
	if(!token.first)
	{
		TRACE_LOG("CPPStatementsParser::ParseFunctionPointerOrFunction, tokens not found when trying to parse for function pointers");
		result.first = eFailure;
		return result;
	}

	do
	{
		if(token.second == "*" || token.second == "&")
		{
			// modifiers to pointer type
			pointerType = (token.second == "*") ? ICPPFunctionPointerDeclerator::eFunctionPointerTypePointer : ICPPFunctionPointerDeclerator::eFunctionPointerTypeReference;

			// get next token for the name
			token = mTokensSource.GetNextToken();	
			if(!token.first)
			{
				TRACE_LOG("CPPStatementsParser::ParseFunctionPointerOrFunction, no token when trying to parse for function pointer name");
				result.first = eFailure;
				break;
			}
		}
		else
			pointerType = ICPPFunctionPointerDeclerator::eFunctionPointerTypeNone;

		decleratorName = token.second;

		token = mTokensSource.GetNextToken();	
		if(!token.first)
		{
			TRACE_LOG("CPPStatementsParser::ParseFunctionPointerOrFunction, expected ) or ( in parsing function pointer declaration, and not found");
			result.first = eFailure;
			break;
		}

		// function pointer declerator. might be the return value function pointer for a function or an actual declaration of a function pointer
		ICPPFunctionPointerDeclerator* aFPDeclarator;

		if(token.second == "(")
		{
			// Aha! function that returns a function pointer case. oh what a headache
			functionDeclerator = inContainer->AddFunctionDefinition(decleratorName);
			if(!functionDeclerator)
			{
				TRACE_LOG1("CPPStatementsParser::ParseFunctionPointerOrFunction, unable to create a function declarator with the name %s.",decleratorName.c_str());
				result.first = eFailure;
				break;
			}
			functionDeclerator->SetupFunctionPointerReturnTypeDeclerator(&returnTypeDeclerator);
			aFPDeclarator = &returnTypeDeclerator;

			// now parse the parameters of this function. seriously.
			token = mTokensSource.GetNextToken();	
		
			DecleratorAsParametersContainer parametersContainer(functionDeclerator->GetParametersContainerForFunctionDefinition());

			while(token.first && token.second != ")" &&  eSuccess == result.first)
			{
				if(token.second == "...")
				{
					functionDeclerator->SetFunctionDefinitionHasElipsis();
					token = mTokensSource.GetNextToken();
					if(!token.first)
					{
						TRACE_LOG("CPPStatementsParser::ParseFunctionPointerOrFunction, unexpected no tokens after elipsis. expected )");
						result.first = eFailure;
					}
					else if(token.second != ")")
					{
						TRACE_LOG1("CPPStatementsParser::ParseFunctionPointerOrFunction, unexpected token after elipsis. expected ), got %s",token.second.c_str());
						result.first = eFailure;
					}
					break;
				}
				else
				{
					result.first = ParseGenericDeclerationStatement(&parametersContainer);
					if(result.first != eSuccess)
					{
						TRACE_LOG("CPPStatementsParser::ParseFunctionPointerOrFunction, failed to parase function pointer paremeter");
						break;
					}
				
					if(parametersContainer.FoundStop())	
						break;

					parametersContainer.Reset();
					token = mTokensSource.GetNextToken();
				}
			}

			if(!token.first || result.first != eSuccess)
				break;

			// done with parameters for this function! consume closing parenthesis of function pointer
			token = mTokensSource.GetNextToken();	
			if(!token.first || token.second != ")")
			{
				TRACE_LOG("CPPStatementsParser::ParseFunctionPointerOrFunction, expected ) in parsing function pointer declaration, and not found");
				result.first = eFailure;
				break;
			}

		}
		else if(token.second == ")")
		{
			// Aha! regular function pointer declaration. easy
			// ok. we have enough, can create a function pointer object, and go on with parsing parameters later
			aFPDeclarator = inContainer->AddFunctionPointerDeclarator(decleratorName);
			if(!aFPDeclarator)
			{
				TRACE_LOG1("CPPStatementsParser::ParseFunctionPointerOrFunction, unable to create a function declarator with the name %s.",decleratorName.c_str());
				result.first = eFailure;
				break;
			}				
		}
		else
		{
			TRACE_LOG("CPPStatementsParser::ParseFunctionPointerOrFunction, expected ) or ( in parsing function pointer declaration, and not found");
			result.first = eFailure;
			break;
		}

		token = mTokensSource.GetNextToken();	
		if(!token.first || token.second != "(")
		{
			TRACE_LOG("CPPStatementsParser::ParseFunctionPointerOrFunction, expected ( in parsing function pointer declaration, and not found");
			result.first = eFailure;
			break;
		}
		
		aFPDeclarator->SetFunctionPointerType(pointerType);
		aFPDeclarator->AppendModifiersForFunctionPointerReturnType(inReturnTypeModifiersList);

		token = mTokensSource.GetNextToken();	
		
		DecleratorAsParametersContainer parametersContainer(aFPDeclarator->GetParametersContainerForFunctionPointer());

		// Now for parameters parsing
		while(token.first && token.second != ")" && eSuccess == result.first)
		{
			if(token.second == "...")
			{
				aFPDeclarator->SetFunctionPointerHasElipsis();
				token = mTokensSource.GetNextToken();
				if(!token.first)
				{
					TRACE_LOG("CPPStatementsParser::ParseFunctionPointerOrFunction, unexpected no tokens after elipsis. expected )");
					result.first = eFailure;
				}
				else if(token.second != ")")
				{
					TRACE_LOG1("CPPStatementsParser::ParseFunctionPointerOrFunction, unexpected token after elipsis. expected ), got %s",token.second.c_str());
					result.first = eFailure;
				}
				break;
			}
			else
			{
				result.first = ParseGenericDeclerationStatement(&parametersContainer);
				if(result.first != eSuccess)
				{
					TRACE_LOG("CPPStatementsParser::ParseFunctionPointerOrFunction, failed to parase function pointer paremeter");
					break;
				}
				
				if(parametersContainer.FoundStop())	
					break;

				parametersContainer.Reset();
				token = mTokensSource.GetNextToken();
			}
		}
		if(result.first != eSuccess)
			break;

		// now, finalize function poiner - either declerator or function return type
		result.first = aFPDeclarator->FinalizeFunctionPointerDefinition();
		if(result.first != eSuccess)
			break;

		// if function pointer declerator stop here and return, with function we need to continue a bit, to see if this is a definition
		if(!functionDeclerator)
			break;

		functionDeclerator->SetReturnType(returnTypeDeclerator.DetachUsedTypeDescriptor());

		result.second = inContainer->VerifyDeclaratorStopper(token.second);
		if(result.second)
		{
			// done here. expression not continued.
			result.first = functionDeclerator->FinalizeFunctionDefinition(false);
			break;
		}

		// if continued, then it must be a block start, for an actual function definition. skip, and note in the function definition object
		if(token.second == "{")
		{
			result.first = SkipBlock();
			if(result.first != eSuccess)
			{
				TRACE_LOG("CPPStatementsParser::ParseFunctionPointerOrFunction, failed to skip function definition");
				break;
			}
			result.first = functionDeclerator->FinalizeFunctionDefinition(true);
			
			// this is a pacularity of function defintion, which when there's a function definition always ends in "}", and not in the
			// regular delcearator stopper (normally a semi-colon). hence, mark the ending token as consumed.
			result.second = true;
			break;
		}
		
		// if it wasn't a block, fail
		TRACE_LOG1("CPPStatementsParser::ParseFunctionPointerOrFunction, expected either expression termination or function definition (block). instead got %s",token.second.c_str());
		result.first = eFailure;


	} while(false);

	return result;
}

EStatusCode CPPStatementsParser::ParseUnionDeclaration()
{
	/*
		// actually quite similar to parsing enumartor...we should consider reuse.

		union [tag] { member-list } [declarators];
		[union] tag declarators;
	*/

	BoolAndString token = mTokensSource.GetNextToken();
	EStatusCode status = eSuccess;
	CPPUnion* aUnion = NULL;

	do
	{
		if(!token.first)
		{
			TRACE_LOG("CPPStatementsParser::ParseUnionDeclaration, synatx error, unable to read past 'enum' keyword");
			status = eFailure;
			break;
		}

		string unionName = (token.second == "{" ? GetNewUnnamedName():token.second);

		if(token.second != "{")
		{
			token = mTokensSource.GetNextToken();
			if(!token.first)
			{
				TRACE_LOG("CPPStatementsParser::ParseUnionDeclaration, synatx error, not found keys list for enumartor");
				status = eFailure;
				break;
			}
		}

		// so now we are surely past the enum tag. now should be "{" or declarator
		if(token.second == "{")
		{
			// union definition case. define the union

			aUnion = mDefinitionContextStack.back()->CreateUnion(unionName,true);
			if(!aUnion)
			{
				status = eFailure;
				break;
			}

			// now cotinue with definining the members
			do
			{
				token = mTokensSource.GetNextToken();
				if(!token.first)
				{
					TRACE_LOG("CPPStatementsParser::ParseUnionDeclaration, synatx error, unexpected end in union variables list");
					status = eFailure;
					break;
				}

				if(token.second == "}")
					break;

				// not done, put back, and define variable
				mTokensSource.PutBackToken(token.second);
				
				DecleratorAsVariableContainer variablesDefinitionDriver(aUnion);

				if(ParseGenericDeclerationStatement(&variablesDefinitionDriver) != eSuccess)
				{
					TRACE_LOG("CPPStatementsParser::ParseUnionDeclaration, synatx error, unable to parse union");
					status = eFailure;
					break;
				}

			}while(eSuccess == status && token.first);
			
			if(status != eSuccess)
				break;

			// here check that it's true and "}"
			if(!token.first || token.second != "}")
			{
				status = eFailure;
				break;
			}

			// get next token to be in either semicolon or declarators
			token = mTokensSource.GetNextToken();
			if(!token.first)
			{
				TRACE_LOG("CPPStatementsParser::ParseUnionDeclaration, synatx error,unexpected end of expression. should be either semicolor or declarators");
				status = eFailure;
				break;
			}			

			if(token.second != ";")
			{
				// variables definition. put back token and parse declarators normally
				mTokensSource.PutBackToken(token.second);

				DecleratorAsVariableContainer variablesDefinitionDriver(mDefinitionContextStack.back());
				variablesDefinitionDriver.SetFlags(aUnion,false,false,false,false,false,false);

				if(ParseDeclarators(&variablesDefinitionDriver) != eSuccess) // will consume the semicolon as well
				{
					TRACE_LOG1("CPPStatementsParser::ParseUnionDeclaration, failed to parse declarators for union %s",unionName.c_str());
					break;
				}
			}

		}
		else
		{
			if(token.second == ";")
			{
				// union declaration only

				aUnion = mDefinitionContextStack.back()->CreateUnion(unionName,false);
				if(!aUnion)
				{
					status = eFailure;
					break;
				}
			}
			else
			{

				// variable declaration . this means that the union should already be defiend [name can be qualified at this point?]
				aUnion = (CPPUnion*)FindUnqualifiedElement(unionName,CPPElement::eCPPElementUnion);
				if(!aUnion)
				{
					TRACE_LOG1("CPPStatementsParser::ParseUnionDeclaration, union %s not found in union variable definition.",unionName.c_str());
					break;
				}

				// variables definition. put back token and parse declarators normally
				mTokensSource.PutBackToken(token.second);

				DecleratorAsVariableContainer variablesDefinitionDriver(mDefinitionContextStack.back());
				variablesDefinitionDriver.SetFlags(aUnion,false,false,false,false,false,false);

				if(ParseDeclarators(&variablesDefinitionDriver) != eSuccess) // will consume the semicolon as well
				{
					TRACE_LOG1("CPPStatementsParser::ParseUnionDeclaration, failed to parse declarators for union %s",unionName.c_str());
					break;
				}
			}
		}
	}while(false);	

	return status;
}

Hummus::EStatusCode CPPStatementsParser::ParseDeclarators(ICPPDeclarationContainerDriver* inContainer)
{
	// Having arrived here, we already know a type - either the return type of a function or a function pointer declaration, or the type of one or more field definitions.
	// The next phase, therefore, is to run, sometimes with a loop, and add to the container declerator(s).
	// This is carried out mainly in this function, and subordinate functions.
	// note that by the end of this function the delimeter for the statement is consumed. this can either be a semicolon in most cases, or a comma in the case
	// when parameters are parsed in a function declaration or function pointer declaration

	EStatusCode status = eSuccess;
	BoolAndString token;


	do
	{
		token = mTokensSource.GetNextToken();
		if(!token.first)
		{
			TRACE_LOG("CPPStatementsParser::ParseDeclarators, unexpected end of statement while parsing for a variable definition");
			status = eFailure;
			break;
		}

		// Now loop for delcarators. actually only field definitions may have more than one declarator, as oppose to other cases,
		// but the loop is ran still, and the underlying methods should fail if more than one definition occurs and it is inappropriate
		do
		{
			// start with some declarators modifiers - pointers/refrences which in turn could be const/volatile (or none)
			DeclaratorModifierList modifiers;

			while(token.first && token.second == "*" || token.second == "&" && (eSuccess == status))
			{
				DeclaratorModifier modifier;
				modifier.Modifier = (token.second == "&" ?  DeclaratorModifier::eDeclaratorModifierReference : DeclaratorModifier::eDeclaratorModifierPointer);

				// look for volatile/const
				token = mTokensSource.GetNextToken();
				if(token.first && (token.second == "volatile" || token.second == "const"))
				{
					if(token.second == "volatile")
						modifier.IsVolatile = true;
					else
						modifier.IsConst = true;
					// since there might be a second modifier, continue
					token = mTokensSource.GetNextToken();
					if(token.first && (token.second == "volatile" || token.second == "const"))
					{
						if(token.second == "volatile")
							modifier.IsVolatile = true;
						else
							modifier.IsConst = true;
						token = mTokensSource.GetNextToken();
					}
				}
				modifiers.push_back(modifier);

				// k. so now token should be post modifier. continue with loop
			}
			
			if(!token.first)
			{
				TRACE_LOG("CPPStatementsParser::ParseDeclarators, cannot read next token to determine functio pointer/variable. syntax error.");
				status = eFailure;
				break; 
			}

			EStatusCodeAndBool continuedParsingResult; 
			// return result from either functions specifies status, and whether they consumed a declarator stopper.
			// if they did, we can finish here.
			if(token.second == "(")
			{
				// Function Pointer, or a function that returns a function pointer. (will always be a single declaration)
				continuedParsingResult = ParseFunctionPointerOrFunction(inContainer,modifiers);
			}
			else
			{
				// Field or Function case. (need to parse the name to know)

				mTokensSource.PutBackToken(token.second);

				continuedParsingResult = ParseFieldOrFunction(inContainer,modifiers);
			}
			status = continuedParsingResult.first;
			if(status != eSuccess || continuedParsingResult.second) 
				break;

			// if underlying methods did not consume the declerator stopper, continue
			token = mTokensSource.GetNextToken();
			if(!token.first)
			{
				TRACE_LOG("CPPStatementsParser::ParseDeclarators, unexpected end, expecting either comma or semicolon");
				status = eFailure;
				break;
			}

			// k. now expecting either a stopper (semicolon for regulaer declarators, comma for parameters)
			if(inContainer->VerifyDeclaratorStopper(token.second))
				break;

			// if it's not, then there should always be a comma (again - there would be a comma and multiple declerators only in the case of multiple field definitions)
			if(token.second != ",")
			{
				TRACE_LOG1("CPPStatementsParser::ParseDeclarators, unexpected token, expecting a comma. got %s",token.second.c_str());
				status = eFailure;
				break;
			}
			
			token = mTokensSource.GetNextToken();
		}while(eSuccess == status && token.first);

	}while(false);

	return status;	
}

EStatusCode CPPStatementsParser::ParseGenericDeclerationStatement(ICPPDeclarationContainerDriver* inContainer)
{
	/* This generic parser method can parse either one of:
	   - variable declaration/definiton statements, be it of a regular type or a function pointer
	   - function definition

	   It can function for parsing anything that defines pairs of types and identifiers:
	   - regular members of general scope, a particular namespace, struct or class
	   - union members
	   - function parameters
	   etc. 
	*/

	EStatusCode status = eSuccess;
	bool typenameFound = false;
	bool isAuto = false;
	bool isRegister = false;
	bool isExtern = false;
	bool isConst = false;
	bool isVolatile = false;
	bool isStatic = false;
	BoolAndString token;

	// start by parsing storage specificers for the initial defined type (be it return type for function pointer, or a regualr field type)
	do
	{
		token = mTokensSource.GetNextToken();
		if(!token.first)
		{
			TRACE_LOG("CPPStatementsParser::ParseGenericDeclerationStatement, unexpected end of statement while parsing for a variable definition");
			status = eFailure;
			break;
		}

		if(token.second == "auto")
		{
			isAuto = true;
		}
		else if(token.second == "register")
		{
			isRegister = true;
		}
		else if(token.second == "extern")
		{
			isExtern = true;
		}
		else if(token.second == "const")
		{
			isConst = true;
		}
		else if(token.second == "volatile")
		{
			isVolatile = true;
		}
		else if(token.second == "static")
		{
			isStatic = true;
		}	
		else
		{
			mTokensSource.PutBackToken(token.second);
			typenameFound = true;
		}
	}while(eSuccess == status && !typenameFound);

	if(status != eSuccess)
		return status;


	do
	{
		// get the type itself
		CPPElement* anElement = GetElementFromCurrentLocation(true);
		if(!anElement)
		{
			TRACE_LOG("CPPStatementsParser::ParseGenericDeclerationStatement, error in using declaration, could not find designated type");
			status = eFailure;
			break;
		}

		// setup container with initial flags, before going to repetetive (potentioally) declarators definition
		if(inContainer->SetFlags(anElement,isAuto,isRegister,isExtern,isConst,isVolatile,isStatic) != eSuccess)
		{
			status = eFailure;
			break;
		}

		// Now continue with parsing declerators - one of more, in the case of fields, or one in the case of function pointer declaration or function definition/declaration
		// - will consume also statement ending
		status = ParseDeclarators(inContainer);
	}while(false);

	return status;
}

EStatusCode CPPStatementsParser::ParseTypedefDeclaration()
{
	// parsing a typedef follows similar syntax rules to variable definition. so let's reuse the main idea there
	DecleratorAsTypedefContainer typeedefDefinitionDriver(mDefinitionContextStack.back());

	return ParseGenericDeclerationStatement(&typeedefDefinitionDriver);
}

// now after having played initial games with types give a shot at variables & functions parsing

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