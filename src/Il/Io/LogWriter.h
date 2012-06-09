// @(#)$Id$
//
// Evita Il - IR - IL
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Io_LogWriter_h)
#define INCLUDE_Il_Io_LogWriter_h

#include "./XhtmlWriter.h"

namespace Il {
namespace Io {

using Il::Ir::IrObject;

class HtmlFormatProvider;
class HtmlFormatter;

class LogWriter : public XhtmlWriter {
    private: bool m_fOwnFormatter;
    private: HtmlFormatProvider* const m_pFormatProvider;
    private: HtmlFormatter* const m_pFormatter;

    // ctor
    public: LogWriter(
        TextWriter* const pTextWriter,
        HtmlFormatProvider* const pFormatProvider = nullptr);

    public: virtual ~LogWriter();

    // [G]
    public: HtmlFormatProvider* GetFormatProvider() const
        { return m_pFormatProvider; }

    public: HtmlFormatter* GetFormatter() const
        { return m_pFormatter; }

    // [W]
    public: void Write(IrObject* const pObject);
    public: void Write(const char* const pszFormat, ...);
    public: void WriteV(const char* const pszFormat, va_list args);
    DISALLOW_COPY_AND_ASSIGN(LogWriter);
}; // LogWriter

} // Io
} // Il

#endif // !defined(INCLUDE_Il_Io_LogWriter_h)
