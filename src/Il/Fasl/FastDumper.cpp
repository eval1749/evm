#include "precomp.h"
// @(#)$Id$
//
// Evita Il - Fasl - Fast Dumper
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./FastDumper.h"

#include "./Name64.h"
#include "./TypeSorter.h"

#include "../Ir.h"
#include "../../Common/Io/Stream.h"

namespace Il {
namespace Fasl {

using Common::Io::Stream;

namespace {

class RefWriter : public Functor {
    private: FastDumper* const m_pFastDumper;

    public: RefWriter(FastDumper* const pFastDumper) :
        m_pFastDumper(pFastDumper) {}

    public: virtual void Process(ArrayType* const pArrayType) override {
        m_pFastDumper->Write(FaslOp_ArrayType);
        m_pFastDumper->WriteRef(pArrayType->element_type());
        m_pFastDumper->WriteV4(pArrayType->rank());
    } // Process ArrayType

    public: virtual void Process(Class* const pClass) override {
        m_pFastDumper->Write(FaslOp_ClassRef);
        m_pFastDumper->WriteRef(pClass->GetOuter());
        m_pFastDumper->Write(pClass->name());
    } // Process Class

    public: virtual void Process(ConstructedClass* const pClass) {
        ASSERT(pClass != nullptr);

        m_pFastDumper->Write(FaslOp_ConstructedClass);
        m_pFastDumper->WriteModifiers(pClass->GetModifiers());
        m_pFastDumper->WriteRef(pClass->GetGenericClass());

        foreach (ConstructedClass::EnumTypeArg, oEnum, pClass) {
            auto const oEntry = oEnum.Get();
            TypeParam* const pTypeParam = oEntry.GetKey();
            m_pFastDumper->Write(pTypeParam->name());
            m_pFastDumper->WriteRef(oEntry.GetValue());
        } // for type arg

        m_pFastDumper->Write(FaslOp_Zero);
    } // Process ConstructedClass

    public: virtual void Process(Field* const pField) {
        ASSERT(pField != nullptr);
        m_pFastDumper->WriteRef(FaslOp_FieldRef);
        m_pFastDumper->WriteRef(pField->owner_class());
        m_pFastDumper->Write(pField->name());
    } // Process Field

    public: virtual void Process(Function* const pFunction) {
        ASSERT(pFunction != nullptr);
        m_pFastDumper->Write(FaslOp_FunctionRef);

        if (auto const pMethod = pFunction->GetMethod()) {
            ASSERT(pFunction->GetOuter() == nullptr);
            m_pFastDumper->WriteRef(pMethod);

        } else {
            m_pFastDumper->WriteRef(pFunction->GetOuter());
        } // if

        m_pFastDumper->Write(&pFunction->name());
        m_pFastDumper->WriteV4(pFunction->GetFlavor());
    } // Process Function

    public: virtual void Process(FunctionType* const pFunctionType) {
        ASSERT(pFunctionType != nullptr);
        m_pFastDumper->Write(FaslOp_FunctionType);
        m_pFastDumper->WriteRef(pFunctionType->return_type());
        m_pFastDumper->WriteRef(pFunctionType->params_type());
    } // Process FunctionType

    public: virtual void Process(Label* const pLabel) {
        ASSERT(pLabel != nullptr);
        m_pFastDumper->Write(FaslOp_Label);
        m_pFastDumper->WriteV4(pLabel->GetBB()->name());
    } // Process Label

