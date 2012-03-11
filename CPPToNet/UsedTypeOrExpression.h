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


// defining equivalence rules for template instances...not required...but will help with keeping the instances to a certain minimum. hopefully.
class UsedTypeOrExpressionListLess : public binary_function<const UsedTypeOrExpressionList&,const UsedTypeOrExpressionList&,bool>
{
public:
	bool operator( ) (const UsedTypeOrExpressionList& inLeft, 
						const UsedTypeOrExpressionList& inRight ) const
	{
		if(inLeft.size() != inRight.size())
			return inLeft < inRight;

		UsedTypeOrExpressionList::const_iterator itLeft = inLeft.begin();
		UsedTypeOrExpressionList::const_iterator itRight = inRight.begin();
		bool isLess = false;

		for(; itLeft != inLeft.end(); ++itLeft,++itRight)
		{
			if((*itLeft)->IsUsedTypeDescriptor != (*itRight)->IsUsedTypeDescriptor)
			{
				// compare the type of element. though should always be the same.
				// let's decide that types are  less than expressions
				isLess = (*itLeft)->IsUsedTypeDescriptor;
				break;
			}

			if((*itLeft)->IsUsedTypeDescriptor)
			{
				if(!(*itLeft)->Descriptor->IsEqual((*itRight)->Descriptor))
				{
					isLess = (*itLeft)->Descriptor->IsLess((*itRight)->Descriptor);
					break;
				}
			}
			else
			{
				// for expressions no compare. so just compare pointers
				if(!(*itLeft)->Expression->IsEqual((*itRight)->Expression))
				{
					isLess = (*itLeft)->Expression->IsLess((*itRight)->Expression);
					break;
				}
				break;
			}
		}

		return isLess;

	}
};