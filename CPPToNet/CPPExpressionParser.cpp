#include "CPPExpressionParser.h"
#include "ITokenProvider.h"
#include "ParenthesisConditionalTokenProvider.h"
#include "Trace.h"
#include "CPPOperator.h"
#include "CPPExpression.h"
#include "CPPOperatorExpression.h"
#include "CPPExpressionInteger.h"
#include "CPPExpressionVariable.h"
#include "CPPExpressionFunctionCall.h"
#include "CPPExpressionTypename.h"
#include "ITypeParserHelper.h"
#include "TokenProviderStateRecovery.h"
#include "BracketsConditionalTokenProvider.h"
#include "CPPExpressionNewOperator.h"
#include "TypedParameter.h"

using namespace Hummus;

CPPExpressionParser::CPPExpressionParser(void)
{
}

CPPExpressionParser::~CPPExpressionParser(void)
{
}

BoolAndCPPExpression CPPExpressionParser::ParseExpression(ITokenProvider* inTokenProvider,ITypeParserHelper* inTypeParserHelper)
{
	mOriginalProvider = inTokenProvider;
	mTypeParserHelper = inTypeParserHelper;
	return ParseExpressionInternal(mOriginalProvider);
}

BoolAndCPPExpression CPPExpressionParser::ParseExpressionInternal(ITokenProvider* inTokenProvider)
{
	BoolAndCPPExpression operandResult;
	BoolAndCPPOperator operatorResult;
	BoolAndCPPExpression result;

	// parse first (and possibly only) operand
	operandResult = ParseOperand(inTokenProvider);

	if(!operandResult.first)
		return FalseExpression();

	// parse operator (binary or conditional)
	operatorResult = ParseMultinaryOperator(inTokenProvider);
	

	// continue with creating expression in main method
	return ParseExpressionInternal(inTokenProvider,operandResult.second,operatorResult.first ? operatorResult.second : NULL);
}

BoolAndCPPExpression CPPExpressionParser::ParseExpressionInternal(ITokenProvider* inTokenProvider,CPPExpression* inFirstOperand,CPPOperator* inOperator)
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
			operatorResult = ParseMultinaryOperator(inTokenProvider);
			if(!operatorResult.first)
				break;
			anOperator = operatorResult.second;
		}
	}

	if(!statusOK)
	{
		delete anOperator;
		delete anOperand;
		BoolAndCPPExpression result;
		result.first = false;
		result.second = NULL;
		return result;
	}
	else
	{
		return BoolAndCPPExpression(true,anOperand);
	}
}

BoolAndCPPExpression CPPExpressionParser::ParseSingleExpression(ITokenProvider* inTokenProvider,
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
			operatorResult = ParseMultinaryOperator(inTokenProvider);
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
		
		if(anOperator->Type == eCPPOperatorSubscript)
		{
			// special case for subscript, 2nd operand is a full expression inside the rectangle brackets
			BracketsConditionalTokenProvider newProvider(mOriginalProvider);
			operandResult = ParseExpressionInternal(&newProvider);
			if(!operandResult.first)
			{
				TRACE_LOG("CPPExpressionParser::ParseSingleExpression, cannot parse subscript parameter");
				statusOK = false;
				break;
			}

			// subscript parameter always precedes, being inside...so no need to look further
			firstOperand = MakeExpression(anOperator,firstOperand,operandResult.second,additionalOperand);
			break;
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
		operatorResult = ParseMultinaryOperator(inTokenProvider);
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
		BoolAndCPPExpression result;
		result.first = false;
		result.second = NULL;
		return result;
	}
	else
	{
		return BoolAndCPPExpression(true,firstOperand);
	}
}

BoolAndCPPExpression CPPExpressionParser::FalseExpression()
{
	BoolAndCPPExpression result;

	result.first = false;
	result.second = NULL;
	return result;
}

BoolAndCPPOperator CPPExpressionParser::ParseMultinaryOperator(ITokenProvider* inProvider)
{
	BoolAndString tokenizerResult = inProvider->GetNextToken();
	BoolAndCPPOperator result;

	if(!tokenizerResult.first)
	{
		result.first = false;
		result.second = NULL;
		return result;
	}

	// k. if this is not an operator just put it back where you took it from
	result = MakeOperator(tokenizerResult.second,true);
	if(!result.first)
		inProvider->PutBackToken(tokenizerResult.second);
	return result;
}


BoolAndCPPOperator CPPExpressionParser::MakeOperator(const string& inToken, bool inIsBinary)
{
	return MakeOperator(inToken,NULL,inIsBinary);
}

