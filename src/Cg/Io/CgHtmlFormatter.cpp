#include "precomp.h"
// @(#)$Id$
//
// Evita Il - Ir - HtmlFormatter
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./CgHtmlFormatter.h"

#include "../../Il/Io/XhtmlWriter.h"

namespace Il {
namespace Cg {

using namespace Il::Ir;

CgHtmlFormatter::CgHtmlFormatter(XhtmlWriter* const pWriter) :
        HtmlFormatter(pWriter) {
    ASSERT(nullptr != pWriter);
} // CgHtmlFormatter

// [P]
void CgHtmlFormatter::Process(Physical* const pPhysical) {
    // TODO 2011-01-9 yosi@msn.com How do we share HTML id between formatter
    // and dumper?
    char szId[100];
    ::wsprintfA(szId, "#o%p", pPhysical);
    this->GetWriter()->StartElement("a", "href", szId);
    this->GetWriter()->WriteString(pPhysical->GetDesc()->m_pszName);
    this->GetWriter()->EndElement("a");
} // Process Physical

} // Cg
} // Il
