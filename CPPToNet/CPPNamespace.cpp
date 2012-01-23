#include "Trace.h"
#include "CPPNamespace.h"
#include "CPPEnumerator.h"
#include "CPPVariable.h"
#include "CPPUnion.h"
#include "CPPTypedef.h"
#include "CPPFunction.h"
#include "CPPPrimitive.h"
#include "UsedTypeDescriptor.h"

using namespace Hummus;

CPPNamespace::CPPNamespace(const string& inName):CPPElement(inName,eCPPElementNamespace)
{
}

CPPNamespace::~CPPNamespace(void)
{
	StringToCPPNamespaceMap::iterator itNamespaces = mSubordinateNamespaces.begin();
	for(; itNamespaces != mSubordinateNamespaces.end(); ++itNamespaces)
		if(mAliases.find(itNamespaces->second) == mAliases.end())
			delete itNamespaces->second;

	StringToCPPEnumeratorMap::iterator itEnumerators = mEnumerators.begin();
	for(; itEnumerators != mEnumerators.end(); ++itEnumerators)
		if(mAliases.find(itEnumerators->second) == mAliases.end())
			delete itEnumerators->second;

	StringToCPPVariableMap::iterator itVariables = mVariables.begin();
	for(; itVariables != mVariables.end(); ++itVariables)
		if(mAliases.find(itVariables->second) == mAliases.end())
			delete itVariables->second;
	
	StringToCPPUnionMap::iterator itUnions = mUnions.begin();
	for(; itUnions != mUnions.end(); ++itUnions)
		if(mAliases.find(itUnions->second) == mAliases.end())
			delete itUnions->second;

	StringToCPPPrimitiveMap::iterator itPrimitives = mPrimitives.begin();
	for(; itPrimitives != mPrimitives.end(); ++itPrimitives)
		if(mAliases.find(itPrimitives->second) == mAliases.end())
			delete itPrimitives->second;

	StringToCPPTypedefMap::iterator itTypedefs = mTypedefs.begin();
	for(; itTypedefs != mTypedefs.end(); ++itTypedefs)
		if(mAliases.find(itTypedefs->second) == mAliases.end())
			delete itTypedefs->second;

	StringToCPPFunctionListMap::iterator itFunctionLists = mFunctions.begin();
	for(; itFunctionLists != mFunctions.end(); ++itFunctionLists)
	{
		CPPFunctionList::iterator itFunctions = itFunctionLists->second.begin();
		for(; itFunctions != itFunctionLists->second.end(); ++itFunctions)
		if(mAliases.find(*itFunctions) == mAliases.end())
			delete *itFunctions;

	}

	// also - enumerator values are owned by their respective enumerators - so don't delete the map that contains them here.
}


CPPNamespace* CPPNamespace::CreateNewNamespace(const string& inNamespaceName)
{
	return AppendNewNamespace(inNamespaceName,NULL);
}

CPPNamespace* CPPNamespace::AppendNewNamespace(const string& inNamespaceName,CPPNamespace* inNewNamespace)
{
	CPPNamespace* newNamespace = GetNamespace(inNamespaceName);
	if(newNamespace)
		return newNamespace;

	// namespaces cannot share the name with anything else (much like primitives), so if creating a nwe one, make sure not
	// to create one with a name that already exists. note that at this point, if something is found, it is surely not a namespace

	CPPElementList existingElements = FindElements(inNamespaceName);
	if(existingElements.size() > 0)
		return NULL;
	else
		return mSubordinateNamespaces.insert(StringToCPPNamespaceMap::value_type(inNamespaceName,inNewNamespace ? inNewNamespace:new CPPNamespace(inNamespaceName))).first->second;
}

CPPNamespace* CPPNamespace::GetNamespace(const string& inNamespaceName)
{
	StringToCPPNamespaceMap::iterator itNamespaces = mSubordinateNamespaces.find(inNamespaceName);
	if(itNamespaces == mSubordinateNamespaces.end())
	{
		itNamespaces = mNamespaceAliases.find(inNamespaceName);
		if(itNamespaces == mNamespaceAliases.end())
			return NULL;
		else
			return itNamespaces->second;
	}
	else
		return itNamespaces->second;
}	

