#include "precomp.h"
// @(#)$Id$
//
// Evita Il - Ir - HtmlFormatter
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./X86HtmlFormatProvider.h"

#include "./X86HtmlFormatter.h"

namespace Il {
namespace Cg {

HtmlFormatter* X86HtmlFormatProvider::CreateFormatter(
    XhtmlWriter* const pWriter) {
    return new X86HtmlFormatter(pWriter);
} // CreateForamtter

} // Cg
} // Il
