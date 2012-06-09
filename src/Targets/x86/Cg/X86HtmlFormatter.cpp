#include "precomp.h"
// @(#)$Id$
//
// Evita Il - Ir - HtmlFormatter
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./X86HtmlFormatter.h"

#include "./X86Defs.h"

#include "../../Shared/Cg/CgDefs.h"

#include "../../../Il/Io/XhtmlWriter.h"

namespace Il {
namespace Cg {

using namespace Il::Ir;

X86HtmlFormatter::X86HtmlFormatter(XhtmlWriter* const pWriter) :
        CgHtmlFormatter(pWriter) {
    ASSERT(nullptr != pWriter);
} // X86HtmlFormatter

// [P]
void X86HtmlFormatter::Process(StackSlot* const pStackSlot) {
    this->GetWriter()->Printf("[ESP+%d]", pStackSlot->GetLocation());
} // Process StackSlot

void X86HtmlFormatter::Process(TttnLit* const pTttnLit) {
    static const char* const s_rgpszTttn[16] = {
        "O",  "NO",     // 0
        "B",  "NB",     // 2
        "Z",  "NZ",     // 4
        "BE", "NBE",    // 6
        "S",  "NS",     // 8
        "P",  "NP",     // 10
        "L",  "GE",     // 12
        "LE", "G",      // 14
    }; // TttnLit::sm_rgpszTttn

    this->GetWriter()->WriteString(s_rgpszTttn[pTttnLit->GetDatum()]);
} // Process TttnLit

} // Cg
} // Il
