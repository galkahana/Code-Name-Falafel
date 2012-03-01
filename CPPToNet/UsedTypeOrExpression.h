#pragma once
#include <stdio.h>
#include <list>

#include "CPPExpression.h"
#include "UsedTypeDescriptor.h"
using namespace std;

struct UsedTypeOrExpression
{
	UsedTypeOrExpression(){Descriptor=NULL;Expression=NULL;IsUsedTypeDescriptor = false;}
	UsedTypeOrExpression(UsedTypeDescriptor* inDescriptor){Descriptor=inDescriptor;Expression=NULL;IsUsedTypeDescriptor = true;}
	UsedTypeOrExpression(CPPExpression* inExpression){Descriptor=NULL;Expression=inExpression;IsUsedTypeDescriptor = false;}
	~UsedTypeOrExpression(){delete Expression; delete Descriptor;}
	
	UsedTypeDescriptor* Descriptor;
	CPPExpression* Expression;
	bool IsUsedTypeDescriptor;

};

typedef list<UsedTypeOrExpression*> UsedTypeOrExpressionList;
