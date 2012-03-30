#include "CPPTokenizer.h"
#include "IByteReader.h"
#include "OutputStringBufferStream.h"

using namespace Hummus;

CPPTokenizer::CPPTokenizer(void)
{
	mStream = NULL;
	ResetReadState();
}

CPPTokenizer::~CPPTokenizer(void)
{
}

void CPPTokenizer::SetReadStream(IByteReader* inSourceStream)
{
	mStream = inSourceStream;
	ResetReadState();
}

void CPPTokenizer::ResetReadState()
{
	mTokenBuffer.clear();
	mNotEncounteredTokensForLine = true;
	mNextTokenCanIncludeParanthesis = false;
}

static const Byte scBackslash[] = {'\\'};
static const Byte scStar[] = {'*'};
static const Byte scSlash[] = {'/'};
static const string scDefine = "#define";
BoolAndString CPPTokenizer::GetNextToken()
{
	BoolAndString result;
	Byte buffer;
	OutputStringBufferStream tokenBuffer;
	
	if(!mStream || !HasMoreTokens())
	{
		result.first = false;
		return result;
	}


	do
	{
		SkipTillToken();
		if(!HasMoreTokens())
		{
			result.first = false;
			break;
		}

		// get the first byte of the token
		if(GetNextByteForToken(buffer) != eSuccess)
		{
			result.first = false;
			break;
		}
		tokenBuffer.Write(&buffer,1);	

		result.first = true; // will only be changed to false in case of read error

		// ok...here we determine what to do according to what token we picked up. special cases:
		// punctuators (pick up the punctuator, with possibly more bytes to make it a complete unmistakable meaningful token)
		// strings, chars
		// numbers
		// default - identifiers
		// comments
		switch(buffer)
		{
			// symbols that are meaningful either as X or X=
			case '!':
			case '%':
			case '^':
			case '*':
			case '=':
				{
					// stream ended is cool, just store char
					if(!HasMoreTokens())
					{
						mNotEncounteredTokensForLine = false;
						result.second = tokenBuffer.ToString();
						break;
					}

					if(GetNextByteForToken(buffer) != eSuccess)
					{	
						result.first = false;
						mNotEncounteredTokensForLine = false;
						break;
					}

					if('=' == buffer)
						tokenBuffer.Write(&buffer,1);
					else // unrelated...end symbol
						SaveTokenBuffer(buffer);
					result.second = tokenBuffer.ToString();
					mNotEncounteredTokensForLine = false;
					break;
				}
			// symbols that are meaningful by themselves, always 
			case '(':
			case ')':
			case '{':
			case '[':
			case ']':
			case ';':
			case '?':
			case ',':
			case '~': // ~ may also be destructor, we'll take care of the difference between the 1 complement operator and destructor in semantic analysis
				{
					result.second = tokenBuffer.ToString();
					mNotEncounteredTokensForLine = false;
					break;
				}


			case '#': 
				{
					// Hurrah! either a preprocessor instruction or a preprocessor operator
			
					// first we check for the preprocessor instruction. it must be the first token in the line. i'm going to also grab the instruction itself
					if(mNotEncounteredTokensForLine)
					{
						// aha! this is an instruction
						BoolAndString instruction = GetNextToken(); // recursion! how exciting :)
						if(instruction.first)
						{
							if(instruction.second.length() > 0)
							{
								tokenBuffer.Write((const Byte*)instruction.second.c_str(),instruction.second.length());	
							}
							else
							{
								// special case, the null instruction...in which case i want to put the token back (it will be a newline)
								PutBackToken(instruction.second);
							}

						}

					}
					else
					{
						// k. this is an operator. might be # or ## or #@ (the last one is MS specific, but i'll let it go)
						// stream ended is cool, just store char
						if(!HasMoreTokens())
						{
							result.second = tokenBuffer.ToString();
							mNotEncounteredTokensForLine = false;
							break;
						}

						if(GetNextByteForToken(buffer) != eSuccess)
						{	
							result.first = false;
							mNotEncounteredTokensForLine = false;
							break;
						}

						if('#' == buffer || '@' == buffer)
						{
							tokenBuffer.Write(&buffer,1);
						}
						else
							SaveTokenBuffer(buffer); // k. it's the stringizing operator
					}


					result.second = tokenBuffer.ToString();
					// #define. if #define is the instruction than the next token may be a macro identifier token which
					// must have paranthesis included in order to identify it as a macro, as opposed to the paranthesis belonging already to the
					if(scDefine == result.second)
						mNextTokenCanIncludeParanthesis = true;
					mNotEncounteredTokensForLine = false;
					break;
				}
			// symbols that appear eitehr as X or XX or X=
			case '&':
			case '-':
			case '+':
			case '|':
				{
					// stream ended is cool, just store char
					Byte originalSymbol = buffer;
					if(!HasMoreTokens())
					{
						result.second = tokenBuffer.ToString();
						mNotEncounteredTokensForLine = false;
						break;
					}

					if(GetNextByteForToken(buffer) != eSuccess)
					{	
						result.first = false;
						mNotEncounteredTokensForLine = false;
						break;
					}

					if('=' == buffer || originalSymbol == buffer)
						tokenBuffer.Write(&buffer,1);
					else // else, unrelated
					{
						// special case for ->
						if(originalSymbol == '-' && buffer == '>')
							tokenBuffer.Write(&buffer,1);
						else
							SaveTokenBuffer(buffer);
					}
					result.second = tokenBuffer.ToString();
					mNotEncounteredTokensForLine = false;
					break;
				}
			// symbols that appear eitehr as X, XX, XX= or X=
			case '<':
			case '>':
				{
					// stream ended is cool, just store char
					Byte originalSymbol = buffer;
					if(!HasMoreTokens())
					{
						result.second = tokenBuffer.ToString();
						mNotEncounteredTokensForLine = false;
						break;
					}

					if(GetNextByteForToken(buffer) != eSuccess)
					{	
						result.first = false;
						mNotEncounteredTokensForLine = false;
						break;
					}

					if('=' == buffer)
					{
						tokenBuffer.Write(&buffer,1);
					}
					else if(buffer == originalSymbol)
					{
						// may be XX or XX=
						tokenBuffer.Write(&buffer,1);
						// basically just repeating the equality part
						if(!HasMoreTokens())
						{
							result.second = tokenBuffer.ToString();
							mNotEncounteredTokensForLine = false;
							break;
						}

						if(GetNextByteForToken(buffer) != eSuccess)
						{	
							result.first = false;
							mNotEncounteredTokensForLine = false;
							break;
						}

						if('=' == buffer)
							tokenBuffer.Write(&buffer,1);
						else // unrelated...end symbol
							SaveTokenBuffer(buffer);

					}
					else
					{
						// unrelated...end symbol
						SaveTokenBuffer(buffer);
					}
					mNotEncounteredTokensForLine = false;
					result.second = tokenBuffer.ToString();
					break;
				}
			// : or ::
			case ':':
				{
					// stream ended is cool, just store char
					if(!HasMoreTokens())
					{
						result.second = tokenBuffer.ToString();
						mNotEncounteredTokensForLine = false;
						break;
					}

					if(GetNextByteForToken(buffer) != eSuccess)
					{	
						result.first = false;
						mNotEncounteredTokensForLine = false;
						break;
					}

					if(':' == buffer)
					{
						tokenBuffer.Write(&buffer,1);
					}
					else
					{
						// unrelated...end symbol
						SaveTokenBuffer(buffer);
					}
					result.second = tokenBuffer.ToString();
					mNotEncounteredTokensForLine = false;
					break;
				}
			// . or ...
			case '.':
				{
					// stream ended is cool, just store char
					if(!HasMoreTokens())
					{
						result.second = tokenBuffer.ToString();
						mNotEncounteredTokensForLine = false;
						break;
					}

					if(GetNextByteForToken(buffer) != eSuccess)
					{	
						result.first = false;
						mNotEncounteredTokensForLine = false;
						break;
					}

					if('.' == buffer) // k. this must be a ... (just verify)
					{
						tokenBuffer.Write(&buffer,1);

						// now, since .. is always meaningless, i'm willing to fail in any case there's a failure
						// to return something
						if(GetNextByteForToken(buffer) != eSuccess || '.' != buffer)
						{	
							result.first = false;
							mNotEncounteredTokensForLine = false;
							break;
						}	
						tokenBuffer.Write(&buffer,1);
					}
					else
					{
						// unrelated...end symbol
						SaveTokenBuffer(buffer);
					}
					result.second = tokenBuffer.ToString();
					mNotEncounteredTokensForLine = false;
					break;
				}

			// characters, strings
			case '\'':
			case '"':
				{
					// string or characters go on until the first unescaped encounter of the same starting symbol
					Byte originalSymbol = buffer;

					bool backSlashEncountered = false;
					while(HasMoreTokens())
					{
						if(GetNextByteForToken(buffer) != eSuccess)
						{	
							result.first = false;
							mNotEncounteredTokensForLine = false;
							break;
						}
				
						if(backSlashEncountered)
						{	// backSlasch nulls anything after it from being active entity closer...
							backSlashEncountered = false;
							tokenBuffer.Write(scBackslash,1);					
							tokenBuffer.Write(&buffer,1);
						}
						else
						{
							if('\\' == buffer)
							{
								backSlashEncountered = true; 
								continue;
							}
							tokenBuffer.Write(&buffer,1);
							if(originalSymbol == buffer) // k. found the end
								break;

						}
					}
					if(result.first)
					{
						mNotEncounteredTokensForLine = false;
						result.second = tokenBuffer.ToString();
					}
					break;
				}
			// division, but may be comment as well
			case '/':
				{
					// stream ended is cool, just store char
					if(!HasMoreTokens())
					{
						result.second = tokenBuffer.ToString();
						mNotEncounteredTokensForLine = false;
						break;
					}

					if(GetNextByteForToken(buffer) != eSuccess)
					{	
						result.first = false;
						mNotEncounteredTokensForLine = false;
						break;
					}

					// k. got 4 different options here
					if('=' == buffer)
					{	
						// /= division assignment
						tokenBuffer.Write(&buffer,1);
					}
					else if('/' == buffer)
					{
						tokenBuffer.Write(&buffer,1);

						// single line comment
						while(HasMoreTokens())
						{
							if(GetNextByteForToken(buffer) != eSuccess)
							{	
								result.first = false;
								mNotEncounteredTokensForLine = false;
								break;
							}
							if(0xD == buffer|| 0xA == buffer)
							{
								SaveTokenBuffer(buffer);
								break;
							}
							tokenBuffer.Write(&buffer,1);
						}
					}
					else if('*' == buffer)
					{
						tokenBuffer.Write(&buffer,1);

						// multiline comment. need to find ending * /...since i'm nice i'll check for balancing
						int balanceLevel = 1;
						bool starEncountered = false;
						bool slashEncountered = false;
						while(balanceLevel > 0 && HasMoreTokens())
						{
							if(GetNextByteForToken(buffer) != eSuccess)
							{	
								result.first = false;
								break;
							}
					
							if(starEncountered && '/' == buffer) // down one balance level
							{
								starEncountered = false;
								tokenBuffer.Write(scStar,1);					
								tokenBuffer.Write(&buffer,1);
								--balanceLevel;
							}
							else if(slashEncountered && '*' == buffer) // up one balance level
							{
								slashEncountered = false;
								tokenBuffer.Write(scSlash,1);					
								tokenBuffer.Write(&buffer,1);
								--balanceLevel;							
							}
							else
							{
								if(slashEncountered)
								{
									tokenBuffer.Write(scSlash,1);
									slashEncountered = false;
								}
								if(starEncountered)
								{
									tokenBuffer.Write(scStar,1);
									starEncountered = false;
								}
								if('/' == buffer)
								{
									slashEncountered = true; 
									continue;
								}
								if('*' == buffer)
								{
									starEncountered = true;
									continue;
								}
								tokenBuffer.Write(&buffer,1);
							}
						}
						if(result.first)
						{
							result.second = tokenBuffer.ToString();
							mNotEncounteredTokensForLine = false;
						}
						break;

					}
					else // unrelated...end symbol
					{
						SaveTokenBuffer(buffer);
					}
					result.second = tokenBuffer.ToString();
					mNotEncounteredTokensForLine = false;
					break;
				}
			// tokenize newlines as well. note that in newlines the mNotEncounteredTokensForLine is set to true
			case 0xA:
				{
					result.second = tokenBuffer.ToString();
					mNotEncounteredTokensForLine = true;
					break;
				}
			case 0xD:
				{
					// may be 0d0a, in which case take as a single token
					if(!HasMoreTokens())
					{
						result.second = tokenBuffer.ToString();
						mNotEncounteredTokensForLine = true;
						break;
					}

					if(GetNextByteForToken(buffer) != eSuccess)
					{	
						result.first = false;
						mNotEncounteredTokensForLine = true;
						break;
					}

					if(0xA == buffer)
						tokenBuffer.Write(&buffer,1);
					else
						SaveTokenBuffer(buffer);
					mNotEncounteredTokensForLine = true;
					result.second = tokenBuffer.ToString();
					break;
				}
			default:
				{
					// numbers
					// identifiers
					// and a special case for wide strings

					// the difference between numbers and identifiers, is that with numbers, the first
					// appearance of the . does not separate entities
					bool allowFirstDot = ('0' <= buffer && buffer <= '9');

					// check for wide string
					if('L' == buffer)
					{
						if(!HasMoreTokens())
						{
							result.second = tokenBuffer.ToString();
							mNotEncounteredTokensForLine = true;
							break;
						}

						if(GetNextByteForToken(buffer) != eSuccess)
						{	
							result.first = false;
							mNotEncounteredTokensForLine = true;
							break;
						}	

						SaveTokenBuffer(buffer); // put it back. in any case
						if('\"' == buffer) // aha! wide string. put back and recurse
						{
							BoolAndString nextToken = GetNextToken();  // no need to check, it will at least have the char we just put back
							tokenBuffer.Write((const Byte*)nextToken.second.c_str(),nextToken.second.length());
							mNotEncounteredTokensForLine = false;
							result.second = tokenBuffer.ToString();
							break;								
						}
						// otherwise go on regularly (remember we saved the token...so it should be like it never happened...)

					}

					while(HasMoreTokens())
					{
						if(GetNextByteForToken(buffer) != eSuccess)
						{	
							result.first = false;
							break;
						}
						if(allowFirstDot && '.' == buffer)
						{
							 allowFirstDot = false;
							 tokenBuffer.Write(&buffer,1);
						}
						else if(IsEntityBreaker(buffer,mNextTokenCanIncludeParanthesis) || IsWhiteSpace(buffer)) // mNextTokenCanIncludeParanthesis is used for #define identifiers that can include them
						{
							SaveTokenBuffer(buffer); // save the token for next read, even if white space. this will enable tokenizing also newlines
							break;
						}
						else
							tokenBuffer.Write(&buffer,1);
					}
					mNextTokenCanIncludeParanthesis = false; // null the flag in any case
					mNotEncounteredTokensForLine = false;
					result.second = tokenBuffer.ToString();
					break;
				}
		}

	} while(false);

	return result;
}

