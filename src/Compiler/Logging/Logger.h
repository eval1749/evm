// @(#)$Id$
//
// Evita Compiler
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Compiler_Logging_Logger_h)
#define INCLUDE_Compiler_Logging_Logger_h

#include "../../Common/Io.h"

namespace Compiler {

class CompilePass;
class CompileSession;
class MethodDef;
using Common::Io::StreamWriter;
using Il::Ir::Module;
using Il::Io::LogWriter;

class Logger {
    private: static int s_cLogs;

    public: class LogSection {
        private: Logger* const m_pLogger;

        public: LogSection(
            Logger* const pLogger,
            const char* const pszFormat, ...) :
                m_pLogger(pLogger) {
            va_list args;
            va_start(args, pszFormat);
            m_pLogger->StartLogSectionV(pszFormat, args);
            va_end(args);
        } // LogSection

        public: ~LogSection() {
            m_pLogger->EndLogSection();
        } // ~LogSection

        DISALLOW_COPY_AND_ASSIGN(LogSection);
    }; // LogSection

    private: bool m_fOwnWriter;
    private: CompileSession* const m_pSession;
    private: StreamWriter* m_pStreamWriter;
    private: LogWriter* m_pWriter;

    // ctor
    public: Logger(
        CompileSession* const pSession,
        const char16* pwszMode,
        const MethodDef* pMethodDef);

    public: Logger(
        CompileSession* const pSession,
        const char16* pwszMode);

    public: Logger(
        CompilePass* const pCompilePass,
        const char16* pwszMode,
        const MethodDef* pMethodDef);

    public: ~Logger();

    // [D]
    public: void Dump(const Module* const pModule);

    // [G]
    public: CompileSession* GetSession() const { return m_pSession; }
    public: LogWriter* GetWriter() const { return m_pWriter; }

    // [E]
    protected: void EndLogSection();

    // [S]
    protected: void StartLogSectionV(
        const char* const pszFormat,
        va_list args);

    // [W]
    public: void Write(const char* const pszFormat, ...);

    DISALLOW_COPY_AND_ASSIGN(Logger);
}; // Logger

} // Compiler

#endif // !defined(INCLUDE_Compiler_Logging_Logger_h)
