// @(#)$Id$
//
// Evita Il
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Targets_X86_HtmlDumper_h)
#define INCLUDE_Il_Targets_X86_HtmlDumper_h

#include "../../../Il/Io/HtmlDumper.h"

namespace Il {
namespace Cg {

using Il::Io::HtmlDumper;
using Il::Io::LogWriter;
using Il::Ir::Output;

class X86HtmlDumper : public HtmlDumper {
    // ctor
    public: X86HtmlDumper(LogWriter* const pWriter);

    // [W]
    public: virtual void WriteAnchor(const Output* const pOutput) override;
}; // X86HtmlDumper

} // Cg
} // Il

#endif // !defined(INCLUDE_Il_Targets_X86_HtmlDumper_h)
