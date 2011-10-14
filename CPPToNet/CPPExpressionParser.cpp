#include "CPPExpressionParser.h"
#include "IPreprocessorConditionTokenProvider.h"
#include "ParenthesisConditionalTokenProvider.h"
#include "Trace.h"
#include "CPPOperator.h"
#include "CPPExpression.h"
#include "CPPOperatorExpression.h"
#include "CPPExpressionSymbol.h"
#include "CPPExpressionInteger.h"

using namespace Hummus;

CPPExpressionParser::CPPExpressionParser(void)
{
}

CPPExpressionParser::~CPPExpressionParser(void)
{
}

BoolAndCPPExpression CPPExpressionParser::ParseExpression(IPreprocessorConditionTokenProvider* inTokenProvider,PreProcessor* inSymbolsSource)
{
	mOriginalProvider = inTokenProvider;
	mSymbolsSource = inSymbolsSource;
	return ParseExpressionInternal(mOriginalProvider);
}

BoolAndCPPExpression CPPExpressionParser::ParseExpressionInternal(IPreprocessorConditionTokenProvider* inTokenProvider)
{
	BoolAndCPPExpression operandResult;
	BoolAndCPPOperator operatorResult;
	BoolAndCPPExpression result;

	// parse first (and possibly only) operand
	operandResult = ParseOperand(inTokenProvider);

	if(!operandResult.first)
		return BoolAndCPPExpression(false,NULL);

	// parse operator (binary or conditional)
	operatorResult = ParseOperator(inTokenProvider,true);
	

	// continue with creating expression in main method
	return ParseExpressionInternal(inTokenProvider,operandResult.second,operatorResult.first ? operatorResult.second : NULL);
}

BoolAndCPPExpression CPPExpressionParser::ParseExpressionInternal(IPreprocessorConditionTokenProvider* inTokenProvider,CPPExpression* inFirstOperand,CPPOperator* inOperator)
{
	BoolAndCPPExpression expressionResult;
	BoolAndCPPOperator operatorResult;
	CPPExpression* anOperand = inFirstOperand;
	CPPOperator* anOperator = inOperator;
	bool statusOK = true;

	// loop, parsing the expression. 
	// each loop starts with an operand and an operator. while in loop, get the next operand which may be compound due to precedence laws.
	// then make an expression out of it, and set it as the new operand. Then either get the next operator as leftover from reading the next operand
	// or read it from the code. so on, till you run out of next operators...which means it's time to return the operand as the end result expression
	while(statusOK && anOperator)
	{
		expressionResult = ParseSingleExpression(inTokenProvider,anOperand,anOperator,&anOperator);
		if(!expressionResult.first)
		{
				statusOK = false;
				break;
		}

		anOperand = expressionResult.second;
		if(!anOperator)
		{
			operatorResult = ParseOperator(inTokenProvider,true);
			if(!operatorResult.first)
				break;
			anOperator = operatorResult.second;
		}
	}

	if(!statusOK)
	{
		delete anOperator;
		delete anOperand;
		return BoolAndCPPExpression(false,NULL);
	}
	else
	{
		return BoolAndCPPExpression(true,anOperand);
	}
}

