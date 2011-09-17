#pragma once

#include <string>
#include <list>

using namespace std;

typedef list<string> StringList;

class ITokenProvider;
class PreProcessor;

class DefineIdentifierDefinition
{
public:
	DefineIdentifierDefinition();
	~DefineIdentifierDefinition(void);

	void PushParameter(const string& inParameter);
	void PushTokenString(const string& inTokenString);

	ITokenProvider* CreateTokenProvider(PreProcessor* inTokenSource);

private:
	StringList mParameters;
	StringList mTokenStrings;

	bool ScanParameter(StringList& inTokens,PreProcessor* inTokenSource,bool& outStopEncountered);

};
