#include "precomp.h"
// @(#)$Id$
//
// Evita Common - Io - StreamReader
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./StreamReader.h"

namespace Common {
namespace Io {

StreamReader::StreamReader(
    HANDLE const hInput,
    uint const nCodePage) :
        m_cb(0),
        m_cwch(0),
        m_nCodePage(nCodePage),
        m_h(hInput),
        m_pwch(m_rgwch) {}

bool StreamReader::fill() {
    if (0 != m_cb) {
        ::CopyMemory(
            m_rgb,
            m_rgb + sizeof(m_rgb) - m_cb,
            m_cb);
    } // if

    DWORD cbRead;
    auto fSucceeded = ::ReadFile(
        m_h,
        m_rgb + m_cb,
        static_cast<int>(lengthof(m_rgb) - m_cb),
        &cbRead,
        nullptr);

    if (!fSucceeded) {
        auto dwError = ::GetLastError();
        Debugger::Printf("ReadFile: %d", dwError);
        return false;
    } // if

    if (0 == cbRead) {
        return m_cb > 0;
    } // if

    m_cb += cbRead;

    uint cb = m_cb;

    for (;;) {
        auto cwch = ::MultiByteToWideChar(
            m_nCodePage,
            MB_ERR_INVALID_CHARS,
            m_rgb,
            cb,
            m_rgwch,
            lengthof(m_rgwch));

            if (cwch >= 1) {
                ::CopyMemory(m_rgb, m_rgb + cb, sizeof(m_rgb) - cb);
                m_cb -= cb;
                m_cwch += cwch;
                break;
            } // if

            if (1 == cb) {
                // Can't traslate byte into Unicode
                return false;
            } // if

            cb -= 1;
    } // for

    m_pwch = m_rgwch;
    return true;
} // fill

uint StreamReader::Read(
    char16* const pwchStart,
    uint const cwchBuffer)  {

    ASSERT(nullptr != pwchStart);

    auto pwchRunner = pwchStart;
    auto const pwchEnd = pwchStart + cwchBuffer;

    while (pwchRunner < pwchEnd) {
        if (0 == m_cwch) {
            if(!this->fill()) {
                break;
            }
        } // if

        auto cwchCopy = min(
            static_cast<uint>(pwchEnd - pwchRunner),
            m_cwch);

        ::CopyMemory(pwchRunner, m_pwch, sizeof(char16) * cwchCopy);

        m_cwch -= cwchCopy;
        m_pwch += cwchCopy;

        pwchRunner += cwchCopy;
    } // while

    return static_cast<uint>(pwchRunner - pwchStart);
} // Read

} // Io
} // Common