MapIterator<StringToCPPNamespaceMap> CPPNamespace::GetNamespaceIterator()
{
	return MapIterator<StringToCPPNamespaceMap>(mSubordinateNamespaces);
}

EStatusCode CPPNamespace::CreateNewNamespaceAlias(const string& inNamespaceName,CPPNamespace* inNamespace)
{
	// namespace aliases, in definition, work a lot like regular namespaces, not allowing other names, unless they are the identical namespace
	CPPNamespace* existingNamespace = GetNamespace(inNamespaceName);
	
	if(existingNamespace)
	{
		if(existingNamespace == inNamespace)
		{
			return eSuccess;
		}
		else
		{
			TRACE_LOG1("CPPNamespace::CreateNewNamespaceAlias, error in defining a namespace alias. a namespace already exists with the same name...but it's not the same namespace, for %s",inNamespaceName.c_str());
			return eFailure;
		}
	}
	else
	{
		CPPElementList existingElements = FindElements(inNamespaceName);
		if(existingElements.size() > 0)
		{
			return eFailure;
		}
		else
		{
			mNamespaceAliases.insert(StringToCPPNamespaceMap::value_type(inNamespaceName,inNamespace));
			return eSuccess;
		}
	}
}

CPPElementList CPPNamespace::FindElements(const string& inElementName)
{
	CPPElementList result;

	StringToCPPNamespaceMap::iterator itNamespaces = mSubordinateNamespaces.find(inElementName);
	if(itNamespaces != mSubordinateNamespaces.end())
		result.push_back(itNamespaces->second);

	StringToCPPNamespaceMap::iterator itNamespacesAliases = mNamespaceAliases.find(inElementName);
	if(itNamespacesAliases != mNamespaceAliases.end())
		result.push_back(itNamespacesAliases->second);

	StringToCPPEnumeratorMap::iterator itEnumerators = mEnumerators.find(inElementName);
	if(itEnumerators != mEnumerators.end())
		result.push_back(itEnumerators->second);


	StringToCPPEnumeratorValueMap::iterator itEnumValues = mEnumeratorValues.find(inElementName);
	if(itEnumValues != mEnumeratorValues.end())
		result.push_back(itEnumValues->second);


	StringToCPPVariableMap::iterator itVariables = mVariables.begin();
	if(itVariables != mVariables.end())
		result.push_back(itVariables->second);

	StringToCPPUnionMap::iterator itUnions = mUnions.begin();
	if(itUnions != mUnions.end())
		result.push_back(itUnions->second);

	StringToCPPPrimitiveMap::iterator itPrimitives = mPrimitives.begin();
	if(itPrimitives != mPrimitives.end())
		result.push_back(itPrimitives->second);

	StringToCPPTypedefMap::iterator itTypedefs = mTypedefs.begin();
	if(itTypedefs != mTypedefs.end())
		result.push_back(itTypedefs->second);

	StringToCPPFunctionListMap::iterator itFunctionLists = mFunctions.begin();
	if(itFunctionLists != mFunctions.end())
		result.insert(result.end(),itFunctionLists->second.begin(),itFunctionLists->second.end());

	return result;
}

