#pragma once

#include <utility>
#include <string>

class IPreprocessorConditionTokenProvider;
class CPPExpression;
class CPPOperator;
class PreProcessor;

using namespace std;

typedef pair<bool,CPPExpression*> BoolAndCPPExpression;
typedef pair<bool,CPPOperator*> BoolAndCPPOperator;

class CPPExpressionParser
{
public:
	CPPExpressionParser(void);
	~CPPExpressionParser(void);

	BoolAndCPPExpression ParseExpression(IPreprocessorConditionTokenProvider* inTokenProvider,PreProcessor* inSymbolsSource);

private:

	IPreprocessorConditionTokenProvider* mOriginalProvider;
	PreProcessor* mSymbolsSource;

	BoolAndCPPExpression ParseExpressionInternal(IPreprocessorConditionTokenProvider* inTokenProvider);
	BoolAndCPPExpression ParseExpressionInternal(IPreprocessorConditionTokenProvider* inTokenProvider,CPPExpression* inFirstOperand,CPPOperator* inOperator);
	BoolAndCPPExpression ParseSingleExpression(IPreprocessorConditionTokenProvider* inTokenProvider,
												 CPPExpression* inFirstOperand,
												 CPPOperator* inOperator,
												 CPPOperator** outNextOperator);

	BoolAndCPPOperator ParseOperator(IPreprocessorConditionTokenProvider* inProvider, bool inIsBinary);
	BoolAndCPPExpression ParseOperand(IPreprocessorConditionTokenProvider* inProvider);
	BoolAndCPPExpression ParseOperandForDefined(IPreprocessorConditionTokenProvider* inProvider);
	CPPExpression* MakeExpression(CPPOperator* inOperator,CPPExpression* inFirstOperand, CPPExpression* inLastOperand, CPPExpression* inOptionalMiddleOperand);
	BoolAndCPPOperator MakeOperator(const string& inToken, bool inIsBinary);
	BoolAndCPPExpression ParseUnaryOperatorOperand(IPreprocessorConditionTokenProvider* inProvider,CPPOperator* inOperator);
	BoolAndCPPExpression MakeCharacter(const string& inToken);
	BoolAndCPPExpression MakeInteger(const string& inToken);

};
