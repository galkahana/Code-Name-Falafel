#pragma once

#include "ITestUnit.h"


#include <string>

using namespace std;

class TokenizerTest : public ITestUnit
{
public:
	TokenizerTest(void);
	~TokenizerTest(void);

	virtual Hummus::EStatusCode Run();

private:
	bool IsNewLine(const string& inToken);

};