Hummus::EStatusCode CPPNamespace::DefineAlias(const string& inAlias,CPPElement* inNewElement)
{
	EStatusCode status = eSuccess;

	// The success of alias definition, and also its storage, are dependent on the type of new element
	// normally there will be an attempted definition, and either a failure in the definition itself, or
	// a failure cause the alias is already taken by an element which is not the same as this one.
	switch(inNewElement->Type)
	{
		case eCPPElementPrimitive:
			status = AppendPrimitive(inAlias,(CPPPrimitive*)inNewElement);
			break;
		case eCPPElementNamespace:
			{
				CPPNamespace* createdElement = AppendNewNamespace(inAlias,(CPPNamespace*)inNewElement);
			
				if(createdElement != inNewElement)
				{
					TRACE_LOG1("CPPNamespace::DefineAlias, failed to define new namespace alias %s . an incompatible element already exists.",inAlias.c_str());
					status = eFailure;
				}
				break;
			}
		case eCPPElementEnumerator:
			{
				CPPEnumerator* aliasElement = (CPPEnumerator*)inNewElement;
				CPPEnumerator* createdElement = AppendEnumerator(inAlias,aliasElement->IsDefinition(),aliasElement);
			
				if(createdElement != aliasElement)
				{
					TRACE_LOG1("CPPNamespace::DefineAlias, failed to define new enumerator alias %s . an incompatible element already exists.",inAlias.c_str());
					status = eFailure;
				}
				break;		
			}
		case eCPPElementUnion:
			{
				CPPUnion* aliasElement = (CPPUnion*)inNewElement;
				CPPUnion* createdElement = AppendUnion(inAlias,aliasElement->IsDefinition(),aliasElement);
			
				if(createdElement != aliasElement)
				{
					TRACE_LOG1("CPPNamespace::DefineAlias, failed to define new union alias %s . an incompatible element already exists.",inAlias.c_str());
					status = eFailure;
				}
				break;	
			}
		case eCPPElementTypedef:
			{
				CPPTypedef* aliasElement = (CPPTypedef*)inNewElement;
				CPPTypedef* createdElement = AppendTypedef(inAlias,aliasElement->GetTypeDescriptor(),aliasElement);
			
				if(createdElement != aliasElement)
				{
					TRACE_LOG1("CPPNamespace::DefineAlias, failed to define new typedef alias %s . an incompatible element already exists.",inAlias.c_str());
					status = eFailure;
				}
				break;	
			}		
		case eCPPElementEnumeratorValue:
			{
				CPPEnumeratorValue* aliasElement = (CPPEnumeratorValue*)inNewElement;
				CPPEnumeratorValue* createdElement = AppendEnumeratorValue(NULL,inAlias,aliasElement);
			
				if(createdElement != aliasElement)
				{
					TRACE_LOG1("CPPNamespace::DefineAlias, failed to define new enumerator value alias %s . an incompatible element already exists.",inAlias.c_str());
					status = eFailure;
				}
				break;	
			}				
		case eCPPElementVariable:
			{
				CPPVariable* aliasElement = (CPPVariable*)inNewElement;
				CPPVariable* createdElement = AppendVariable(inAlias,aliasElement->GetTypeDescriptor(),aliasElement);
			
				if(createdElement != aliasElement)
				{
					TRACE_LOG1("CPPNamespace::DefineAlias, failed to define new variable alias %s . an incompatible element already exists.",inAlias.c_str());
					status = eFailure;
				}
				break;	
			}			
		case eCPPElementFunction:
			{
				CPPFunction* aliasElement = (CPPFunction*)inNewElement;
				CPPFunction* createdElement = AppendFunction(	inAlias,
																aliasElement->GetReturnType(),
																aliasElement->GetDeclaredParameterList(),
																aliasElement->HasElipsis(),
																aliasElement->IsDefinition(),
																aliasElement);
			
				if(createdElement != aliasElement)
				{
					TRACE_LOG1("CPPNamespace::DefineAlias, failed to define new function alias %s . an incompatible element already exists.",inAlias.c_str());
					status = eFailure;
				}
				break;	
			}	
	}


	// insert into aliases map, to indicate this being a mere alias, and not a defined content
	if(eSuccess == status)
		mAliases.insert(inNewElement);
	return status;	
}

CPPEnumerator* CPPNamespace::CreateEnumerator(const string& inEnumeratorName,bool inIsDefinition)
{
	return AppendEnumerator(inEnumeratorName,inIsDefinition,NULL);
}

