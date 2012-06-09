#include "precomp.h"
// @(#)$Id$
//
// Evita Il - Ir - HtmlFormatter
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./HtmlFormatter.h"

#include "./XhtmlWriter.h"

#include "../Ir.h"

namespace Il {
namespace Io {

// ctor
HtmlFormatter::HtmlFormatter(
    XhtmlWriter* const pWriter) :
        m_pWriter(pWriter) {
    ASSERT(m_pWriter != nullptr);
}

// [P]
void HtmlFormatter::Process(ArrayType* const pType) {
    ASSERT(pType != nullptr);

    pType->element_type().Apply(this);

    m_pWriter->Write('[');

    auto const iRank = pType->rank();
    for (auto k = 1; k < iRank; k++) {
        m_pWriter->Write(',');
    }

    m_pWriter->Write(']');
}

void HtmlFormatter::Process(BoolType* const pType) {
    ASSERT(pType != nullptr);
    m_pWriter->Write("bool");
}

void HtmlFormatter::Process(BoolOutput* const pBool) {
    ASSERT(pBool != nullptr);

    if (pBool == True) {
        m_pWriter->StartElement("span", "class", "b");
        m_pWriter->WriteString("%true");
        m_pWriter->EndElement("span");
        return;
    }

    if (pBool == False) {
        m_pWriter->StartElement("span", "class", "b");
        m_pWriter->WriteString("%false");
        m_pWriter->EndElement("span");
        return;
    }

    Process(static_cast<SsaOutput*>(pBool));
}

void HtmlFormatter::Process(BBlock* const pBBlock) {
    ASSERT(pBBlock != nullptr);

    char szIdRef[100];
    ::wsprintfA(szIdRef, "#b%p", pBBlock);

    m_pWriter->StartElement("a", "href", szIdRef);
    m_pWriter->Printf("BB%d", pBBlock->name());
    m_pWriter->EndElement("a");
}

// [C]
void HtmlFormatter::Process(Class* const pClass) {
    ASSERT(pClass != nullptr);

    m_pWriter->StartElement("span", "class", "t");
    WriteClassName(pClass);
    m_pWriter->EndElement("span");
}

void HtmlFormatter::Process(ConstructedClass* const pClass) {
    ASSERT(pClass != nullptr);

    WriteClassName(&pClass->generic_class());

    m_pWriter->Write('<');

    const char* pszComma = "";

    foreach (
            GenericClass::EnumTypeParam,
            oEnum,
            *pClass->GetGenericClass()) {
        m_pWriter->Write(pszComma);
        pszComma = ",";

        auto const pTypeParam = oEnum.Get();
        if (auto const pType = pClass->GetTypeArg(pTypeParam)) {
            pType->Apply(this);

        } else {
            Write(pTypeParam->name());
        }
    }

    m_pWriter->Write('>');
}

// [F]
void HtmlFormatter::Process(Field* const pField) {
    ASSERT(pField != nullptr);
    m_pWriter->WriteString("(Field ");
    m_pWriter->StartElement("span", "class", "f");
    pField->owner_class().Apply(this);
    m_pWriter->Write('.');
    m_pWriter->WriteString(pField->name().ToString());
    m_pWriter->EndElement("span");
    m_pWriter->Write(')');
}

void HtmlFormatter::Process(Function* const pFunction) {
    ASSERT(pFunction != nullptr);

    char szIdRef[100];
    ::wsprintfA(szIdRef, "#f%p", pFunction);

    m_pWriter->StartElement("a", "href", szIdRef);

    m_pWriter->Printf("(Function %ls %p)",
        pFunction->name().ToString(),
        pFunction);

    m_pWriter->EndElement("a");
}

void HtmlFormatter::Process(Module* const pModule) {
    ASSERT(pModule != nullptr);

    char szIdRef[100];
    ::wsprintfA(szIdRef, "#fs%p", pModule);

    m_pWriter->StartElement("a", "href", szIdRef);
    m_pWriter->Printf("(Module %p)", pModule);
    m_pWriter->EndElement("a");
}

/// <summary>
///   Emits "(FunctionType Return Params)"
/// </summary>
void HtmlFormatter::Process(FunctionType* const pFunctionType) {
    ASSERT(pFunctionType != nullptr);

    m_pWriter->WriteString("(FunctionType ");
    pFunctionType->return_type().Apply(this);
    m_pWriter->WriteString(" (");

    char* psz = "";
    foreach (FunctionType::EnumParamType, oEnum, pFunctionType) {
        m_pWriter->WriteString(psz);
        oEnum.Get()->Apply(this);
        psz = " ";
    }

    m_pWriter->Write("))");
}

// [G]
void HtmlFormatter::Process(GenericClass* const pClass) {
    ASSERT(pClass != nullptr);
    Process(static_cast<Class*>(pClass));

    const char* pSeparator = "&lt;";
    foreach (GenericClass::EnumTypeParam, oEnum, *pClass) {
        auto const pTypeParam = oEnum.Get();
        m_pWriter->WriteString(pSeparator);
        m_pWriter->WriteString(pTypeParam->name().ToString());
    }

    m_pWriter->WriteString("&gt;");
}

// [I]
void HtmlFormatter::Process(Instruction* const pI) {
    ASSERT(pI != nullptr);

    char szIdRef[100];
    ::wsprintfA(szIdRef, "#i%p", pI);

    m_pWriter->StartElement("a", "href", szIdRef);

    m_pWriter->Printf("BB%d %04d %hs",
        nullptr == pI->GetBBlock()
            ? 0
            : pI->GetBBlock()->name(),
        pI->GetIndex(),
        pI->GetMnemonic());

    m_pWriter->EndElement("a");

    if (Void != pI->GetOutput()) {
        m_pWriter->Write(' ');
        pI->GetTy()->Apply(this);
        m_pWriter->Write(' ');
        pI->GetOutput()->Apply(this);
        m_pWriter->Write(" <=");
    }

    foreach (Instruction::EnumOperand, oEnum, pI) {
        m_pWriter->Write(' ');
        oEnum.GetBox()->GetOperand()->Apply(this);
    }
}

// [L]
void HtmlFormatter::Process(Label* const pLabel) {
    ASSERT(pLabel != nullptr);
    Process(pLabel->GetBB());
}

void HtmlFormatter::Process(Literal* const pLiteral) {
    ASSERT(pLiteral != nullptr);

    if (pLiteral->GetTy() == Ty_String) {
        m_pWriter->StartElement("span", "class", L"l");

        m_pWriter->Write('"');
        m_pWriter->WriteString(pLiteral->ToString());
        m_pWriter->Write('"');

        m_pWriter->EndElement("span");

    } else if (auto pChar = pLiteral->GetChar()) {
        m_pWriter->StartElement("span", "class", L"l");

        m_pWriter->Write('\'');
        m_pWriter->Write(pChar->GetCode());
        m_pWriter->Write('\'');

        m_pWriter->EndElement("span");

    } else {
        m_pWriter->Write('(');

        m_pWriter->WriteString(
            pLiteral->GetTy()->StaticCast<Class>()->name().ToString());

        m_pWriter->Write(' ');

        m_pWriter->StartElement("span", "class", L"l");

        m_pWriter->Printf("%ld", pLiteral->GetInt64());

        m_pWriter->EndElement("span");
        m_pWriter->Write(')');
    }
}

// [M]
void HtmlFormatter::Process(Method* const pMethod) {
    ASSERT(pMethod != nullptr);

    auto const pMethodGroup = pMethod->GetMethodGroup();

    m_pWriter->WriteString("(Method ");

    Write(&pMethod->return_type());
    m_pWriter->Write(' ');

    WriteClassName(&pMethodGroup->owner_class());

    m_pWriter->Printf(".%ls(", pMethodGroup->name().ToString());

    auto iRest = pMethod->CountParams();
    foreach (Method::EnumParamType, oEnum, pMethod) {
        Write(oEnum.Get());
        iRest--;
        if (iRest > 0) {
            m_pWriter->Write(' ');
        }
    }

    m_pWriter->Printf("))");
}

void HtmlFormatter::Process(MethodGroup* const pMethodGroup) {
    ASSERT(pMethodGroup != nullptr);
    m_pWriter->WriteString("(MethodGroup ");
    WriteClassName(&pMethodGroup->owner_class());
    m_pWriter->Printf(".%ls)", pMethodGroup->name().ToString());
}

// {P][N]
void HtmlFormatter::Process(NameRef* const pNameRef) {
    ASSERT(pNameRef != nullptr);

    m_pWriter->WriteString("(NameRef");
    m_pWriter->StartElement("span", "class", L"n");

    char16 wch = ' ';

    foreach (NameRef::EnumInner, oEnum, pNameRef->ComputeMostOuter()) {
        auto const pCurrent = oEnum.Get();
        m_pWriter->Write(wch);
        m_pWriter->WriteString(pCurrent->name().ToString());
        wch = '.';
    }

    m_pWriter->EndElement("span");

    m_pWriter->Write(')');
}

void HtmlFormatter::Process(Namespace* const pNamespace) {
    ASSERT(pNamespace != nullptr);

    class Local {
        public: static void WriteNamespace(
            XhtmlWriter* const pWriter,
            Namespace* const pNamespace) {

            auto const pOuter = pNamespace->GetOuter();

            if (pOuter != nullptr && Namespace::Global != pOuter) {
                Local::WriteNamespace(pWriter, pOuter->StaticCast<Namespace>());
                pWriter->Write('.');
            }

            pWriter->WriteString(pNamespace->name().ToString());
        }
    }; // Local

    m_pWriter->StartElement("span", "class", "s");

    Local::WriteNamespace(m_pWriter, pNamespace);

    m_pWriter->EndElement("span");
}

// {P][N]
#if 0
void HtmlFormatter::Process(NullOperand* const pNull) {
    m_pWriter->StartElement("span", "class", "b");
    m_pWriter->WriteString("(");
    pNull->GetTy()->Apply(this);
    m_pWriter->WriteString(") %null");
    m_pWriter->EndElement("span");
}
#endif

// [O]
void HtmlFormatter::Process(Operand* const pOperand) {
    m_pWriter->StartElement("span", "class", "n");

    m_pWriter->Printf("(%s %p)",
        pOperand->GetKind(),
        pOperand);

    m_pWriter->EndElement("span");
}

void HtmlFormatter::Process(OperandBox* const pOperandBox) {
    ASSERT(pOperandBox != nullptr);
    pOperandBox->GetOperand()->Apply(this);
}

void HtmlFormatter::Process(Output* const pOutput) {
    ASSERT(pOutput != nullptr);

    char szIdRef[100];
    ::wsprintfA(szIdRef, "#o%p", pOutput);

    m_pWriter->StartElement("a", "href", szIdRef);

    m_pWriter->Printf("%%%c%d",
        pOutput->GetPrefixChar(),
        pOutput->name());

    m_pWriter->EndElement("a");
}

// [P]
void HtmlFormatter::Process(PhiOperandBox* const pOperandBox) {
    ASSERT(pOperandBox != nullptr);

    m_pWriter->Write('(');
    pOperandBox->GetBB()->Apply(this);
    m_pWriter->Write(' ');
    pOperandBox->GetOperand()->Apply(this);
    m_pWriter->Write(')');
}

void HtmlFormatter::Process(PointerType* const pType) {
    ASSERT(pType != nullptr);
    pType->pointee_type().Apply(this);
    m_pWriter->Write('*');
}

// [S]
void HtmlFormatter::Process(SwitchOperandBox* const pOperandBox) {
    ASSERT(pOperandBox != nullptr);

    m_pWriter->Write('(');
    pOperandBox->GetBB()->Apply(this);
    m_pWriter->Write(' ');
    pOperandBox->GetOperand()->Apply(this);
    m_pWriter->Write(')');
}

// [T]
void HtmlFormatter::Process(TypeParam* const pTypeParam) {
    ASSERT(pTypeParam != nullptr);

    m_pWriter->StartElement("span",
        "class", pTypeParam->IsBound() ? "t" : "u");

    if (pTypeParam->IsBound()) {
        auto const pOwner = pTypeParam->GetOwner();

        if (auto const pClass = pOwner->DynamicCast<GenericClass>()) {
            WriteClassName(pClass);

        } else if (auto const pMethod = pOwner->DynamicCast<GenericMethod>()) {
            m_pWriter->WriteString(pMethod->name().ToString());

        } else {
            m_pWriter->Write('*');
        }

        m_pWriter->Write('.');

    } else {
        m_pWriter->WriteString("?.");
    }

    m_pWriter->WriteString(pTypeParam->name().ToString());

    m_pWriter->EndElement("span");
}

// [U]
void HtmlFormatter::Process(TypeVar* const pTypeVar) {
    ASSERT(pTypeVar != nullptr);

    m_pWriter->StartElement("span", "class", "u");

    char sz[100];
    ::wsprintfA(sz, "?.%d", pTypeVar->GetCounter());

    m_pWriter->WriteString(sz);

    m_pWriter->EndElement("span");
}

void HtmlFormatter::Process(UnresolvedType* const pUnresolvedType) {
    ASSERT(pUnresolvedType != nullptr);

    m_pWriter->StartElement("span", "class", "u");

    auto const pNameRef = pUnresolvedType->GetNameRef();

    const char* psz = "";

    foreach (NameRef::EnumInner, oEnum, pNameRef->ComputeMostOuter()) {
        auto const pCurrent = oEnum.Get();
        m_pWriter->WriteString(psz);
        m_pWriter->WriteString(pCurrent->name().ToString());
        psz = ".";
    }

    char sz[100];
    ::wsprintfA(sz, "@%p", pUnresolvedType);

    m_pWriter->WriteString(sz);

    m_pWriter->EndElement("span");
}

// [V]
void HtmlFormatter::Process(ValuesType* const pValuesType) {
    ASSERT(pValuesType != nullptr);

    m_pWriter->WriteString("(Values");

    foreach (ValuesType::Enum, oEnum, pValuesType) {
        m_pWriter->Write(' ');
        auto const pType = oEnum.Get();
        pType->Apply(this);
    }

    m_pWriter->Write(')');
}

void HtmlFormatter::Process(Variable* const pVariable) {
    ASSERT(pVariable != nullptr);

    m_pWriter->Write("(Variable ");
    m_pWriter->StartElement("span", "class", L"v");
    m_pWriter->WriteString(pVariable->name().ToString());
    m_pWriter->EndElement("span");
    m_pWriter->Write(')');
}

void HtmlFormatter::Process(VoidOutput* const pVoidOutput) {
    ASSERT(pVoidOutput != nullptr);
    m_pWriter->WriteString("%void");
}

// [W]
void HtmlFormatter::Write(const Name& name) {
  m_pWriter->WriteString(name.ToString());
}

void HtmlFormatter::Write(Type* const pType) {
    ASSERT(pType != nullptr);
    pType->Apply(this);
}

void HtmlFormatter::WriteClassName(Class* const pClass) {
    ASSERT(pClass != nullptr);

    auto const pNamespace = pClass->GetNamespace();

    if (pNamespace == Namespace::Global) {
        m_pWriter->Write('.');

    } else if (pNamespace == Namespace::Common) {
        // No prefix

    } else {
        pNamespace->Apply(this);
        m_pWriter->Write('.');
    }

    m_pWriter->WriteString(pClass->name().ToString());
}
}
}
