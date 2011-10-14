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

	if(hFind == INVALID_HANDLE_VALUE)
	{
		return false;
	}
	else
	{
		FindClose(hFind);
		return true;
	}
}

BoolAndTm WindowsFileSystem::GetFileModificationTime(const string& inFileName)
{
	WIN32_FIND_DATA findFileData;
	HANDLE hFind;

	hFind = FindFirstFile(UTF8ToUTF16Wide(inFileName).c_str(), &findFileData);

	if(hFind == NULL)
	{
		return BoolAndTm(false,tm());
	}
	else
	{
		tm aTm;
		SYSTEMTIME systemTime;

		if(!FileTimeToSystemTime(&findFileData.ftLastWriteTime,&systemTime))
			return BoolAndTm(false,tm());
		aTm.tm_sec = systemTime.wSecond;
		aTm.tm_min = systemTime.wMinute;
		aTm.tm_mday = systemTime.wDay;
		aTm.tm_mon = systemTime.wMonth - 1;
		aTm.tm_year = systemTime.wYear - 1900;
		aTm.tm_wday = systemTime.wDayOfWeek;
		// never mind about the last two, i don't care about them.
		//aTm.tm_yday = 
		// aTm.tm_isdst ==

		FindClose(hFind);
		return BoolAndTm(true,aTm);
	}	
}