BoolAndCPPOperator MakeOperator(const string& inToken, CPPExpression* inParameter,bool inIsBinary)
{
	// notice that does not contain casting operator. casting is created separately as a special case

	if(inToken == "defined" && !inIsBinary)
		return BoolAndCPPOperator(true,new CPPOperator(eCPPOperatorDefined));
	else if(inToken == "." && inIsBinary)
		return BoolAndCPPOperator(true,new CPPOperator(eCPPOperatorMemberSelectionObject));
	else if(inToken == "->" && inIsBinary)
		return BoolAndCPPOperator(true,new CPPOperator(eCPPOperatorMemberSelectionPointer));
	else if(inToken == "[" && inIsBinary)
		return BoolAndCPPOperator(true,new CPPOperator(eCPPOperatorSubscript));
	else if(inToken == "++" && !inIsBinary)
		return BoolAndCPPOperator(true,new CPPOperator(eCPPOperatorPrefixIncrement));
	else if(inToken == "--" && !inIsBinary)
		return BoolAndCPPOperator(true,new CPPOperator(eCPPOperatorPrefixDecrement));
	else if(inToken == "typeid" && !inIsBinary)
		return BoolAndCPPOperator(true,new CPPOperator(eCPPOperatorTypeID));
	else if(inToken == "const_cast" && !inIsBinary)
		return BoolAndCPPOperator(true,new CPPOperator(eCPPOperatorConstCast,inParameter));
	else if(inToken == "static_cast" && !inIsBinary)
		return BoolAndCPPOperator(true,new CPPOperator(eCPPOperatorStaticCast,inParameter));
	else if(inToken == "dynamic_cast" && !inIsBinary)
		return BoolAndCPPOperator(true,new CPPOperator(eCPPOperatorDynamicCast,inParameter));
	else if(inToken == "reinterpret_cast" && !inIsBinary)
		return BoolAndCPPOperator(true,new CPPOperator(eCPPOperatorReinterpretCast,inParameter));
	else if(inToken == "static_cast" && !inIsBinary)
		return BoolAndCPPOperator(true,new CPPOperator(eCPPOperatorStaticCast,inParameter));
	else if(inToken == "sizeof" && !inIsBinary)
		return BoolAndCPPOperator(true,new CPPOperator(eCPPOperatorSizeof));
	else if(inToken == "~" && !inIsBinary)
		return BoolAndCPPOperator(true,new CPPOperator(eCPPOperatorOnesComplement));
	else if(inToken == "!" && !inIsBinary)
		return BoolAndCPPOperator(true,new CPPOperator(eCPPOperatorNot));
	else if(inToken == "-" && !inIsBinary)
		return BoolAndCPPOperator(true,new CPPOperator(eCPPOperatorUnaryMinus));
	else if(inToken == "+" && !inIsBinary)
		return BoolAndCPPOperator(true,new CPPOperator(eCPPOperatorUnaryPlus));
	else if(inToken == "&" && !inIsBinary)
		return BoolAndCPPOperator(true,new CPPOperator(eCPPOperatorAddressOf));
	else if(inToken == "*")
		return BoolAndCPPOperator(true,new CPPOperator( inIsBinary ? eCPPOperatorMultiplication : eCPPOperatorIndirection));
	else if(inToken == "new" && !inIsBinary)
		return BoolAndCPPOperator(true,new CPPOperator(eCPPOperatorNew));
	else if(inToken == "delete" && !inIsBinary)
		return BoolAndCPPOperator(true,new CPPOperator(eCPPOperatorDelete));
	else if(inToken == "/" && inIsBinary)
		return BoolAndCPPOperator(true,new CPPOperator(eCPPOperatorDivision));
	else if(inToken == "%" && inIsBinary)
		return BoolAndCPPOperator(true,new CPPOperator(eCPPOperatorModulus));
	else if(inToken == "+" && inIsBinary)
		return BoolAndCPPOperator(true,new CPPOperator(eCPPOperatorAddition));
	else if(inToken == "-" && inIsBinary)
		return BoolAndCPPOperator(true,new CPPOperator(eCPPOperatorSubtraction));
	else if(inToken == "<<" && inIsBinary)
		return BoolAndCPPOperator(true,new CPPOperator(eCPPOperatorLeftByteShift));
	else if(inToken == ">>" && inIsBinary)
		return BoolAndCPPOperator(true,new CPPOperator(eCPPOperatorRightByteShift));
	else if(inToken == "<" && inIsBinary)
		return BoolAndCPPOperator(true,new CPPOperator(eCPPOperatorLessThan));
	else if(inToken == ">" && inIsBinary)
		return BoolAndCPPOperator(true,new CPPOperator(eCPPOperatorGreaterThan));
	else if(inToken == "<=" && inIsBinary)
		return BoolAndCPPOperator(true,new CPPOperator(eCPPOperatorLessThanOrEqual));
	else if(inToken == ">=" && inIsBinary)
		return BoolAndCPPOperator(true,new CPPOperator(eCPPOperatorGreaterThanOrEqual));
	else if(inToken == "==" && inIsBinary)
		return BoolAndCPPOperator(true,new CPPOperator(eCPPOperatorEquality));
	else if(inToken == "!=" && inIsBinary)
		return BoolAndCPPOperator(true,new CPPOperator(eCPPOperatorInequality));
	else if(inToken == "&" && inIsBinary)
		return BoolAndCPPOperator(true,new CPPOperator(eCPPOperatorBitwiseAnd));
	else if(inToken == "^" && inIsBinary)
		return BoolAndCPPOperator(true,new CPPOperator(eCPPOperatorExclusiveOr));
	else if(inToken == "|" && inIsBinary)
		return BoolAndCPPOperator(true,new CPPOperator(eCPPOperatorBitwiseAnd));
	else if(inToken == "&&" && inIsBinary)
		return BoolAndCPPOperator(true,new CPPOperator(eCPPOperatorLogicalAnd));
	else if(inToken == "||" && inIsBinary)
		return BoolAndCPPOperator(true,new CPPOperator(eCPPOperatorLogicalOr));
	else if(inToken == "?" && inIsBinary)
		return BoolAndCPPOperator(true,new CPPOperator(eCPPOperatorConditional));
	else if(inToken == ":" && inIsBinary)
		return BoolAndCPPOperator(true,new CPPOperator(eCPPOperatorConditionalSecond));
	else if(inToken == "=" && inIsBinary)
		return BoolAndCPPOperator(true,new CPPOperator(eCPPOperatorAssigment));
	else if(inToken == "*=" && inIsBinary)
		return BoolAndCPPOperator(true,new CPPOperator(eCPPOperatorMultiplicationAssigment));
	else if(inToken == "/=" && inIsBinary)
		return BoolAndCPPOperator(true,new CPPOperator(eCPPOperatorDivisionAssigment));
	else if(inToken == "%=" && inIsBinary)
		return BoolAndCPPOperator(true,new CPPOperator(eCPPOperatorModulusAssigment));
	else if(inToken == "+=" && inIsBinary)
		return BoolAndCPPOperator(true,new CPPOperator(eCPPOperatorAdditionAssigment));
	else if(inToken == "-=" && inIsBinary)
		return BoolAndCPPOperator(true,new CPPOperator(eCPPOperatorSubstractionAssigment));
	else if(inToken == "<<=" && inIsBinary)
		return BoolAndCPPOperator(true,new CPPOperator(eCPPOperatorLeftShiftAssigment));
	else if(inToken == ">>=" && inIsBinary)
		return BoolAndCPPOperator(true,new CPPOperator(eCPPOperatorRightShiftAssigment));
	else if(inToken == "&=" && inIsBinary)
		return BoolAndCPPOperator(true,new CPPOperator(eCPPOperatorBitwiseAndAssigment));
	else if(inToken == "|=" && inIsBinary)
		return BoolAndCPPOperator(true,new CPPOperator(eCPPOperatorBitwiseOrAssigment));
	else if(inToken == "^=" && inIsBinary)
		return BoolAndCPPOperator(true,new CPPOperator(eCPPOperatorBitwiseExclusiveOrAssigment));
	else if(inToken == "throw" && !inIsBinary)
		return BoolAndCPPOperator(true,new CPPOperator(eCPPOperatorThrow));
	else
		return BoolAndCPPOperator(false,(CPPOperator*)NULL);
}

