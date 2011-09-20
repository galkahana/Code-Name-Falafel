#pragma once

#include "WindowsPath.h"

#include <utility>
#include <string>

using namespace std;

typedef pair<bool,string> BoolAndString;

class WindowsFileSystem
{
public:
	WindowsFileSystem(void);
	~WindowsFileSystem(void);

	BoolAndString GetExistingFilePath(const WindowsPath& inBasePath,const WindowsPath& inRelativePath);
	bool FileExists(const WindowsPath& inFilePath);
};
