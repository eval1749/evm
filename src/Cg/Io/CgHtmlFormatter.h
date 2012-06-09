// @(#)$Id$
//
// Evita Il
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Targets_Cg_HtmlFormatter_h)
#define INCLUDE_Il_Targets_Cg_HtmlFormatter_h

#include "../../Il/Io/HtmlFormatter.h"

namespace Il {
namespace Cg {

using Il::Io::HtmlFormatter;
using Il::Io::XhtmlWriter;

class CgHtmlFormatter : public HtmlFormatter {
    // ctor
    public: CgHtmlFormatter(XhtmlWriter* const pWriter);

    // [P]
    public: virtual void Process(Physical* const pPhysical) override;
}; // CgHtmlFormatter

} // Cg
} // Il

#endif // !defined(INCLUDE_Il_Targets_Cg_HtmlFormatter_h)
