#pragma once

#include <string>
#include <list>

using namespace std;

class UsedTypeDescriptor;

struct TypedParameter
{
	TypedParameter();
	TypedParameter(UsedTypeDescriptor* inType);
	TypedParameter(const string& inName,UsedTypeDescriptor* inType);
	~TypedParameter();

	string Name;
	UsedTypeDescriptor* Type;
};

typedef list<TypedParameter*> TypedParameterList;