BoolAndCPPExpression CPPExpressionParser::ParseSingleExpression(IPreprocessorConditionTokenProvider* inTokenProvider,
																 CPPExpression* inFirstOperand,
																 CPPOperator* inOperator,
																 CPPOperator** outNextOperator)
{
	/*
		The method recieved the first agrument and operator, and should now parse
		the next operand, as an end result it should return the expression for this operator, and optionally the next operator. 
		To realize precedence rules, the next operator is also sought after (parsed).
		if it precedes the first operator than recursion is being called, with the next operand and next operator, to form an operand from them.
		The returning value from the recursion is an operand, and also possibly an operator. that operator does not precede the operator for which
		the recursion was called (otherwise it wouldn't return, but rather recurse in turn)...however it may precede the original first operator. in such a case
		repert the the recursion, and continue repeating until the first operator which does not precede the original operator. Good! now we got operand, operator and operand and can make an expression.
		return this as an expression and if in this process there's a later operator (which at this point should not precede the original operator), return it too (for higher level precedence check, or just
		to continue the parsing).
	*/

	BoolAndCPPExpression operandResult;
	BoolAndCPPOperator operatorResult;
	CPPExpression* secondOperand = NULL;
	CPPExpression* firstOperand = inFirstOperand;
	CPPExpression* additionalOperand = NULL;
	CPPOperator* anOperator = inOperator;
	CPPOperator* laterOperator = NULL;
	bool statusOK = true;
	*outNextOperator = NULL;

	do
	{
		// check for the conditional operator case
		if(anOperator->Type == eCPPOperatorConditional)
		{
			operandResult = ParseOperand(inTokenProvider);
			if(!operandResult.first || !operandResult.second)
			{
				TRACE_LOG("CPPExpressionParser::ParseSingleExpression, could not parse operand for a conditional");
				statusOK = false;
				break;
			}
			operatorResult = ParseOperator(inTokenProvider,true);
			if(!operatorResult.first || !operatorResult.second || 
				operatorResult.second->Type != eCPPOperatorConditionalSecond)
			{
				TRACE_LOG("CPPExpressionParser::ParseSingleExpression, cannot find 2nd operator of condtional");
				delete operatorResult.second;
				delete operandResult.second;
				statusOK = false;
				break;
			}
			additionalOperand = operandResult.second;
		}

		// parse 2nd (or third for conditional) operand
		operandResult = ParseOperand(inTokenProvider);
		if(!operandResult.first)
		{
			TRACE_LOG("CPPExpressionParser::ParseSingleExpression, cannot parse 2nd operand for a binary operator (or 3rd for a conditional)");
			statusOK = false;
			break;
		}
		secondOperand = operandResult.second;

		// parse for next operator, to check precedence (possibly, no next operator)
		operatorResult = ParseOperator(inTokenProvider,true);
		if(!operatorResult.first || !operatorResult.second)
		{
			// no next operator, finish
			firstOperand = MakeExpression(anOperator,firstOperand,secondOperand,additionalOperand);
			break;
		}		

		laterOperator = operatorResult.second;

		// loop the later operator, each time calling a recursion, until the 2nd (or third) operand is realized
		while(laterOperator && laterOperator->Precedes(anOperator) && statusOK)
		{
			// should parse the next operator expression...need to recurse
			operandResult = ParseSingleExpression(inTokenProvider, secondOperand,laterOperator,&laterOperator);
			if(!operandResult.first)
			{
				TRACE_LOG("CPPExpressionParser::ParseSingleExpression, failed in recursion, parsing for the next, preceding, operator");
				statusOK = false;
				break;
			}
			secondOperand = operandResult.second;
		}

		if(!statusOK)
			break;

		// at this point, either there's no later operator (ended parsing) or that the next operator does not precede this one, save it for caller
		*outNextOperator = laterOperator;

		// make an expression out of what was collected
		firstOperand = MakeExpression(anOperator,firstOperand,secondOperand,additionalOperand);
	}while(false);

	if(!statusOK)
	{
		delete firstOperand;
		delete secondOperand;
		delete additionalOperand;
		delete anOperator;
		delete laterOperator;
		return BoolAndCPPExpression(false,NULL);
	}
	else
	{
		return BoolAndCPPExpression(true,firstOperand);
	}
}

BoolAndCPPOperator CPPExpressionParser::ParseOperator(IPreprocessorConditionTokenProvider* inProvider, bool inIsBinary)
{
	BoolAndString tokenizerResult = inProvider->GetNextToken();

	if(!tokenizerResult.first)
		return BoolAndCPPOperator(false,NULL);

	return MakeOperator(tokenizerResult.second,inIsBinary);
}