CPPEnumerator* CPPNamespace::AppendEnumerator(const string& inEnumeratorName,bool inIsDefinition,CPPEnumerator* inEnumerator)
{
	CPPElementList existingElements = FindElements(inEnumeratorName);

	if(existingElements.size() > 0)
	{
		// for enumerators, the following is true:
		// other values (functions, variables) can share it's name. types cannot.
		// there may be multiple declerations but only one definition

		bool hasProblematicDefinition = false;
		CPPEnumerator* hadDefinedEnumerator = NULL;
		CPPElementList::iterator it = existingElements.begin();

		for(; it != existingElements.end(); ++it)
		{
			if((*it)->Type == CPPElement::eCPPElementEnumerator)
			{
				CPPEnumerator* otherEnumerator = (CPPEnumerator*)(*it);

				if(otherEnumerator->IsDefinition() && inIsDefinition)
				{
					TRACE_LOG1("CPPNamespace::CreateEnumerator, problem when creating a new enumerator declaration/definition. multiple definitons exist for a enumerator. This is not allowed. for enumerator %s",inEnumeratorName.c_str());
					hasProblematicDefinition = true;
					break;
				}

				if(inIsDefinition)
					otherEnumerator->SetIsDefinition();

				hadDefinedEnumerator = otherEnumerator;

			}
			else if (	(*it)->Type == CPPElement::eCPPElementUnion || 
						(*it)->Type == CPPElement::eCPPElementPrimitive ||
						(*it)->Type == CPPElement::eCPPElementNamespace)
			{
				hasProblematicDefinition = true;
				break;
			}
		}

		if(hasProblematicDefinition)
			return NULL;

		if(hadDefinedEnumerator)
			return hadDefinedEnumerator;
	}

	return mEnumerators.insert(StringToCPPEnumeratorMap::value_type(inEnumeratorName,inEnumerator ? inEnumerator : new CPPEnumerator(inEnumeratorName,inIsDefinition))).first->second;
}

CPPEnumeratorValue* CPPNamespace::CreateEnumeratorValue(CPPEnumerator* inEnumerator,const string& inEnumeratorValueName)
{
	return AppendEnumeratorValue(inEnumerator,inEnumeratorValueName,NULL);
}

CPPEnumeratorValue* CPPNamespace::AppendEnumeratorValue(CPPEnumerator* inEnumerator,const string& inEnumeratorValueName,CPPEnumeratorValue* inEnumeratorValue)
{
	// enumerator values behave like functions and variables, being non-types but rather values.
	if(HasNonTypesWithName(inEnumeratorValueName))
	{
		TRACE_LOG1("CPPNamespace::CreateEnumeratorValue, cannot create new variable. another element already exists with which a name may not be shared for %s", inEnumeratorValueName.c_str());
		return NULL;
	}
	else
		return mEnumeratorValues.insert(StringToCPPEnumeratorValueMap::value_type(inEnumeratorValueName, inEnumeratorValue ? inEnumeratorValue : inEnumerator->AddEnumeratorValue(inEnumeratorValueName))).first->second;
}

CPPVariable* CPPNamespace::CreateVariable(const string& inVariableName,
										  UsedTypeDescriptor* inTypeDescriptor)
{
	return AppendVariable(inVariableName,inTypeDescriptor,NULL);
}

CPPVariable* CPPNamespace::AppendVariable(const string& inVariableName,
										  UsedTypeDescriptor* inTypeDescriptor,
										  CPPVariable* inVariable)
{
	if(HasNonTypesWithName(inVariableName))
	{
		TRACE_LOG1("CPPNamespace::CreateVariable, cannot create new variable. another element already exists with which a name may not be shared for %s", inVariableName.c_str());
		return NULL;
	}
	else
		return mVariables.insert(StringToCPPVariableMap::value_type(inVariableName,inVariable ? inVariable : new CPPVariable(inVariableName,inTypeDescriptor))).first->second;
}

bool CPPNamespace::HasNonTypesWithName(const string& inName)
{
	CPPElementList existingElements = FindElements(inName);

	if(existingElements.size() > 0)
	{
		bool hasNonTypesWithName = false;
		CPPElementList::iterator it = existingElements.begin();

		for(; it != existingElements.end() && !hasNonTypesWithName; ++it)
			hasNonTypesWithName = 
				(*it)->Type != CPPElement::eCPPElementUnion &&
				(*it)->Type != CPPElement::eCPPElementEnumerator;
		return hasNonTypesWithName;
	}
	else
		return false;
}

CPPUnion* CPPNamespace::CreateUnion(const string& inUnionName,bool inIsDefinition)
{
	return AppendUnion(inUnionName,inIsDefinition,NULL);
}

