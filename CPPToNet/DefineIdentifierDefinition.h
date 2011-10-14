#pragma once

#include <string>
#include <list>

using namespace std;

typedef list<string> StringList;

class IPreprocessorTokenProvider;
class PreProcessor;

class DefineIdentifierDefinition
{
public:
	DefineIdentifierDefinition();
	~DefineIdentifierDefinition(void);

	void PushParameter(const string& inParameter);
	void SetTokenStrings(const string& inTokenStrings);

	IPreprocessorTokenProvider* CreateTokenProvider(PreProcessor* inTokenSource);
	IPreprocessorTokenProvider* CreateNoSpaceEntityProvider(PreProcessor* inTokenSource);

private:
	StringList mParameters;
	string mTokenStrings;

	bool ScanParameter(StringList& inTokens,PreProcessor* inTokenSource,bool& outStopEncountered);
	IPreprocessorTokenProvider* CreateProvider(PreProcessor* inTokenSource,const StringList& inTokenStrings);

};
