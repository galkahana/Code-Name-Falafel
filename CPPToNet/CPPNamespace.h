#pragma once

#include "EStatusCode.h"
#include "MapIterator.h"
#include "CPPElement.h"
#include "AbstractCPPContainer.h"

#include <set>

class CPPNamespace;
class CPPPrimitive;

typedef map<string,CPPNamespace*> StringToCPPNamespaceMap;
typedef map<string,CPPPrimitive*> StringToCPPPrimitiveMap;

class CPPNamespace : public CPPElement, public AbstractCPPContainer
{
public:
	CPPNamespace(const string& inName);
	virtual ~CPPNamespace(void);

	// Create  a new namespace with the input name. will return a namespace if one already exists
	CPPNamespace* CreateNewNamespace(const string& inNamespaceName);
	// Get an existing namespace. will returhn NULL if not found
	CPPNamespace* GetNamespace(const string& inNamespaceName);
	Hummus::MapIterator<StringToCPPNamespaceMap> GetNamespaceIterator();
	Hummus::EStatusCode CreateNewNamespaceAlias(const string& inNamespaceName,CPPNamespace* inNamespace);

	// add predefined primitive (normally should just be done on the global namespace)
	Hummus::EStatusCode AppendPrimitive(const string& inTypeName,CPPPrimitive* inNewType);

	// overrides on ICPPDefinitionsContainerElement to include namespace specific elements
	virtual CPPElementList FindElements(const string& inElementName);
	virtual Hummus::EStatusCode DefineAlias(const string& inAlias,CPPElement* inNewElement);

protected:

	// override to assert that a function is not virtual
	virtual CPPFunction* AppendFunction(const string& inFunctionName,
											bool inIsVirtual,
											bool inIsStatic,											
											UsedTypeDescriptor* inReturnTypeDescriptor,
											const TypedParameterList& inParametersList,
											bool inHasElipsis,		
											bool inIsPure,
											const UsedTypeOrExpressionList& inTemplateSpecializationList,
											bool inIsDefinition,
											CPPFunction* inFunction);
	virtual CPPFunction* AppendFunctionTemplate(const string& inFunctionName,
											bool inIsVirtual,
											bool inIsStatic,											
											UsedTypeDescriptor* inReturnTypeDescriptor,
											const TypedParameterList& inParametersList,
											bool inHasElipsis,		
											bool inIsPure,
											const CPPElementList& inTemplateParameters,
											const UsedTypeOrExpressionList& inTemplateParametersSpecialization,
											bool inIsDefinition,
											CPPFunction* inFunctionTemplate);
private:

	// namespaces
	StringToCPPNamespaceMap mSubordinateNamespaces;
	StringToCPPNamespaceMap mNamespaceAliases;

	// basic types
	StringToCPPPrimitiveMap mPrimitives;

	CPPNamespace* AppendNewNamespace(const string& inNamespaceName,CPPNamespace* inNewNamespace);


};