CPPUnion* CPPNamespace::AppendUnion(const string& inUnionName,bool inIsDefinition,CPPUnion* inUnion)
{
	CPPElementList existingElements = FindElements(inUnionName);

	if(existingElements.size() > 0)
	{
		// for enumerators, the following is true:
		// other values (functions, variables) can share it's name. types cannot.
		// there may be multiple declerations but only one definition

		bool hasProblematicDefinition = false;
		CPPUnion* hadDefinedUnion = NULL;
		CPPElementList::iterator it = existingElements.begin();

		for(; it != existingElements.end(); ++it)
		{
			if((*it)->Type == CPPElement::eCPPElementUnion)
			{
				CPPUnion* otherUnion = (CPPUnion*)(*it);

				if(otherUnion->IsDefinition() && inIsDefinition)
				{
					TRACE_LOG1("CPPNamespace::CreateUnion, problem when creating a new union declaration/definition. multiple definitons exist for a union. This is not allowed. for union %s",inUnionName.c_str());
					hasProblematicDefinition = true;
					break;
				}

				if(inIsDefinition)
					otherUnion->SetIsDefinition();

				hadDefinedUnion = otherUnion;

			}
			else if (	(*it)->Type == CPPElement::eCPPElementEnumerator || 
						(*it)->Type == CPPElement::eCPPElementPrimitive  ||
						(*it)->Type == CPPElement::eCPPElementNamespace)
			{
				hasProblematicDefinition = true;
				break;
			}
		}

		if(hasProblematicDefinition)
			return NULL;

		if(hadDefinedUnion)
			return hadDefinedUnion;
	}

	return mUnions.insert(StringToCPPUnionMap::value_type(inUnionName,inUnion ? inUnion : new CPPUnion(inUnionName,inIsDefinition))).first->second;
}

EStatusCode CPPNamespace::AppendPrimitive(const string& inTypeName,CPPPrimitive* inNewType)
{
	CPPElementList existingElements = FindElements(inTypeName);

	// k. primitive never allow anything else with the same name. m'k?

	if(existingElements.size() > 0)
	{
		TRACE_LOG1("CPPNamespace::AppendPrimitive, syntax error, typename already exists for %s",
					inTypeName.c_str());
		return eFailure;
	}
	else
	{
		mPrimitives.insert(StringToCPPPrimitiveMap::value_type(inTypeName,inNewType));
		return eSuccess;
	}
}

CPPTypedef* CPPNamespace::CreateTypedef(const string& inTypedefName,
										UsedTypeDescriptor* inTypeDescriptor)
{
	return AppendTypedef(inTypedefName,inTypeDescriptor,NULL);
}

