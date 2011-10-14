#include "SimpleFileComparer.h"
#include "InputFile.h"

using namespace Hummus;

SimpleFileComparer::SimpleFileComparer(void)
{
}

SimpleFileComparer::~SimpleFileComparer(void)
{
}


bool SimpleFileComparer::Same(const string& inFileA,const string& inFileB)
{
	bool same = true;
	InputFile fileA,fileB;
	Byte bufferA,bufferB;

	fileA.OpenFile(inFileA);
	fileB.OpenFile(inFileB);

	while(same && fileA.GetInputStream()->NotEnded() && fileB.GetInputStream()->NotEnded())
	{
		fileA.GetInputStream()->Read(&bufferA,1);
		fileB.GetInputStream()->Read(&bufferB,1);

		same = (bufferA == bufferB);
	}

	if(same)
		return (fileA.GetInputStream()->NotEnded() == fileB.GetInputStream()->NotEnded());
	else 
		return false;
}
