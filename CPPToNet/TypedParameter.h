#pragma once

#include <string>
#include <list>

using namespace std;

class UsedTypeDescriptor;

struct TypedParameter
{
	TypedParameter();
	~TypedParameter();

	string Name;
	UsedTypeDescriptor* Type;
};

typedef list<TypedParameter*> TypedParameterList;
