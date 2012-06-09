// @(#)$Id$
//
// Evita Il
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Targets_X86_HtmlFormatter_h)
#define INCLUDE_Il_Targets_X86_HtmlFormatter_h

#include "../../Shared/Cg/Io/CgHtmlFormatter.h"

namespace Il {
namespace Cg {

using Il::Cg::CgHtmlFormatter;
using Il::Io::XhtmlWriter;

class X86HtmlFormatter : public CgHtmlFormatter {
    // ctor
    public: X86HtmlFormatter(XhtmlWriter* const pWriter);

    // [P]
    public: virtual void Process(StackSlot* const StackSlot) override;
    public: virtual void Process(TttnLit* const pTttnLit) override;
}; // X86HtmlFormatter

} // Cg
} // Il

#endif // !defined(INCLUDE_Il_Targets_X86_HtmlFormatter_h)
