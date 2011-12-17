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
#include "ICPPDeclerator.h"
#include "ICPPDeclaratorContainer.h"
#include "CPPTypedef.h"
#include "UsedTypeDescriptor.h"

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
		return EStatusCodeAndHeaderUnit(eFailure,NULL);
	}
}

void CPPStatementsParser::SetupPrimitiveTypes()
{
	// iterate primitive enums and create a type for each
	int i = eCPPBool;
	for(; i < eCPPPrimitivesMax; ++i)
	{
		CPPPrimitive* newPrimitive = new CPPPrimitive(scPrimitivesLabels[(int)i],(ECPPPrimitiveType)i);
		mWorkingUnit->GetGlobalNamespace()->AppendBasicType(scPrimitivesLabels[(int)i],newPrimitive);
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

			// TODO + now, if haven't done yet, get back to sizeof and defined and casting operators and complete with also parsing type "expressions"

			

			status = eFailure;
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
			// qualified
			anElement = (firstToken.second == "::") ? mWorkingUnit->GetGlobalNamespace() : FindUnqualifiedElement(firstToken.second);
			BoolAndString token = (firstToken.second == "::") ? secondToken : mTokensSource.GetNextToken(); // token should now have the thing next to ::
			
			while(anElement && token.first)
			{
				if(IsTypenamesContainer(anElement))
				{
					// k. now look the element named in token inside it
					anElement = ((ICPPDefinitionsContainerElement*)anElement)->FindElement(token.second);
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
				else
				{
					TRACE_LOG1("CPPStatementsParser::GetElementFromCurrentLocation, element is qualifying another element, but is not a container of types, %s",anElement->Name.c_str());
					anElement = NULL;
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
			anElement = FindUnqualifiedElement(elementName);

		}

	}while(false);

	return anElement;
}

CPPElement* CPPStatementsParser::FindUnqualifiedElement(const string& inElementName)
{
	// first go through the current stack (back to front)
	ICPPDefinitionsContainerElementList::reverse_iterator itStack =  mDefinitionContextStack.rbegin();
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

// declarator container for variables definition
class DecleratorAsVariabeContainer : public ICPPDeclaratorContainer
{
public:
	DecleratorAsVariabeContainer(ICPPVariablesContainerElement* inStorage);

	virtual EStatusCode SetFlags(CPPElement* inType,bool inIsAuto,bool inIsRegister,bool inIsExtern,bool inIsConst,bool inIsVolatile, bool inIsStatic);
	virtual ICPPDeclerator* AddDeclarator(const string& inDecleratorName);
	virtual ICPPFunctionPointerDeclerator* AddFunctionPointerDeclarator(const string& inDecleratorName);
	virtual bool VerifyDeclaratorStopper(const string& inTokenToExamine);

private:

	ICPPVariablesContainerElement* mStorage;
	CPPElement* mType;
	bool mIsAuto;
	bool mIsRegister;
	bool mIsExtern;
	bool mIsConst;
	bool mIsVolatile;
	bool mIsStatic;
};

DecleratorAsVariabeContainer::DecleratorAsVariabeContainer(ICPPVariablesContainerElement* inStorage)
{
	mStorage = inStorage;
	mIsAuto = false;
	mIsRegister = false;
	mIsExtern = false;
	mIsConst = false;
	mIsVolatile = false;
	mIsStatic = false;	
}

EStatusCode DecleratorAsVariabeContainer::SetFlags(CPPElement* inType,bool inIsAuto,bool inIsRegister,bool inIsExtern,bool inIsConst,bool inIsVolatile, bool inIsStatic)
{
	mType = inType;
	mIsAuto = inIsAuto;
	mIsRegister = inIsRegister;
	mIsExtern = inIsExtern;
	mIsConst = inIsConst;
	mIsVolatile = inIsVolatile;
	mIsStatic = inIsStatic;	

	return eSuccess;
}

ICPPDeclerator* DecleratorAsVariabeContainer::AddDeclarator(const string& inDecleratorName)
{
	UsedTypeDescriptor* newTypeDescriptor = new UsedTypeDescriptor(mType,mIsAuto,mIsRegister,mIsExtern,mIsConst,mIsVolatile,mIsStatic);

	CPPVariable* aVariable = mStorage->CreateVariable(inDecleratorName,newTypeDescriptor);
	return aVariable ? aVariable->GetTypeDescriptor()->GetFieldDescriptor() : NULL;
}


ICPPFunctionPointerDeclerator* DecleratorAsVariabeContainer::AddFunctionPointerDeclarator(const string& inDecleratorName)
{
	UsedTypeDescriptor* returnTypeDescriptor = new UsedTypeDescriptor(mType,mIsAuto,mIsRegister,mIsExtern,mIsConst,mIsVolatile,mIsStatic);
	UsedTypeDescriptor* functionPointerDescriptor = new UsedTypeDescriptor(returnTypeDescriptor);

	CPPVariable* aVariable = mStorage->CreateVariable(inDecleratorName,functionPointerDescriptor);
	return aVariable ? aVariable->GetTypeDescriptor()->GetFunctionPointerDescriptor() : NULL;
}


bool DecleratorAsVariabeContainer::VerifyDeclaratorStopper(const string& inTokenToExamine)
{
	return inTokenToExamine == ";";
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

			anEnumerator = mDefinitionContextStack.back()->CreateEnumerator(enumeratorName);
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
		}
		else
		{
			// variable declaration only. this means that the enumerator should already be defiend [name can be qualified at this point?]
			CPPElement* anElement = FindUnqualifiedElement(enumeratorName);
			if(!anElement)
			{
				TRACE_LOG1("CPPStatementsParser::ParseEnumeratorDeclaration, enumerator %s not found in enumerator variable definition.",enumeratorName.c_str());
				break;
			}

			if(anElement->Type != CPPElement::eCPPElementEnumerator)
			{
				TRACE_LOG1("CPPStatementsParser::ParseEnumeratorDeclaration, type for enumerator definition is not an enumerator for %s",enumeratorName.c_str());
				break;
			}

			anEnumerator = (CPPEnumerator*)anElement;
		}

		if(token.second != ";")
		{
			// variables definition. put back token and parse declarators normally
			mTokensSource.PutBackToken(token.second);

			DecleratorAsVariabeContainer variablesDefinitionDriver(mDefinitionContextStack.back());
			variablesDefinitionDriver.SetFlags(anEnumerator,false,false,false,false,false,false);

			if(ParseAndDefineDeclarators(&variablesDefinitionDriver) != eSuccess) // will consume the semicolon as well
			{
				TRACE_LOG1("CPPStatementsParser::ParseEnumeratorDeclaration, failed to parse declarators for enumerator %s",enumeratorName.c_str());
				break;
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

EStatusCode CPPStatementsParser::ParseAndDefineField(ICPPDeclaratorContainer* inContainer,const DeclaratorModifierList& inFieldModifiersList)
{
	EStatusCode status = eSuccess;
	string decleratorName;

	BoolAndString token = mTokensSource.GetNextToken();	
	if(!token.first)
	{
		TRACE_LOG("CPPStatementsParser::ParseAndDefineField, tokens not found when trying to parse for fields");
		return eFailure;
	}

	do
	{
		ICPPDeclerator* aDeclarator;

		if(token.second != "[" && !inContainer->VerifyDeclaratorStopper(token.second))
			aDeclarator = inContainer->AddDeclarator(decleratorName);
		else
			aDeclarator = inContainer->AddDeclarator(""); // unnamed declarator. normally only found in function parameters
		if(!aDeclarator)
		{
			TRACE_LOG1("CPPStatementsParser::ParseAndDefineField, unable to create a declarator with the name %s.",decleratorName.c_str());
			status = eFailure;
			break;
		}
		
		aDeclarator->AppendModifiers(inFieldModifiersList);

		if(inContainer->VerifyDeclaratorStopper(token.second))
		{
			// k. unnamed parameter, break;
			mTokensSource.PutBackToken(token.second); // put back this token, which is not used
			break;
		}

		// subscripts
		token = mTokensSource.GetNextToken();
		if(!token.first)
		{
			TRACE_LOG("CPPStatementsParser::ParseAndDefineField, unexpected end, should have semicolon, initializor, comma or subscript marks");
			status = eFailure;
			break;
		}

		while(token.first && token.second == "[")
		{
			aDeclarator->AddSubscript();

			token = mTokensSource.GetNextToken();
			if(!token.first)
			{
				TRACE_LOG("CPPStatementsParser::ParseAndDefineField, unexpected end, expected an expression or closing bracket ']'");
				status = eFailure;
				break;
			}

			if(token.second != "]")
			{
				mTokensSource.PutBackToken(token.second);
				status = SkipConstantExpression();
				if(status != eSuccess)
					break;

				token = mTokensSource.GetNextToken();
				if(!token.first || token.second != "]")
				{
					TRACE_LOG("CPPStatementsParser::ParseAndDefineField, unexpected end or wrong token, expected closing bracket ']'");
					status = eFailure;
					break;
				}
			}
			token = mTokensSource.GetNextToken();
		}
		if(status != eSuccess)
			break;

		// k. done with subscripts, now initializers skipping
		if(token.second == "=")
		{
			status = SkipInitializer();
			if(status != eSuccess)
				break;
		}
		else
			mTokensSource.PutBackToken(token.second);
	}while(false);

	return status;

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

Hummus::EStatusCode CPPStatementsParser::SkipBlock()
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

class DecleratorAsParametersContainer : public ICPPDeclaratorContainer
{
public:

	DecleratorAsParametersContainer(ICPPFunctionPointerDeclerator* inFunctionPointer);
	
	virtual EStatusCode SetFlags(CPPElement* inType,bool inIsAuto,bool inIsRegister,bool inIsExtern,bool inIsConst,bool inIsVolatile, bool inIsStatic);
	virtual ICPPDeclerator* AddDeclarator(const string& inDecleratorName);
	virtual ICPPFunctionPointerDeclerator* AddFunctionPointerDeclarator(const string& inDecleratorName);
	virtual bool VerifyDeclaratorStopper(const string& inTokenToExamine);

	bool FoundStop();
	void Reset();

private:
	ICPPFunctionPointerDeclerator* mFunctionPointer;

	bool mIsConst;
	bool mIsVolatile;	
	CPPElement* mType;
	bool mFoundStop;
	bool mAlreadyDefinedOne;
};

DecleratorAsParametersContainer::DecleratorAsParametersContainer(ICPPFunctionPointerDeclerator* inFunctionPointer)
{
	mFoundStop = false;
	mFunctionPointer = inFunctionPointer;
	mAlreadyDefinedOne = true;
}

EStatusCode DecleratorAsParametersContainer::SetFlags(CPPElement* inType,bool inIsAuto,bool inIsRegister,bool inIsExtern,bool inIsConst,bool inIsVolatile, bool inIsStatic)
{
	mIsConst = inIsConst;
	mIsVolatile = inIsVolatile;
	mType = inType;

	if(inIsAuto || inIsRegister || inIsExtern || inIsStatic)
	{
		TRACE_LOG("DecleratorAsParametersContainer::SetFlags, irelevant flags set for typedef. Should not set auto, extern, register or static. only const and volatile are allowed.");
		return eFailure;
	}
	else
		return eSuccess;
}

ICPPDeclerator* DecleratorAsParametersContainer::AddDeclarator(const string& inDecleratorName)
{	
	if(mAlreadyDefinedOne)
	{
		TRACE_LOG("DecleratorAsParametersContainer::AddDeclarator, typedef statement contains multiple declarator, this is a syntax error");
		return NULL;
	}

	mAlreadyDefinedOne = true;

	UsedTypeDescriptor* usedTypeDescriptor = new UsedTypeDescriptor(mType,false,false,false,mIsConst,mIsVolatile,false);
	FunctionParameter* newParameter = mFunctionPointer->CreateParameter(inDecleratorName,usedTypeDescriptor);
	return newParameter ? newParameter->Type->GetFieldDescriptor():NULL;
}

ICPPFunctionPointerDeclerator* DecleratorAsParametersContainer::AddFunctionPointerDeclarator(const string& inDecleratorName)
{
	if(mAlreadyDefinedOne)
	{
		TRACE_LOG("DecleratorAsParametersContainer::AddFunctionPointerDeclarator, typedef statement contains multiple declarator, this is a syntax error");
		return NULL;
	}

	mAlreadyDefinedOne = true;

	UsedTypeDescriptor* returnTypeDescriptor = new UsedTypeDescriptor(mType,false,false,false,mIsConst,mIsVolatile,false);
	UsedTypeDescriptor* functionPointerDescriptor = new UsedTypeDescriptor(returnTypeDescriptor);

	FunctionParameter* newParameter = mFunctionPointer->CreateParameter(inDecleratorName,functionPointerDescriptor);
	return newParameter ? newParameter->Type->GetFunctionPointerDescriptor():NULL;
}


void DecleratorAsParametersContainer::Reset()
{
	mAlreadyDefinedOne = false;
	mIsConst = false;
	mIsVolatile = false;
}

bool DecleratorAsParametersContainer::VerifyDeclaratorStopper(const string& inTokenToExamine)
{
	if(inTokenToExamine == ",")
		return true;
	else if(inTokenToExamine == ")")
	{
		mFoundStop = true;
		return true;
	}
	else 
		return false;
}

bool DecleratorAsParametersContainer::FoundStop()
{
	return mFoundStop;
}

EStatusCode CPPStatementsParser::ParseAndDefineFunctionPointer(ICPPDeclaratorContainer* inContainer,const DeclaratorModifierList& inReturnTypeModifiersList)
{
	// function pointers look like this:
	// T (name)(params)
	// or
	// T (*name)(params)
	// or
	// T (&name)(params)
	// at this point we already have the "(" part consumed, so now there should be 
	// either "*" or "&" or the name
	EStatusCode status = eSuccess;
	ICPPFunctionPointerDeclerator::EFunctionPointerType pointerType;
	string decleratorName;

	BoolAndString token = mTokensSource.GetNextToken();	
	if(!token.first)
	{
		TRACE_LOG("CPPStatementsParser::ParseAndDefineFunctionPointer, tokens not found when trying to parse for function pointers");
		return eFailure;
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
				TRACE_LOG("CPPStatementsParser::ParseAndDefineFunctionPointer, no token when trying to parse for function pointer name");
				status = eFailure;
				break;
			}
		}
		else
			pointerType = ICPPFunctionPointerDeclerator::eFunctionPointerTypeNone;

		decleratorName = token.second;

		token = mTokensSource.GetNextToken();	
		if(!token.first || token.second != ")")
		{
			TRACE_LOG("CPPStatementsParser::ParseAndDefineFunctionPointer, expected ) in parsing function pointer declaration, and not found");
			status = eFailure;
			break;
		}
		token = mTokensSource.GetNextToken();	
		if(!token.first || token.second != "(")
		{
			TRACE_LOG("CPPStatementsParser::ParseAndDefineFunctionPointer, expected ( in parsing function pointer declaration, and not found");
			status = eFailure;
			break;
		}
		
		// ok. we have enough, can create a function pointer object, and go on with parsing parameters later
		ICPPFunctionPointerDeclerator* aDeclarator = inContainer->AddFunctionPointerDeclarator(decleratorName);
		if(!aDeclarator)
		{
			TRACE_LOG1("CPPStatementsParser::ParseAndDefineFunctionPointer, unable to create a function declarator with the name %s.",decleratorName.c_str());
			status = eFailure;
			break;
		}

		aDeclarator->SetFunctionPointerType(pointerType);
		aDeclarator->AppendModifiersForReturnType(inReturnTypeModifiersList);

		token = mTokensSource.GetNextToken();	
		
		DecleratorAsParametersContainer parametersContainer(aDeclarator);

		// Now for parameters parsing
		while(token.first && token.second != ")" && eSuccess == status)
		{
			if(token.second == "...")
			{
				aDeclarator->SetHasElipsis();
				token = mTokensSource.GetNextToken();
				if(!token.first)
				{
					TRACE_LOG("CPPStatementsParser::ParseAndDefineFunctionPointer, unexpected no tokens after elipsis. expected )");
					status = eFailure;
				}
				else if(token.second != ")")
				{
					TRACE_LOG1("CPPStatementsParser::ParseAndDefineFunctionPointer, unexpected token after elipsis. expected ), got %s",token.second.c_str());
					status = eFailure;
				}
				break;
			}
			else
			{
				status = ParseVariablesDefinitionStatement(&parametersContainer);
				if(status != eSuccess)
				{
					TRACE_LOG("CPPStatementsParser::ParseAndDefineFunctionPointer, failed to parase function pointer paremeter");
					break;
				}
				
				if(parametersContainer.FoundStop())	
					break;

				parametersContainer.Reset();
			}
		}
	} while(false);

	return status;
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

			aUnion = mDefinitionContextStack.back()->CreateUnion(unionName);
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
				
				DecleratorAsVariabeContainer variablesDefinitionDriver(aUnion);

				if(ParseVariablesDefinitionStatement(&variablesDefinitionDriver) != eSuccess)
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
		}
		else
		{
			// variable declaration only. this means that the enumerator should already be defiend [name can be qualified at this point?]
			CPPElement* anElement = FindUnqualifiedElement(unionName);
			if(!anElement)
			{
				TRACE_LOG1("CPPStatementsParser::ParseUnionDeclaration, union %s not found in union variable definition.",unionName.c_str());
				break;
			}

			if(anElement->Type != CPPElement::eCPPElementUnion)
			{
				TRACE_LOG1("CPPStatementsParser::ParseUnionDeclaration, type for union definition is not an union for %s",unionName.c_str());
				break;
			}

			aUnion = (CPPUnion*)anElement;
		}

		if(token.second != ";")
		{
			// variables definition. put back token and parse declarators normally
			mTokensSource.PutBackToken(token.second);

			DecleratorAsVariabeContainer variablesDefinitionDriver(mDefinitionContextStack.back());
			variablesDefinitionDriver.SetFlags(aUnion,false,false,false,false,false,false);

			if(ParseAndDefineDeclarators(&variablesDefinitionDriver) != eSuccess) // will consume the semicolon as well
			{
				TRACE_LOG1("CPPStatementsParser::ParseUnionDeclaration, failed to parse declarators for union %s",unionName.c_str());
				break;
			}
		}

	}while(false);	

	return status;
}

Hummus::EStatusCode CPPStatementsParser::ParseAndDefineDeclarators(ICPPDeclaratorContainer* inContainer)
{
	// This could be either a regular field variable or a function pointer. both are similar but essentially different. so parse in accordance.
	// note that variable definitions statement may contain more than one element defined, so hence there's a loop internally. the container
	// should provide information on when to stop

	EStatusCode status = eSuccess;
	BoolAndString token;


	do
	{
		token = mTokensSource.GetNextToken();
		if(!token.first)
		{
			TRACE_LOG("CPPStatementsParser::ParseVariablesDefinitionStatement, unexpected end of statement while parsing for a variable definition");
			status = eFailure;
			break;
		}

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
				TRACE_LOG("CPPStatementsParser::ParseAndDefineDeclarators, anonymous declarators encountered. invalid at this point.");
				status = eFailure;
				break; // anonymous declarator. at this point not allowed.
			}

			// now, either function pointer token or name
			if(token.second == "(")
			{
				status = ParseAndDefineFunctionPointer(inContainer,modifiers);
			}
			else
			{
				// Field case

				mTokensSource.PutBackToken(token.second);

				status = ParseAndDefineField(inContainer,modifiers);
			}
			// run either parsing method. [make sure they put back if the token is outside]

			token = mTokensSource.GetNextToken();
			if(!token.first)
			{
				TRACE_LOG("CPPStatementsParser::ParseAndDefineDeclarators, unexpected end, expecting either comma or semicolon");
				status = eFailure;
				break;
			}

			// k. now expecting either a stopper (semicolon for regulaer declarators, comma for parameters)
			if(inContainer->VerifyDeclaratorStopper(token.second))
				break;

			// if it's not, then there should always be a comma
			if(token.second != ",")
			{
				TRACE_LOG1("CPPStatementsParser::ParseAndDefineDeclarators, unexpected token, expecting a comma. got %s",token.second.c_str());
				status = eFailure;
				break;
			}
			
			token = mTokensSource.GetNextToken();
		}while(eSuccess == status && token.first);

	}while(false);

	return status;	
}

