#include "precomp.h"
// @(#)$Id$
//
// Evita Il - Ir - HtmlDumper
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./HtmlDumper.h"

#include "./LogWriter.h"

#include "../Ir.h"

namespace Il {
namespace Io {

// ctor
HtmlDumper::HtmlDumper(
    LogWriter* const pWriter) :
        m_pWriter(pWriter) {
    ASSERT(nullptr != m_pWriter);
} // HtmlDumper

// [D]
void HtmlDumper::Dump(const BBlock* const pBBlock) {
    ASSERT(nullptr != pBBlock);

    char szIdDef[100];
    ::wsprintfA(szIdDef, "b%p", pBBlock);

    m_pWriter->StartElement("h6", "id", szIdDef);
    m_pWriter->Printf("BB%d", pBBlock->name());
    m_pWriter->EndElement("h6");

    m_pWriter->WriteElement("h7", "Edges");
    {
        m_pWriter->StartElement("table",
            "border", "1",
            "cellpadding", "4",
            "cellspacing", "4");

        {
            m_pWriter->StartElement("tr");
            m_pWriter->StartElement("th");
            m_pWriter->WriteString("In");
            m_pWriter->EndElement("th");

            m_pWriter->StartElement("td");
            const char* pszDelimiter = "";
            foreach (BBlock::EnumInEdge, oEnum, pBBlock) {
                m_pWriter->WriteString(pszDelimiter);
                pszDelimiter= " ";
                this->Dump(oEnum.Get());
            } // for edge

            if (0 == *pszDelimiter) {
                m_pWriter->StartElement("i");
                m_pWriter->WriteString("none");
                m_pWriter->EndElement("i");
            } // if
            m_pWriter->EndElement("td");
        }

        {
            m_pWriter->StartElement("tr");
            m_pWriter->StartElement("th");
            m_pWriter->WriteString("Out");
            m_pWriter->EndElement("th");

            m_pWriter->StartElement("td");
            const char* pszDelimiter = "";
            foreach (BBlock::EnumOutEdge, oEnum, pBBlock) {
                m_pWriter->WriteString(pszDelimiter);
                pszDelimiter= " ";
                this->Dump(oEnum.Get());
            } // for edge

            if (0 == *pszDelimiter) {
                m_pWriter->StartElement("i");
                m_pWriter->WriteString("none");
                m_pWriter->EndElement("i");
            } // if
            m_pWriter->EndElement("td");
        }

        m_pWriter->EndElement("table");
    }

    m_pWriter->WriteElement("h7", "Instructions");
    {
        m_pWriter->StartElement("table");

        foreach (BBlock::EnumI, oEnum, pBBlock) {
            m_pWriter->StartElement("tr");
            this->Dump(oEnum.Get());
            m_pWriter->EndElement("tr");
        } // for inst

        m_pWriter->EndElement("table");
    }
} // Dump BBlock

void HtmlDumper::Dump(const CfgEdge* const pCfgEdge) {
    ASSERT(nullptr != pCfgEdge);

    static const char* const k_rgpszEdge[] = {
        "",            // normal
        "(exit)",
        "(nonlocal)",
        "(pseudo)",
    }; // k_rgpwszEdge

    m_pWriter->Write("%p=>%p%sx%d%s",
        pCfgEdge->GetFrom(),
        pCfgEdge->GetTo(),
        k_rgpszEdge[pCfgEdge->GetEdgeKind()],
        pCfgEdge->GetCount(),
        pCfgEdge->IsBackward() ? "!" : "");
} // Dump CfgEdge


void HtmlDumper::Dump(const Function* const pFunction) {
    ASSERT(nullptr != pFunction);

    {
        char szIdDef[100];
        ::wsprintfA(szIdDef, "f%p", pFunction);

        m_pWriter->StartElement("h4", "id", szIdDef);

        m_pWriter->Printf("(Function %ls %p)",
            pFunction->name().ToString(),
            pFunction);

        m_pWriter->EndElement("h4");
    }

    m_pWriter->Write("type=%p", pFunction->GetFunty());

    m_pWriter->WriteElement("h5", "Variables");
    {
        if (pFunction->HasVariable()) {
            m_pWriter->StartElement("table",
                "border", "1",
                "cellpadding", "4");

            auto iNth = 0;
            foreach (Function::EnumVar, oEnum, pFunction) {
                auto const pVaraible = oEnum.Get();

                iNth++;
                m_pWriter->StartElement("tr");

                m_pWriter->StartElement("td", "align", "right");
                m_pWriter->Printf("%d", iNth);
                m_pWriter->EndElement("td");

                this->Dump(pVaraible);

                m_pWriter->EndElement("tr");
                m_pWriter->WriteLine();
            } // for varaible
            m_pWriter->EndElement("table");

        } else {
            m_pWriter->WriteString("No variables.\n");
        } // if
    } // Variable

    m_pWriter->WriteElement("h5", "BBlocks");
    {
        foreach (Function::EnumBBlock, oEnum, pFunction) {
            this->Dump(oEnum.Get());
        } // for bblock
    }
} // Dump Function

void HtmlDumper::Dump(const Instruction* const pI) {
    ASSERT(nullptr != pI);

    char szIdDef[100];
    ::wsprintfA(szIdDef, "i%p", pI);

    m_pWriter->StartElement("td");
    m_pWriter->Printf("%04d", pI->GetIndex());
    m_pWriter->EndElement("td");

    m_pWriter->StartElement("td", "width", L"10");
    m_pWriter->EndElement("td");

    m_pWriter->StartElement("td", "class", "i", "id", szIdDef);

    m_pWriter->WriteString(pI->GetMnemonic());
    m_pWriter->EndElement("td");

    if (Void == pI->GetOutput()) {
        m_pWriter->StartElement("td");
        m_pWriter->EndElement("td");

        m_pWriter->StartElement("td");
        m_pWriter->EndElement("td");

        m_pWriter->StartElement("td");
        m_pWriter->EndElement("td");

    } else {
        m_pWriter->StartElement("td");
        m_pWriter->Write(pI->GetTy());
        m_pWriter->EndElement("td");

        m_pWriter->StartElement("td");
        this->WriteAnchor(pI->GetOutput());
        m_pWriter->EndElement("td");

        m_pWriter->StartElement("td");
        m_pWriter->WriteString(" <=");
        m_pWriter->EndElement("td");
    } // if

    m_pWriter->StartElement("td");

    foreach (Instruction::EnumOperand, oEnum, pI) {
        auto const pBox = oEnum.GetBox();
        m_pWriter->Write(" %p", pBox);
    } // for oeprand

    m_pWriter->EndElement("td");

    // Output Variable
    m_pWriter->StartElement("td");
    if (auto const pRd = pI->GetRd()) {
        if (auto const pVar = pRd->GetVariable()) {
            m_pWriter->Write(pVar);
        } // if
    } // if
    m_pWriter->EndElement("td");

    // Instruction SourceInfo
    m_pWriter->StartElement("td");
    this->WriteRef(pI->GetSourceInfo());
    m_pWriter->EndElement("td");
} // Dump Instruction

void HtmlDumper::Dump(const Module* const pModule) {
    ASSERT(nullptr != pModule);

    char szIdDef[100];
    ::wsprintfA(szIdDef, "fs%p", pModule);

    m_pWriter->StartElement("h2", "id", szIdDef);
    m_pWriter->Printf("(Module %p)", pModule);
    m_pWriter->EndElement("h1");

    this->DumpModule(pModule);
} // Dump Module

void HtmlDumper::DumpModule(const Module* const pModule) {
    ASSERT(nullptr != pModule);

    m_pWriter->WriteElement("h3", "Functions");

    m_pWriter->StartElement("ol");

    foreach (Module::EnumFunction, oEnum, pModule) {
        m_pWriter->StartElement("li");
        m_pWriter->Write(oEnum.Get());
        m_pWriter->EndElement("li");
    } // for Function

    m_pWriter->EndElement("ol");

    foreach (Module::EnumFunction, oEnum, pModule) {
        this->Dump(oEnum.Get());
    } // for
} // DumpModule

void HtmlDumper::Dump(const Method* const pMethod) {
    auto const pMethodGroup = pMethod->GetMethodGroup();
    Class& clazz = pMethodGroup->owner_class();

    m_pWriter->StartElement("h2");
    {
        m_pWriter->WriteString("(Method ");

        m_pWriter->Write(&pMethod->return_type());
        m_pWriter->Write(" ");
        m_pWriter->Write(&clazz);
        m_pWriter->Write(".");
        m_pWriter->WriteString(pMethodGroup->name().ToString());

        const char* psz = "(";
        foreach (Method::EnumParamType, oEnum, pMethod) {
            m_pWriter->WriteString(psz);
            psz = " ";
            m_pWriter->Write(oEnum.Get());
        } // for

        m_pWriter->Printf("))");
    }
    m_pWriter->EndElement("h2");

    m_pWriter->WriteElement("h3", "Parameters");
    {
        m_pWriter->StartElement("table", "border", "2");

        auto iNth = 0;
        Method::EnumParamName oEnumName(pMethod);
        foreach (Method::EnumParamType, oEnum, pMethod) {
            auto const pType = oEnum.Get();
            auto const pName = oEnumName.Get();
            m_pWriter->StartElement("tr");

            m_pWriter->StartElement("td");
                m_pWriter->Printf("%d", iNth);
            m_pWriter->EndElement("td");

            m_pWriter->StartElement("td");
                m_pWriter->Write(pType);
            m_pWriter->EndElement("td");

            m_pWriter->StartElement("td");
                m_pWriter->WriteString(pName->ToString());
            m_pWriter->EndElement("td");

            m_pWriter->EndElement("tr");

            oEnumName.Next();
            iNth++;
        } // for param

        m_pWriter->EndElement("table");
    }

    this->DumpModule(pMethod->GetFunction()->module());
} // Dump Method

void HtmlDumper::Dump(const Output* const pOutput) {
    this->WriteAnchor(pOutput);
} // Dump Output

void HtmlDumper::Dump(const Variable* const pVariable) {
    ASSERT(nullptr != pVariable);

    static const char* k_rgpszStorage[] = {
        "closed-cell",
        "literal",
        "register",
        "stack",
    }; // k_rgpszStorage

    m_pWriter->StartElement("td");
    m_pWriter->WriteString(pVariable->name().ToString());
    m_pWriter->EndElement("td");

    m_pWriter->StartElement("td");
    m_pWriter->Write(pVariable->GetTy());
    m_pWriter->EndElement("td");

    m_pWriter->StartElement("td");
    m_pWriter->WriteString(k_rgpszStorage[pVariable->GetStorage()]);
    m_pWriter->EndElement("td");

    m_pWriter->StartElement("td");
    m_pWriter->Printf(" UpRef=%d", pVariable->CountUpRefs());
    m_pWriter->EndElement("td");

    m_pWriter->StartElement("td");
    m_pWriter->Printf(" Location=%d", pVariable->GetLocation());
    m_pWriter->EndElement("td");

    m_pWriter->StartElement("td");
    this->WriteRef(pVariable->GetSourceInfo());
    m_pWriter->EndElement("td");
} // Dump Variable

void HtmlDumper::WriteAnchor(const Output* const pOutput) {
    ASSERT(nullptr != pOutput);
    char szIdDef[100];
    ::wsprintfA(szIdDef, "o%p", pOutput);

    m_pWriter->StartElement("span", "class", "o", "id", szIdDef);

    m_pWriter->Printf("%%%c%d",
        pOutput->GetPrefixChar(),
        pOutput->name());

    m_pWriter->EndElement("span");
} // WriteAnchor Output

void HtmlDumper::WriteRef(const SourceInfo* const pSourceInfo) {
    ASSERT(nullptr != pSourceInfo);

    if (!pSourceInfo->HasInfo()) {
        m_pWriter->StartElement("b", "class", "alert");
        m_pWriter->WriteString("none");
        m_pWriter->EndElement("b");
        return;
    } // if

    m_pWriter->Printf("%ls(%d,%d)",
        nullptr == pSourceInfo->GetFileName()
            ? L"*"
            : pSourceInfo->GetFileName().value(),
        pSourceInfo->GetLine(),
        pSourceInfo->GetColumn());
} // Dump SourceInfo

} // Io
} // Il
