#include "AbstractCPPContainer.h"
#include "Trace.h"
#include "CPPEnumerator.h"
#include "CPPVariable.h"
#include "CPPUnion.h"
#include "CPPTypedef.h"
#include "CPPFunction.h"
#include "UsedTypeDescriptor.h"
#include "CPPElement.h"
#include "CPPClass.h"
#include "CPPStruct.h"
#include "CPPTemplateValue.h"
#include "CPPTemplateTemplateParameter.h"


using namespace Hummus;

AbstractCPPContainer::AbstractCPPContainer(void)
{
}


AbstractCPPContainer::~AbstractCPPContainer(void)
{
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

	itFunctionLists = mFunctionTemplates.begin();
	for(; itFunctionLists != mFunctionTemplates.end(); ++itFunctionLists)
	{
		CPPFunctionList::iterator itFunctions = itFunctionLists->second.begin();
		for(; itFunctions != itFunctionLists->second.end(); ++itFunctions)
			if(mAliases.find(*itFunctions) == mAliases.end())
				delete *itFunctions;

	}

	itFunctionLists = mFunctionTemplateInstances.begin();
	for(; itFunctionLists != mFunctionTemplates.end(); ++itFunctionLists)
	{
		CPPFunctionList::iterator itFunctions = itFunctionLists->second.begin();
		for(; itFunctions != itFunctionLists->second.end(); ++itFunctions)
			if(mAliases.find(*itFunctions) == mAliases.end())
				delete *itFunctions;

	}

	StringToCPPClassMap::iterator itClasses = mClasses.begin();
	for(; itClasses != mClasses.end(); ++itClasses)
		if(mAliases.find(itClasses->second) == mAliases.end())
			delete itClasses->second;

	StringToCPPClassMap::iterator itClassTemplates = mClassTemplates.begin();
	for(; itClassTemplates!= mClassTemplates.end(); ++itClassTemplates)
		if(mAliases.find(itClassTemplates->second) == mAliases.end())
			delete itClassTemplates->second;


	StringToCPPStructMap::iterator itStructs = mStructs.begin();
	for(; itStructs != mStructs.end(); ++itStructs)
		if(mAliases.find(itStructs->second) == mAliases.end())
			delete itStructs->second;

	StringToCPPStructMap::iterator itStructTemplates = mStructTemplates.begin();
	for(; itStructTemplates!= mStructTemplates.end(); ++itStructTemplates)
		if(mAliases.find(itStructTemplates->second) == mAliases.end())
			delete itStructTemplates->second;

	// Enumerator values are owned by their respective enumerators - so don't delete the map that contains them here.
}

CPPElementList AbstractCPPContainer::FindElements(const string& inElementName)
{
	CPPElementList result;

	StringToCPPEnumeratorMap::iterator itEnumerators = mEnumerators.find(inElementName);
	if(itEnumerators != mEnumerators.end())
		result.push_back(itEnumerators->second);


	StringToCPPEnumeratorValueMap::iterator itEnumValues = mEnumeratorValues.find(inElementName);
	if(itEnumValues != mEnumeratorValues.end())
		result.push_back(itEnumValues->second);


	StringToCPPVariableMap::iterator itVariables = mVariables.find(inElementName);
	if(itVariables != mVariables.end())
		result.push_back(itVariables->second);

	StringToCPPUnionMap::iterator itUnions = mUnions.find(inElementName);
	if(itUnions != mUnions.end())
		result.push_back(itUnions->second);


	StringToCPPTypedefMap::iterator itTypedefs = mTypedefs.find(inElementName);
	if(itTypedefs != mTypedefs.end())
		result.push_back(itTypedefs->second);

	StringToCPPFunctionListMap::iterator itFunctionLists = mFunctions.find(inElementName);
	if(itFunctionLists != mFunctions.end())
		result.insert(result.end(),itFunctionLists->second.begin(),itFunctionLists->second.end());

	itFunctionLists = mFunctionTemplates.find(inElementName);
	if(itFunctionLists != mFunctionTemplates.end())
		result.insert(result.end(),itFunctionLists->second.begin(),itFunctionLists->second.end());

	StringToCPPClassMap::iterator itClasses = mClasses.find(inElementName);
	if(itClasses != mClasses.end())
		result.push_back(itClasses->second);

	StringToCPPClassMap::iterator itClassTemplates = mClassTemplates.find(inElementName);
	if(itClassTemplates != mClassTemplates.end())
		result.push_back(itClassTemplates->second);


	StringToCPPStructMap::iterator itStructs = mStructs.find(inElementName);
	if(itStructs != mStructs.end())
		result.push_back(itStructs->second);


	StringToCPPStructMap::iterator itStructTemplates = mStructTemplates.find(inElementName);
	if(itStructTemplates != mStructTemplates.end())
		result.push_back(itStructTemplates->second);

	// Note that not looking for class/struct template specializations! this is important. want to fetch the main template, always. 

	return result;
}

