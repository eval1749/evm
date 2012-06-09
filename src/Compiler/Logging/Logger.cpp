#include "precomp.h"
// @(#)$Id$
//
// Evita Compiler - Dumper
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./Logger.h"

#include "../CompilePass.h"
#include "../CompileSession.h"

#include "../Ir/ClassDef.h"
#include "../Ir/MethodDef.h"

#include "../../Common/Io/StreamWriter.h"

#include "../../Il/Io/HtmlDumper.h"
#include "../../Il/Io/HtmlFormatter.h"
#include "../../Il/Io/HtmlFormatProvider.h"

bool CreateDirectoriesW(const char16* const pwszName) {
    if (::CreateDirectoryW(pwszName, nullptr)) {
        return true;
    } // if

    auto const dwError = ::GetLastError();

    switch (dwError) {
    case ERROR_ALREADY_EXISTS:
        return true;

    case ERROR_PATH_NOT_FOUND: {
        char16 wsz[MAX_PATH];
        ::lstrcpyW(wsz, pwszName);
        if (auto const pwszSlash = ::lstrrchrW(wsz, '\\')) {
            *pwszSlash = 0;

        } else if (auto const pwszSlash = ::lstrrchrW(wsz, '/')) {
            *pwszSlash = 0;

        } else {
            return false;
        } // if

        return CreateDirectoriesW(wsz) && ::CreateDirectoriesW(pwszName);
    } // ERROR_PATH_NOT_FOUND
    } // swtich

    return false;
} // CreateDirectories

namespace Compiler {

using Common::Io::StreamWriter;

int Logger::s_cLogs;

namespace {

class Static {

    public: static LogWriter* CreateWriter(
        CompileSession* const pSession,
        const int cLogs,
        const char16* pwszMode) {

        ASSERT(pSession != nullptr);
        ASSERT(pwszMode != nullptr);

        char16 wszLogFileName[MAX_PATH];

        ::wsprintfW(wszLogFileName, L"%ls.log",
            pSession->GetOutputFileName());

        ::CreateDirectoriesW(wszLogFileName);

        ::wsprintfW(wszLogFileName, L"%ls\\%03d.%ls.html",
            wszLogFileName,
            cLogs,
            pwszMode);

        auto const pTextWriter = new StreamWriter(wszLogFileName);

        auto const pWriter = new LogWriter(
            pTextWriter,
            new HtmlFormatProvider());

        Static::WriterHeader(pWriter, pwszMode);

        return pWriter;
    } // CreateWriter

    public: static LogWriter* CreateWriter(
        CompileSession* const pSession,
        const int cLogs,
        const char16* pwszMode,
        const MethodDef* pMethodDef)  {

        ASSERT(pwszMode != nullptr);
        ASSERT(pMethodDef != nullptr);

        const SourceInfo* pSourceInfo = pMethodDef->GetSourceInfo();
        auto const source_file_name = pSourceInfo->file_name();

        char16 wszLogFileName[MAX_PATH];

        ::wsprintfW(wszLogFileName, L"%ls.log\\%ls",
            pSession->GetOutputFileName(),
            source_file_name.value());

        ::CreateDirectoriesW(wszLogFileName);

        ::wsprintfW(wszLogFileName, L"%ls\\%ls.%ls.%d.%03d.%ls.html",
            wszLogFileName,
            pMethodDef->GetOwnerClassDef()->name().ToString().value(),
            pMethodDef->name().ToString().value(),
            pMethodDef->GetIndex(),
            cLogs,
            pwszMode);

        auto const pTextWriter = new StreamWriter(wszLogFileName);

        auto const pWriter = new LogWriter(pTextWriter, new HtmlFormatProvider());

        Static::WriterHeader(pWriter, pwszMode, pMethodDef);

        return pWriter;
    } // CreateWriter

    private: static void WriterHeader(
        LogWriter* const pWriter,
        const char16* pwszMode) {

        ASSERT(pWriter != nullptr);
        ASSERT(pwszMode != nullptr);

        WritePrologue(pWriter, pwszMode);
    } // WriterHeader

    private: static void WriterHeader(
        LogWriter* const pWriter,
        const char16* pwszMode,
        const MethodDef* pMethodDef)  {

        ASSERT(pMethodDef != nullptr);
        ASSERT(pWriter != nullptr);
        ASSERT(pwszMode != nullptr);

        char16 wszTitle[100];
        ::wsprintfW(wszTitle, L"%ls %ls.%ls.%d",
            pwszMode,
            pMethodDef->GetOwnerClassDef()->name().ToString().value(),
            pMethodDef->name().ToString().value(),
            pMethodDef->GetIndex());

        WritePrologue(pWriter, wszTitle);

        if (auto const pFun = pMethodDef->GetFunction()) {
            pWriter->StartElement("p");

            pWriter->Write("See instructions of %p.",
                pFun->module());

            pWriter->EndElement("p");
        } // if
    } // WriterHeader