CPPTypedef* CPPNamespace::AppendTypedef(const string& inTypedefName,
										UsedTypeDescriptor* inTypeDescriptor,
										CPPTypedef* inTypedef)
{
	CPPElementList existingElements = FindElements(inTypedefName);

	if(existingElements.size() > 0)
	{
		CPPTypedef* alreadyDefined = NULL;

		EStatusCode status = eSuccess;
		CPPElementList::iterator it = existingElements.begin();

		for(; it != existingElements.end();++it)
		{
			// Typedef allows existing element with the same name if and only if
			// that other element is a typedef that is completely equal to this typedef.
			if((*it)->Type != CPPElement::eCPPElementTypedef)
			{
				TRACE_LOG1("CPPNamespace::CreateTypedef, cannot create new typedef. non-typedef element already exists for %s", inTypedefName.c_str());
				status = eFailure;
				break;
			}

			// use CreateRealTypeDescriptor to recurse and accumulate internal typedefs
			UsedTypeDescriptor* realThisTypeDecsriptor = 
				(inTypeDescriptor->GetUsedType() == UsedTypeDescriptor::eUsedTypeFunctionPointer ||
					inTypeDescriptor->GetFieldDescriptor()->GetType()->Type != CPPElement::eCPPElementTypedef) ? 
				inTypeDescriptor:
				CreateRealTypeDescriptor(inTypeDescriptor);
			UsedTypeDescriptor* otherTypeDescriptor = ((CPPTypedef*)(*it))->GetTypeDescriptor();
			UsedTypeDescriptor* realOtherTypeDescriptor = 
				(otherTypeDescriptor->GetUsedType() == UsedTypeDescriptor::eUsedTypeFunctionPointer ||
					otherTypeDescriptor->GetFieldDescriptor()->GetType()->Type != CPPElement::eCPPElementTypedef) ? 
				otherTypeDescriptor:
				CreateRealTypeDescriptor(otherTypeDescriptor);
				
			bool testEquality = realOtherTypeDescriptor->IsEqual(realThisTypeDecsriptor);
			if(realThisTypeDecsriptor != inTypeDescriptor)
				delete realThisTypeDecsriptor;
			if(realOtherTypeDescriptor != otherTypeDescriptor)
				delete realOtherTypeDescriptor;
			if(!testEquality)
			{
				TRACE_LOG1("CPPNamespace::CreateTypedef, cannot create new typedef. a typedef element already exists for %s, and it points to a different type", inTypedefName.c_str());
				status = eFailure;
				break;
			}
			else
			{
				// this means that the typedef already exists - so no need to define it again...
				alreadyDefined = (CPPTypedef*)(*it);
			}
		}
		
		if(status != eSuccess)
			return NULL;

		if(alreadyDefined)
			return alreadyDefined;
	}

	return mTypedefs.insert(StringToCPPTypedefMap::value_type(inTypedefName,inTypedef ? inTypedef : new CPPTypedef(inTypedefName,inTypeDescriptor))).first->second;
}

UsedTypeDescriptor* CPPNamespace::CreateRealTypeDescriptor(UsedTypeDescriptor* inTypeDescriptor)
{
	if(inTypeDescriptor->GetUsedType() == UsedTypeDescriptor::eUsedTypeFunctionPointer ||
		inTypeDescriptor->GetFieldDescriptor()->GetType()->Type != CPPElement::eCPPElementTypedef)
	{
		return inTypeDescriptor->Clone();
	}
	else 
	{
		// typedef :)
		CPPTypedef* aTypedef = (CPPTypedef*)inTypeDescriptor->GetFieldDescriptor()->GetType();

		// recurse to get the real type from the lower level descriptor
		UsedTypeDescriptor* recursionResult = CreateRealTypeDescriptor(aTypedef->GetTypeDescriptor());

		// now modify the recursion result based on this used type descriptor

		// Declarator modifiers (pointer/refernce) do two things:
		// 1. If there are any susbscripts of a lower level, for all relevant purposes, they become pointers. and so...i'm making them such
		// 2. In addition the new modifiers are added

		if(inTypeDescriptor->GetFieldDescriptor()->GetModifiersCount() > 0)
		{
			while(recursionResult->GetFieldDescriptor()->GetSubscriptCount() > 0)
			{
				DeclaratorModifier modifier;
				modifier.Modifier = DeclaratorModifier::eDeclaratorModifierPointer;
				modifier.IsConst = false;
				modifier.IsVolatile = false;

				recursionResult->GetFieldDescriptor()->AppendModifier(modifier);
				recursionResult->GetFieldDescriptor()->RemoveSubscript();
			}


			Hummus::SingleValueContainerIterator<DeclaratorModifierList> modifiersIt = inTypeDescriptor->GetFieldDescriptor()->GetModifiersListIterator();
			while(modifiersIt.MoveNext())
				recursionResult->GetFieldDescriptor()->AppendModifier(modifiersIt.GetItem());
		}

		// Subscripts from this level are simply added
		for(unsigned long i=0;i < inTypeDescriptor->GetFieldDescriptor()->GetSubscriptCount();++i)
			recursionResult->GetFieldDescriptor()->AddSubscript();


		return recursionResult;
	}
}


CPPFunction* CPPNamespace::CreateFunction(const string& inFunctionName,
										UsedTypeDescriptor* inReturnTypeDescriptor,
										const FunctionParameterList& inParametersList,
										bool inHasElipsis,		
										bool inIsDefinition)
{
	return AppendFunction(inFunctionName,inReturnTypeDescriptor,inParametersList,inHasElipsis,inIsDefinition,NULL);
}

