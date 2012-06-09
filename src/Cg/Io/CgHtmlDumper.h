// @(#)$Id$
//
// Evita Il
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Targets_Cg_HtmlDumper_h)
#define INCLUDE_Il_Targets_Cg_HtmlDumper_h

#include "../../Il/Io/HtmlDumper.h"

namespace Il {
namespace Cg {

using Il::Io::HtmlDumper;
using Il::Io::LogWriter;
using Il::Ir::Output;

class CgHtmlDumper : public HtmlDumper {
    // ctor
    public: CgHtmlDumper(LogWriter* const pWriter);

    // [W]
    public: virtual void WriteAnchor(const Output* const pOutput) override;
}; // CgHtmlDumper

} // Cg
} // Il

#endif // !defined(INCLUDE_Il_Targets_Cg_HtmlDumper_h)
