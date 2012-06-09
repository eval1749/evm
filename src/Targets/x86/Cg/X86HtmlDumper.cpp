#include "precomp.h"
// @(#)$Id$
//
// Evita Il - Ir - HtmlDumper
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./X86HtmlDumper.h"

#include "./X86Defs.h"

#include "../../Shared/Cg/CgDefs.h"

#include "../../../Il/Io/LogWriter.h"

namespace Il {
namespace Cg {

using namespace Il::Ir;

X86HtmlDumper::X86HtmlDumper(LogWriter* const pWriter) :
        HtmlDumper(pWriter) {
    ASSERT(nullptr != pWriter);
} // X86HtmlDumper


void X86HtmlDumper::WriteAnchor(const Output* const pOutput) {
    if (auto const pPhysical = pOutput->DynamicCast<Physical>()) {
        char szId[100];
        ::wsprintfA(szId, "o%p", pPhysical);
        this->GetWriter()->StartElement("span", "id", szId);
        this->GetWriter()->WriteString(pPhysical->GetDesc()->m_pszName);
        this->GetWriter()->EndElement("span");
        return;
    } // if

    if (auto const pStackSlot = pOutput->DynamicCast<StackSlot>()) {
        this->GetWriter()->Write(pStackSlot);
        return;
    } // if

    this->HtmlDumper::WriteAnchor(pOutput);
} // WriteAnchor

} // Cg
} // Il