BoolAndCPPOperator CPPExpressionParser::MakeOperator(const string& inToken, bool inIsBinary)
{
	if(inToken == "~")
		return BoolAndCPPOperator(true,new CPPOperator(eCPPOperatorOnesComplement));
	else if(inToken == "!")
		return BoolAndCPPOperator(true,new CPPOperator(eCPPOperatorNot));
	else if(inToken == "-" && !inIsBinary)
		return BoolAndCPPOperator(true,new CPPOperator(eCPPOperatorUnaryMinus));
	else if(inToken == "+" && !inIsBinary)
		return BoolAndCPPOperator(true,new CPPOperator(eCPPOperatorUnaryPlus));
	else if(inToken == "*")
		return BoolAndCPPOperator(true,new CPPOperator(eCPPOperatorMultiplication));
	else if(inToken == "/")
		return BoolAndCPPOperator(true,new CPPOperator(eCPPOperatorDivision));
	else if(inToken == "%")
		return BoolAndCPPOperator(true,new CPPOperator(eCPPOperatorModulus));
	else if(inToken == "+" && inIsBinary)
		return BoolAndCPPOperator(true,new CPPOperator(eCPPOperatorAddition));
	else if(inToken == "-" && inIsBinary)
		return BoolAndCPPOperator(true,new CPPOperator(eCPPOperatorSubtraction));
	else if(inToken == "<<")
		return BoolAndCPPOperator(true,new CPPOperator(eCPPOperatorLeftByteShift));
	else if(inToken == ">>")
		return BoolAndCPPOperator(true,new CPPOperator(eCPPOperatorRightByteShift));
	else if(inToken == "<")
		return BoolAndCPPOperator(true,new CPPOperator(eCPPOperatorLessThan));
	else if(inToken == ">")
		return BoolAndCPPOperator(true,new CPPOperator(eCPPOperatorGreaterThan));
	else if(inToken == "<=")
		return BoolAndCPPOperator(true,new CPPOperator(eCPPOperatorLessThanOrEqual));
	else if(inToken == ">=")
		return BoolAndCPPOperator(true,new CPPOperator(eCPPOperatorGreaterThanOrEqual));
	else if(inToken == "==")
		return BoolAndCPPOperator(true,new CPPOperator(eCPPOperatorEquality));
	else if(inToken == "!=")
		return BoolAndCPPOperator(true,new CPPOperator(eCPPOperatorInequality));
	else if(inToken == "&")
		return BoolAndCPPOperator(true,new CPPOperator(eCPPOperatorBitwiseAnd));
	else if(inToken == "^")
		return BoolAndCPPOperator(true,new CPPOperator(eCPPOperatorExclusiveOr));
	else if(inToken == "|")
		return BoolAndCPPOperator(true,new CPPOperator(eCPPOperatorBitwiseAnd));
	else if(inToken == "&&")
		return BoolAndCPPOperator(true,new CPPOperator(eCPPOperatorLogicalAnd));
	else if(inToken == "||")
		return BoolAndCPPOperator(true,new CPPOperator(eCPPOperatorLogicalOr));
	else if(inToken == "?")
		return BoolAndCPPOperator(true,new CPPOperator(eCPPOperatorConditional));
	else if(inToken == ":")
		return BoolAndCPPOperator(true,new CPPOperator(eCPPOperatorConditionalSecond));
	else if(inToken == "defined")
		return BoolAndCPPOperator(true,new CPPOperator(eCPPOperatorDefined));
	else
		return BoolAndCPPOperator(false,NULL);
}

