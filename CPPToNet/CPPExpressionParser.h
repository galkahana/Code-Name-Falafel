#pragma once

#include <utility>
#include <string>
#include <list>

class ITokenProvider;
class CPPExpression;
class CPPOperator;

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

	BoolAndCPPExpression ParseExpression(ITokenProvider* inTokenProvider);

private:

	ITokenProvider* mOriginalProvider;

	BoolAndCPPExpression ParseExpressionInternal(ITokenProvider* inTokenProvider);
	BoolAndCPPExpression ParseExpressionInternal(ITokenProvider* inTokenProvider,CPPExpression* inFirstOperand,CPPOperator* inOperator);
	BoolAndCPPExpression ParseSingleExpression(ITokenProvider* inTokenProvider,
												 CPPExpression* inFirstOperand,
												 CPPOperator* inOperator,
												 CPPOperator** outNextOperator);

	BoolAndCPPOperator ParseOperator(ITokenProvider* inProvider, bool inIsBinary);
	BoolAndCPPExpression ParseOperand(ITokenProvider* inProvider);
	CPPExpression* MakeExpression(CPPOperator* inOperator,CPPExpression* inFirstOperand, CPPExpression* inLastOperand, CPPExpression* inOptionalMiddleOperand);
	BoolAndCPPOperator MakeOperator(const string& inToken, bool inIsBinary);
	BoolAndCPPExpression ParseUnaryOperatorOperand(ITokenProvider* inProvider,CPPOperator* inOperator);
	BoolAndCPPExpression MakeCharacter(const string& inToken);
	BoolAndCPPExpression MakeInteger(const string& inToken);
	BoolAndCPPExpression MakeVariable(const string& inToken,const StringList& inScopes);
	BoolAndCPPExpression ParseFunctionCall(const string& inToken,const StringList& inScopes,ITokenProvider* inTokenProvider);
	BoolAndCPPExpression MakeFunctionCall(const string& inToken,const StringList& inScopes,const CPPExpressionList& inParameters);
	bool IsPostFixOperator(const string& inToken);
	BoolAndCPPExpression ParsePostFixOperatorOperand(ITokenProvider* inProvider,CPPExpression* inOperand,const string& inFirstOperatorToken);

};
