#pragma once

struct TypedParameter;

class ITemplateWithDefault
{
public:

	virtual ~ITemplateWithDefault(){}

	virtual void SetDefaultTypename(TypedParameter* inDefault) = 0;
};