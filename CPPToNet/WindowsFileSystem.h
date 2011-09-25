#pragma once

#include "WindowsPath.h"

#include <utility>
#include <string>
#include <time.h>

using namespace std;

typedef pair<bool,string> BoolAndString;
typedef pair<bool,tm> BoolAndTm;

class WindowsFileSystem
{
public:
	WindowsFileSystem(void);
	~WindowsFileSystem(void);

	BoolAndString GetExistingFilePath(const WindowsPath& inBasePath,const WindowsPath& inRelativePath);
	bool FileExists(const WindowsPath& inFilePath);

	BoolAndTm GetFileModificationTime(const string& inFileName);
};
