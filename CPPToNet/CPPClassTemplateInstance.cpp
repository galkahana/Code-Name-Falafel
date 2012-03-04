#include "CPPClassTemplateInstance.h"
#include "CPPClass.h"

CPPClassTemplateInstance::CPPClassTemplateInstance(const string& inName,CPPClass* inTemplate,const UsedTypeOrExpressionList& inInstanceParameters):CPPElement(inName,CPPElement::eCPPElementClassTemplateInstance)
{
	mSourceTamplate = inTemplate;
	mInstanceParameters = inInstanceParameters;
}


CPPClassTemplateInstance::~CPPClassTemplateInstance(void)
{
	UsedTypeOrExpressionList::iterator it = mInstanceParameters.begin();
	for(; it != mInstanceParameters.end(); ++it)
		delete *it;
}

CPPElementList CPPClassTemplateInstance::FindElements(const string& inElementName)
{
	// here there's a bit of a foul. obviously, what should be done is that the right instance is created
	// and through the parameters the real types are intepreted. not gonna do this. for now.
	// and so, i'll use the template itself, in hope that for most usages of this project, it will suffice
	return mSourceTamplate->FindElements(inElementName);
}

CPPClass* CPPClassTemplateInstance::GetSourceTemplate()
{
	return mSourceTamplate;
}

UsedTypeOrExpressionList& CPPClassTemplateInstance::GetInstanceParameters()
{
	return mInstanceParameters;
}

