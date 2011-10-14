#pragma once

#include "ITestUnit.h"

class PreTokenizerDecoderTest : public ITestUnit
{
public:
	PreTokenizerDecoderTest(void);
	~PreTokenizerDecoderTest(void);

	virtual Hummus::EStatusCode Run();
};
