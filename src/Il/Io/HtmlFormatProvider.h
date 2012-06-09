// @(#)$Id$
//
// Evita Il - IR - HtmlFormatProvider
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Io_HtmlFormatProvider_h)
#define INCLUDE_Il_Io_HtmlFormatProvider_h

#include "../Functor.h"

namespace Il {
namespace Io {

class HtmlFormatter;
class XhtmlWriter;

class HtmlFormatProvider {
  public: virtual ~HtmlFormatProvider() {}
  public: virtual HtmlFormatter* CreateFormatter(
    XhtmlWriter* const pWriter);
}; // HtmlFormatProvider

} // Io
} // Il

#endif // !defined(INCLUDE_Il_Io_HtmlFormatProvider_h)
