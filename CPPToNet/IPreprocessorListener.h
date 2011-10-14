#pragma once

#include <string>

using namespace std;

class IPreprocessorListener
{
public:

	virtual ~IPreprocessorListener(){}

	virtual void OnNewLine(const string& inNewLineString) = 0;

};