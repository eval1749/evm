#include "precomp.h"
// @(#)$Id$
//
// Evita Il - Ir - HtmlFormatter
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./CgHtmlFormatProvider.h"

#include "./CgHtmlFormatter.h"

namespace Il {
namespace Cg {

HtmlFormatter* CgHtmlFormatProvider::CreateFormatter(
    XhtmlWriter* const pWriter) {
    return new CgHtmlFormatter(pWriter);
} // CreateForamtter

} // Cg
} // Il