EStatusCode CPPStatementsParser::ParseVariablesDefinitionStatement(ICPPDeclaratorContainer* inContainer)
{
	// This could be either a regular field variable or a function pointer. both are similar but essentially different. so parse in accordance.
	// note that variable definitions statement may contain more than one element defined, so hence there's a loop internally. the container
	// should provide information on when to stop

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
			TRACE_LOG("CPPStatementsParser::ParseVariablesDefinitionStatement, unexpected end of statement while parsing for a variable definition");
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
		CPPElement* anElement = GetElementFromCurrentLocation();
		if(!anElement)
		{
			TRACE_LOG("CPPStatementsParser::ParseVariablesDefinitionStatement, error in using declaration, could not find designated type");
			status = eFailure;
			break;
		}

		// make sure it's a type
		if(!anElement->IsType())
		{
			TRACE_LOG1("CPPStatementsParser::ParseVariablesDefinitionStatement, expecting to find type, but found %s",anElement->Name);
			status = eFailure;
			break;
		}

		// setup container with initial flags, before going to repetetive (potentioally) declarators definition
		if(inContainer->SetFlags(anElement,isAuto,isRegister,isExtern,isConst,isVolatile,isStatic) != eSuccess)
		{
			status = eFailure;
			break;
		}

		status = ParseAndDefineDeclarators(inContainer);
	}while(false);

	return status;
}

