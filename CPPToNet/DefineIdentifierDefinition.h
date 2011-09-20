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
	void SetTokenStrings(const string& inTokenStrings);

	ITokenProvider* CreateTokenProvider(PreProcessor* inTokenSource);
	ITokenProvider* CreateNoSpaceEntityProvider(PreProcessor* inTokenSource);

private:
	StringList mParameters;
	string mTokenStrings;

	bool ScanParameter(StringList& inTokens,PreProcessor* inTokenSource,bool& outStopEncountered);
	ITokenProvider* CreateProvider(PreProcessor* inTokenSource,const StringList& inTokenStrings);

};
