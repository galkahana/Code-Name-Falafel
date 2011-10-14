#include "WindowsFileSystemTest.h"
#include "WindowsPath.h"
#include "WindowsFileSystem.h"
#include "TestsRunner.h"

#include <iostream>

using namespace Hummus;

WindowsFileSystemTest::WindowsFileSystemTest(void)
{
}

WindowsFileSystemTest::~WindowsFileSystemTest(void)
{
}

EStatusCode WindowsFileSystemTest::Run()
{
	EStatusCode status = eSuccess;
	WindowsFileSystem wfs;

	// existing file path
	BoolAndString existingFilePathResult = wfs.GetExistingFilePath(WindowsPath("c:\\CPPToNetTests\\TestMaterials"),WindowsPath("a.txt"));

	if(!existingFilePathResult.first  || existingFilePathResult.second != "c:\\CPPToNetTests\\TestMaterials\\a.txt")
	{
		cout<<"WindowsFileSystemTest::Run, failure in finding an existing file. should be found and is "
			<<(existingFilePathResult.first ? "found":"not found")<<" and path is "<<existingFilePathResult.second<<" and should be c:\\CPPToNetTests\\TestMaterials\\a.txt\n";
		status = eFailure;
	}

	existingFilePathResult = wfs.GetExistingFilePath(WindowsPath("c:\\CPPToNetTests\\TestMaterials"),WindowsPath("b.txt"));
	
	if(existingFilePathResult.first)
	{
		cout<<"WindowsFileSystemTest::Run, failure in finding a non-existing file. should be found not be found and is found\n";
		status = eFailure;
	}

	existingFilePathResult = wfs.GetExistingFilePath(WindowsPath("c:\\CPPToNetTests\\TestMaterials"),WindowsPath("notherFolder\\b.txt"));
	if(!existingFilePathResult.first  || existingFilePathResult.second != "c:\\CPPToNetTests\\TestMaterials\\notherFolder\\b.txt")
	{
		cout<<"WindowsFileSystemTest::Run, failure in finding an existing file. should be found and is "
			<<(existingFilePathResult.first ? "found":"not found")<<" and path is "<<existingFilePathResult.second<<" and should be c:\\CPPToNetTests\\TestMaterials\\notherFolder\\b.txt\n";
		status = eFailure;
	}


	if(!wfs.FileExists(WindowsPath("c:\\CPPToNetTests\\TestMaterials\\notherFolder\\b.txt")))
	{
		cout<<"WindowsFileSystemTest::Run, failure in FileExists command. should find c:\\CPPToNetTests\\TestMaterials\\notherFolder\\b.txt and does not\n";
		status = eFailure;
	}

	if(wfs.FileExists(WindowsPath("c:\\CPPToNetTests\\TestMaterials\\b.txt")))
	{
		cout<<"WindowsFileSystemTest::Run, failure in FileExists command. should not find c:\\CPPToNetTests\\TestMaterials\\b.txt and does\n";
		status = eFailure;
	}

	return status;
}

ADD_CATEGORIZED_TEST(WindowsFileSystemTest,"WindowsOS")