#pragma once


#include <string>

using namespace std;

class SimpleFileComparer
{
public:
	SimpleFileComparer(void);
	~SimpleFileComparer(void);

	bool Same(const string& inFileA,const string& inFileB);
};