    public: virtual void Process(Literal* const pLiteral) {
        ASSERT(pLiteral != nullptr);

        if (auto const pChar = pLiteral->GetChar()) {
            m_pFastDumper->Write(FaslOp_Char);
            m_pFastDumper->WriteV8(pChar->GetCode());
            return;
        } // if Char

        if (pLiteral->GetTy() == Ty_String) {
            auto str = pLiteral->GetString();
            m_pFastDumper->Write(FaslOp_String);
            m_pFastDumper->WriteV8(static_cast<int>(str.length()));
            foreach (String::EnumChar, oEnum, str) {
                m_pFastDumper->WriteV8(oEnum.Get());
            } // for char
            return;
        } // if String

        auto const pType = pLiteral->GetTy();

        if (pType->IsInt()) {
            auto const i64 = pLiteral->GetInt64();

            if (pType != Ty_Int32) {
                m_pFastDumper->Write(FaslOp_Literal);
                m_pFastDumper->WriteRef(pType);
            } // if

            if (i64 >= 0) {
                m_pFastDumper->Write(FaslOp_PosInt);
                m_pFastDumper->WriteV8(i64);
            } else {
                m_pFastDumper->Write(FaslOp_NegInt);
                m_pFastDumper->WriteV8(-i64);
            } // if
            return;
        } // if Int32

        // TODO 2011-01-16 yosi@msn.com Write Literal
        CAN_NOT_HAPPEN();
    } // Process Literal

    public: virtual void Process(Method* const pMethod) {
        ASSERT(pMethod != nullptr);

        DEBUG_PRINTF("MethodRef %ls.%ls\n",
            pMethod->owner_class().name()->GetString(),
            pMethod->name()->GetString());

        m_pFastDumper->Write(FaslOp_MethodRef);
        m_pFastDumper->WriteRef(pMethod->owner_class());
        m_pFastDumper->WriteRef(pMethod->function_type());
        m_pFastDumper->Write(pMethod->name());
    } // Process Method

    public: virtual void Process(MethodGroup* const pMethodGroup) {
        ASSERT(pMethodGroup != nullptr);
        m_pFastDumper->Write(FaslOp_MethodGroupRef);
        m_pFastDumper->WriteRef(pMethodGroup->owner_class());
        m_pFastDumper->Write(pMethodGroup->name());
    } // Process MethodGroup

    public: virtual void Process(Namespace* const pNamespace) {
        ASSERT(pNamespace != Namespace::Global);
        m_pFastDumper->Write(FaslOp_NamespaceRef);
        m_pFastDumper->WriteRef(pNamespace->GetOuter());
        m_pFastDumper->Write(pNamespace->name());
    } // Process Namespace

#if 0
    public: virtual void Process(NullOperand* const pNullOperand) {
        ASSERT(pNullOperand != nullptr);
        m_pFastDumper->Write(FaslOp_Null);
    } // Process NullOperand
#endif

    public: virtual void Process(Operand* const pOperand) {
        ASSERT(pOperand != nullptr);
        DEBUG_PRINTF("NYI: %hs\n", pOperand->GetKind());
        CAN_NOT_HAPPEN();
    } // Process Operand

    public: virtual void Process(PointerType* const pPointerType) {
        ASSERT(pPointerType != nullptr);
        m_pFastDumper->Write(FaslOp_PointerType);
        m_pFastDumper->WriteRef(pPointerType->pointee_type());
    } // Process PointerType

    public: virtual void Process(SsaOutput* const pOutput) {
        ASSERT(pOutput != nullptr);
        m_pFastDumper->WriteOutput(pOutput);
    } // Process SsaOutput

    public: virtual void Process(TypeParam* const pTypeParam) {
        ASSERT(pTypeParam != nullptr);
        CAN_NOT_HAPPEN();
    } // Process TypeParam

    public: virtual void Process(ValuesType* const pValuesType) {
        m_pFastDumper->Write(FaslOp_ValuesType);
        foreach (ValuesType::Enum, oEnum, pValuesType) {
            m_pFastDumper->WriteRef(oEnum.Get());
        } // for type
        m_pFastDumper->Write(FaslOp_Zero);
    } // Process ValuesType
}; // RefWriter

class Static {
  public: static const char16* ComputeOperandName(
    const Operand* const pOperand) {
    ASSERT(pOperand != nullptr);

    if (Class* pClass = pOperand->DynamicCast<Class>()) {
        return pClass->name()->GetString();
    } // if

    if (Method* pMethod = pOperand->DynamicCast<Method>()) {
        return pMethod->name()->GetString();
    } // if

    if (MethodGroup* pMethodGroup =
            pOperand->DynamicCast<MethodGroup>()) {
        return pMethodGroup->name().GetString();
    } // if

    return L"";
  } // ComputeOprandName