BoolAndCPPExpression CPPExpressionParser::ParseOperand(IPreprocessorConditionTokenProvider* inProvider)
{
	BoolAndString tokenizerResult = inProvider->GetNextToken();
	bool statusOK = true;
	CPPExpression* result = NULL;
	BoolAndCPPExpression expressionResult;

	do
	{
		if(!tokenizerResult.first)
		{
			statusOK = false;
			break;
		}
		
		// try to parse and operator, if succesful we are looking at a unary operator
		BoolAndCPPOperator operatorResult = MakeOperator(tokenizerResult.second,false);
		if(operatorResult.first)
		{
			expressionResult = ParseUnaryOperatorOperand(inProvider,operatorResult.second);
			statusOK = expressionResult.first;
			result = expressionResult.second;
			if(!statusOK)
				TRACE_LOG("CPPExpressionParser::ParseOperand, failed to parse operand for unary operator");

			break;
		}
	
		// if open paranthesis, then operand is a complete expression, that should be read as a complete
		// unit and returned
		if(tokenizerResult.second == "(")
		{
			IPreprocessorConditionTokenProvider* newProvider = new ParenthesisConditionalTokenProvider(mOriginalProvider);
			expressionResult = ParseExpressionInternal(newProvider);
			statusOK = expressionResult.first;
			result = expressionResult.second;
			if(!statusOK)
				TRACE_LOG("CPPExpressionParser::ParseOperand, failed to parse expression in paranthesis");
			break;
		}

		expressionResult = MakeInteger(tokenizerResult.second);
		if(expressionResult.first)
		{
			result = expressionResult.second;
			break;
		}
		
		expressionResult = MakeCharacter(tokenizerResult.second);
		if(expressionResult.first)
		{
			result = expressionResult.second;
			break;
		}

		// else, undefined symbol, error
		if(!statusOK)
			TRACE_LOG1("CPPExpressionParser::ParseOperand, undefined symbol while parsing a constant expression for preprocessor - %s",tokenizerResult.second.c_str());
		statusOK = false;

	}while(false);
	
	if(statusOK)
		return BoolAndCPPExpression(true,result);
	else
		return BoolAndCPPExpression(false,NULL);
}

BoolAndCPPExpression CPPExpressionParser::ParseUnaryOperatorOperand(IPreprocessorConditionTokenProvider* inProvider,CPPOperator* inOperator)
{
	BoolAndCPPExpression operandResult = 
		inOperator->Type == eCPPOperatorDefined ?
			ParseOperandForDefined(inProvider) :
			ParseOperand(inProvider);

	if(!operandResult.first)
	{
		delete inOperator;
		TRACE_LOG("CPPExpressionParser::ParseUnaryOperatorOperand, failed to parse operand for operator");
		return BoolAndCPPExpression(false,NULL);
	}
	else
		return BoolAndCPPExpression(true,MakeExpression(inOperator,operandResult.second,NULL,NULL));
}

BoolAndCPPExpression CPPExpressionParser::ParseOperandForDefined(IPreprocessorConditionTokenProvider* inProvider)
{
	// that's an easy one, just get the "(", "Symbol" and ")"
	bool statusOK = true;
	CPPExpression* result = NULL;

	do
	{
		BoolAndString tokenResult = inProvider->GetNextToken();
		if(!tokenResult.first || tokenResult.second != "(")
		{
			TRACE_LOG1("CPPExpressionParser::ParseOperandForDefined, defined function first token is not left paranthesis - %s",tokenResult.first ? tokenResult.second.c_str():"(empty)");
			statusOK = false;
			break;
		}

		tokenResult = inProvider->GetNextTokenNoMacroReplacement();

		if(!tokenResult.first)
		{	
			TRACE_LOG("CPPExpressionParser::ParseOperandForDefined, failed to parse parameter for defined() function");
			statusOK = false;
			break;
		}

		result = new CPPExpressionSymbol(tokenResult.second);

		tokenResult = inProvider->GetNextToken();
		if(!tokenResult.first || tokenResult.second != ")")
		{
			TRACE_LOG1("CPPExpressionParser::ParseOperandForDefined, defined function last token is not left paranthesis - %s",tokenResult.first ? tokenResult.second.c_str():"(empty)");
			statusOK = false;
		}

	}while(false);

	if(statusOK)
	{
		return BoolAndCPPExpression(true,result);
	}
	else
	{
		delete result;
		return BoolAndCPPExpression(false,NULL);
	}
}