EStatusCode CPPTokenizer::GetNextByteForToken(Byte& outByte)
{
	if(mTokenBuffer.size() > 0)
	{
		outByte = mTokenBuffer.front();
		mTokenBuffer.pop_front();
		return eSuccess;
	}
	else
		return (mStream->Read(&outByte,1) != 1) ? eFailure:eSuccess;
}

bool CPPTokenizer::HasMoreTokens()
{
	return mTokenBuffer.size() > 0 || mStream->NotEnded();
}

void CPPTokenizer::SkipTillToken()
{
	Byte buffer = 0;

	if(!mStream)
		return;

	// skip till hitting first non space, or segment end
	while(HasMoreTokens())
	{
		if(GetNextByteForToken(buffer) != eSuccess)
			break;

		if(!IsNonNewlineWhiteSpace(buffer))
		{
			SaveTokenBuffer(buffer);
			break;
		}
	}
}

static const Byte scWhiteSpaces[] = {0,0x9,0x20,0xB,0xC,0xD,0xA};
bool CPPTokenizer::IsWhiteSpace(Byte inCharacter)
{
	bool isWhiteSpace = false;
	for(int i=0; i < 7 && !isWhiteSpace; ++i)
		isWhiteSpace =  (scWhiteSpaces[i] == inCharacter);
	return isWhiteSpace;	
}