class DecleratorAsTypedefContainer : public ICPPDeclaratorContainer
{

public:
	DecleratorAsTypedefContainer(ICPPDefinitionsContainerElement* inStorage);

	virtual EStatusCode SetFlags(CPPElement* inType,bool inIsAuto,bool inIsRegister,bool inIsExtern,bool inIsConst,bool inIsVolatile, bool inIsStatic);
	virtual ICPPDeclerator* AddDeclarator(const string& inDeclaratorName);
	virtual ICPPFunctionPointerDeclerator* AddFunctionPointerDeclarator(const string& inDecleratorName);
	virtual bool VerifyDeclaratorStopper(const string& inTokenToExamine);

private:
	ICPPDefinitionsContainerElement* mStorage;
	CPPElement* mType;
	bool mIsConst;
	bool mIsVolatile;
	bool mAlreadyDefinedOne;
};

DecleratorAsTypedefContainer::DecleratorAsTypedefContainer(ICPPDefinitionsContainerElement* inStorage)
{
	mStorage = inStorage;
	mAlreadyDefinedOne = false;
}

EStatusCode DecleratorAsTypedefContainer::SetFlags(CPPElement* inType,bool inIsAuto,bool inIsRegister,bool inIsExtern,bool inIsConst,bool inIsVolatile, bool inIsStatic)
{
	mIsConst = inIsConst;
	mIsVolatile = inIsVolatile;
	mType = inType;

	if(inIsAuto || inIsRegister || inIsExtern || inIsStatic)
	{
		TRACE_LOG("DecleratorAsTypedefContainer::SetFlags, irelevant flags set for typedef. Should not set auto, extern, register or static. only const and volatile are allowed.");
		return eFailure;
	}
	else
		return eSuccess;
}