CPPExpression* CPPExpressionParser::MakeExpression(CPPOperator* inOperator,CPPExpression* inFirstOperand, CPPExpression* inLastOperand, CPPExpression* inOptionalMiddleOperand)
{
	CPPOperatorExpression* result = new CPPOperatorExpression(inOperator,mSymbolsSource);
	result->PushOperand(inFirstOperand);
	if(inOptionalMiddleOperand)
		result->PushOperand(inOptionalMiddleOperand);
	if(inLastOperand)
		result->PushOperand(inLastOperand);
	return result;
}

typedef pair<bool,char> BoolAndChar;

BoolAndChar sParseChar(const string& inToken, string::size_type& ioPosition)
{
	// simple char, no escape
	if(inToken.at(ioPosition) != '\\')
	{
		++ioPosition;
		return BoolAndChar(true,inToken.at(ioPosition));
	}

	++ioPosition;

	// escape cases, make sure that there's at least one more character
	if(ioPosition == inToken.size() - 1)
		return BoolAndChar(false,0);

	// escapes
	if(inToken.at(ioPosition) >= '0' && inToken.at(ioPosition) <= '8')
	{
		// octal case, maximum 3 chars
		char buffer = 0;
		string::size_type i=ioPosition;
		for(;i<inToken.size()-1 && i<ioPosition+3;++i)
		{
			if(inToken.at(i) >= '0' && inToken.at(i) <= '8')
				buffer = (buffer<<3) + (inToken.at(i) - '0');
			else
				break;
		}
		ioPosition = i;
		return BoolAndChar(true,buffer);
	}

	// rest of escapes are handled through switch case of the 3rd char

	switch(inToken.at(ioPosition))
	{
		case('n'):
			++ioPosition;
			return BoolAndChar(true,'\n');
		case('t'):
			++ioPosition;
			return BoolAndChar(true,'\t');
		case('v'):
			++ioPosition;
			return BoolAndChar(true,'\v');
		case('b'):
			++ioPosition;
			return BoolAndChar(true,'\b');
		case('r'):
			++ioPosition;
			return BoolAndChar(true,'\r');
		case('f'):
			++ioPosition;
			return BoolAndChar(true,'\f');
		case('a'):
			++ioPosition;
			return BoolAndChar(true,'\a');
		case('\\'):
			++ioPosition;
			return BoolAndChar(true,'\\');
		case('?'):
			++ioPosition;
			return BoolAndChar(true,'\?');
		case('\''):
			++ioPosition;
			return BoolAndChar(true,'\\');
		case('\"'):
			++ioPosition;
			return BoolAndChar(true,'\"');
		case('0'):
			++ioPosition;
			return BoolAndChar(true,'\0');
		case('x'):
		{
			// hexadecimal, which may overflow
			char buffer = 0;
			++ioPosition;
			for(;ioPosition<inToken.size()-1;++ioPosition)
			{
				if(inToken.at(ioPosition) >= '0' && inToken.at(ioPosition) <= '9')
				{
					buffer = (buffer<<4) + (inToken.at(ioPosition) - '0');
				}
				else if(inToken.at(ioPosition) >= 'A' && inToken.at(ioPosition) <= 'F')
				{
					buffer = (buffer<<4) + (inToken.at(ioPosition) - 'A');
				}
				else if(inToken.at(ioPosition) >= 'a' && inToken.at(ioPosition) <= 'f')
				{
					buffer = (buffer<<4) + (inToken.at(ioPosition) - 'a');
				}
				else
					break;
			}
			return BoolAndChar(true,buffer);
		}
		default:
			return BoolAndChar(false,0);
	}
}

typedef pair<bool,wchar_t> BoolAndWChar;