    private: static void WritePrologue(
        LogWriter* const pWriter,
        const char16* const pwszTitle) {

        ASSERT(pwszTitle != nullptr);
        ASSERT(pWriter != nullptr);
        ASSERT(0 != pwszTitle);

        char szNow[100];
        {
            SYSTEMTIME stNow;
            ::GetLocalTime(&stNow);

            ::wsprintfA(szNow, "%d-%02d-%02d %02d:%02d:%02d",
                stNow.wYear,
                stNow.wMonth,
                stNow.wDay,
                stNow.wHour,
                stNow.wMinute,
                stNow.wSecond);
        }

        char16 wszTitle[100];
        ::wsprintfW(wszTitle, L"%ls  %hs",
            pwszTitle,
            szNow);

        pWriter->StartElement("html");
        pWriter->StartElement("head");
        pWriter->WriteElement("title", wszTitle);

        pWriter->StartElement("style", "type", L"text/css");
        pWriter->WriteString(
            ".alert {"
                "color: #ff0000;"
            "}" // alert

            ".b {" // bool
                "color: #000099;"
            "}" // bool

            ".f {" // field
                "color: #990099;"
            "}" // field

            ".i { font-family: Consolas; }" // instruction

            ".l {" // literal
                "color: #990000;"
                "font-family: Consolas;"
            "}" // literal

            ".m {" // method ref and method group ref
                "color: #990000;"
            "}"

            ".n {" // nameref
                "color: #990000;"
            "}" // nameref

            ".s {" // namespace
                "color: #009900;"
            "}" // literal

            ".o { color: #7777cc; }"        // output

            ".t {"  // type
                "color: #009999;"
                "font-style: italic;"
            "}"    // type

            ".u {"  // unresolved type
                "color: #990000;"
                "font-style: italic;"
            "}"    // type

            ".v { font-weight: bold; }"     // variable

            "body {"
            "   font-size: 10pt;"
            "   font-family: Tahoma;"
            "}"

            "h1 { font-size: 160%; }"   // filename
            "h2 {"
                "border-top: 5px solid #009900;"
                "font-size: 150%;"
            "}"   // Module
            "h3 { font-size: 140%; }"   // Module section
            "h4 { font-size: 130%; }"   // funciton
            "h5 { font-size: 120%; }"   // Function section
            "h6 { font-size: 110%; }"   // bblock
            "h7 { font-size: 100%; font-weight: bold; }"   // bblock section
            "table {"
                "border-collapse: collapse;"
                "font-size: 10pt;"
            "}"
            );

        pWriter->EndElement("style");
        pWriter->EndElement("head");
        pWriter->StartElement("body");
        pWriter->WriteElement("h1", wszTitle);
    } // WritePrologue
}; // Static
} // namespace

// ctor
Logger::Logger(
    CompileSession* const pSession,
    const char16* pwszMode,
    const MethodDef* pMethodDef) :
        m_fOwnWriter(true),
        m_pSession(pSession),
        m_pWriter(
            Static::CreateWriter(pSession, ++s_cLogs, pwszMode, pMethodDef)) {
    ASSERT(m_pSession != nullptr);
} // Logger

Logger::Logger(
    CompileSession* const pSession,
    const char16* pwszMode) :
        m_fOwnWriter(true),
        m_pSession(pSession),
        m_pWriter(Static::CreateWriter(pSession, ++s_cLogs, pwszMode)) {
    ASSERT(m_pSession != nullptr);
} // Logger

Logger::Logger(
    CompilePass* const pCompilePass,
    const char16* pwszMode,
    const MethodDef* pMethodDef) :
        m_fOwnWriter(true),
        m_pSession(pCompilePass->GetCompileSession()),
        m_pWriter(
            Static::CreateWriter(
                pCompilePass->GetCompileSession(),
                ++s_cLogs,
                pwszMode,
                pMethodDef)) {
    ASSERT(m_pSession != nullptr);
} // Logger

Logger::~Logger() {
    if (m_pWriter != nullptr) {
        m_pWriter->EndElement("body");
        m_pWriter->EndElement("html");
        m_pWriter->Close();

        if (m_fOwnWriter) {
            delete m_pWriter->GetFormatProvider();
            delete m_pWriter->GetFormatter();
            delete m_pWriter->GetTextWriter();
            delete m_pWriter;
        } // if
    } // if
} // ~Logger

// [D]
void Logger::Dump(const Module* const pModule) {
    ASSERT(pModule != nullptr);
    if (this->GetWriter() != nullptr) {
        HtmlDumper oDumper(this->GetWriter());
        oDumper.Dump(pModule);
    } // if
} // Dump

// [E]
void Logger::EndLogSection() {
} // EndLogSection

// [S]
void Logger::StartLogSectionV(const char*, va_list) {
} // StartLogSectionV

// [W]
void Logger::Write(const char* const pszFormat, ...) {
    if (m_pWriter == nullptr) {
        return;
    } // if
    va_list args;
    va_start(args, pszFormat);
    m_pWriter->WriteV(pszFormat, args);
    va_end(args);
} // Write

} // Compiler
