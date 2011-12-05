#pragma once
#include <string>

class CPPElement;
class ICPPDeclerator;

using namespace std;

class ICPPDeclaratorContainer
{
	public:

		virtual ~ICPPDeclaratorContainer(){}

		virtual ICPPDeclerator* AddDeclarator(const string& inVariableName,
										      CPPElement* inType) = 0;
};