BoolAndWChar sParseWChar(const string& inToken, string::size_type& ioPosition)
{
	// simple char, no escape
	if(inToken.at(ioPosition) != '\\')
	{
		++ioPosition;
		return BoolAndWChar(true,(wchar_t)inToken.at(ioPosition));
	}

	++ioPosition;

	// escape cases, make sure that there's at least one more character
	if(ioPosition == inToken.size() - 1)
		return BoolAndWChar(false,0);

	// escapes
	if(inToken.at(ioPosition) >= '0' && inToken.at(ioPosition) <= '8')
	{
		// octal case, maximum 3 chars
		wchar_t buffer = 0;
		string::size_type i=ioPosition;
		for(;i<inToken.size()-1 && i<ioPosition+3;++i)
		{
			if(inToken.at(i) >= '0' && inToken.at(i) <= '8')
				buffer = (buffer<<3) + (inToken.at(i) - '0');
			else
				break;
		}
		ioPosition = i;
		return BoolAndWChar(true,buffer);
	}

	// rest of escapes are handled through switch case of the 3rd char

	switch(inToken.at(ioPosition))
	{
		case('n'):
			++ioPosition;
			return BoolAndWChar(true,L'\n');
		case('t'):
			++ioPosition;
			return BoolAndWChar(true,L'\t');
		case('v'):
			++ioPosition;
			return BoolAndWChar(true,L'\v');
		case('b'):
			++ioPosition;
			return BoolAndWChar(true,L'\b');
		case('r'):
			++ioPosition;
			return BoolAndWChar(true,L'\r');
		case('f'):
			++ioPosition;
			return BoolAndWChar(true,L'\f');
		case('a'):
			++ioPosition;
			return BoolAndWChar(true,L'\a');
		case('\\'):
			++ioPosition;
			return BoolAndWChar(true,L'\\');
		case('?'):
			++ioPosition;
			return BoolAndWChar(true,L'\?');
		case('\''):
			++ioPosition;
			return BoolAndWChar(true,L'\\');
		case('\"'):
			++ioPosition;
			return BoolAndWChar(true,L'\"');
		case('0'):
			++ioPosition;
			return BoolAndWChar(true,L'\0');
		case('x'):
		{
			// hexadecimal, which may overflow
			wchar_t buffer = 0;
			++ioPosition;
			for(;ioPosition<inToken.size()-1;++ioPosition)
			{
				if(inToken.at(ioPosition) >= '0' && inToken.at(ioPosition) <= '9')
				{
					buffer = (buffer<<4) + (inToken.at(ioPosition) - '0');
				}
				else if(inToken.at(ioPosition) >= 'A' && inToken.at(ioPosition) <= 'F')
				{
					buffer = (buffer<<4) + (inToken.at(ioPosition) - 'A');
				}
				else if(inToken.at(ioPosition) >= 'a' && inToken.at(ioPosition) <= 'f')
				{
					buffer = (buffer<<4) + (inToken.at(ioPosition) - 'a');
				}
				else
					break;
			}
			return BoolAndWChar(true,buffer);
		}
		default:
			return BoolAndWChar(false,0);
	}
}


BoolAndCPPExpression CPPExpressionParser::MakeCharacter(const string& inToken)
{
	// wide character case
	if(inToken.size() > 3 && inToken.at(0) == 'L' && inToken.at(1) == '\'' && inToken.at(inToken.size() - 1) == '\'')
	{
		string::size_type currentPosition = 2; // skip first 2 markers

		BoolAndWChar parseResult = sParseWChar(inToken,currentPosition);
		if(!parseResult.first)
			return BoolAndCPPExpression(false,NULL);
		
		return BoolAndCPPExpression(true,new CPPExpressionInteger(parseResult.second));
	}
	else
	{
		// single byte or multibyte
		if(inToken.size() < 3 || inToken.at(0) != '\'' || inToken.at(inToken.size() - 1) != '\'')
			return BoolAndCPPExpression(false,NULL);

		string::size_type currentPosition = 1; // skip first

		BoolAndChar parseResult = sParseChar(inToken,currentPosition);

		if(!parseResult.first)
			return BoolAndCPPExpression(false,NULL);

		// single byte case...finish
		if(currentPosition == (inToken.size() - 1))
			return BoolAndCPPExpression(true,new CPPExpressionInteger(parseResult.second));

		// multibyte case, parse another byte and make an integer
		BoolAndChar parseResult2 = sParseChar(inToken,currentPosition);

		if(!parseResult2.first)
			return BoolAndCPPExpression(false,NULL);

		return BoolAndCPPExpression(true,new CPPExpressionInteger((int)( (((unsigned)parseResult.second)<<8) + (unsigned)parseResult2.second)));
	}
}




