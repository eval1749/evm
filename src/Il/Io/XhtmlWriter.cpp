#include "precomp.h"
// @(#)$Id$
//
// Evita Il - IR - Module
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./XhtmlWriter.h"

#include "../../Common/Io/StringWriter.h"
#include "../../Common/Io/TextWriter.h"

namespace Il {
namespace Io {

using Common::Io::StringWriter;

XhtmlWriter::XhtmlWriter(TextWriter* const pTextWriter) :
    m_pTextWriter(pTextWriter) {}

// [C]
void XhtmlWriter::Close() { m_pTextWriter->Close(); }

// [F]
void XhtmlWriter::Flush() { m_pTextWriter->Flush(); }

// [E]
void XhtmlWriter::EndElement(const char* pszElementName) {
    m_pTextWriter->Write("</%s>", pszElementName);
} // EndElement

// [P]
void XhtmlWriter::Printf(const char* pszFormat, ...) {
    va_list args;
    va_start(args, pszFormat);
    this->PrintfV(pszFormat, args);
    va_end(args);
} // Printf

void XhtmlWriter::PrintfV(const char* pszFormat, va_list args) {
    char sz[256];
    ::wvsprintfA(sz, pszFormat, args);
    this->WriteString(sz);
} // Printf

// [S]
void XhtmlWriter::StartElement(const char* pszElementName) {
    m_pTextWriter->Write("<%s>", pszElementName);
} // StartElement

void XhtmlWriter::StartElement(
    const char* pszElementName,
    const char* pszName1,
    const char* const pszValue1) {
    NameValuePairA rgo[1];
    rgo[0].m_pszName = pszName1;
    rgo[0].m_pszValue = pszValue1;
    this->StartElement(pszElementName, rgo, lengthof(rgo));
} // StartElement

void XhtmlWriter::StartElement(
    const char* pszElementName,
    const char* pszName1,
    const char* const pszValue1,
    const char* pszName2,
    const char* const pszValue2) {
    NameValuePairA rgo[2];
    rgo[0].m_pszName = pszName1;
    rgo[0].m_pszValue = pszValue1;
    rgo[1].m_pszName = pszName2;
    rgo[1].m_pszValue = pszValue2;
    this->StartElement(pszElementName, rgo, lengthof(rgo));
} // StartElement

void XhtmlWriter::StartElement(
    const char* pszElementName,
    const char* pszName1,
    const char* const pszValue1,
    const char* pszName2,
    const char* const pszValue2,
    const char* pszName3,
    const char* const pszValue3) {
    NameValuePairA rgo[3];
    rgo[0].m_pszName = pszName1;
    rgo[0].m_pszValue = pszValue1;
    rgo[1].m_pszName = pszName2;
    rgo[1].m_pszValue = pszValue2;
    rgo[2].m_pszName = pszName3;
    rgo[2].m_pszValue = pszValue3;
    this->StartElement(pszElementName, rgo, lengthof(rgo));
} // StartElement

void XhtmlWriter::StartElement(
    const char* pszElementName,
    const NameValuePairA* prgoNameValuePairA,
    size_t cNameValuePairAs) {
    ASSERT(nullptr != prgoNameValuePairA);
    ASSERT(nullptr != pszElementName);
    m_pTextWriter->Write('<');
    m_pTextWriter->Write(pszElementName);

    auto const pEnd = prgoNameValuePairA + cNameValuePairAs;
    for (auto p = prgoNameValuePairA; p < pEnd; p++) {
        m_pTextWriter->Write(' ');
        m_pTextWriter->Write(p->m_pszName);
        m_pTextWriter->Write("=\"");
        m_pTextWriter->Write(p->m_pszValue);
        m_pTextWriter->Write('"');
    } // for

    m_pTextWriter->Write('>');
} // StartElement

void XhtmlWriter::StartElement(
    const char* pszElementName,
    const char* pszName1,
    const char16* const pwszValue1) {
    NameValuePair rgo[1];
    rgo[0].m_pszName = pszName1;
    rgo[0].m_pwszValue = pwszValue1;
    this->StartElement(pszElementName, rgo, lengthof(rgo));
} // StartElement

void XhtmlWriter::StartElement(
    const char* pszElementName,
    const char* pszName1,
    const char16* const pwszValue1,
    const char* pszName2,
    const char16* const pwszValue2) {
    NameValuePair rgo[2];
    rgo[0].m_pszName = pszName1;
    rgo[0].m_pwszValue = pwszValue1;
    rgo[1].m_pszName = pszName2;
    rgo[1].m_pwszValue = pwszValue2;
    this->StartElement(pszElementName, rgo, lengthof(rgo));
} // StartElement

void XhtmlWriter::StartElement(
    const char* pszElementName,
    const NameValuePair* prgoNameValuePair,
    size_t cNameValuePairs) {
    ASSERT(nullptr != prgoNameValuePair);
    ASSERT(nullptr != pszElementName);
    m_pTextWriter->Write('<');
    m_pTextWriter->Write(pszElementName);

    auto const pEnd = prgoNameValuePair + cNameValuePairs;
    for (auto p = prgoNameValuePair; p < pEnd; p++) {
        m_pTextWriter->Write(' ');
        m_pTextWriter->Write(p->m_pszName);
        m_pTextWriter->Write("=\"");
        m_pTextWriter->Write(p->m_pwszValue);
        m_pTextWriter->Write('"');
    } // for

    m_pTextWriter->Write('>');
} // StartElement

// [W]
void XhtmlWriter::Write(char16 const wch) {
    switch (wch) {
    case '"':
        m_pTextWriter->Write("&quot;");
        break;

    case '&':
        m_pTextWriter->Write("&amp;");
        break;

    case '<':
        m_pTextWriter->Write("&lt;");
        break;

    case '>':
        m_pTextWriter->Write("&gt;");
        break;

    default:
        m_pTextWriter->Write(wch);
        break;
    } // switch wch
} // Write

void XhtmlWriter::WriteElement(
    const char* const pszElementName,
    const char16* const pwszContent) {
    this->StartElement(pszElementName);
    this->WriteString(pwszContent);
    this->EndElement(pszElementName);
} // WriteElement

void XhtmlWriter::WriteElement(
    const char* const pszElementName,
    const char* const pszFormat, ...) {

    this->StartElement(pszElementName);

    va_list args;
    va_start(args, pszFormat);
    this->PrintfV(pszFormat, args);
    va_end(args);

    this->EndElement(pszElementName);
} // WriteElement


void XhtmlWriter::WriteString(const char* pchString, size_t const cch) {
    ASSERT(nullptr != pchString);
    auto const pchEnd = pchString + cch;
    for (auto pch = pchString; pch < pchEnd; pch++) {
        m_pTextWriter->Write(*pch);
    } // for
} // WriteString

void XhtmlWriter::WriteString(const char16* pwchString, size_t const cwch) {
    ASSERT(nullptr != pwchString);
    auto const pwchEnd = pwchString + cwch;
    for (auto pwch = pwchString; pwch < pwchEnd; pwch++) {
        m_pTextWriter->Write(*pwch);
    } // for
} // WriteString

} // Io
} // Il
