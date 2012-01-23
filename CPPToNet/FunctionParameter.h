#pragma once

#include <string>
#include <list>

using namespace std;

class UsedTypeDescriptor;

struct FunctionParameter
{
	FunctionParameter();
	~FunctionParameter();

	string Name;
	UsedTypeDescriptor* Type;
};

typedef list<FunctionParameter*> FunctionParameterList;

