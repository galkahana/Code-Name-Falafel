#include "WindowsPathTest.h"
#include "WindowsPath.h"
#include "TestsRunner.h"

#include <iostream>

using namespace Hummus;

WindowsPathTest::WindowsPathTest(void)
{
}

WindowsPathTest::~WindowsPathTest(void)
{
}

EStatusCode WindowsPathTest::Run()
{
	EStatusCode status = eSuccess;

	// empty path from->to
	WindowsPath aPath;

	aPath.SetPathFromString("");

	if(aPath.ToString() != "")
	{
		cout<<"WindowsPathTest::Run, failed in testing empty path. path should be empty and is "<<aPath.ToString()<<"\n";
		status = eFailure;
	}

	// absolute path from->to
	aPath.SetPathFromString("c:\\test\\test1\\");

	if(aPath.ToString() != "c:\\test\\test1")
	{
		cout<<"WindowsPathTest::Run, failed in testing absolute path. path should be c:\\test\\test1 and is "<<aPath.ToString()<<"\n";
		status = eFailure;
	}

	if(!aPath.IsAbsolute())
	{
		cout<<"WindowsPathTest::Run, failed in 2nd testing of absolute path. path is not considered absolute\n";
		status = eFailure;
	}


	aPath.SetPathFromString("c:\\test\\test1");

	if(aPath.ToString() != "c:\\test\\test1")
	{
		cout<<"WindowsPathTest::Run, failed in 2nd testing of absolute path. path should be c:\\test\\test1 and is "<<aPath.ToString()<<"\n";
		status = eFailure;
	}

	if(!aPath.IsAbsolute())
	{
		cout<<"WindowsPathTest::Run, failed in 2nd testing of absolute path. path is not considered absolute\n";
		status = eFailure;
	}

	// relative path from->to
	aPath.SetPathFromString("test\\test1");

	if(aPath.ToString() != "test\\test1")
	{
		cout<<"WindowsPathTest::Run, failed in testing of relative path. path should be test\\test1 and is "<<aPath.ToString()<<"\n";
		status = eFailure;
	}


	if(aPath.IsAbsolute())
	{
		cout<<"WindowsPathTest::Run, failed in 2nd testing of absolute path. path is considered absolute\n";
		status = eFailure;
	}

	// with .
	aPath.SetPathFromString("c:\\test\\.\\test1\\.\\");
	if(aPath.ToString() != "c:\\test\\test1")
	{
		cout<<"WindowsPathTest::Run, failed in testing of path with \".\"s. path should be c:\\test\\test1 and is "<<aPath.ToString()<<"\n";
		status = eFailure;
	}

	// with .. which can minimize
	aPath.SetPathFromString("c:\\test\\..\\test1\\..\\");
	if(aPath.ToString() != "c:\\")
	{
		cout<<"WindowsPathTest::Run, failed in testing of path with \"..\"s. path should be c:\\ and is "<<aPath.ToString()<<"\n";
		status = eFailure;
	}

	// with .. that cannot
	aPath.SetPathFromString("..\\..\\test1");
	if(aPath.ToString() != "..\\..\\test1")
	{
		cout<<"WindowsPathTest::Run, failed in testing of path with \"..\"s. path should be ..\\..\\test1 and is "<<aPath.ToString()<<"\n";
		status = eFailure;
	}

	// build path using push/pop/drive letter (with and without)
	aPath.SetPathFromString("");
	aPath.PushPathComponent("test");
	aPath.PushPathComponent("test1");

	if(aPath.ToString() != "test\\test1")
	{
		cout<<"WindowsPathTest::Run, failed in building path from pushs. path should be test\\test1 and is "<<aPath.ToString()<<"\n";
		status = eFailure;
	}

	aPath.SetDriveLetter("d");
	if(aPath.ToString() != "d:\\test\\test1")
	{
		cout<<"WindowsPathTest::Run, failed in building path from pushs. path should be d:\\test\\test1 and is "<<aPath.ToString()<<"\n";
		status = eFailure;
	}


	if(aPath.GetFolder().ToString() != "d:\\test")
	{
		cout<<"WindowsPathTest::Run, failed in getting parent folder. path should be d:\\test and is "<<aPath.GetFolder().ToString()<<"\n";
		status = eFailure;
	}


	// path determining
	// simple
	// with ..s
	// at this point aPath should be holding d:\test\test1

	if(WindowsPath("test2\\test3").InterpretFrom(aPath).ToString() != "d:\\test\\test1\\test2\\test3")
	{
		cout<<"WindowsPathTest::Run, failed in constructing paths. path should be d:\\test\\test1\\test2\\test3 and is "<<WindowsPath("test2\\test3").InterpretFrom(aPath).ToString()<<"\n";
		status = eFailure;
	}

	if(WindowsPath("..\\test3").InterpretFrom(aPath).ToString() != "d:\\test\\test3")
	{
		cout<<"WindowsPathTest::Run,failed in constructing paths. path should be d:\\test\\test3 and is "<<WindowsPath("..\\test3").InterpretFrom(aPath).ToString()<<"\n";
		status = eFailure;
	}
	
	
	return status;
}

ADD_CATEGORIZED_TEST(WindowsPathTest,"WindowsOS")