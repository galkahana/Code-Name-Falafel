#pragma once

#include "ITestUnit.h"

class WindowsPathTest : public ITestUnit
{
public:
	WindowsPathTest(void);
	~WindowsPathTest(void);

	virtual Hummus::EStatusCode Run();

};