BoolAndCPPExpression CPPExpressionParser::ParseOperand(ITokenProvider* inProvider)
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
		
		// try to parse and operator, if succesful we are looking at a unary operator (new is a special case of unary operator)
		if(tokenizerResult.second == "new")
		{
			expressionResult = ParseNewExpression(inProvider);
			statusOK = expressionResult.first;
			result = expressionResult.second;
			if(!statusOK)
				TRACE_LOG("CPPExpressionParser::ParseOperand, failed to parse new expression");

			break;
		}
		else
		{
			BoolAndCPPOperator operatorResult = ParseUnaryOperator(inProvider,tokenizerResult.second);
			if(operatorResult.first)
			{
				expressionResult = ParseUnaryOperatorOperand(inProvider,operatorResult.second);
				statusOK = expressionResult.first;
				result = expressionResult.second;
				if(!statusOK)
					TRACE_LOG("CPPExpressionParser::ParseOperand, failed to parse operand for unary operator");

				break;
			}
		}
	
		// if open paranthesis, then operand is a complete expression, that should be read as a complete
		// unit and returned
		if(tokenizerResult.second == "(")
		{
			ParenthesisConditionalTokenProvider newProvider(mOriginalProvider);
			expressionResult = ParseExpressionInternal(&newProvider);
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

		// else should be a variable, function, enumerator (including members, . ->). which are all basically the same thing:
		// XXX[(...)]
		// [::]YYY[::AAA::ZZZ...]::XXX[(....)]
		

		// grab the whole string, including all scoping as the variable name. then identify if this is a function, and create a special kind of operand, which is a function call
		StringList scopes;
		
		if(tokenizerResult.second == "::")
		{
			scopes.push_back(""); // this will mark global scope
			tokenizerResult = inProvider->GetNextToken();
			if(!tokenizerResult.first)
			{
				TRACE_LOG("CPPExpressionParser::ParseOperand, syntax error, could not parse a token after global scope specification");
				statusOK = false;
				break;
			}

		}

		// now loop till no more :: are seen. the last one is the actual name
		string variableName;
		do
		{
			variableName = tokenizerResult.second;
			tokenizerResult = inProvider->GetNextToken();
			if(!tokenizerResult.first) // finished provider, finish here
				break;
			
			if(tokenizerResult.second == "::")
			{
				scopes.push_back(variableName);
				tokenizerResult = inProvider->GetNextToken();
				if(!tokenizerResult.first)
				{
					TRACE_LOG("CPPExpressionParser::ParseOperand, syntax error, could not parse a token after scope specification");
					statusOK = false;
					break;
				}
				// will continue, therefore, only if the tokenizer result was :: and there's a name now. must find out if there's more scoping
			}
			else
			{
				inProvider->PutBackToken(tokenizerResult.second);
				break;
			}
		}while(statusOK); // will break eventually

		if(!statusOK)
			break;
		// now we have in scopes all the scopes and as variable name, the name of the actual variable. great. now just figure out if this is a function
		tokenizerResult = inProvider->GetNextToken();
		if(tokenizerResult.first)
		{
			if(tokenizerResult.second == "(")
			{
				// humff, function call
				expressionResult = ParseFunctionCall(variableName,scopes,inProvider);
				if(expressionResult.first)
				{
					result = expressionResult.second;
					break;
				}
			}
			else
			{
				// humff variable. put back next token
				inProvider->PutBackToken(tokenizerResult.second);
				expressionResult = MakeVariable(variableName,scopes);
				if(expressionResult.first)
				{
					result = expressionResult.second;
					break;
				}				
			}
		}
		else
		{
			// humff. must be a variable
			expressionResult = MakeVariable(variableName,scopes);
			if(expressionResult.first)
			{
				result = expressionResult.second;
				break;
			}		
		}

		// k. if not stopped till here, then this must be some kind of syntax error. report
		TRACE_LOG("CPPExpressionParser::ParseOperand, syntax error, no suitable expression type found");
		statusOK = false;

	}while(false);
	
	if(statusOK)
	{
		// consider also postfix operators (subscript, postfix incremenet, postfix decrement...)
		do
		{
			tokenizerResult = inProvider->GetNextToken();
			if(!tokenizerResult.first)
				break;

			if(IsPostFixOperator(tokenizerResult.second))
			{
				// parse postfix operator, and consider it + the operand as a joint expression
				expressionResult = ParsePostFixOperatorOperand(inProvider,result,tokenizerResult.second);
				if(expressionResult.first)
				{
					result = expressionResult.second;
				}		
				else
				{
					TRACE_LOG("CPPExpressionParser::ParseOperand, error in parsing postfix operator");
					statusOK = false;
					// no need to release "result" here, ParsePostFixOperatorOperand takes care of this.
					result = NULL;
				}
			}
			else
			{
				inProvider->PutBackToken(tokenizerResult.second);
			}
		}while(false);
	}


	if(statusOK)
	{
		return BoolAndCPPExpression(true,result);
	}
	else
	{
		result = NULL;
		return BoolAndCPPExpression(false,result);
	}
}