bool CPPTokenizer::IsNonNewlineWhiteSpace(Byte inCharacter)
{
	bool isNonNewlineWhiteSpace = false;
	for(int i=0; i < 5 && !isNonNewlineWhiteSpace; ++i)
		isNonNewlineWhiteSpace =  (scWhiteSpaces[i] == inCharacter);
	return isNonNewlineWhiteSpace;	
}


void CPPTokenizer::SaveTokenBuffer(Byte inToSave)
{
	mTokenBuffer.push_back(inToSave);
}

void CPPTokenizer::PutBackToken(const string& inToken)
{
	// putting back a token places the chars to the start of the list, because it was read prior to the current saved tokens
	string::const_reverse_iterator it = inToken.rbegin();
	for(; it != inToken.rend();++it)
		mTokenBuffer.push_front(*it);
}


bool CPPTokenizer::IsEntityBreaker(Byte inCharacter,bool inCanIncludeParanthesis)
{
	// for identifiers, entity breakers are punctuation marks
	bool result;
	switch(inCharacter)
	{
		case '!':
		case '%':
		case '^':
		case '&':
		case '*':
		case '-':
		case '+':
		case '=':
		case '{':
		case '}':
		case '|':
		case '~':
		case '[':
		case ']':
		case '\\': 
		case ';':
		case '\'': 
		case ':':
		case '"':
		case '<':
		case '>':
		case '?':
		case ',':
		case '.':
		case '/': 
		case '#':
			result = true;
			break;
		case '(':
		case ')':
			result = !inCanIncludeParanthesis;
			break;
		default:
			result = false;
			break;
	}

	return result;
}

