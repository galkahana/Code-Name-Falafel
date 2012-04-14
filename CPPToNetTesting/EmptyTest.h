#pragma once

#include "ITestUnit.h"

class EmptyTest : public ITestUnit
{
public:
	EmptyTest(void);
	~EmptyTest(void);

	// ITestUnit interface
	virtual Hummus::EStatusCode Run();
};