BoolAndCPPOperator CPPExpressionParser::ParseUnaryOperator(ITokenProvider* inProvider, const string& inToken)
{
	if(inToken == "(" && IsOperandToParseAType(inProvider))
	{
		// Should be casting... if the closing parenthesis is after the type
		TokenProviderStateRecovery recoverableTokenProvider(inProvider);
		recoverableTokenProvider.TakeTokenSnapshot();

		BoolAndCPPExpression result = ParseExpressionType(&recoverableTokenProvider,")");
		
		if(result.first)
		{
			recoverableTokenProvider.CancelSnapshot();
			return BoolAndCPPOperator(true,new CPPOperator(eCPPOperatorCasting,result.second));
		}
		else
		{
			// not casting, so continue to other options (though this probably means that there's a failure here....)
			recoverableTokenProvider.RevertToTopSnapshot();
			return MakeOperator(inToken,false); 
		}
	}
	else if(inToken == "static_cast" || inToken == "reinterpret_cast" || inToken == "dynamic_cast" || inToken == "const_cast")
	{
		TokenProviderStateRecovery recoverableTokenProvider(inProvider);
		recoverableTokenProvider.TakeTokenSnapshot();
		CPPExpression* parameter = NULL;

		bool statusOK = true;
		// these special operators take as parameter a type usage parameter
		do
		{
			BoolAndString tokenResult = inProvider->GetNextToken();
			if(!tokenResult.first || tokenResult.second != "<")
			{
				TRACE_LOG1("CPPExpressionParser::ParseUnaryOperator, no parameters for %s",inToken.c_str());
				statusOK = false;
				break;
			}

			TokenProviderStateRecovery recoverableTokenProvider(inProvider);
			recoverableTokenProvider.TakeTokenSnapshot();

			BoolAndCPPExpression result = ParseExpressionType(&recoverableTokenProvider,">");
			statusOK = result.first;
			parameter = statusOK ? result.second: NULL;
		}while(false);

		if(statusOK)
		{
			recoverableTokenProvider.CancelSnapshot();
			return MakeOperator(inToken,parameter,false);
		}
		else
		{
			recoverableTokenProvider.RevertToTopSnapshot();
			return MakeOperator(inToken,false); 
		}
	}
	else if(inToken == "delete")
	{
		// could be either delete or delete[]
		TokenProviderStateRecovery recoverableTokenProvider(inProvider);
		recoverableTokenProvider.TakeTokenSnapshot();
		bool isArray = false;

		do
		{
			BoolAndString tokenResult = inProvider->GetNextToken();

			if(!tokenResult.first || tokenResult.second != "[")
				break;

			tokenResult = inProvider->GetNextToken();
			if(!tokenResult.first || tokenResult.second != "]")
				break;

			isArray = true;
		}while(false);
		
		if(isArray)
		{
			recoverableTokenProvider.CancelSnapshot();
			return BoolAndCPPOperator(true,new CPPOperator(eCPPOperatorDeleteArray));
		}
		else
		{
			recoverableTokenProvider.RevertToTopSnapshot();
			return BoolAndCPPOperator(true,new CPPOperator(eCPPOperatorDelete));
		}
	}
	else
		return MakeOperator(inToken,false);
}

