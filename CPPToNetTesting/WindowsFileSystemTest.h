#pragma once

#include "ITestUnit.h"

class WindowsFileSystemTest : public ITestUnit
{
public:
	WindowsFileSystemTest(void);
	~WindowsFileSystemTest(void);

	virtual Hummus::EStatusCode Run();


};
