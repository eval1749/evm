#include "precomp.h"
// @(#)$Id$
//
// Evita Il - Ir - HtmlDumper
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./CgHtmlDumper.h"

#include "../../Il/Io/LogWriter.h"

namespace Il {
namespace Cg {

using namespace Il::Ir;

CgHtmlDumper::CgHtmlDumper(LogWriter* const pWriter) :
        HtmlDumper(pWriter) {
    ASSERT(nullptr != pWriter);
} // CgHtmlDumper


void CgHtmlDumper::WriteAnchor(const Output* const pOutput) {
    if (auto const pPhysical = pOutput->DynamicCast<Physical>()) {
        char szId[100];
        ::wsprintfA(szId, "o%p", pPhysical);
        this->GetWriter()->StartElement("span", "id", szId);
        this->GetWriter()->WriteString(pPhysical->GetDesc()->m_pszName);
        this->GetWriter()->EndElement("span");
        return;
    } // if

    this->HtmlDumper::WriteAnchor(pOutput);
} // WriteAnchor

} // Cg
} // Il
