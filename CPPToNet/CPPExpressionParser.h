#pragma once

#include <utility>
#include <string>
#include <list>

class ITokenProvider;
class CPPExpression;
class CPPOperator;
class ITypeParserHelper;
struct TypedParameter;

using namespace std;

typedef pair<bool,CPPExpression*> BoolAndCPPExpression;
typedef pair<bool,CPPOperator*> BoolAndCPPOperator;
typedef list<CPPExpression*> CPPExpressionList;
typedef list<string> StringList;

class CPPExpressionParser
{
public:
	CPPExpressionParser(void);
	~CPPExpressionParser(void);

	// pass inTypeParserHelper element in order to parse expressions where there may be type information required (sizof, casting)
	BoolAndCPPExpression ParseExpression(ITokenProvider* inTokenProvider,ITypeParserHelper* inTypeParserHelper =  NULL);

private:

	ITokenProvider* mOriginalProvider;
	ITypeParserHelper* mTypeParserHelper;

	BoolAndCPPExpression ParseExpressionInternal(ITokenProvider* inTokenProvider);
	BoolAndCPPExpression ParseExpressionInternal(ITokenProvider* inTokenProvider,CPPExpression* inFirstOperand,CPPOperator* inOperator);
	BoolAndCPPExpression ParseSingleExpression(ITokenProvider* inTokenProvider,
												 CPPExpression* inFirstOperand,
												 CPPOperator* inOperator,
												 CPPOperator** outNextOperator);

	BoolAndCPPOperator ParseMultinaryOperator(ITokenProvider* inProvider);
	BoolAndCPPExpression ParseOperand(ITokenProvider* inProvider);
	CPPExpression* MakeExpression(CPPOperator* inOperator,CPPExpression* inFirstOperand, CPPExpression* inLastOperand, CPPExpression* inOptionalMiddleOperand);
	BoolAndCPPOperator MakeOperator(const string& inToken, bool inIsBinary);
	BoolAndCPPOperator MakeOperator(const string& inToken, CPPExpression* inParameter,bool inIsBinary);
	BoolAndCPPExpression ParseUnaryOperatorOperand(ITokenProvider* inProvider,CPPOperator* inOperator);
	BoolAndCPPExpression MakeCharacter(const string& inToken);
	BoolAndCPPExpression MakeString(const string& inToken);
	BoolAndCPPExpression MakeNumber(const string& inToken);
	BoolAndCPPExpression MakeVariable(const string& inToken,const StringList& inScopes);
	BoolAndCPPExpression ParseFunctionCall(const string& inToken,const StringList& inScopes,ITokenProvider* inTokenProvider);
	BoolAndCPPExpression MakeFunctionCall(const string& inToken,const StringList& inScopes,const CPPExpressionList& inParameters);
	BoolAndCPPExpression ParseExpressionType(ITokenProvider* inProvider,const string& inTypeDelimiter);
	BoolAndCPPExpression MakeTypename(TypedParameter* inTypename);
	bool IsPostFixOperator(const string& inToken);
	BoolAndCPPExpression ParsePostFixOperatorOperand(ITokenProvider* inProvider,CPPExpression* inOperand,const string& inFirstOperatorToken);
	BoolAndCPPOperator ParseUnaryOperator(ITokenProvider* inProvider, const string& inToken);

	bool IsOperandToParseAType(ITokenProvider* inProvider);
	BoolAndCPPExpression FalseExpression();
	BoolAndCPPExpression ParseNewExpression(ITokenProvider* inProvider);

};