  public: static bool IsFixedOperands(const Instruction* const pI) {
    switch (pI->GetOp()) {
      case Op_Call:
      case Op_Phi:
      case Op_Values:
        return false;
    } // switch op
    return true;
  } // IsFixedOperands

  public: static bool IsName64(const Name* pName) {
    for (auto pwsz = pName->GetString(); 0 != *pwsz; pwsz++) {
      if (Name64::FromChar(*pwsz) == 0) {
        return false;
      }
    } // for
    return true;
  } // IsName64
}; // Static

static const char* const k_rgpszFaslOp[] = {
    #define DEFOP(mp_name) # mp_name,
    #include "./FaslOp.inc"
}; // k_rgpszFaslOp

} // namespace

FastDumper::FastDumper(Session& session, Stream& stream)
    : Tasklet(L"FastDumper", &session),
      m_iNameId(0),
      m_iOperandId(0),
      m_oNameMap(zone_),
      m_oOperandMap(zone_),
      bit_writer_(stream),
      m_pContextOperand(nullptr),
      m_pSourceInfo(nullptr) {
  Remember(Ty_Bool);
  Remember(Ty_Int32);
  Remember(Ty_Object);
  Remember(Ty_String);
  Remember(Ty_Void);
  Remember(Void);
  Write(Name::Intern(FileMagic));
} // FastDumper

FastDumper::~FastDumper() {
  bit_writer_.Flush();
} // ~FastDumper

// [D]
// Class
//  Name
//  Modifiers
//  Outer
//  BaseClass
//  Interace*
//  SourceInfo?
void FastDumper::DumpClass(FaslOp const eOpCode, const Class& clazz) {
  ASSERT(m_oOperandMap.Get(&clazz) == 0);

  Write(eOpCode);
  Write(clazz.name());

  if (clazz.GetOuter() == Namespace::Global) {
    Write(FaslOp_Zero);
  } else {
    WriteRef(clazz.GetOuter());
  } // if

  WriteModifiers(clazz.GetModifiers());

  if (auto const pBaseClass = clazz.GetBaseClass()) {
    WriteRef(pBaseClass);
  } else {
    Write(FaslOp_Zero);
  } // if

  foreach (Class::EnumInterface, oEnum, clazz) {
    WriteRef(oEnum.Get());
  } // for interface

  Write(FaslOp_Zero);

  Remember(&clazz);
} // Dumclazz

void FastDumper::DumpGenericClass(const GenericClass& clazz) {
  foreach (GenericClass::EnumTypeParam, oEnum, clazz) {
    auto const pTypeParam = oEnum.Get();
    WriteDef(FaslOp_TypeParam, pTypeParam);
    Write(pTypeParam->name());
    Write(FaslOp_Zero);
    Remember(pTypeParam);
  } // for type arg

  DumpClass(FaslOp_GenericClass, clazz);

  foreach (GenericClass::EnumTypeParam, oEnum, clazz) {
    auto const pTypeParam = oEnum.Get();
    WriteRef(pTypeParam);
  } // for type arg

  Write(FaslOp_Zero);
} // DumpGenericClass

void FastDumper::DumpTypes(const Type::Set& type_set) {
  ScopedPtr_<Type::Array> types(TypeSorter::Sort(type_set));

  // Write type.
  foreach (Type::Array::Enum, tys, types) {
    auto const type = *tys;

    if (auto const p = type->DynamicCast<GenericClass>()) {
      DumpGenericClass(*p);

    } else if (auto const p = type->DynamicCast<Class>()) {
      DumpClass(FaslOp_Class, *p);

    } else {
      WriteRef(type);
    } // if
  } // for type

  // Write field
  foreach (Type::Array::Enum, tys, types) {
    auto const clazz = tys.Get()->DynamicCast<Class>();
    if (clazz == nullptr) {
      continue;
    } // if

    {
      Class::EnumField oEnum(clazz);
      if (oEnum.AtEnd()) {
          continue;
      } // if
    } // if

    WriteSetContext(clazz);

    foreach (Class::EnumField, fields, clazz) {
      auto const field = fields.Get();
      WriteFieldDef(field);
    } // for field
  } // for type

  // Write method groups
  foreach (Type::Array::Enum, tys, types) {
    auto const clazz = tys.Get()->DynamicCast<Class>();
    if (clazz == nullptr) {
      continue;
    } // if

    auto fEmitOpen = false;

    foreach (Class::EnumMember, members, clazz) {
      auto const member = members.Get();
      if (auto const method_group = member->DynamicCast<MethodGroup>()) {
        if (!fEmitOpen) {
          WriteSetContext(clazz);
          fEmitOpen = true;
        } // if
        WriteMethodGroupDef(method_group);
      } // if
    } // for method group
  } // for type

  DEBUG_PRINTF("TotalBits=%d"
      " Op_Limit=%d FaslOp_Limit=%d"
      " Name=%d Operand=%d\n",
      bit_writer_.total_bits(),
      Op_Limit,
      FaslOp_Limit,
      m_iNameId,
      m_iOperandId);
} // Dumtypes

// [R]
int FastDumper::Remember(const Operand* const pOperand) {
    ASSERT(pOperand != Namespace::Global);

    m_iOperandId++;
    m_oOperandMap.Add(pOperand, m_iOperandId);

    DEBUG_PRINTF("Operand[%d]=%hs %p %ls\n",
        m_iOperandId,
        pOperand->GetKind(),
        pOperand,
        Static::ComputeOperandName(pOperand));

    return m_iOperandId;
} // Remember

// [W]
void FastDumper::Write(FaslOp const eFaslOp) {
    DEBUG_PRINTF("FaslOp %hs(%d)\n",
        k_rgpszFaslOp[eFaslOp], eFaslOp);

    WriteV4(eFaslOp);
} // Write FaslOp

void FastDumper::Write(Instruction* const pI) {
    ASSERT(pI != nullptr);

    DEBUG_PRINTF("%hs(%d) ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n",
        pI->GetMnemonic(), pI->GetOp());

    WriteOp(pI->GetOp());
    WriteRef(pI->GetTy());

    if (auto const pOutput = pI->GetOutput()->DynamicCast<SsaOutput>()) {
        Remember(pOutput);
    } // if

    foreach (Instruction::EnumOperand, oEnum, pI) {
        auto const pBox = oEnum.GetBox();

        if (auto const pPhiBox =
                        pBox->DynamicCast<PhiOperandBox>()) {
            WriteRef(pPhiBox->GetBB()->GetLabel());

        } else if (auto const pSwBox =
                        pBox->DynamicCast<SwitchOperandBox>()) {
            WriteRef(pSwBox->GetBB()->GetLabel());
        } // if

        WriteRef(pBox->GetOperand());
    } // for operand

    switch (pI->GetArity()) {
    case Instruction::Arity_Any:
    case Instruction::Arity_MoreThanZero:
        Write(FaslOp_Zero);
        break;
    } // switch arity

    WriteSourceInfo(pI->GetSourceInfo());
} // Write Instruction

// Name V8+ 0
// Name64 I6+ 0
void FastDumper::Write(const Name& name) {
    ASSERT(pName != nullptr);

    if (auto const iValue = m_oNameMap.Get(pName)) {
        WriteRef(iValue);
        return;
    } // if

    m_iNameId++;
    m_oNameMap.Add(pName, m_iNameId);

    DEBUG_PRINTF("Name[%d] %ls\n", m_iNameId, pName->GetString());

    if (Static::IsName64(pName)) {
        Write(FaslOp_Name64);
        for (auto pwsz = pName->GetString(); 0 != *pwsz; pwsz++) {
            bit_writer_.Write6(Name64::FromChar(*pwsz));
        } // for wch
        bit_writer_.Write6(0);
        return;
    } // if

    Write(FaslOp_Name);
    WriteV8(::lstrlenW(pName->GetString()));
    for (auto pwsz = pName->GetString(); 0 != *pwsz; pwsz++) {
        WriteV8(*pwsz);
    } // for wch
} // Write Name

void FastDumper::WriteDef(FaslOp const eOpCode, Operand* const pOperand) {
    ASSERT(pOperand != nullptr);
    ASSERT(m_oOperandMap.Get(pOperand) == 0);
    Write(eOpCode);
} // WriteDef

// Field
//  Name
//  TypeRef
//  Modifiers
void FastDumper::WriteFieldDef(Field* const pField) {
    ASSERT(pField != nullptr);
    DEBUG_PRINTF("%ls\n", pField->name().GetString());
    WriteDef(FaslOp_Field, pField);
    Write(pField->name());
    WriteModifiers(pField->GetModifiers());
    WriteRef(pField->field_type());
    Remember(pField);
} // WriteField

// Function
//  Name
//  Ref(FunctionType)
//  Ref(Variable)+
//  V4(BBlockCount)
//  Instructions+
void FastDumper::WriteFunctionDef(Function* const pFunction) {
    ASSERT(pFunction != nullptr);

    Write(FaslOp_FunctionDef);
    WriteRef(pFunction);
    WriteRef(pFunction->GetFunty());

    foreach (Function::EnumVar, oEnum, pFunction) {
        WriteVariableDef(oEnum.Get());
    } // for var

    // Renumber bblock and output
    {
        auto iBBlock = 0;
        auto iOutput = 0;
        foreach (Function::EnumBBlock, oEnum, pFunction) {
            auto const pBBlock = oEnum.Get();
            iBBlock++;
            pBBlock->SetName(iBBlock);
            foreach (BBlock::EnumI, oEnum, pBBlock) {
                auto const pI = oEnum.Get();
                if (auto const pOutput = pI->GetOutput()) {
                    if (pOutput != Void) {
                        iOutput++;
                        pOutput->SetName(iOutput);
                    } // if
                } // if
            } // for instruction
        } // for bblock

        ASSERT(pFunction->GetEntryBB()->name() == 1);
        ASSERT(pFunction->GetStartBB()->name() == 2);
        ASSERT(pFunction->GetExitBB()->name() == iBBlock);

        Write(FaslOp_PosInt);
        WriteV8(iBBlock);
    }

    m_pSourceInfo = nullptr;

    foreach (Function::EnumBBlock, oEnum, pFunction) {
        auto const pBBlock = oEnum.Get();
        foreach (BBlock::EnumI, oEnum, pBBlock) {
            Write(oEnum.Get());
        } // for instruction
    } // for bblock

    WriteOp(Op_None);
} // WriteFunctionDef

// Method
//  Modifier
//  FunctionType
//  Name*
//  Ref(Function)*
//  Def(Function)*
void FastDumper::WriteMethodDef(Method* const pMethod) {
    ASSERT(pMethod != nullptr);

    DEBUG_PRINTF("%ls.%ls 0x%X\n",
        pMethod->owner_class().name()->GetString(),
        pMethod->name()->GetString(),
        pMethod->GetModifiers());

    // Note: Method may be already remembered by MethodRef.
    if (auto const pGenericMethod = pMethod->DynamicCast<GenericMethod>()) {
        Write(FaslOp_GenericMethod);

        foreach (GenericMethod::EnumTypeParam, oEnum, *pGenericMethod) {
            auto const pTypeParam = oEnum.Get();
            WriteDef(FaslOp_TypeParam, pTypeParam);
            Write(pTypeParam->name());
            Write(FaslOp_Zero);
            Remember(pTypeParam);
        } // for type param

    } else {
        Write(FaslOp_Method);
    } // if

    WriteModifiers(pMethod->GetModifiers());
    WriteRef(pMethod->function_type());

    if (m_oOperandMap.Get(pMethod) == 0) {
        Remember(pMethod);
    } // if

    // Param*
    foreach (Method::EnumParamName, oEnum, pMethod) {
        Write(oEnum.Get());
    } // for name

    if (auto const pGenericMethod = pMethod->DynamicCast<GenericMethod>()) {
        foreach (GenericMethod::EnumTypeParam, oEnum, *pGenericMethod) {
            auto const pTypeParam = oEnum.Get();
            WriteRef(pTypeParam);
        } // for type arg
    } // if

    // Function*
    if (auto const pFun = pMethod->GetFunction()) {
        auto const pModule = pFun->module();

        foreach (Module::EnumFunction, oEnum, pModule) {
            WriteFunctionDef(oEnum.Get());
        } // for function
    } // if pFun
} // WriteMethod

// MethodGroup
//  Name
//  Method*
void FastDumper::WriteMethodGroupDef(MethodGroup* const pMethodGroup) {
    ASSERT(pMethodGroup != nullptr);
    DEBUG_PRINTF("%ls.%ls\n", 
        pMethodGroup->owner_class().name()->GetString(),
        pMethodGroup->name().GetString());

    WriteDef(FaslOp_MethodGroup, pMethodGroup);
    Write(pMethodGroup->name());
    WriteRef(pMethodGroup->owner_class());
    Remember(pMethodGroup);

    foreach (MethodGroup::EnumMethod, oEnum, pMethodGroup) {
        WriteMethodDef(oEnum.Get());
    } // for method
} // WriteMethodGroup

void FastDumper::WriteModifiers(const int iModifiers) {
    Write(FaslOp_PosInt);
    WriteV8(iModifiers);
} // WriteModifiers

void FastDumper::WriteOp(Op const eOp) {
    WriteV4(eOp);
} // WriteOp

/// <summary>
///     Emits Output command.
/// </summary>
void FastDumper::WriteOutput(SsaOutput* const pOutput) {
    ASSERT(pOutput != nullptr);

    auto const iName = pOutput->name();
    DEBUG_PRINTF("%hs %d\n", pOutput->GetKind(), iName);

    if (iName < 16) {
        Write(FaslOp_Reg1);
        bit_writer_.Write4(iName);
        return;
    } // if

    if (iName < 16 * 16) {
        Write(FaslOp_Reg2);
        bit_writer_.Write8(iName >> 4);
        return;
    } // if

    Write(FaslOp_RegV);
    WriteV4(iName);
} // WriteOutput

/// <summary>
///     Emits Ref command.
/// </summary>
void FastDumper::WriteRef(int const iId) {
    ASSERT(iId > 0);

    if (iId < 16) {
        Write(FaslOp_Ref1);
        bit_writer_.Write4(iId);
        return;
    } // if

    if (iId < 16 * 16) {
        Write(FaslOp_Ref2);
        bit_writer_.Write4(iId >> 4);
        bit_writer_.Write4(iId >> 0);
        return;
    } // if

    if (iId < 16 * 16 * 16) {
        Write(FaslOp_Ref3);
        bit_writer_.Write8(iId >> 8);
        bit_writer_.Write4(iId >> 0);
        return;
    } // if

    Write(FaslOp_RefV);
    WriteV4(iId);
} // WriteRef

void FastDumper::WriteRef(BBlock* const pBBlock) {
    ASSERT(pBBlock != nullptr);
    WriteV4(pBBlock->name());
} // WriteRef BBlock

void FastDumper::WriteRef(const Operand* const pOperand) {
    if (pOperand == nullptr) {
        Write(FaslOp_Zero);
        return;
    } // if

    if (pOperand == m_pContextOperand) {
        ASSERT(pOperand != Namespace::Global);
        Write(FaslOp_RefContext);
        return;
    } // if

    if (pOperand == Namespace::Global) {
        Write(FaslOp_Zero);
        return;
    } // if

    if (auto const iValue = m_oOperandMap.Get(pOperand)) {
        WriteRef(iValue);
        return;
    } // if

    {
        RefWriter oRefWriter(this);
        const_cast<Operand*>(pOperand)->Apply(&oRefWriter);
        Remember(pOperand);
    }
} // WriteRef

void FastDumper::WriteSetContext(Operand* const pOperand) {
    ASSERT(pOperand != nullptr);
    Write(FaslOp_SetContext);
    WriteRef(pOperand);
    m_pContextOperand = pOperand;
} // WriteSetContext

void FastDumper::WriteSourceInfo(const SourceInfo* pSourceInfo) {
    if (pSourceInfo == nullptr) {
        DEBUG_PRINTF("No source info\n");
        Write(FaslOp_Zero);
        return;
    } // if

    if (!pSourceInfo->HasInfo()) {
        DEBUG_PRINTF("No source info\n");
        Write(FaslOp_Zero);
        return;
    } // if

    if (m_pSourceInfo == nullptr
            || m_pSourceInfo->GetFileName() != pSourceInfo->GetFileName()) {

        DEBUG_PRINTF("Full Source Info: %ls(%d,%d)\n",
            pSourceInfo->GetFileName().value(),
            pSourceInfo->GetLine(),
            pSourceInfo->GetColumn());

        auto& name = Name::Intern(*pSourceInfo->GetFileName());
        Write(FaslOp_LineInfo);
        Write(&name);
        WriteV8(pSourceInfo->GetLine());
        WriteV7(pSourceInfo->GetColumn());

        m_pSourceInfo = pSourceInfo;
        return;
    } // if

    m_pSourceInfo = pSourceInfo;

    auto const iLineDelta = pSourceInfo->GetLine()
        - m_pSourceInfo->GetLine();

    auto const iColumnDelta = pSourceInfo->GetColumn()
        - m_pSourceInfo->GetColumn();

    if (iLineDelta > 0) {
        DEBUG_PRINTF("Line Delta %d\n", iLineDelta);
        Write(FaslOp_LineAdd);
        WriteV4(iLineDelta);
        WriteV7(pSourceInfo->GetColumn());
        return;
    } // if

    if (iLineDelta < 0) {
        DEBUG_PRINTF("Line Delta %d\n", iLineDelta);
        Write(FaslOp_LineSub);
        WriteV4(-iLineDelta);
        WriteV7(pSourceInfo->GetColumn());
        return;
    } // if

    ASSERT(0 == iLineDelta);

    DEBUG_PRINTF("Column Delta %d\n", iColumnDelta);

    if (iColumnDelta >= 0) {
        Write(FaslOp_LineColAdd);
        WriteV4(iColumnDelta);

    } else {
        Write(FaslOp_LineColSub);
        WriteV4(-iColumnDelta);
    } // if
} // WriteSourceInfo

void FastDumper::WriteV4(int const iValue, int const iMsb) {
    if (iValue > 7) {
        WriteV4(iValue >> 3, 8);
    } // if
    bit_writer_.Write4((iValue & 7) | iMsb);
} // WriteV4

void FastDumper::WriteV5(int const iValue, int const iMsb) {
    if (iValue > 0x0F) {
        WriteV7(iValue >> 4, 0x10);
    } // if
    bit_writer_.Write5((iValue & 0x0F) | iMsb);
} // WriteV5

void FastDumper::WriteV6(int const iValue, int const iMsb) {
    if (iValue > 0x1F) {
        WriteV7(iValue >> 5, 0x20);
    } // if
    bit_writer_.Write6((iValue & 0x1F) | iMsb);
} // WriteV6

void FastDumper::WriteV7(int const iValue, int const iMsb) {
    if (iValue > 0x3F) {
        WriteV7(iValue >> 6, 0x40);
    } // if
    bit_writer_.Write7((iValue & 0x3F) | iMsb);
} // WriteV7

void FastDumper::WriteV8(int const iValue, int const iMsb) {
    if (iValue > 0x7F) {
        WriteV8(iValue >> 7, 0x80);
    } // if
    bit_writer_.Write8((iValue & 0x7F) | iMsb);
} // WriteV8

void FastDumper::WriteV8(int64 const iValue, int const iMsb) {
    if (iValue > 0x7F) {
        WriteV8(iValue >> 7, 0x80);
    } // if
    bit_writer_.Write8((iValue & 0x7F) | iMsb);
} // WriteV8

void FastDumper::WriteVariableDef(Variable* const pVar) {
    ASSERT(pVar != nullptr);
    WriteDef(FaslOp_Variable, pVar);
    Write(pVar->name());
    Remember(pVar);
} // WriteVariable

} // Fasl
} // Il