BoolAndCPPExpression CPPExpressionParser::ParseUnaryOperatorOperand(ITokenProvider* inProvider,CPPOperator* inOperator)
{
	// special cases
	if(inOperator->Type == eCPPOperatorSizeof)
	{
		// sizoef may have parenthesis, and then as parameter eitehr expression or type usage, OR no parenthesis and then it must be an expression
		bool statusOK = true;
		CPPExpression* anOperand = NULL;

		do
		{
			BoolAndString tokenResult = inProvider->GetNextToken();
			if(!tokenResult.first)
			{
				TRACE_LOG("CPPExpressionParser::ParseUnaryOperatorOperand, no operand after sizeof");
				statusOK = false;
				break;
			}
		
			bool withParenthesis = (tokenResult.second == "(");

			BoolAndCPPExpression result;
			if(!withParenthesis && IsOperandToParseAType(inProvider))
			{
				result = ParseExpressionType(inProvider,")");
				if(!result.first)
				{
					TRACE_LOG("CPPExpressionParser::ParseUnaryOperatorOperand, problem in parsing operand for sizeof operator, where the operand is a type usage");
					statusOK = false;
					break;
				}
				anOperand = result.second;
			}
			else
			{
				result = ParseExpressionInternal(inProvider);
				if(!result.first)
				{
					TRACE_LOG("CPPExpressionParser::ParseUnaryOperatorOperand, problem in parsing operand for sizeof operator that is an expression");
					statusOK = false;
					break;
				}
			
				anOperand = result.second;

				tokenResult = inProvider->GetNextToken();
				if(!tokenResult.first || tokenResult.second != ")")
				{
					TRACE_LOG1("CPPExpressionParser::ParseUnaryOperatorOperand, sizeof last token is not left paranthesis - %s",tokenResult.first ? tokenResult.second.c_str():"(empty)");
					statusOK = false;
				}
			}
		}while(false);

		if(statusOK)
		{
			return BoolAndCPPExpression(true,MakeExpression(inOperator,anOperand,NULL,NULL));
		}
		else
		{
			delete inOperator;
			delete anOperand;
			return FalseExpression();
		}
	}
	else if (inOperator->Type == eCPPOperatorTypeID)
	{
		// type id will always have parenthesis, and inside either an expression or a typename
		bool statusOK = true;
		CPPExpression* anOperand = NULL;

		do
		{
			BoolAndString tokenResult = inProvider->GetNextToken();
			if(!tokenResult.first || tokenResult.second != "(")
			{
				TRACE_LOG("CPPExpressionParser::ParseUnaryOperatorOperand, no operand after typeid");
				statusOK = false;
				break;
			}
		
			BoolAndCPPExpression result;
			if(IsOperandToParseAType(inProvider))
			{
				result = ParseExpressionType(inProvider,")");
				if(!result.first)
				{
					TRACE_LOG("CPPExpressionParser::ParseUnaryOperatorOperand, problem in parsing operand for typeid operator, where the operand is a type usage");
					statusOK = false;
					break;
				}
				anOperand = result.second;
			}
			else
			{
				result = ParseExpressionInternal(inProvider);
				if(!result.first)
				{
					TRACE_LOG("CPPExpressionParser::ParseUnaryOperatorOperand, problem in parsing operand for typeid operator that is an expression");
					statusOK = false;
					break;
				}
			
				anOperand = result.second;

				tokenResult = inProvider->GetNextToken();
				if(!tokenResult.first || tokenResult.second != ")")
				{
					TRACE_LOG1("CPPExpressionParser::ParseUnaryOperatorOperand, typeid last token is not left paranthesis - %s",tokenResult.first ? tokenResult.second.c_str():"(empty)");
					statusOK = false;
				}
			}
		}while(false);

		if(statusOK)
		{
			return BoolAndCPPExpression(true,MakeExpression(inOperator,anOperand,NULL,NULL));
		}
		else
		{
			delete inOperator;
			delete anOperand;
			return FalseExpression();
		}
	}
	else if(inOperator->Type == eCPPOperatorDefined ||
			inOperator->Type == eCPPOperatorConstCast ||
			inOperator->Type == eCPPOperatorDynamicCast ||
			inOperator->Type == eCPPOperatorReinterpretCast ||
			inOperator->Type == eCPPOperatorStaticCast)
	{
		// There's paranthesis, then expression which is the operand and then another parenthesis
		bool statusOK = true;
		CPPExpression* anOperand = NULL;

		do
		{
			BoolAndString tokenResult = inProvider->GetNextToken();
			if(!tokenResult.first || tokenResult.second != "(")
			{
				TRACE_LOG1("CPPExpressionParser::ParseUnaryOperatorOperand, defined function first token is not left paranthesis - %s",tokenResult.first ? tokenResult.second.c_str():"(empty)");
				statusOK = false;
				break;
			}
		
			BoolAndCPPExpression result = ParseExpressionInternal(inProvider);
			if(!result.first)
			{
				TRACE_LOG("CPPExpressionParser::ParseUnaryOperatorOperand, problem in parsing operand for unary operator");
				statusOK = false;
				break;
			}
			
			anOperand = result.second;

			tokenResult = inProvider->GetNextToken();
			if(!tokenResult.first || tokenResult.second != ")")
			{
				TRACE_LOG1("CPPExpressionParser::ParseUnaryOperatorOperand, defined function last token is not left paranthesis - %s",tokenResult.first ? tokenResult.second.c_str():"(empty)");
				statusOK = false;
			}

		}while(false);

		if(statusOK)
		{
			return BoolAndCPPExpression(true,MakeExpression(inOperator,anOperand,NULL,NULL));
		}
		else
		{
			delete inOperator;
			delete anOperand;
			return FalseExpression();

		}
	}
	else
	{
		
		// the rest

		BoolAndCPPExpression operandResult = ParseOperand(inProvider);

		if(!operandResult.first)
		{
			delete inOperator;
			TRACE_LOG("CPPExpressionParser::ParseUnaryOperatorOperand, failed to parse operand for operator");
			return FalseExpression();

		}
		else
			return BoolAndCPPExpression(true,MakeExpression(inOperator,operandResult.second,NULL,NULL));
	}
}

