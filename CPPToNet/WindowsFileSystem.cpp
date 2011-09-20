#include "WindowsFileSystem.h"
#include "SafeBufferMacrosDefs.h"

#include <windows.h>

using namespace Hummus;

WindowsFileSystem::WindowsFileSystem(void)
{
}

WindowsFileSystem::~WindowsFileSystem(void)
{
}

BoolAndString WindowsFileSystem::GetExistingFilePath(const WindowsPath& inBasePath,const WindowsPath& inRelativePath)
{
	WindowsPath resultPath = inRelativePath.InterpretFrom(inBasePath);

	return BoolAndString(FileExists(resultPath),resultPath.ToString());
}

bool WindowsFileSystem::FileExists(const WindowsPath& inFilePath)
{
	WIN32_FIND_DATA findFileData;
	HANDLE hFind;

	hFind = FindFirstFile(UTF8ToUTF16Wide(inFilePath.ToString()).c_str(), &findFileData);

	if(hFind == NULL)
	{
		return false;
	}
	else
	{
		FindClose(hFind);
		return true;
	}
}
