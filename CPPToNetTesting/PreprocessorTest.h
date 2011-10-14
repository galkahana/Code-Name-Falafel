#pragma once

#include "ITestUnit.h"
#include "IPreprocessorListener.h"

namespace Hummus
{
	class IByteWriter;
}

class PreprocessorTest : public ITestUnit, public IPreprocessorListener
{
public:
	PreprocessorTest(void);
	~PreprocessorTest(void);


	// ITestUnit interface
	virtual Hummus::EStatusCode Run();

	// IPreprocessorListener interface
	virtual void OnNewLine(const string& inNewLineString);

private:

	Hummus::EStatusCode RunTest(const string& inName, const string& inOriginalFile, const string& inOutputFile, const string& inComparisonFile);

	bool mStartRow;
	Hummus::IByteWriter* mCurrentStream;
};
