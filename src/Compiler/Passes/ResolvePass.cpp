#include "precomp.h"
// @(#)$Id$
//
// Evita Compiler - ResolvePass
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./ResolvePass.h"

#include "../CompileSession.h"
#include "../Ir.h"
#include "../Logging/Logger.h"
#include "../Tasks/ResolveMethodCallTask.h"

#include "../../Il/Ir/Context.h"
#include "../../Il/Ir/Instructions.h"
#include "../../Il/Ir/Operands/Namespace.h"
#include "../../Il/Ir/Operands/ValuesTypeBuilder.h"

#include "../../Il/Tasks/NormalizeTask.h"

namespace Compiler {

// TODO 2011-01-03 yosi@msn.com We should use ArrayList_ rather than Stack,
// once ArrayList_ supports automatic expansion.
using Common::Collections::HashSet_;

using Il::Ir::SelectI;
using Il::Ir::Values;
using Il::Ir::ValuesType;

using namespace Il::Tasks;

namespace {

static Register* GetThisReg(Function* const pFunction) {
    ASSERT(pFunction != nullptr);
    auto const pPrologueI = pFunction->GetPrologueI();
    foreach (Values::EnumUser, oEnum, pPrologueI->GetVd()) {
        auto const pI = oEnum.GetI();
        if (auto const pSelectI = pI->DynamicCast<SelectI>()) {
            if (pSelectI->GetIy() == 0) {
                return pSelectI->GetRd();
            } // if
        } // if
    } // for
    CAN_NOT_HAPPEN();
} // GetThisReg

} // namespace

// ctor
ResolvePass::ResolvePass(
    CompileSession* const pCompileSession) :
        CompilePass(L"ResolvePass", pCompileSession),
        m_eState(State_None),
        m_pNameMap(nullptr),
        m_pNamespaceBody(nullptr) {}

// [C]
bool ResolvePass::CanAccess(Operand* const pOperand) {
    ASSERT(pOperand != nullptr);
    // TODO 2011-04-16 yosi@msn.com NYI: ResolvePass::CanAccess
    return true;
} // CanAccess

MethodGroup* ResolvePass::ComputeMethodGroup(MethodDef* pMethodDef) {
    ASSERT(pMethodDef != nullptr);
    auto const pMethodGroupDef = pMethodDef->GetMethodGroupDef();
    if (auto const pMethodGroup = pMethodGroupDef->GetMethodGroup()) {
        this->WriteLog("Found %p", pMethodGroup);
        return pMethodGroup;
    } // if

    auto const pClass = pMethodDef->GetOwnerClassDef()->GetClass();

    auto const pMethodGroup = new MethodGroup(
        pClass,
        pMethodDef->name());

    pClass->Add(pMethodDef->name(), pMethodGroup);

    pMethodGroupDef->SetMethodGroup(pMethodGroup);

    this->WriteLog("Register %p", pMethodGroup);

    return pMethodGroup;
} // ComputeMethodGroup

/// <summary>
///     Compute method signature. Note: method signature doesn't contain
///     "this" parameter.
/// </summary>
FunctionType* ResolvePass::ComputeMethodSignature(
    MethodDef* const pMethodDef) {

    ASSERT(pMethodDef != nullptr);

    ValuesTypeBuilder oBuilder(pMethodDef->CountParams());

    foreach (MethodDef::EnumParam, oEnum, pMethodDef) {
        auto const pType = this->Resolve(oEnum.Get()->GetTy());
        oBuilder.Append(pType);
    } // for param

    return FunctionType::Intern(
        this->Resolve(pMethodDef->GetReturnType()),
        oBuilder.GetValuesType());
} // ComputeMethodSignature

// [F]
/// <summary>
///     <list>
///         <item>Set field types</item>
///     </list>
/// </summary>
void ResolvePass::FinalizeClassDef(ClassDef* const pClassDef) {
    ASSERT(pClassDef != nullptr);

    auto const pClass = pClassDef->GetClass();

    LogSection oSection(this, "Resolve fields");

    foreach (ClassDef::EnumMember, oEnum, pClassDef) {
        auto const pThing = oEnum.Get();
        if (auto const pFieldDef = pThing->DynamicCast<FieldDef>()) {
            auto const pType = this->Resolve(pFieldDef->GetTy());
            auto const pName = pFieldDef->name();
            if (pType->Is<UnresolvedType>()) {
                this->WriteLog("Can't resolve type for %p", pName);
                continue;
            } // if

            this->WriteLog("%p : %p", pName, pType);

            pClass->Add(
                pName,
                new Field(
                    pClass,
                    pFieldDef->GetModifiers(),
                    pType,
                    pName,
                    pFieldDef->GetSourceInfo()));
        } // if
    } // for
} // FinalizeClassDef

Class* ResolvePass::FixClassDef(ClassDef* const pClassDef) {
    ASSERT(pClassDef != nullptr);

    if (pClassDef->IsRealized()) {
        return pClassDef->GetClass();
    } // if

    // This ClassDef must NOT be realized.
    ASSERT(pClassDef->GetClass() == nullptr);

    if (pClassDef->IsPending()) {
        return nullptr;
    } // if

    auto const pOuterDef = pClassDef->GetOuter();
    if (auto const pOuterClassDef = pOuterDef->DynamicCast<ClassDef>()) {
        if (!pOuterClassDef->IsRealized()) {
            this->Postpone(pClassDef);
            return nullptr;
        } // if
    } // if

    DEBUG_PRINTF("process class %ls\n",
        pClassDef->name()->GetString());

    ArrayList_<Class*> oBaseClasses;

    foreach (ClassDef::EnumBaseSpec, oEnum, pClassDef) {
        auto const pType = this->Resolve(oEnum.Get());

        if (auto const pClass = pType->DynamicCast<Class>()) {
            if (pClass->CanBeBaseClass()) {
                oBaseClasses.Add(pClass);
            }

        } else if (pType->Is<UnresolvedType>()) {
            DEBUG_PRINTF("postpone %ls by %ls\n",
                pClassDef->name()->GetString(),
                pType->StaticCast<UnresolvedType>()
                    ->GetNameRef()->name()->GetString());

            this->Postpone(pClassDef);
            return nullptr;

        } else {
            this->AddError(
                pClassDef->GetSourceInfo(),
                CompileError_Resolve_Class_InvalidBase,
                pType);
        } // if
    } // for

    switch (oBaseClasses.Count()) {
    case 0:
        oBaseClasses.Add(Ty_Object);
        break;

    case 1:
        break;

    default:
        this->AddError(
            pClassDef->GetSourceInfo(),
            CompileError_Resolve_Class_MoreThanOneBase,
            oBaseClasses.Get(0),
            oBaseClasses.Get(1));
        break;
    } // switch

    auto const pBaseClass = oBaseClasses.Get(0);

    this->WriteLog("Create class %s : %p",
        pClassDef->name(),
        pBaseClass);

    DEBUG_PRINTF("Create class %ls : %ls\n",
        pClassDef->name()->GetString(),
        pBaseClass->name()->GetString());

    ArrayList_<TypeParam*> oTypeParams;

    foreach (ClassDef::EnumTypeParam, oEnum, pClassDef) {
        oTypeParams.Add(oEnum.Get()->GetTy()->StaticCast<TypeParam>());
    } // for

    auto const pClass = oTypeParams.IsEmpty()
        ? new Class(
            pOuterDef->Is<NamespaceDef>()
                ? pOuterDef->StaticCast<NamespaceDef>()->GetNamespace()
                : pOuterDef->StaticCast<ClassDef>()->GetClass(),
            pClassDef->GetModifiers(),
            pClassDef->name(),
            pBaseClass,
            pClassDef->GetSourceInfo())
        : new GenericClass(
            pOuterDef->Is<NamespaceDef>()
                ? pOuterDef->StaticCast<NamespaceDef>()->GetNamespace()
                : pOuterDef->StaticCast<ClassDef>()->GetClass(),
            pClassDef->GetModifiers(),
            pClassDef->name(),
            pBaseClass,
            &oTypeParams,
            pClassDef->GetSourceInfo());

    pClassDef->SetClass(pClass);

    // Bind TypeParamDef to TypeParam.
    if (auto const pGenericClass = pClass->DynamicCast<GenericClass>()) {
        GenericClass::EnumTypeParam oEnumTypeParam(pGenericClass);

        foreach (ClassDef::EnumTypeParam, oEnum, pClassDef) {
            auto const pTypeParamDef = oEnum.Get();
            pTypeParamDef->SetTy(oEnumTypeParam.Get());
            oEnumTypeParam.Next();
        } // for
    } // if GenericClass

    foreach (ClassDef::EnumMember, oEnum, pClassDef) {
        auto const pThing = oEnum.Get();
        if (auto const pInnerClassDef = pThing->DynamicCast<ClassDef>()) {
            pClass->Add(pThing->name(), pInnerClassDef->GetTy());
        } // if
    } // for

    {
        NameScope oScope(this, pClassDef);

        foreach (ClassDef::EnumMember, oEnum, pClassDef) {
            if (auto const pInnerClassDef =
                    oEnum.Get()->DynamicCast<ClassDef>()) {
                this->FixClassDef(pInnerClassDef);
            } // if
        } // for
    }

    return pClass;
} // FixClassDef

void ResolvePass::FixMethodSignature(MethodDef* pMethodDef) {
    ASSERT(pMethodDef != nullptr);

    Logger oLogger(this, L"FixMethodSignature", pMethodDef);

    this->Resolve(pMethodDef->GetReturnType());

    {
        LogSection oSection(this, "Resolve method parameter types");

        auto const pFunction = pMethodDef->GetFunction();

        auto const pPrologueI = pFunction->GetPrologueI();

        ValuesTypeBuilder oBuilder(
            pMethodDef->IsStatic()
                ? pMethodDef->CountParams()
                : pMethodDef->CountParams() + 1);

        if (!pMethodDef->IsStatic()) {
            oBuilder.Append(pMethodDef->GetOwnerClassDef()->GetClass());
        } // if

        foreach (MethodDef::EnumParam, oEnum, pMethodDef) {
            auto const pMethodParamDef = oEnum.Get();
            auto const pTy = this->Resolve(pMethodParamDef->GetTy());
            oBuilder.Append(pTy);
        } // for param

        this->WriteLog("Update output type of %p to %p",
            pPrologueI,
            oBuilder.GetValuesType());

        pPrologueI->SetTy(oBuilder.GetValuesType());
    }

    {
        LogSection oSection(this, "Resolve local variable types");

        foreach (MethodDef::EnumVar, oEnum, pMethodDef) {
            auto const pLocalVarDef = oEnum.Get();
            auto const pVar = pLocalVarDef->GetVariable();
            auto const pType = this->Resolve(pVar->GetTy());

            if (pType->Is<UnresolvedType>()) {
                this->WriteLog("Can't resolve type for %p", pVar);
                continue;
            } // if

            this->WriteLog("Update type of %p %p to %p",
                pVar,
                pVar->GetTy(),
                pType);

            pVar->SetTy(pType);
            pLocalVarDef->SetTy(pType);

            auto const pDefI = pVar->GetDefI();
            pDefI->SetTy(Ty_ClosedCell->Construct(pType));
            this->WriteLog("queue %p", pDefI);
        } // for var
    }

    {
        LogSection oSection(this, "Resolve upvar types");

        foreach (
            Module::EnumFunction,
            oEnum,
            pMethodDef->module()) {
            auto const pFun = oEnum.Get();
            foreach (Function::EnumUpVar, oEnum, pFun) {
                auto const pVar = oEnum.Get();
                auto const pUpVarI = oEnum.GetI();
                pUpVarI->SetTy(pVar->GetDefI()->GetTy());
                this->WriteLog("queue %p", pUpVarI);
            } // for
        } // for
    }

    oLogger.Dump(pMethodDef->module());
} // FixMethodSignature

// [I]
NamespaceDef* ResolvePass::Import(Namespace* const pNamespace) {
    ASSERT(pNamespace != nullptr);

    if (pNamespace == Namespace::Global) {
        return NamespaceDef::Global;
    } // if

    auto const pOuterDef = this->Import(pNamespace->GetOuter());

    if (auto const pPresent = pOuterDef->Find(pNamespace->name())) {
        if (auto const pNsDef = pPresent->DynamicCast<NamespaceDef>()) {
            return pNsDef;
        } // if
    } // if

    auto const pNamespaceDef = new NamespaceDef(
        pOuterDef,
        pNamespace->name());

    pOuterDef->Add(pNamespaceDef);

    return pNamespaceDef;
} // Import

// [L]
Namespace* ResolvePass::Load(NamespaceDef* const pNamespaceDef) {
    ASSERT(pNamespaceDef != nullptr);

    if (auto const pNamespace = pNamespaceDef->GetNamespace()) {
        return pNamespace;
    } // if

    auto const pOuterDef = pNamespaceDef->GetOuter();
    auto const pOuter = this->Realize(pOuterDef);
    auto const pName = pNamespaceDef->name();

    if (auto const pFound = pOuter->Find(pName)) {
        if (auto const pNamespace = pFound->DynamicCast<Namespace>()) {
            pNamespaceDef->SetNamespace(pNamespace);
            return pNamespace;
        } // if

        this->AddError(
            pNamespaceDef->GetSourceInfo(),
            CompileError_Resolve_NotNamespace,
            pFound);
        return nullptr;
    } // if

    // TODO 2011-01-03 yosi@msn.com Load namespace from external storage.

    auto const pNamespace = new Namespace(pOuter, pName);
    pOuter->Add(pName, pNamespace);

    pNamespaceDef->SetNamespace(pNamespace);

    return pNamespace;
} // Load

/// <summary>
///     <list type="number">
///         <item>Simple Name => Lookup Simple Name.</item>
///         <item>Left Most</item>
///             <list type="number">
///                 <item>NameRef is a class member.</item>
///                 <item>NameRef is a qualified name.</item>
///             </list>
///     </list>
/// </summary>
Operand* ResolvePass::Lookup(NameRef* const pNameRef) {
    ASSERT(pNameRef != nullptr);

    if (pNameRef->IsSimple()) {
        return this->LookupSimpleName(pNameRef);
    } // if

    auto const pMostOuter = pNameRef->ComputeMostOuter();

    auto pContainer = this->LookupSimpleName(pMostOuter);
    if (pContainer == nullptr) {
        pContainer = this->LookupIn(
            pMostOuter->name(),
            NamespaceDef::Global);

        if (pContainer == nullptr) {
            return nullptr;
        } // if
    } // if

    if (auto const pClass = pContainer->DynamicCast<Class>()) {
        return this->LookupClassMember(pMostOuter, pClass);
    } // if

    if (auto const pNs = pContainer->DynamicCast<Namespace>()) {
        return this->LookupNamespaceMember(
            pMostOuter,
            this->Import(pNs));
    } // if

    if (pContainer->Is<UnresolvedType>()) {
        return nullptr;
    } // if

    this->AddError(
        pNameRef->GetSourceInfo(),
        CompileError_Resolve_ExpectClass,
        pNameRef,
        pContainer);
    return nullptr;
} // Lookup

Operand* ResolvePass::LookupAlias(const Name& name) const {
    ASSERT(pName != nullptr);
    ASSERT(m_pNamespaceBody != nullptr);

    foreach (NamespaceBody::EnumOuter, oEnum, m_pNamespaceBody) {
        if (auto const pAliasDef = oEnum.Get()->FindAliasDef(pName)) {
            // Target NameRef in using aliases can't use alias defined in
            // same namespace body.
            if (State_ResolveAlias == m_eState) {
                if (m_pNamespaceBody == oEnum.Get()) {
                    continue;
                }
            } // if

            return pAliasDef->GetTarget();
        } // if
    } // for

    return nullptr;
} // LookupAlias

Operand* ResolvePass::LookupClassMember(
    NameRef* const pNameRef,
    Class* const pClass) {

    ASSERT(pClass != nullptr);
    ASSERT(pNameRef != nullptr);
    ASSERT(pNameRef->GetInner() != nullptr);
    //ASSERT(pNameRef->ComputerMostOuter()->name() == pClass->name());

    auto pClassRunner = pClass;

    foreach (NameRef::EnumInner, oEnum, pNameRef->GetInner()) {
        auto const pCurrent = oEnum.Get();

        auto const pOperand = pClassRunner->Find(pCurrent->name());
        if (pOperand == nullptr) {
            return nullptr;
        } // if

        if (!this->CanAccess(pOperand)) {
            this->AddError(
                oEnum.Get()->GetSourceInfo(),
                CompileError_Resolve_Inaccessible,
                pOperand);
        } // if

        if (pCurrent->GetInner() == nullptr) {
            return pOperand;
        } // if

        pClassRunner = pOperand->DynamicCast<Class>();
        if (pClassRunner == nullptr) {
            return nullptr;
        }
    } // for

    CAN_NOT_HAPPEN();
} // LookupClassMember

Operand* ResolvePass::LookupIn(
    const Name& name,
    NamespaceDef* const pNamespaceDef) {

    ASSERT(pName != nullptr);
    ASSERT(pNamespaceDef != nullptr);

    if (auto const pFound = pNamespaceDef->Find(pName)) {
        return pFound->GetOperand();
    } // if

    auto const pNamespace = pNamespaceDef->GetNamespace();

    if (pNamespace == nullptr) {
        return nullptr;
    } // if

    return pNamespace->Find(pName);
} // LookupIn

Operand* ResolvePass::LookupNamespaceMember(
    NameRef* const pNameRef,
    NamespaceDef* const pNamespaceDef) {

    ASSERT(pNameRef != nullptr);
    ASSERT(pNamespaceDef != nullptr);

    auto const pMostOuter = pNameRef->ComputeMostOuter();
    ASSERT(pMostOuter->name() == pNamespaceDef->name());

    NamespaceDef* pNsRunner = pNamespaceDef;

    foreach (NameRef::EnumInner, oEnum, pMostOuter) {
        auto const pCurrent = oEnum.Get();

        auto const pPresent = this->LookupIn(
            pCurrent->name(),
            pNsRunner);

        if (pPresent == nullptr) {
            return nullptr;
        } // if

        if (auto const pClass = pPresent->DynamicCast<Class>()) {
            auto const pPresent = this->LookupClassMember(
                pCurrent,
                pClass);

            if (pPresent == nullptr) {
                return nullptr;
            } // if

            auto const pNsDef = this->Import(pClass->GetNamespace());
            if (!m_pNamespaceBody->CanAccess(pNsDef)) {
                DEBUG_PRINTF("Inaccessible: %ls in namespace %ls\n",
                    pNsDef->name(),
                    m_pNamespaceBody->GetNamespaceDef()
                        ->name()->GetString());

                this->AddError(
                    pCurrent->GetSourceInfo(),
                    CompileError_Resolve_Inaccessible,
                    pCurrent);
                return nullptr;
            } // if

            return pPresent;
        } // if

        pNsRunner = pPresent->DynamicCast<NamespaceDef>();
        if (pNsRunner == nullptr) {
            this->AddError(
                pCurrent->GetSourceInfo(),
                CompileError_Resolve_ExpectNamespace,
                pCurrent,
                pPresent);
            return nullptr;
        } // if
    } // for

    return pNsRunner->GetNamespace();
} // LookupNamespaceMember

/// <summary>
///     <list type="number">
///         <item>Name in enclosed class.</item>
///         <item>Name in base classes.</item>
///         <item>Name in outer class or namespace.</item>
///         <item>Name is an alias.</item>
///         <item>Name is an using nsmaespace.</item>
///     </list>
/// </summary>
Operand* ResolvePass::LookupSimpleName(NameRef* const pNameRef) {
    ASSERT(pNameRef != nullptr);

    auto const pName = pNameRef->name();

    HashSet_<PtrKey_<Operand>> oFounds(this);
    HashSet_<PtrKey_<Operand>> oInaccsssibles(this);

    if (m_pNameMap != nullptr) {
        DEBUG_PRINTF("Lookup %ls in enclosing class is %ls\n",
            pName->GetString(),
            m_pNameMap->name()->GetString());

        this->LookupSimpleNameIn(
            pName,
            m_pNameMap,
            &oFounds,
            &oInaccsssibles);

        HashSet_<PtrKey_<Operand>>::Enum oEnum(&oFounds);
        if (!oEnum.AtEnd()) {
            auto const pFound = oEnum.Get();
            oEnum.Next();
            if (!oEnum.AtEnd()) {
                this->AddError(
                    pNameRef->GetSourceInfo(),
                    CompileError_Resolve_Ambiguous,
                    pNameRef,
                    pFound);
            } // if
            return pFound;
        } // if
    } // if

    // Alias
    if (auto const pFound = this->LookupAlias(pName)) {
        oFounds.Add(pFound);
    } // if

    // Name in using namespaces.
    foreach (NamespaceBody::EnumUsing, oEnum, m_pNamespaceBody) {
        auto const pNs = oEnum.Get()->GetNamespaceDef();
        if (auto const pFound = this->LookupIn(pName, pNs)) {
            oFounds.Add(pFound);
        } // if
    } // for

    // Using toplevel namespace
    if (auto const pFound = this->LookupIn(pName, NamespaceDef::Global)) {
        if (auto pNs = pFound->DynamicCast<NamespaceDef>()) {
            if (m_pNamespaceBody->CanAccess(pNs)) {
                oFounds.Add(pFound);

            } else {
                oInaccsssibles.Add(pFound);
            } // if

        } else {
            // We found type in global namespace.
            oFounds.Add(pFound);
        } // if
    } // for

    HashSet_<PtrKey_<Operand>>::Enum oEnumFound(&oFounds);
    HashSet_<PtrKey_<Operand>>::Enum oEnumInaccessible(&oInaccsssibles);

    if (oEnumFound.AtEnd()) {
        if (!oEnumInaccessible.AtEnd()) {
            DEBUG_PRINTF("Found inaccessible %ls\n",
                pNameRef->name()->GetString());

            this->AddError(
                pNameRef->GetSourceInfo(),
                CompileError_Resolve_Inaccessible,
                pNameRef,
                oEnumInaccessible.Get());
        } // if

        DEBUG_PRINTF("Not found %ls\n", pName->GetString());
        return nullptr;
    } // if

    auto const pFound = oEnumFound.Get();
    oEnumFound.Next();

    if (!oEnumFound.AtEnd()) {
        this->AddError(
            pNameRef->GetSourceInfo(),
            CompileError_Resolve_Ambiguous,
            pNameRef,
            pFound,
            oEnumFound.Get());
    } // if

    return pFound;
} // LookupSimpleName

void ResolvePass::LookupSimpleNameIn(
    const Name& name,
    Class* const pClass,
    HashSet_<PtrKey_<Operand>>* const pFounds,
    HashSet_<PtrKey_<Operand>>* const pInaccessibles) {

    ASSERT(pClass != nullptr);
    ASSERT(pFounds != nullptr);
    ASSERT(pInaccessibles != nullptr);
    ASSERT(pName != nullptr);

    if (auto const pThing = pClass->Find(pName)) {
        pFounds->Add(pThing);
        return;
    } // if

    // Base Class
    foreach (Class::EnumBaseSpec, oEnum, pClass) {
        auto const pClass = oEnum.Get();
        if (auto const pPresent = pClass->Find(pName)) {
            if (this->CanAccess(pPresent)) {
                pFounds->Add(pPresent);
            } else {
                pInaccessibles->Add(pPresent);
            } // if
        } // if
    } // for

    if (!pFounds->IsEmpty()) {
        return;
    } // if

    // Outer Class
    foreach (Class::EnumOuter, oEnum , pClass) {
        auto const pThing = oEnum.Get();

        if (auto const pNs = pThing->DynamicCast<Namespace>()) {
            DEBUG_PRINTF("Find in namespace %ls\n",
                pNs->name()->GetString());

            if (auto const pFound = pNs->Find(pName)) {
                DEBUG_PRINTF("Found %ls.%ls\n",
                    pNs->name()->GetString(),
                    pName->GetString());

                pFounds->Add(pFound);
                return;
            } // if

            foreach (Namespace::EnumOuter, oEnum, pNs) {
                auto const pOuter = oEnum.Get();
                if (auto const pFound = pOuter->Find(pName)) {
                    DEBUG_PRINTF("Found %ls.%ls\n",
                        pOuter->name()->GetString(),
                        pName->GetString());

                    pFounds->Add(pFound);
                    return;
                } // if
            } // for

        } else if (auto const pOuter= pThing->DynamicCast<Class>()) {
            this->LookupSimpleNameIn(
                pName,
                pOuter,
                pFounds,
                pInaccessibles);

            if (!pFounds->IsEmpty()) {
                return;
            } // if
        } // if
    } // for
} // LookupSimpleNameIn

void ResolvePass::LookupSimpleNameIn(
    const Name& name,
    ClassOrNamespaceDef* const pNameMap,
    HashSet_<PtrKey_<Operand>>* const pFounds,
    HashSet_<PtrKey_<Operand>>* const pInaccessibles) {

    ASSERT(pFounds != nullptr);
    ASSERT(pInaccessibles != nullptr);
    ASSERT(pName != nullptr);
    ASSERT(pNameMap != nullptr);

    if (auto const pClassDef = pNameMap->DynamicCast<ClassDef>()) {
        auto const pClass = pClassDef->GetClass();
        this->LookupSimpleNameIn(pName, pClass, pFounds, pInaccessibles);
        return;
    } // if

    if (auto const pThing = pNameMap->Find(pName)) {
        pFounds->Add(pThing->GetOperand());
        return;
    } // if

    if (auto const pOuter = 
            pNameMap->GetOuter()->DynamicCast<ClassOrNamespaceDef>()) {
        this->LookupSimpleNameIn(
            pName,
            pOuter,
            pFounds,
            pInaccessibles);
    } // if
} // LookupSimpleNameIn

// [P]
void ResolvePass::Postpone(ClassDef* const pClassDef) {
    ASSERT(pClassDef != nullptr);
    if (pClassDef->IsPending()) {
        return;
    } // if

    pClassDef->MarkPending();
    m_oPendingQ.Give(pClassDef);
} // Postpone

void ResolvePass::Process(AliasDef* const pAliasDef) {
    ASSERT(pAliasDef != nullptr);
    ASSERT(m_pNameMap == nullptr);

    if (m_eState != State_ResolveAlias) {
        return;
    } // if

    auto const pTargetNameRef = pAliasDef->GetTargetNameRef();

    auto const pOperand = this->Lookup(pTargetNameRef);
    if (pOperand == nullptr) {
        this->AddError(
            pAliasDef->GetSourceInfo(),
            CompileError_Resolve_NotFound,
            pTargetNameRef);
        return;
    } // if

    if (auto const pClass= pOperand->DynamicCast<Class>()) {
        pAliasDef->SetTarget(pClass);
        return;
    } // if

    if (auto const pNamespace = pOperand->DynamicCast<Namespace>()) {
        pAliasDef->SetTarget(pNamespace);
        return;
    } // if

    this->AddError(
        pAliasDef->GetSourceInfo(),
        CompileError_Resolve_ExpectClassOrNamespaceDef,
        pTargetNameRef,
        pOperand);
} // Process

/// <summary>
///     Resolves base class name and interface names.
/// </summary>
void ResolvePass::Process(ClassDef* const pClassDef) {
    ASSERT(pClassDef != nullptr);

    DEBUG_PRINTF("%ls %hs\n",
        pClassDef->name()->GetString(),
        pClassDef->IsRealized() ? "realized" : "not realized");

    switch (m_eState) {
    case State_ResolveAlias:
        return;

    case State_FixClassDef:
        this->FixClassDef(pClassDef);
        return;

    case State_FixMethodDef: {
        ASSERT(pClassDef->IsRealized());

        NameScope oScope(this, pClassDef);
        this->FinalizeClassDef(pClassDef);
        this->Base::Process(pClassDef);
        return;
    } // State_FixMethodDef

    case State_Finalize: {
        // TODO 2010-01-02 yosi@msn.com It is not Ok to enumerate NameDef
        // since partial class definitions are defined in other namespace
        // body. We violate namespace accessibility.
        ASSERT(pClassDef->IsRealized());

        NameScope oScope(this, pClassDef);
        this->Base::Process(pClassDef);
        return;
    } // State_Finalize
    } // swtich

    CAN_NOT_HAPPEN();
} // Process ClassDef

// Note: We can't get MethoDef from Function. So, we enumrate MethodDef
// contents by ourselves.
void ResolvePass::Process(MethodDef* const pMethodDef) {
    ASSERT(pMethodDef != nullptr);

    switch (m_eState) {
    case State_FixMethodDef: {
        DEBUG_PRINTF("Fix method %ls\n",
            pMethodDef->GetMethodGroupDef()->name()->GetString());

        NameScope oScope(this, pMethodDef);
        this->Realize(pMethodDef);
        this->FixMethodSignature(pMethodDef);
        break;
    } // FixMethodSignature

    case State_Finalize: {
        DEBUG_PRINTF("Finalize method %ls\n",
            pMethodDef->GetMethodGroupDef()->name()->GetString());

        auto const pModule = pMethodDef->module();

        {
            Logger oLogger(this, L"ResolveMethodGroup", pMethodDef);

            NameScope oScope(this, pMethodDef);

            foreach (Module::EnumFunction,  oEnum,  pModule) {
                auto const pFunction = oEnum.Get();
                LogSection oSection(this, "Process %p", pFunction);
                this->ResolveNameRefI(pFunction);
            } // for function

            oLogger.Dump(pModule);
        }
        break;
    } // State_Finalize

    default:
        CAN_NOT_HAPPEN();
    } // switch
} // Process MethodDef

/// <summary>
///     Remember current namespace body for resolution.
/// </summary>
void ResolvePass::Process(NamespaceBody* const pNamespaceBody) {
    ASSERT(pNamespaceBody != nullptr);
    auto const pNs = this->Realize(pNamespaceBody->GetNamespaceDef());

    foreach (NamespaceBody::EnumMember, oEnum, pNamespaceBody) {
        auto const pNameDef = oEnum.Get();
        if (auto const pClassDef = pNameDef->DynamicCast<ClassDef>()) {
            pNs->Add(pClassDef->name(), pClassDef->GetTy());
        } // if
    } // for

    m_pNamespaceBody = pNamespaceBody;
    m_pNameMap = nullptr;
    this->Base::Process(pNamespaceBody);
} // Process

// [R]
Method* ResolvePass::Realize(MethodDef* const pMethodDef) {
    ASSERT(pMethodDef != nullptr);

    auto const pFunction = pMethodDef->GetFunction();
    auto const pMethodGroup = this->ComputeMethodGroup(pMethodDef);

    ArrayList_<TypeParam*> oTypeParams;

    foreach (MethodDef::EnumTypeParam, oEnum, pMethodDef) {
        oTypeParams.Add(oEnum.Get()->GetTy()->StaticCast<TypeParam>());
    } // for

    auto const pMethod = oTypeParams.IsEmpty()
        ? new Method(
            pMethodGroup,
            pMethodDef->GetModifiers(),
            this->ComputeMethodSignature(pMethodDef),
            pMethodDef->GetSourceInfo())
        : new GenericMethod(
            pMethodGroup,
            pMethodDef->GetModifiers(),
            this->ComputeMethodSignature(pMethodDef),
            &oTypeParams,
            pMethodDef->GetSourceInfo());

    {
        auto iIndex = 0;
        foreach (MethodDef::EnumParam, oEnum, pMethodDef) {
            pMethod->SetParamName(iIndex,  oEnum.Get()->name());
            iIndex++;
        } // for param
    }

    pMethod->SetFunction(pFunction);
    pMethodDef->SetMethod(pMethod);

    pFunction->SetMethod(pMethod);

    if (pMethod->IsStatic()) {
        // Function type of static method is identical to method
        // signature, since static method doesn't take "this"
        // parameter.
        pFunction->SetFunctionType(pMethod->GetFunctionType());

    } else {
        // Compute function type of method implementation which
        // contains "this" parameter.
        ValuesTypeBuilder oBuilder(pMethodDef->CountParams() + 1);

        oBuilder.Append(pMethodGroup->GetClass());

        foreach (MethodDef::EnumParam, oEnum, pMethodDef) {
            auto const pType = this->Resolve(oEnum.Get()->GetTy());
            oBuilder.Append(pType);
        } // for param

        pFunction->SetFunctionType(
            FunctionType::Intern(
                pMethod->GetReturnType(),
                oBuilder.GetValuesType()));
    } // if

    pMethodGroup->Add(pMethod);

    return pMethod;
} // Realize

Namespace* ResolvePass::Realize(NamespaceDef* const pNamespaceDef) {
    ASSERT(pNamespaceDef != nullptr);

    if (auto const pNamespace = pNamespaceDef->GetNamespace()) {
        return pNamespace;
    } // if

    auto const pOuterDef = pNamespaceDef->GetOuter();
    auto const pOuter = this->Realize(pOuterDef);
    auto const pName = pNamespaceDef->name();

    if (auto const pFound = pOuter->Find(pName)) {
        if (auto const pNamespace = pFound->DynamicCast<Namespace>()) {
            pNamespaceDef->SetNamespace(pNamespace);
            return pNamespace;
        } // if

        this->AddError(
            pNamespaceDef->GetSourceInfo(),
            CompileError_Resolve_NotNamespace,
            pFound);
        return nullptr;
    } // if

    auto const pNamespace = new Namespace(pOuter, pName);
    pOuter->Add(pName, pNamespace);

    pNamespaceDef->SetNamespace(pNamespace);

    return pNamespace;
} // Realize

Namespace* ResolvePass::Realize(Using* const pUsing) {
    ASSERT(pUsing != nullptr);

    auto const pNamespaceDef = pUsing->GetNamespaceDef();

    if (auto const pNs = this->Load(pNamespaceDef)) {
        pUsing->SetNamespace(pNs);
        return pNs;
    } // if

    this->AddError(
        pUsing->GetSourceInfo(),
        CompileError_Resolve_Namespace_NotFound,
        pNamespaceDef);

    return nullptr;
} // Realize

Type* ResolvePass::Resolve(Type* const pType) {
    ASSERT(pType != nullptr);

    if (auto const pArrayType = pType->DynamicCast<ArrayType>()) {
        return ArrayType::Intern(
            this->Resolve(pArrayType->GetElementType()),
            pArrayType->GetRank());
    } // if ArrayType

    if (auto const pFunType = pType->DynamicCast<FunctionType>()) {
        return FunctionType::Intern(
            this->Resolve(pFunType->GetReturnType()),
            this->Resolve(pFunType->GetParamsType()));
    } // if FunctuonType

    if (auto const pPointerType = pType->DynamicCast<PointerType>()) {
        return PointerType::Intern(
            this->Resolve(pPointerType->GetPointeeType()));
    } // if PointerType

    if (auto const pValuesType = pType->DynamicCast<ValuesType>()) {
        auto oTypes = Type::List(pValuesType->Count());

        foreach (ValuesType::Enum, oEnum, pValuesType) {
            oTypes.Add(this->Resolve(oEnum.Get()));
        } // for

        return ValuesType::Intern(&oTypes);
    } // if ValuesType

    auto const pUnresolved = pType->DynamicCast<UnresolvedType>();
    if (pUnresolved == nullptr) {
        return pType;
    } // if

    if (pUnresolved->IsResolved()) {
        return pUnresolved->GetResolvedType();
    } // if

    if (auto const pClassDef = pUnresolved->GetAttachment<ClassDef>()) {
        return pClassDef->GetClass();
    } // if

    auto const pNameRef = pUnresolved->GetNameRef();

    auto const pOperand = this->Lookup(pNameRef);

    if (pOperand == nullptr) {
        #if 0
        this->AddError(
            pNameRef->GetSourceInfo(),
            CompileError_Resolve_Type_NotFound,
            pNameRef);
        #endif
        return pType;
    } // if

    auto const pType2 = pOperand->DynamicCast<Type>();
    if (pType2 == nullptr) {
        this->AddError(
            pNameRef->GetSourceInfo(),
            CompileError_Resolve_Type_NotType,
            pNameRef,
            pOperand);
        return nullptr;
    } // if

    return pType2;
} // Resolve

bool ResolvePass::ResolveNameRefI(Function* const pFunction) {
    ASSERT(pFunction != nullptr);

    NormalizeTask oNormalizeTask(this->GetSession());

    WorkList_<Instruction> oNameRefIs;

    {
        LogSection oSection(this, "Scan instructions");

        foreach (Function::EnumBBlock, oEnum, pFunction) {
            auto const pBBlock = oEnum.Get();

            foreach (BBlock::EnumI, oEnum, pBBlock) {
                auto const pI = oEnum.Get();

                if (auto pNameRefI = pI->DynamicCast<NameRefI>()) {
                    oNameRefIs.Push(pNameRefI);

                } else {
                    auto const pTy = this->Resolve(pI->GetTy());
                    if (pTy != pI->GetTy()) {
                        this->WriteLog("update output type of %p to %p",
                            pI,
                            pTy);

                        pI->SetTy(pTy);
                    } // if

                    // ctor
                    if (pI->Is<CallI>()) {
                        if (auto const pTy = pI->GetSx()->DynamicCast<Type>()) {
                            auto const pTy2 = this->Resolve(pTy);
                            pI->GetOperandBox(0)->Replace(pTy2);
                        } // if
                    } // if

                    oNormalizeTask.Add(pI);
                } // if
            } // for instruction
        } // for bblock
    }

    oNormalizeTask.Start();

    ResolveMethodCallTask oResolveMethodCallTask(
        this->GetCompileSession(),
        &oNormalizeTask);

    while (!oNameRefIs.IsEmpty()) {
        auto const pNameRefI = oNameRefIs.Pop();
        auto const pNameRef = pNameRefI->GetOperand(1)->StaticCast<NameRef>();
        auto const pFound = this->Lookup(pNameRef);

        LogSection oSection(this, "Resolve %p", pNameRefI);

        oNormalizeTask.Add(pNameRefI);

        if (pFound == nullptr) {
            DEBUG_PRINTF("NotFound: %ls\n",
                pNameRef->name()->GetString());

            this->AddError(
                pNameRefI->GetSourceInfo(),
                CompileError_Resolve_NotFound,
                pNameRef);
            continue;
        } // if

        this->WriteLog("resolve to %p", pFound);

        DEBUG_PRINTF("Found: %ls %s %p\n",
            pNameRef->name()->GetString(),
            pFound->GetKind(),
            pFound);

        if (auto const pField = pFound->DynamicCast<Field>()) {
            auto const pFieldPtrI = 
                new FieldPtrI(
                    PointerType::Intern(pField->GetFieldType()),
                    pNameRefI->GetRd(),
                    GetThisReg(pFunction),
                    pField);

            pNameRefI->GetBB()->ReplaceI(pFieldPtrI, pNameRefI);
            oNormalizeTask.Add(pFieldPtrI);

            foreach (Register::EnumUser, oEnum, pFieldPtrI->GetRd()) {
                oNormalizeTask.Add(oEnum.GetI());
            } // for
            continue;
        } // if

        if (auto const pMethodGroup = pFound->DynamicCast<MethodGroup>()) {
            auto const pRd = pNameRefI->GetRd();

            foreach (Register::EnumUser, oEnum, pRd) {
                auto const pUserI = oEnum.GetI();

                if (auto const pLoadI = pUserI->DynamicCast<LoadI>()) {
                    oNormalizeTask.Add(pLoadI);

                    WorkList_<Instruction> oUsers;
                    foreach (Register::EnumUser, oEnum, pLoadI->GetRd()) {
                        oUsers.Push(oEnum.GetI());
                    } // for

                    while (!oUsers.IsEmpty()) {
                        auto const pUserI = oUsers.Pop();
                        if (auto const pCallI =
                                pUserI->DynamicCast<CallI>()) {
                            this->WriteLog("found %p", pCallI);
                            oResolveMethodCallTask.Add(pCallI);
                            pCallI->GetOperandBox(0)->Replace(pMethodGroup);
                        } // if
                    } // while

                } else {
                    this->AddError(
                        pNameRefI->GetSourceInfo(),
                        CompileError_Resolve_BadNameRef);
                } // if
            } // for user

            continue;
        } // if

        if (pFound->Is<Namespace>()) {
            this->AddError(
                pNameRefI->GetSourceInfo(),
                CompileError_Resolve_Method_NamespaceInOperand,
                pFound);
            continue;
        } // if

        // TODO 2010-01-04 yosi@msn.com Type, Field
        CAN_NOT_HAPPEN();
    } // while

    LogSection oSection(this, "Resolve Method Call");
    oResolveMethodCallTask.Start();
    oNormalizeTask.Start();
    return oResolveMethodCallTask.IsChanged();
} // ResolveNameRefI

// [S]
void ResolvePass::Start() {
    auto const pSession = this->GetCompileSession();

    m_eState = State_ResolveAlias;
    pSession->Apply(this);

    m_eState = State_FixClassDef;
    pSession->Apply(this);

    DEBUG_PRINTF("Fix pending classes\n");
    {
        auto fFixed = false;
        do {
            Queue_<ClassDef*> oReadyQ;

            while (!m_oPendingQ.IsEmpty()) {
                auto const pClassDef = m_oPendingQ.Take();
                pClassDef->MarkReady();
                oReadyQ.Give(pClassDef);
            } // while

            fFixed = false;
            while (!oReadyQ.IsEmpty()) {
                auto const pClassDef = oReadyQ.Take();

                NameScope oScope(
                    this, 
                    pClassDef->GetOuter()->StaticCast<ClassOrNamespaceDef>());

                if (this->FixClassDef(pClassDef) != nullptr) {
                    fFixed = true;
                } // if
            } // while
        } while (fFixed);

        while (!m_oPendingQ.IsEmpty()) {
            auto const pClassDef = m_oPendingQ.Take();
            ASSERT(pClassDef->IsPending());

            this->AddError(
                pClassDef->GetSourceInfo(),
                CompileError_Resolve_Class_CyclicBase,
                pClassDef);
        } // while
    }

    if (pSession->HasError()) {
        return;
    } // if

    m_eState = State_FixMethodDef;
    pSession->Apply(this);

    if (pSession->HasError()) {
        return;
    } // if

    m_eState = State_Finalize;
    pSession->Apply(this);
} // Start

} // Compiler