CPPFunction* CPPNamespace::AppendFunction(const string& inFunctionName,
										UsedTypeDescriptor* inReturnTypeDescriptor,
										const FunctionParameterList& inParametersList,
										bool inHasElipsis,		
										bool inIsDefinition,
										CPPFunction* inFunction)
{
	CPPElementList existingElements = FindElements(inFunctionName);

	if(existingElements.size() > 0)
	{
		// for functions, the following is true:
		// other types (structs, classes, enums etc.) can share its name. anything else can't but functions
		// with functions of the same name, this is true:
		// functions that may share the name if they have a different paramters list. 
		// if they share the parameters list, than there can't be multiple definitions (so value of "inIsDefinition" for
		// the function, if you come by one). also - in that case they may not have different return value.

		bool hasProblematicDefinition = false;
		CPPFunction* hasDefinedFunction = NULL;
		CPPElementList::iterator it = existingElements.begin();

		for(; it != existingElements.end(); ++it)
		{

			if((*it)->Type == CPPElement::eCPPElementFunction)
			{
				// compare the parameter list to see if same overload
				CPPFunction* otherFunction = (CPPFunction*)(*it);

				Hummus::SingleValueContainerIterator<FunctionParameterList> otherIt = otherFunction->GetParametersListIterator();
				otherIt.MoveNext();
				FunctionParameterList::const_iterator newIt = inParametersList.begin();
				bool sameOverload = true;

				while(newIt != inParametersList.end() && !otherIt.IsFinished() && sameOverload)
				{
					sameOverload = (*newIt)->Type->IsEqual(otherIt.GetItem()->Type);
					otherIt.MoveNext();
					++newIt;
				}

				// same overload means also that the lists both ended...
				sameOverload = sameOverload & ((newIt == inParametersList.end()) == otherIt.IsFinished());
				
				if(!sameOverload)
					continue;

				// same overload. so for sure it's not a new thing. just assert the following:
				// - that they have the same value for elipsis
				// - that they have the same return value
				// - and that if this is a definition (not declaration), that there wasn't a definiton in the past...and update if there wasn't...that now there is.
				if(otherFunction->HasElipsis() != inHasElipsis)
				{
					TRACE_LOG1("CPPNamespace::CreateFunction, problem when creating a new function declaration/definition. a function already exists with ambiguity due to similar parameter list but the usage of elipsis, for function %s",inFunctionName.c_str());
					hasProblematicDefinition = true;
					break;
				}

				if(!otherFunction->GetReturnType()->IsEqual(inReturnTypeDescriptor))
				{
					TRACE_LOG1("CPPNamespace::CreateFunction, problem when creating a new function declaration/definition. a function already exists the same parameter list but a different return type, for function %s",inFunctionName.c_str());
					hasProblematicDefinition = true;
					break;
				}

				if(otherFunction->IsDefinition() && inIsDefinition)
				{
					TRACE_LOG1("CPPNamespace::CreateFunction, problem when creating a new function declaration/definition. multiple definitons exist for a function. This is not allowed. for function %s",inFunctionName.c_str());
					hasProblematicDefinition = true;
					break;
				}

				if(inIsDefinition)
					otherFunction->SetIsDefinition();

				hasDefinedFunction = otherFunction;
			}
			else if((*it)->Type != CPPElement::eCPPElementUnion &&
					(*it)->Type != CPPElement::eCPPElementEnumerator)
			{
				hasProblematicDefinition = true;
				break;
			}
		}

		if(hasProblematicDefinition)
			return NULL;

		if(hasDefinedFunction)
			return hasDefinedFunction;
	}

	StringToCPPFunctionListMap::iterator it = mFunctions.find(inFunctionName);
	if(it == mFunctions.end())
		it = mFunctions.insert(StringToCPPFunctionListMap::value_type(inFunctionName, CPPFunctionList())).first;

	CPPFunction* aFunction = inFunction ? inFunction : new CPPFunction(inFunctionName,inReturnTypeDescriptor,inParametersList,inHasElipsis,inIsDefinition);
	it->second.push_back(aFunction);
	return aFunction;
}