BoolAndCPPExpression CPPExpressionParser::MakeInteger(const string& inToken)
{
	unsigned long long buffer = 0;
	string::size_type i=0;
	bool isUnsigned = false;

	if(inToken == "true" || inToken == "false")
		return BoolAndCPPExpression(true,new CPPExpressionInteger(inToken == "true"));

	if(inToken.at(0) < '0' || inToken.at(0) > '9')
		return BoolAndCPPExpression(false,NULL);


	if(inToken.at(0) == '0')
	{
		if(inToken.size() > 1 && inToken.at(1) == 'x')
		{
			i = 2;
			// hex case
			for(;i<inToken.size();++i)
			{
				if(inToken.at(i) >= '0' && inToken.at(i) <= '9')
				{
					buffer = (buffer<<4) + (inToken.at(i) - '0');
				}
				else if(inToken.at(i) >= 'A' && inToken.at(i) <= 'F')
				{
					buffer = (buffer<<4) + (inToken.at(i) - 'A');
				}
				else if(inToken.at(i) >= 'a' && inToken.at(i) <= 'f')
				{
					buffer = (buffer<<4) + (inToken.at(i) - 'a');
				}
				else
					break;
			}
		}
		else
		{
			i = 1;
			// oct case
			for(;i<inToken.size();++i)
			{
				if(inToken.at(i) >= '0' && inToken.at(i) <= '8')
					buffer = (buffer<<3) + (inToken.at(i) - '0');
				else
					break;
			}
		}
	}
	else
	{
		// decimal case
		for(; i < inToken.size();++i)
		{
			if(inToken.at(i) >= '0' && inToken.at(i) <= '9')
				buffer = buffer*10 + (inToken.at(i) - '0');
			else
				break;
		}
	}

	if(i < inToken.size() - 1)
	{
		// still more to read, parhaps type flags		
		if(inToken.at(i) == 'u' || inToken.at(i) == 'U')
		{
			isUnsigned = true;
			++i;
		}

		if(i < inToken.size() - 1)
		{
			if(
				(i < inToken.size() - 3 && inToken.substr(i,3) == "i64") ||	
				(i < inToken.size() - 2 && inToken.substr(i,2) == "LL") ||	
				(i < inToken.size() - 2 && inToken.substr(i,2) == "ll") 	
			)
			{
				return BoolAndCPPExpression(true,
											new CPPExpressionInteger( 
												isUnsigned ? 
												(unsigned long long)buffer :
												(long long)buffer));
			}												 
			else if(
				(i < inToken.size() - 1 && inToken.at(i) == 'l') ||	
				(i < inToken.size() - 1 && inToken.at(i) == 'L')
			)
			{
				return BoolAndCPPExpression(true,
											new CPPExpressionInteger( 
												isUnsigned ? 
												(unsigned long)buffer :
												(long)buffer));
			}	
			else
			{
				return BoolAndCPPExpression(true,new CPPExpressionInteger(
														isUnsigned ? 
														(unsigned)buffer :
														(int)buffer));
			}
			
		}
		else
		{
			return BoolAndCPPExpression(true,new CPPExpressionInteger(
													isUnsigned ? 
													(unsigned)buffer :
													(int)buffer));
		}
	}
	else
		return BoolAndCPPExpression(true,new CPPExpressionInteger((int)buffer));

}
