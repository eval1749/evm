#include "precomp.h"
// @(#)$Id$
//
// Evita Il - Ir - HtmlFormatter
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./HtmlFormatProvider.h"

#include "./HtmlFormatter.h"

namespace Il {
namespace Io {

HtmlFormatter* HtmlFormatProvider::CreateFormatter(
    XhtmlWriter* const pWriter) {
    return new HtmlFormatter(pWriter);
} // CreateForamtter

} // Io
} // Il