Hummus::EStatusCode AbstractCPPContainer::DefineAlias(const string& inAlias,CPPElement* inNewElement)
{
	EStatusCode status = eSuccess;

	// The success of alias definition, and also its storage, are dependent on the type of new element
	// normally there will be an attempted definition, and either a failure in the definition itself, or
	// a failure cause the alias is already taken by an element which is not the same as this one.
	switch(inNewElement->Type)
	{
		case CPPElement::eCPPElementEnumerator:
			{
				CPPEnumerator* aliasElement = (CPPEnumerator*)inNewElement;
				CPPEnumerator* createdElement = AppendEnumerator(inAlias,aliasElement->IsDefinition(),aliasElement);
			
				if(createdElement != aliasElement)
				{
					TRACE_LOG1("AbstractCPPContainer::DefineAlias, failed to define new enumerator alias %s . an incompatible element already exists.",inAlias.c_str());
					status = eFailure;
				}
				break;		
			}
		case CPPElement::eCPPElementUnion:
			{
				CPPUnion* aliasElement = (CPPUnion*)inNewElement;
				CPPUnion* createdElement = AppendUnion(inAlias,aliasElement->IsDefinition(),aliasElement);
			
				if(createdElement != aliasElement)
				{
					TRACE_LOG1("AbstractCPPContainer::DefineAlias, failed to define new union alias %s . an incompatible element already exists.",inAlias.c_str());
					status = eFailure;
				}
				break;	
			}
		case CPPElement::eCPPElementTypedef:
			{
				CPPTypedef* aliasElement = (CPPTypedef*)inNewElement;
				CPPTypedef* createdElement = AppendTypedef(inAlias,aliasElement->GetTypeDescriptor(),aliasElement);
			
				if(createdElement != aliasElement)
				{
					TRACE_LOG1("AbstractCPPContainer::DefineAlias, failed to define new typedef alias %s . an incompatible element already exists.",inAlias.c_str());
					status = eFailure;
				}
				break;	
			}		
		case CPPElement::eCPPElementEnumeratorValue:
			{
				CPPEnumeratorValue* aliasElement = (CPPEnumeratorValue*)inNewElement;
				CPPEnumeratorValue* createdElement = AppendEnumeratorValue(NULL,inAlias,aliasElement);
			
				if(createdElement != aliasElement)
				{
					TRACE_LOG1("AbstractCPPContainer::DefineAlias, failed to define new enumerator value alias %s . an incompatible element already exists.",inAlias.c_str());
					status = eFailure;
				}
				break;	
			}				
		case CPPElement::eCPPElementVariable:
			{
				CPPVariable* aliasElement = (CPPVariable*)inNewElement;
				CPPVariable* createdElement = AppendVariable(inAlias,aliasElement->GetTypeDescriptor(),aliasElement);
			
				if(createdElement != aliasElement)
				{
					TRACE_LOG1("AbstractCPPContainer::DefineAlias, failed to define new variable alias %s . an incompatible element already exists.",inAlias.c_str());
					status = eFailure;
				}
				break;	
			}			
		case CPPElement::eCPPElementFunction:
			{
				CPPFunction* aliasElement = (CPPFunction*)inNewElement;
				CPPFunction* createdElement = aliasElement->IsFunctionTemplate() ?
													AppendFunctionTemplate(inAlias,
																	aliasElement->IsVirtual(),
																	aliasElement->IsStatic(),
																	aliasElement->GetReturnType(),
																	aliasElement->GetDeclaredParameterList(),
																	aliasElement->HasElipsis(),
																	aliasElement->IsPure(),
																	aliasElement->GetFunctionTemplateParameters(),
																	aliasElement->GetFunctionTemplateSpecializationParametrs(),
																	aliasElement->IsDefinition(),
																	aliasElement) :
													AppendFunction(	
																inAlias,
																aliasElement->IsVirtual(),
																aliasElement->IsStatic(),
																aliasElement->GetReturnType(),
																aliasElement->GetDeclaredParameterList(),
																aliasElement->HasElipsis(),
																aliasElement->IsPure(),
																aliasElement->GetFunctionTemplateSpecializationParametrs(),
																aliasElement->IsDefinition(),
																aliasElement);
			
				if(createdElement != aliasElement)
				{
					TRACE_LOG1("AbstractCPPContainer::DefineAlias, failed to define new function alias %s . an incompatible element already exists.",inAlias.c_str());
					status = eFailure;
				}
				break;	
			}	
		case CPPElement::eCPPElementClass:
			{
				CPPClass* aliasElement = (CPPClass*)inNewElement;
				CPPClass* createdElement = aliasElement->IsTemplate() ?
											AppendClassTemplate(inAlias,
																aliasElement->GetTemplateParameters(),
																aliasElement->IsDefinition(),
																aliasElement) :
					
											AppendClass(inAlias,
														aliasElement->IsDefinition(),
														aliasElement);
			
				if(createdElement != aliasElement)
				{
					TRACE_LOG1("AbstractCPPContainer::DefineAlias, failed to define new class alias %s . an incompatible element already exists.",inAlias.c_str());
					status = eFailure;
				}
				break;	

			}
		case CPPElement::eCPPElementStruct:
			{
				CPPStruct* aliasElement = (CPPStruct*)inNewElement;
				CPPStruct* createdElement = aliasElement->IsTemplate() ?
											AppendStructTemplate(inAlias,
																aliasElement->GetTemplateParameters(),
																aliasElement->IsDefinition(),
																aliasElement) :
					
											AppendStruct(inAlias,
														aliasElement->IsDefinition(),
														aliasElement);
			
				if(createdElement != aliasElement)
				{
					TRACE_LOG1("AbstractCPPContainer::DefineAlias, failed to define new struct alias %s . an incompatible element already exists.",inAlias.c_str());
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



CPPEnumerator* AbstractCPPContainer::CreateEnumerator(const string& inEnumeratorName,bool inIsDefinition)
{
	return AppendEnumerator(inEnumeratorName,inIsDefinition,NULL);
}

CPPEnumerator* AbstractCPPContainer::AppendEnumerator(const string& inEnumeratorName,bool inIsDefinition,CPPEnumerator* inEnumerator)
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
					TRACE_LOG1("AbstractCPPContainer::CreateEnumerator, problem when creating a new enumerator declaration/definition. multiple definitons exist for a enumerator. This is not allowed. for enumerator %s",inEnumeratorName.c_str());
					hasProblematicDefinition = true;
					break;
				}

				if(inIsDefinition)
					otherEnumerator->SetIsDefinition();

				hadDefinedEnumerator = otherEnumerator;

			}
			else if (	(*it)->Type <= CPPElement::eCPPTypenames ||
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

CPPEnumeratorValue* AbstractCPPContainer::CreateEnumeratorValue(CPPEnumerator* inEnumerator,const string& inEnumeratorValueName)
{
	return AppendEnumeratorValue(inEnumerator,inEnumeratorValueName,NULL);
}

CPPEnumeratorValue* AbstractCPPContainer::AppendEnumeratorValue(CPPEnumerator* inEnumerator,const string& inEnumeratorValueName,CPPEnumeratorValue* inEnumeratorValue)
{
	// enumerator values behave like functions and variables, being non-types but rather values.
	if(HasNonTypesWithName(inEnumeratorValueName))
	{
		TRACE_LOG1("AbstractCPPContainer::CreateEnumeratorValue, cannot create new variable. another element already exists with which a name may not be shared for %s", inEnumeratorValueName.c_str());
		return NULL;
	}
	else
		return mEnumeratorValues.insert(StringToCPPEnumeratorValueMap::value_type(inEnumeratorValueName, inEnumeratorValue ? inEnumeratorValue : inEnumerator->AddEnumeratorValue(inEnumeratorValueName))).first->second;
}

bool AbstractCPPContainer::HasNonTypesWithName(const string& inName)
{
	CPPElementList existingElements = FindElements(inName);

	if(existingElements.size() > 0)
	{
		bool hasNonTypesWithName = false;
		CPPElementList::iterator it = existingElements.begin();

		for(; it != existingElements.end() && !hasNonTypesWithName; ++it)
			hasNonTypesWithName = ((*it)->Type >= CPPElement::eCPPTypenames);
		return hasNonTypesWithName;
	}
	else
		return false;
}



CPPVariable* AbstractCPPContainer::CreateVariable(const string& inVariableName,
										  UsedTypeDescriptor* inTypeDescriptor)
{
	return AppendVariable(inVariableName,inTypeDescriptor,NULL);
}

CPPVariable* AbstractCPPContainer::AppendVariable(const string& inVariableName,
										  UsedTypeDescriptor* inTypeDescriptor,
										  CPPVariable* inVariable)
{
	if(HasNonTypesWithName(inVariableName))
	{
		TRACE_LOG1("AbstractCPPContainer::AppendVariable, cannot create new variable. another element already exists with which a name may not be shared for %s", inVariableName.c_str());
		return NULL;
	}
	else
		return mVariables.insert(StringToCPPVariableMap::value_type(inVariableName,inVariable ? inVariable : new CPPVariable(inVariableName,inTypeDescriptor))).first->second;
}

CPPUnion* AbstractCPPContainer::CreateUnion(const string& inUnionName,bool inIsDefinition)
{
	return AppendUnion(inUnionName,inIsDefinition,NULL);
}

CPPUnion* AbstractCPPContainer::AppendUnion(const string& inUnionName,bool inIsDefinition,CPPUnion* inUnion)
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
					TRACE_LOG1("AbstractCPPContainer::CreateUnion, problem when creating a new union declaration/definition. multiple definitons exist for a union. This is not allowed. for union %s",inUnionName.c_str());
					hasProblematicDefinition = true;
					break;
				}

				if(inIsDefinition)
					otherUnion->SetIsDefinition();

				hadDefinedUnion = otherUnion;

			}
			else if (	(*it)->Type <= CPPElement::eCPPTypenames || 
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

CPPTypedef* AbstractCPPContainer::CreateTypedef(const string& inTypedefName,
												UsedTypeDescriptor* inTypeDescriptor)
{
	return AppendTypedef(inTypedefName,inTypeDescriptor,NULL);
}

CPPTypedef* AbstractCPPContainer::AppendTypedef(const string& inTypedefName,
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
				TRACE_LOG1("AbstractCPPContainer::CreateTypedef, cannot create new typedef. non-typedef element already exists for %s", inTypedefName.c_str());
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
				TRACE_LOG1("AbstractCPPContainer::CreateTypedef, cannot create new typedef. a typedef element already exists for %s, and it points to a different type", inTypedefName.c_str());
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

UsedTypeDescriptor* AbstractCPPContainer::CreateRealTypeDescriptor(UsedTypeDescriptor* inTypeDescriptor)
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


CPPFunction* AbstractCPPContainer::CreateFunction(const string& inFunctionName,
										bool inIsVirtual,
										bool inIsStatic,
										UsedTypeDescriptor* inReturnTypeDescriptor,
										const TypedParameterList& inParametersList,
										bool inHasElipsis,
										bool inIsPure,
										const UsedTypeOrExpressionList& inTemplateSpecializationList,
										bool inIsDefinition)
{
	return AppendFunction(inFunctionName,inIsVirtual,inIsStatic,inReturnTypeDescriptor,inParametersList,inHasElipsis,inIsPure,inTemplateSpecializationList,inIsDefinition,NULL);
}

CPPFunction* AbstractCPPContainer::AppendFunction(const string& inFunctionName,
										bool inIsVirtual,
										bool inIsStatic,
										UsedTypeDescriptor* inReturnTypeDescriptor,
										const TypedParameterList& inParametersList,
										bool inHasElipsis,		
										bool inIsPure,
										const UsedTypeOrExpressionList& inTemplateAssignmentList,
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

			if((*it)->Type == CPPElement::eCPPElementFunction && 
				!((CPPFunction*)(*it))->IsFunctionTemplate() &&
				!(inTemplateAssignmentList.size() > 0))
			{
				// compare the parameter list to see if same overload
				CPPFunction* otherFunction = (CPPFunction*)(*it);

				// check if same parameter list (same overload)
				if(!IsEquivalentParametersList(inParametersList,otherFunction->GetDeclaredParameterList()))
					continue;

				// same overload. so for sure it's not a new thing. just assert the following:
				// - that they have the same value for elipsis, virtual and static
				// - that they have the same return value
				// - and that if this is a definition (not declaration), that there wasn't a definiton in the past...and update if there wasn't...that now there is.
				if(otherFunction->HasElipsis() != inHasElipsis)
				{
					TRACE_LOG1("AbstractCPPContainer::AppendFunction, problem when creating a new function declaration/definition. a function already exists with ambiguity due to similar parameter list but the usage of elipsis, for function %s",inFunctionName.c_str());
					hasProblematicDefinition = true;
					break;
				}

				if(otherFunction->IsVirtual() != inIsVirtual)
				{
					TRACE_LOG1("AbstractCPPContainer::AppendFunction, problem when creating a new function declaration/definition. a function already exists with ambiguity due difference in virtual flag, for function %s",inFunctionName.c_str());
					hasProblematicDefinition = true;
					break;
				}


				if(otherFunction->IsStatic() != inIsStatic)
				{
					TRACE_LOG1("AbstractCPPContainer::AppendFunction, problem when creating a new function declaration/definition. a function already exists with ambiguity due difference in static flag, for function %s",inFunctionName.c_str());
					hasProblematicDefinition = true;
					break;
				}


				if(otherFunction->IsPure() != inIsPure)
				{
					TRACE_LOG1("AbstractCPPContainer::AppendFunction, problem when creating a new function declaration/definition. a function already exists with ambiguity due difference in pure flag, for function %s",inFunctionName.c_str());
					hasProblematicDefinition = true;
					break;
				}

				if(!otherFunction->GetReturnType()->IsEqual(inReturnTypeDescriptor))
				{
					TRACE_LOG1("AbstractCPPContainer::AppendFunction, problem when creating a new function declaration/definition. a function already exists the same parameter list but a different return type, for function %s",inFunctionName.c_str());
					hasProblematicDefinition = true;
					break;
				}

				if(otherFunction->IsDefinition() && inIsDefinition)
				{
					TRACE_LOG1("AbstractCPPContainer::AppendFunction, problem when creating a new function declaration/definition. multiple definitons exist for a function. This is not allowed. for function %s",inFunctionName.c_str());
					hasProblematicDefinition = true;
					break;
				}

				if(inIsDefinition)
					otherFunction->SetIsDefinition();

				hasDefinedFunction = otherFunction;
			}
			else if((*it)->Type == CPPElement::eCPPElementFunction)
			{
				// allow functions, functions instances and function template to share names
				continue;
			}
			else if((*it)->Type >= CPPElement::eCPPTypenames)
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

	if(inTemplateAssignmentList.size() > 0)
	{
		StringToCPPFunctionListMap::iterator it = mFunctionTemplateInstances.find(inFunctionName);
		if(it == mFunctions.end())
			it = mFunctions.insert(StringToCPPFunctionListMap::value_type(inFunctionName, CPPFunctionList())).first;

		CPPFunction* aFunction = inFunction ? 
								inFunction : 
								new CPPFunction(inFunctionName,
												inIsStatic,
												inIsVirtual,
												inReturnTypeDescriptor,
												inParametersList,
												inHasElipsis,
												inIsPure,
												inIsDefinition,
												inTemplateAssignmentList);
		it->second.push_back(aFunction);
		return aFunction;
	}
	else
	{
		StringToCPPFunctionListMap::iterator it = mFunctions.find(inFunctionName);
		if(it == mFunctions.end())
			it = mFunctions.insert(StringToCPPFunctionListMap::value_type(inFunctionName, CPPFunctionList())).first;


		CPPFunction* aFunction = inFunction ? 
								inFunction : 
								new CPPFunction(inFunctionName,
												inIsStatic,
												inIsVirtual,
												inReturnTypeDescriptor,
												inParametersList,
												inHasElipsis,
												inIsPure,
												inIsDefinition);
		it->second.push_back(aFunction);
		return aFunction;
	}
}

bool AbstractCPPContainer::IsEquivalentParametersList(const TypedParameterList& inParametersListLeft,const TypedParameterList& inParametersListRight)
{
	if(inParametersListLeft.size() != inParametersListRight.size())
		return false;

	bool sameOverload = true;

	TypedParameterList::const_iterator itLeft = inParametersListLeft.begin();
	TypedParameterList::const_iterator itRight = inParametersListRight.begin();

	for(; itLeft != inParametersListLeft.end() && sameOverload; ++itLeft,++itRight)
		sameOverload = (*itLeft)->Type->IsEqual((*itRight)->Type);

	return sameOverload;
}

CPPFunction* AbstractCPPContainer::CreateFunctionTemplate(
														const string& inFunctionName,
														bool inIsVirtual,
														bool inIsStatic,
														UsedTypeDescriptor* inReturnTypeDescriptor,
														const TypedParameterList& inParametersList,
														bool inHasElipsis,
														bool inIsPure,
														const CPPElementList& inTemplateParameters,
														const UsedTypeOrExpressionList& inTemplateParametersSpecialization,
														bool inIsDefinition)
{
	return AppendFunctionTemplate(inFunctionName,inIsVirtual,inIsStatic,inReturnTypeDescriptor,inParametersList,inHasElipsis,inIsPure,inTemplateParameters,inTemplateParametersSpecialization,inIsDefinition,NULL);
}

CPPFunction* AbstractCPPContainer::AppendFunctionTemplate(const string& inFunctionName,
										bool inIsVirtual,
										bool inIsStatic,											
										UsedTypeDescriptor* inReturnTypeDescriptor,
										const TypedParameterList& inParametersList,
										bool inHasElipsis,		
										bool inIsPure,
										const CPPElementList& inTemplateParameters,
										const UsedTypeOrExpressionList& inTemplateParametersSpecialization,
										bool inIsDefinition,
										CPPFunction* inFunctionTemplate)
{
	CPPElementList existingElements = FindElements(inFunctionName);

	if(existingElements.size() > 0)
	{

		// function templates are much like functions, only they also have the template parameters to check, according to the rules of specialization

		bool hasProblematicDefinition = false;
		CPPFunction* hasDefinedFunction = NULL;
		CPPElementList::iterator it = existingElements.begin();

		for(; it != existingElements.end(); ++it)
		{

			if((*it)->Type == CPPElement::eCPPElementFunction && 
				((CPPFunction*)(*it))->IsFunctionTemplate() && 
				((CPPFunction*)(*it))->GetFunctionTemplateSpecializationParametrs().size() == 0)
			{
				// compare the parameter list to see if same overload
				CPPFunction* otherFunction = (CPPFunction*)(*it);

				if(!IsEquivalentParametersList(inParametersList,inTemplateParameters,otherFunction->GetDeclaredParameterList(),otherFunction->GetFunctionTemplateParameters()))
					continue;

				// same overload. so for sure it's not a new thing. just assert the following:
				// - that they have the same value for elipsis, virtual and static
				// - that they have the same return value
				// - and that if this is a definition (not declaration), that there wasn't a definiton in the past...and update if there wasn't...that now there is.
				if(otherFunction->HasElipsis() != inHasElipsis)
				{
					TRACE_LOG1("AbstractCPPContainer::AppendFunctionTemplate, problem when creating a new function declaration/definition. a function already exists with ambiguity due to similar parameter list but the usage of elipsis, for function %s",inFunctionName.c_str());
					hasProblematicDefinition = true;
					break;
				}

				if(otherFunction->IsVirtual() != inIsVirtual)
				{
					TRACE_LOG1("AbstractCPPContainer::AppendFunctionTemplate, problem when creating a new function declaration/definition. a function already exists with ambiguity due difference in virtual flag, for function %s",inFunctionName.c_str());
					hasProblematicDefinition = true;
					break;
				}


				if(otherFunction->IsStatic() != inIsStatic)
				{
					TRACE_LOG1("AbstractCPPContainer::AppendFunctionTemplate, problem when creating a new function declaration/definition. a function already exists with ambiguity due difference in static flag, for function %s",inFunctionName.c_str());
					hasProblematicDefinition = true;
					break;
				}


				if(otherFunction->IsPure() != inIsPure)
				{
					TRACE_LOG1("AbstractCPPContainer::AppendFunctionTemplate, problem when creating a new function declaration/definition. a function already exists with ambiguity due difference in pure flag, for function %s",inFunctionName.c_str());
					hasProblematicDefinition = true;
					break;
				}

				if(!otherFunction->GetReturnType()->IsEqual(inReturnTypeDescriptor))
				{
					TRACE_LOG1("AbstractCPPContainer::AppendFunctionTemplate, problem when creating a new function declaration/definition. a function already exists the same parameter list but a different return type, for function %s",inFunctionName.c_str());
					hasProblematicDefinition = true;
					break;
				}

				if(otherFunction->IsDefinition() && inIsDefinition)
				{
					TRACE_LOG1("AbstractCPPContainer::AppendFunctionTemplate, problem when creating a new function declaration/definition. multiple definitons exist for a function. This is not allowed. for function %s",inFunctionName.c_str());
					hasProblematicDefinition = true;
					break;
				}

				if(inIsDefinition)
					otherFunction->SetIsDefinition();

				hasDefinedFunction = otherFunction;
			}
			else if((*it)->Type == CPPElement::eCPPElementFunction)
			{
				continue;
			}
			else if((*it)->Type >= CPPElement::eCPPTypenames)
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

	StringToCPPFunctionListMap::iterator it = mFunctionTemplates.find(inFunctionName);
	if(it == mFunctionTemplates.end())
		it = mFunctionTemplates.insert(StringToCPPFunctionListMap::value_type(inFunctionName, CPPFunctionList())).first;

	CPPFunction* aFunction = inFunctionTemplate ? 
							inFunctionTemplate : 
							(inTemplateParametersSpecialization.size() > 0 ?
								new CPPFunction(inFunctionName,
											inIsStatic,
											inIsVirtual,
											inReturnTypeDescriptor,
											inParametersList,
											inHasElipsis,
											inIsPure,
											inIsDefinition,
											inTemplateParameters) :
								new CPPFunction(inFunctionName,
											inIsStatic,
											inIsVirtual,
											inReturnTypeDescriptor,
											inParametersList,
											inHasElipsis,
											inIsPure,
											inIsDefinition,
											inTemplateParameters,
											inTemplateParametersSpecialization)
								);
	it->second.push_back(aFunction);
	return aFunction;
}


bool AbstractCPPContainer::IsEquivalentParametersList(const TypedParameterList& inParametersListLeft,
										const CPPElementList& inTemplateParametersListLeft,
										const TypedParameterList& inParametersListRight,
										const CPPElementList& inTemplateParametersListRight)
{
	// to check template override parameters equivalency, first check that the template parameter list are equivalent.
	// then check parameters list. when checking parameters list note that any parameter that relised on a template parameter should
	// be checked for equivalency, rather than equality. equivalency here is simply a matter of having the same template parameter type and order in the
	// template parameters list

	// check template parameters first
	if(!IsEquivalentTemplateParametersList(inTemplateParametersListLeft,inTemplateParametersListRight))
		return false;

	// having checked that the template parameters list is equivalent, just verify that the template
	// parameters are equivalent. this uses the regular parameters equivalence check, which can also handle template parameters
	return IsEquivalentParametersList(inParametersListLeft,inParametersListRight);
}

bool AbstractCPPContainer::IsEquivalentTemplateParametersList(const CPPElementList& inTemplateParametersListLeft,const CPPElementList& inTemplateParametersListRight)
{
		if(inTemplateParametersListLeft.size() != inTemplateParametersListRight.size())
		return false;


	bool sameTemplate = true;

	CPPElementList::const_iterator itTemplateLeft = inTemplateParametersListLeft.begin();
	CPPElementList::const_iterator itTemplateRight = inTemplateParametersListLeft.begin();

	for(; itTemplateLeft != inTemplateParametersListLeft.end() && sameTemplate; ++itTemplateLeft,++itTemplateRight)
	{
		// verify equality according to type
		if((*itTemplateLeft)->Type != (*itTemplateRight)->Type)
		{
			sameTemplate = false;
			break;
		}

		// for values, check type equivalence, for templates, check their template parameters
		// regular typename/class are always equivalent
		if((*itTemplateLeft)->Type == CPPElement::eCPPElementTemplateValue)
		{
			CPPTemplateValue* templateValueLeft = (CPPTemplateValue*)(*itTemplateLeft);
			CPPTemplateValue* templateValueRight = (CPPTemplateValue*)(*itTemplateRight);

			sameTemplate = templateValueLeft->GetTypeOfValue()->IsEqual(templateValueRight->GetTypeOfValue());
		}
		else if((*itTemplateLeft)->Type == CPPElement::eCPPElemeentTemplateTemplateParameter)
		{
			// for template parameter, compare the template parameters list
			CPPTemplateTemplateParameter* templateLeft = (CPPTemplateTemplateParameter*)(*itTemplateLeft);
			CPPTemplateTemplateParameter* templateRight = (CPPTemplateTemplateParameter*)(*itTemplateRight);

			sameTemplate = IsEquivalentTemplateParametersList(templateLeft->GetTemplateParameters(),templateRight->GetTemplateParameters());
		}// else it will be a typename/class which is automatically equivalent

	}

	return sameTemplate;
}

CPPClass* AbstractCPPContainer::CreateClass(const string& inClassName,
											bool inIsDefinition)
{
	return AppendClass(inClassName,inIsDefinition,NULL);
}

CPPClass* AbstractCPPContainer::AppendClass(  const string& inClassName,
											  bool inIsDefinition,
											  CPPClass* inClass)
{
	CPPElementList existingElements = FindElements(inClassName);

	if(existingElements.size() > 0)
	{

		bool hasProblematicDefinition = false;
		CPPClass* hadDefinedClass = NULL;
		CPPElementList::iterator it = existingElements.begin();

		for(; it != existingElements.end(); ++it)
		{
			if((*it)->Type == CPPElement::eCPPElementClass && !((CPPClass*)(*it))->IsTemplate())
			{
				CPPClass* otherClass = (CPPClass*)(*it);

				if(otherClass->IsDefinition() && inIsDefinition)
				{
					TRACE_LOG1("AbstractCPPContainer::AppendClass, problem when creating a new class declaration/definition. multiple definitons exist for a class. This is not allowed. for class %s",inClassName.c_str());
					hasProblematicDefinition = true;
					break;
				}

				if(inIsDefinition)
					otherClass->SetIsDefinition();

				hadDefinedClass = otherClass;

			}
			else if (	(*it)->Type <= CPPElement::eCPPTypenames || 
						(*it)->Type == CPPElement::eCPPElementPrimitive  ||
						(*it)->Type == CPPElement::eCPPElementNamespace)
			{
				hasProblematicDefinition = true;
				break;
			}
		}

		if(hasProblematicDefinition)
			return NULL;

		if(hadDefinedClass)
			return hadDefinedClass;
	}

	return mClasses.insert(StringToCPPClassMap::value_type(inClassName,inClass ? inClass : new CPPClass(inClassName,inIsDefinition))).first->second;
}

CPPClass* AbstractCPPContainer::CreateClassTemplate(
								const string& inClassName,
								const CPPElementList& inTemplateParameters,
								bool inIsDefinition)
{
	return AppendClassTemplate(inClassName,inTemplateParameters,inIsDefinition,NULL);
}

CPPClass* AbstractCPPContainer::AppendClassTemplate(
									const string& inClassName,
									const CPPElementList& inTemplateParameters,
									bool inIsDefinition,
									CPPClass* inClass)
{
	CPPElementList existingElements = FindElements(inClassName);

	if(existingElements.size() > 0)
	{

		bool hasProblematicDefinition = false;
		CPPClass* hadDefinedClass = NULL;
		CPPElementList::iterator it = existingElements.begin();

		for(; it != existingElements.end(); ++it)
		{
			if((*it)->Type == CPPElement::eCPPElementClass && ((CPPClass*)(*it))->IsTemplate())
			{
				CPPClass* otherClass = (CPPClass*)(*it);

				if(otherClass->IsDefinition() && inIsDefinition)
				{
					TRACE_LOG1("AbstractCPPContainer::AppendClassTemplate, problem when creating a new class declaration/definition. multiple definitons exist for a class. This is not allowed. for class %s",inClassName.c_str());
					hasProblematicDefinition = true;
					break;
				}

				if(inIsDefinition)
					otherClass->SetIsDefinition();

				hadDefinedClass = otherClass;

			}
			else if (	(*it)->Type <= CPPElement::eCPPTypenames || 
						(*it)->Type == CPPElement::eCPPElementPrimitive  ||
						(*it)->Type == CPPElement::eCPPElementNamespace)
			{
				hasProblematicDefinition = true;
				break;
			}
		}

		if(hasProblematicDefinition)
			return NULL;

		if(hadDefinedClass)
			return hadDefinedClass;
	}

	return mClassTemplates.insert(StringToCPPClassMap::value_type(inClassName,inClass ? 
									inClass:new CPPClass(inClassName,inTemplateParameters,inIsDefinition))).first->second;

}


CPPStruct* AbstractCPPContainer::CreateStruct(const string& inStructName,
											bool inIsDefinition)
{
	return AppendStruct(inStructName,inIsDefinition,NULL);
}

CPPStruct* AbstractCPPContainer::AppendStruct( const string& inStructName,
											  bool inIsDefinition,
											  CPPStruct* inStruct)
{
	CPPElementList existingElements = FindElements(inStructName);

	if(existingElements.size() > 0)
	{

		bool hasProblematicDefinition = false;
		CPPStruct* hadDefinedStruct = NULL;
		CPPElementList::iterator it = existingElements.begin();

		for(; it != existingElements.end(); ++it)
		{
			if((*it)->Type == CPPElement::eCPPElementStruct && !((CPPStruct*)(*it))->IsTemplate())
			{
				CPPStruct* otherStruct = (CPPStruct*)(*it);

				if(otherStruct->IsDefinition() && inIsDefinition)
				{
					TRACE_LOG1("AbstractCPPContainer::AppendStruct, problem when creating a new Struct declaration/definition. multiple definitons exist for a Struct. This is not allowed. for Struct %s",inStructName.c_str());
					hasProblematicDefinition = true;
					break;
				}

				if(inIsDefinition)
					otherStruct->SetIsDefinition();

				hadDefinedStruct = otherStruct;

			}
			else if (	(*it)->Type <= CPPElement::eCPPTypenames || 
						(*it)->Type == CPPElement::eCPPElementPrimitive  ||
						(*it)->Type == CPPElement::eCPPElementNamespace)
			{
				hasProblematicDefinition = true;
				break;
			}
		}

		if(hasProblematicDefinition)
			return NULL;

		if(hadDefinedStruct)
			return hadDefinedStruct;
	}

	return mStructs.insert(StringToCPPStructMap::value_type(inStructName,inStruct ? inStruct : new CPPStruct(inStructName,inIsDefinition))).first->second;
}

CPPStruct* AbstractCPPContainer::CreateStructTemplate(
								const string& inStructName,
								const CPPElementList& inTemplateParameters,
								bool inIsDefinition)
{
	return AppendStructTemplate(inStructName,inTemplateParameters,inIsDefinition,NULL);
}

CPPStruct* AbstractCPPContainer::AppendStructTemplate(  
	  										    const string& inStructName,
												const CPPElementList& inTemplateParameters,
												bool inIsDefinition,
												CPPStruct* inStruct)
{
	CPPElementList existingElements = FindElements(inStructName);

	if(existingElements.size() > 0)
	{

		bool hasProblematicDefinition = false;
		CPPStruct* hadDefinedStruct = NULL;
		CPPElementList::iterator it = existingElements.begin();

		for(; it != existingElements.end(); ++it)
		{
			if((*it)->Type == CPPElement::eCPPElementStruct && ((CPPStruct*)(*it))->IsTemplate())
			{
				CPPStruct* otherStruct = (CPPStruct*)(*it);

				if(otherStruct->IsDefinition() && inIsDefinition)
				{
					TRACE_LOG1("AbstractCPPContainer::AppendStructTemplate, problem when creating a new Struct declaration/definition. multiple definitons exist for a Struct. This is not allowed. for Struct %s",inStructName.c_str());
					hasProblematicDefinition = true;
					break;
				}

				if(inIsDefinition)
					otherStruct->SetIsDefinition();

				hadDefinedStruct = otherStruct;

			}
			else if (	(*it)->Type <= CPPElement::eCPPTypenames || 
						(*it)->Type == CPPElement::eCPPElementPrimitive  ||
						(*it)->Type == CPPElement::eCPPElementNamespace)
			{
				hasProblematicDefinition = true;
				break;
			}
		}

		if(hasProblematicDefinition)
			return NULL;

		if(hadDefinedStruct)
			return hadDefinedStruct;
	}

	return mStructTemplates.insert(StringToCPPStructMap::value_type(inStructName,inStruct ? 
								inStruct:new CPPStruct(inStructName,inTemplateParameters,inIsDefinition))).first->second;

}