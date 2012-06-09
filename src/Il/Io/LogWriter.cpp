#include "precomp.h"
// @(#)$Id$
//
// Evita Il - Io - LogWriter
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./LogWriter.h"

#include "./HtmlFormatProvider.h"
#include "./HtmlFormatter.h"

#include "../Ir/IrObject.h"

namespace Il {
namespace Io {

// ctor
LogWriter::LogWriter(
    TextWriter* const pTextWriter,
    HtmlFormatProvider* pFormatProvider) :
        XhtmlWriter(pTextWriter),
        m_fOwnFormatter(nullptr == pFormatProvider),
        m_pFormatProvider(pFormatProvider),
        ALLOW_THIS_IN_INITIALIZER_LIST(
            m_pFormatter(nullptr == pFormatProvider
              ? new HtmlFormatter(this)
              : pFormatProvider->CreateFormatter(this))) {
    ASSERT(nullptr != m_pFormatter);
} // LogWriter

LogWriter::~LogWriter() {
    if (m_fOwnFormatter) {
        delete m_pFormatter;
    }
} // ~LogWriter

// [W]
void LogWriter::Write(IrObject* const pObject) {
    ASSERT(nullptr != pObject);
    pObject->Apply(m_pFormatter);
} // Write

void LogWriter::Write(const char* const pszFormat, ...) {
    va_list args;
    va_start(args, pszFormat);
    this->WriteV(pszFormat, args);
    va_end(args);
} // Write

void LogWriter::WriteV(const char* const pszFormat, va_list args) {
    for (auto pszRunner = pszFormat; 0 != *pszRunner; pszRunner++) {
        if ('%' != *pszRunner) {
            this->XhtmlWriter::Write(*pszRunner);
            continue;
        } // if

        pszRunner++;

        auto fLong = false;
        if ('l' == *pszRunner) {
            fLong = true;
            pszRunner++;
        } // if

        switch (*pszRunner) {
        case 'd': case 'i': {
            char sz[100];
            if (fLong) {
                ::wsprintfA(sz, "%ld", va_arg(args, int64));
            } else {
                ::wsprintfA(sz, "%d", va_arg(args, int32));
            } // if
            this->WriteString(sz);
            break;
        } // d

        case 'p': {
            if (auto const pObject = va_arg(args, IrObject*)) {
                ASSERT(pObject->m_iNotGood == 0);
                pObject->Apply(m_pFormatter);
            } else {
                this->WriteString("null");
            } // if
            break;
        } // 'p'

        case 's':
            if (fLong) {
                this->WriteString(va_arg(args, char16*));
            } else {
                this->WriteString(va_arg(args, char*));
            } // if
            break;

        case 'x': case 'X': {
            char sz[100];
            if (fLong) {
                ::wsprintfA(sz, "%lx", va_arg(args, int64));
            } else {
                ::wsprintfA(sz, "%x", va_arg(args, int32));
            } // if
            this->WriteString(sz);
            break;
        } // x
        } // switch
    } // for pszRunner
} // WriteV

} // Io
} // Il