string CPPTokenizer::GetStringTillEndOfLine()
{
	stringstream aStream;

	Byte buffer = 0;

	if(!mStream)
		return "";

	// skip till hitting first non space, or segment end
	while(HasMoreTokens())
	{
		if(GetNextByteForToken(buffer) != eSuccess)
			break;

		if(buffer == '\n' || buffer == '\r')
		{
			SaveTokenBuffer(buffer);
			break;
		}

		aStream.put(buffer);
	}

	return aStream.str();
}

BoolAndString CPPTokenizer::GetNextNoSpaceEntity()
{
	BoolAndString result;
	Byte buffer;
	OutputStringBufferStream tokenBuffer;
	
	if(!mStream || !HasMoreTokens())
	{
		result.first = false;
		return result;
	}


	do
	{
		SkipAnySpaceTillToken();
		if(!HasMoreTokens())
		{
			result.first = false;
			break;
		}

		while(HasMoreTokens())
		{
			if(GetNextByteForToken(buffer) != eSuccess)
			{	
				result.first = false;
				break;
			}
			else if(IsWhiteSpace(buffer))
			{
				SaveTokenBuffer(buffer); // save the token for next read, even if white space. this will enable tokenizing also newlines
				break;
			}
			else
				tokenBuffer.Write(&buffer,1);
		}
		result.second = tokenBuffer.ToString();
		result.first = true;
	} while(false);

	return result;
}

void CPPTokenizer::SkipAnySpaceTillToken()
{
	Byte buffer = 0;

	if(!mStream)
		return;

	// skip till hitting first non space, or segment end
	while(HasMoreTokens())
	{
		if(GetNextByteForToken(buffer) != eSuccess)
			break;

		if(!IsWhiteSpace(buffer))
		{
			SaveTokenBuffer(buffer);
			break;
		}
	}	
}