ICPPDeclerator* DecleratorAsTypedefContainer::AddDeclarator(const string& inDeclaratorName)
{
	if(mAlreadyDefinedOne)
	{
		TRACE_LOG("DecleratorAsTypedefContainer::AddDeclarator, typedef statement contains multiple declarator, this is a syntax error");
		return NULL;
	}

	mAlreadyDefinedOne = true;

	UsedTypeDescriptor* usedTypeDescriptor = new UsedTypeDescriptor(mType,false,false,false,mIsConst,mIsVolatile,false);

	CPPTypedef* newTypeDef = mStorage->CreateTypedef(inDeclaratorName,usedTypeDescriptor);
	return newTypeDef ? newTypeDef->GetTypeDescriptor()->GetFieldDescriptor() : NULL;
}

ICPPFunctionPointerDeclerator* DecleratorAsTypedefContainer::AddFunctionPointerDeclarator(const string& inDecleratorName)
{
	if(mAlreadyDefinedOne)
	{
		TRACE_LOG("DecleratorAsTypedefContainer::AddDeclarator, typedef statement contains multiple declarator, this is a syntax error");
		return NULL;
	}

	mAlreadyDefinedOne = true;

	UsedTypeDescriptor* returnTypeDescriptor = new UsedTypeDescriptor(mType,false,false,false,mIsConst,mIsVolatile,false);
	UsedTypeDescriptor* functionPointerDescriptor = new UsedTypeDescriptor(returnTypeDescriptor);

	CPPTypedef* newTypeDef = mStorage->CreateTypedef(inDecleratorName,functionPointerDescriptor);
	return newTypeDef ? newTypeDef->GetTypeDescriptor()->GetFunctionPointerDescriptor() : NULL;
}


bool DecleratorAsTypedefContainer::VerifyDeclaratorStopper(const string& inTokenToExamine)
{
	return inTokenToExamine== ";";
}


EStatusCode CPPStatementsParser::ParseTypedefDeclaration()
{
	// parsing a typedef follows similar syntax rules to variable definition. so let's reuse the main idea there
	DecleratorAsTypedefContainer typeedefDefinitionDriver(mDefinitionContextStack.back());

	return ParseVariablesDefinitionStatement(&typeedefDefinitionDriver);
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