CPPExpression* CPPExpressionParser::MakeExpression(CPPOperator* inOperator,CPPExpression* inFirstOperand, CPPExpression* inLastOperand, CPPExpression* inOptionalMiddleOperand)
{
	CPPOperatorExpression* result = new CPPOperatorExpression(inOperator);
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
			return FalseExpression();

		
		return BoolAndCPPExpression(true,new CPPExpressionInteger(parseResult.second));
	}
	else
	{
		// single byte or multibyte
		if(inToken.size() < 3 || inToken.at(0) != '\'' || inToken.at(inToken.size() - 1) != '\'')
		{
			BoolAndCPPExpression result;
			result.first = false;
			result.second = NULL;
			return result;
		}

		string::size_type currentPosition = 1; // skip first

		BoolAndChar parseResult = sParseChar(inToken,currentPosition);

		if(!parseResult.first)
			return FalseExpression();

		// single byte case...finish
		if(currentPosition == (inToken.size() - 1))
			return BoolAndCPPExpression(true,new CPPExpressionInteger(parseResult.second));

		// multibyte case, parse another byte and make an integer
		BoolAndChar parseResult2 = sParseChar(inToken,currentPosition);

		if(!parseResult2.first)
			return FalseExpression();

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
			return FalseExpression();



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

BoolAndCPPExpression CPPExpressionParser::MakeVariable(const string& inToken,const StringList& inScopes)
{
	return BoolAndCPPExpression(true,new CPPExpressionVariable(inToken,inScopes));
}


BoolAndCPPExpression CPPExpressionParser::ParseFunctionCall(const string& inToken,const StringList& inScopes,ITokenProvider* inProvider)
{
	// function call is basically a series of expressions separated by a comma, with ending ")"
	BoolAndString tokenizerResult = inProvider->GetNextToken();
	BoolAndCPPExpression expressionResult;
	CPPExpressionList params;
	bool status = true;
	CPPExpression* functionCall = NULL;
	
	do
	{
		if(!tokenizerResult.first)
		{
			TRACE_LOG("CPPExpressionParser::ParseFunctionCall, syntax error, unable to read past function start '('");
			status = false;
			break;
		}

		inProvider->PutBackToken(tokenizerResult.second);

		while(tokenizerResult.second != ")") 
		{
			
			// parse expression
			expressionResult = ParseExpressionInternal(inProvider); // not limiting here by ,...cause i don't want to start checking, i'll trust the parser and hope for the best
			if(!expressionResult.first)
			{
				TRACE_LOG("CPPExpressionParser::ParseFunctionCall, error in parsing function parameter");
				status = false;
				break;
			}
			params.push_back(expressionResult.second);
			tokenizerResult = inProvider->GetNextToken();
			if(!tokenizerResult.first)
			{
				TRACE_LOG("CPPExpressionParser::ParseFunctionCall, syntax error, expecting either ',' or ')' to finish/continue function parameters list");
				status = false;
				break;
			}
			if(tokenizerResult.second != ",")
				break;
		}
		if(!status)
			break;

		if(tokenizerResult.second != ")")
		{
			TRACE_LOG("CPPExpressionParser::ParseFunctionCall, syntax error, expecting ')' to finish function parameters list");
			status = false;
			break;
		}

		// k. now we got the params, and the current token is the end of the list..which is cool. can continue to create now the expression
		expressionResult = MakeFunctionCall(inToken,inScopes,params);
		if(!expressionResult.first)
		{
			TRACE_LOG("CPPExpressionParser::ParseFunctionCall, failure in creating function call expression");
			status = false;
			break;			
		}
		else
			functionCall = expressionResult.second;
	}
	while(false);

	if(status)
	{
		return BoolAndCPPExpression(true,functionCall);		
	}
	else
	{
		CPPExpressionList::iterator it = params.begin();
		for(; it != params.end(); ++it)
			delete *it;

		return FalseExpression();

	}
}

BoolAndCPPExpression CPPExpressionParser::MakeFunctionCall(const string& inToken,const StringList& inScopes,const CPPExpressionList& inParameters)
{
	return BoolAndCPPExpression(true,new CPPExpressionFunctionCall(inToken,inScopes,inParameters));
}

bool CPPExpressionParser::IsPostFixOperator(const string& inToken)
{
	return inToken == "[" || inToken == "++" || inToken == "--";
}

BoolAndCPPExpression CPPExpressionParser::ParsePostFixOperatorOperand(ITokenProvider* inProvider,CPPExpression* inOperand,const string& inFirstOperatorToken)
{
	// postfix operators may be with parameters or not...so parse according to operator
	CPPOperator* anOperator = NULL;
	CPPExpression* secondaryOperand = NULL;
	CPPExpression* primaryOperand = inOperand;
	string operatorToken = inFirstOperatorToken;
	bool statusOK = true;
	BoolAndString token;

	do
	{
		if(operatorToken == "++")
		{
			anOperator  = new CPPOperator(eCPPOperatorPostfixIncrement);
		}
		else if(operatorToken == "--")
		{
			anOperator  = new CPPOperator(eCPPOperatorPostfixDecrement);
		}
		else if(operatorToken == "[")
		{
			// parse the operand
			BoolAndCPPExpression expressionResult = ParseExpressionInternal(inProvider); 
			if(!expressionResult.first)
			{
				TRACE_LOG("CPPExpressionParser::ParsePostFixOperatorOperand, error in parsing subscript parameter");
				statusOK = false;
			}
			else
			{
				secondaryOperand = expressionResult.second;
				// take next token which has to be subscript end
				token = inProvider->GetNextToken();
				if(!token.first)
				{
					TRACE_LOG("CPPExpressionParser::ParsePostFixOperatorOperand, syntax error, unfinished subscript");
					delete secondaryOperand;
					secondaryOperand = NULL;
					statusOK = false;
				}
				if(token.second != "]")
				{
					TRACE_LOG1("CPPExpressionParser::ParsePostFixOperatorOperand, syntax error,subscript expression finished, but no subscript end. instead there's this - %s",token.second.c_str());
					delete secondaryOperand;
					secondaryOperand = NULL;
					statusOK = false;
				}
				anOperator = new CPPOperator(eCPPOperatorSubscript);
			}
		}
		else
		{
			TRACE_LOG1("CPPExpressionParser::ParsePostFixOperatorOperand, unknown postfix operator '%s'",operatorToken.c_str());
			statusOK = false;
		}

		if(statusOK)
		{
			primaryOperand = MakeExpression(anOperator,primaryOperand,secondaryOperand,NULL);
			secondaryOperand = NULL;
			anOperator = NULL;

			// now continue to the next loop, if there's more postfix oprators
			token = inProvider->GetNextToken();
			if(!token.first)
				break;

			if(IsPostFixOperator(token.second))
			{
				// aha! another postfix, store and continue
				operatorToken = token.second;
			}
			else
			{
				// not postfix. stop
				inProvider->PutBackToken(token.second);
				break;
			}

		}
	}while(statusOK);

	if(statusOK)
	{
		return BoolAndCPPExpression(true,primaryOperand);
	}
	else
	{
		delete primaryOperand;
		delete anOperator;
		delete secondaryOperand;
		return FalseExpression();

	}
}


bool CPPExpressionParser::IsOperandToParseAType(ITokenProvider* inProvider)
{
	// With the help of the type parsing helper, try to determine if the next parsed content is 

	return mTypeParserHelper && mTypeParserHelper->IsAboutToParseType(inProvider);
}

BoolAndCPPExpression CPPExpressionParser::ParseExpressionType(ITokenProvider* inProvider,const string& inTypeDelimiter)
{
	if(mTypeParserHelper)
	{
		TypedParameter* parsedParameter = mTypeParserHelper->ParseType(inProvider,inTypeDelimiter);

		if(parsedParameter)
			return MakeTypename(parsedParameter);
		else
			return FalseExpression();

	}
	else
		return FalseExpression();
}

BoolAndCPPExpression CPPExpressionParser::MakeTypename(TypedParameter* inTypename)
{
	return BoolAndCPPExpression(true,new CPPExpressionTypename(inTypename));
}

BoolAndCPPExpression CPPExpressionParser::ParseNewExpression(ITokenProvider* inProvider)
{
	// New expression looks like this: new LimitedUsedType(initializer)
	// we'll use the helper to parse the used type, using a specialized method, and then parse the initalizer like a regular function parameter call


	// type parser helper is mandatory this time
	if(!mTypeParserHelper)
		return FalseExpression();

	bool status = true;
	CPPExpression* newOperatorExpression = NULL;
	CPPExpressionList initializerParams;
	TypedParameter* parsedParameter = NULL;
	BoolAndCPPExpression expressionResult;

	do
	{
		// check for optional parenthesis
		BoolAndString token = inProvider->GetNextToken();
		if(!token.first)
		{
			TRACE_LOG("CPPExpressionParser::ParseNewExpression, no token after new. fail");
			status = false;
			break;
		}

		if(token.second == "(")
		{
			ParenthesisConditionalTokenProvider tokenProvider(inProvider);
			parsedParameter = mTypeParserHelper->ParseTypeForNew(&tokenProvider);
		}
		else
		{
			inProvider->PutBackToken(token.second);
			parsedParameter = mTypeParserHelper->ParseTypeForNew(inProvider);
		}
		if(!parsedParameter)
		{
			TRACE_LOG("CPPExpressionParser::ParseNewExpression, error in parsing type for new expression");
			status = false;
			break;
		}

		// check for initializer
		token = inProvider->GetNextToken();
		if(!token.first)
			break;

		if(token.second == "(")
		{
			token = inProvider->GetNextToken();
			if(!token.first)
			{
				TRACE_LOG("CPPExpressionParser::ParseNewExpression, syntax error, unable to read past initializer start '('");
				status = false;
				break;
			}

			while(token.second != ")") 
			{
			
				// parse expression
				expressionResult = ParseExpressionInternal(inProvider); // not limiting here by ,...cause i don't want to start checking, i'll trust the parser and hope for the best
				if(!expressionResult.first)
				{
					TRACE_LOG("CPPExpressionParser::ParseNewExpression, error in parsing function parameter");
					status = false;
					break;
				}
				initializerParams.push_back(expressionResult.second);
				token = inProvider->GetNextToken();
				if(!token.first)
				{
					TRACE_LOG("CPPExpressionParser::ParseNewExpression, syntax error, expecting either ',' or ')' to finish/continue function parameters list");
					status = false;
					break;
				}
				if(token.second != ",")
					break;
			}
			if(!status)
				break;

			if(token.second != ")")
			{
				TRACE_LOG("CPPExpressionParser::ParseNewExpression, syntax error, expecting ')' to finish function parameters list");
				status = false;
				break;
			}


		}

		newOperatorExpression = new CPPExpressionNewOperator(parsedParameter,initializerParams);
	} while(false);


	if(status)
	{
		return BoolAndCPPExpression(true,newOperatorExpression);
	}
	else
	{
		CPPExpressionList::iterator it = initializerParams.begin();
		for(; it != initializerParams.end(); ++it)
			delete *it;
		delete parsedParameter;

		return FalseExpression();
	}
}
