// @(#)$Id$
//
// Evita Il - IR - HtmlFormatProvider
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Targets_Cg_HtmlFormatProvider_h)
#define INCLUDE_Il_Targets_Cg_HtmlFormatProvider_h

#include "../../Il/Io/HtmlFormatProvider.h"

namespace Il {
namespace Cg {

using Il::Io::HtmlFormatProvider;
using Il::Io::HtmlFormatter;
using Il::Io::XhtmlWriter;

class CgHtmlFormatProvider : public HtmlFormatProvider {
    public: virtual HtmlFormatter* CreateFormatter(
        XhtmlWriter* const pWriter) override;
}; // CgHtmlFormatProvider

} // Cg
} // Il

#endif // !defined(INCLUDE_Il_Targets_Cg_HtmlFormatProvider_h)
