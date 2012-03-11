#pragma once

#include "UsedTypeOrExpression.h"
#include "CPPElement.h"
#include "CPPTemplateTemplateParameter.h"
#include "CPPTemplateTypename.h"
#include "CPPTemplateValue.h"
#include <list>
#include <utility>

class CPPElement;

typedef list<CPPElement*> CPPElementList;
typedef pair<CPPElementList,UsedTypeOrExpressionList> CPPElementListAndUsedTypeOrExpressionList;

// in essence, required for the template specialization equivelence function. pairs template parameter that were set, and those that were not

class CPPElementListAndUsedTypeOrExpressionListLess : public binary_function<const CPPElementListAndUsedTypeOrExpressionList&,const CPPElementListAndUsedTypeOrExpressionList&,bool>
{
public:
	bool operator( ) (const CPPElementListAndUsedTypeOrExpressionList& inLeft, 
						const CPPElementListAndUsedTypeOrExpressionList& inRight ) const
	{
		// start by comparing the UsedTypeOrExpressionlist part which represents the template assigned parameters
		if(inLeft.second.size() != inRight.second.size())
			return inLeft.second.size() < inRight.second.size();

		UsedTypeOrExpressionList::const_iterator itLeft = inLeft.second.begin();
		UsedTypeOrExpressionList::const_iterator itRight = inRight.second.begin();
		bool isLess = false;
		bool foundNonEqual = false;

		for(; itLeft != inLeft.second.end(); ++itLeft,++itRight)
		{
			if((*itLeft)->IsUsedTypeDescriptor != (*itRight)->IsUsedTypeDescriptor)
			{
				// compare the type of element. though should always be the same.
				// let's decide that types are  less than expressions
				isLess = (*itLeft)->IsUsedTypeDescriptor;
				foundNonEqual = true;
				break;
			}

			if((*itLeft)->IsUsedTypeDescriptor)
			{
				if(!(*itLeft)->Descriptor->IsEqual((*itRight)->Descriptor))
				{
					isLess = (*itLeft)->Descriptor->IsLess((*itRight)->Descriptor);
					foundNonEqual = true;
					break;
				}
			}
			else
			{
				// for expressions no compare. so just compare pointers
				if(!(*itLeft)->Expression->IsEqual((*itRight)->Expression))
				{
					isLess = (*itLeft)->Expression->IsLess((*itRight)->Expression);
					foundNonEqual = true;
					break;
				}
			}
		}

		if(foundNonEqual)
			return isLess;

		// continue by comparing the template non-assigned parameters. all must be template parameter types
		if(inLeft.first.size() != inRight.first.size())
			return inLeft.first.size() < inRight.first.size();

		CPPElementList::const_iterator itLeftT = inLeft.first.begin();
		CPPElementList::const_iterator itRightT = inRight.first.begin();
		bool isEqual = true;

		for(; itLeftT != inLeft.first.end() && isEqual; ++itLeftT,++itRightT)
		{
			if((*itLeftT)->Type != (*itRightT)->Type)
			{
				isLess = (*itLeftT)->Type < (*itRightT)->Type;
				break;
			}

			// can only be one of the three
			switch((*itLeftT)->Type)
			{
				case CPPElement::eCPPElemeentTemplateTemplateParameter:
					isEqual	= ((CPPTemplateTemplateParameter*)(*itLeftT))->IsEqual((CPPTemplateTemplateParameter*)(*itRightT));
					if(!isEqual)
						isLess = ((CPPTemplateTemplateParameter*)(*itLeftT))->IsLess((CPPTemplateTemplateParameter*)(*itRightT));
					break;
				case CPPElement::eCPPElemeentTemplateTypename:
					isEqual	= ((CPPTemplateTypename*)(*itLeftT))->IsEqual((CPPTemplateTypename*)(*itRightT));
					if(!isEqual)
						isLess = ((CPPTemplateTypename*)(*itLeftT))->IsLess((CPPTemplateTypename*)(*itRightT));
					break;
				case CPPElement::eCPPElementTemplateValue:
					isEqual	= ((CPPTemplateValue*)(*itLeftT))->IsEqual((CPPTemplateValue*)(*itRightT));
					if(!isEqual)
						isLess = ((CPPTemplateValue*)(*itLeftT))->IsLess((CPPTemplateValue*)(*itRightT));
					break;
			}
		}

		return isLess;
	}
};
