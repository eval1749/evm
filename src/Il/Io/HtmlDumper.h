// @(#)$Id$
//
// Evita Il - IR - IL
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Ir_HtmlDumper_h)
#define INCLUDE_Il_Ir_HtmlDumper_h

#include "../Functor.h"

#include "../Io/HtmlFormatter.h"
#include "../Io/LogWriter.h"
#include "../Io/XhtmlWriter.h"

namespace Il {
namespace Io {

class LogWriter;

class HtmlDumper {
    private: LogWriter* const m_pWriter;

    // ctor
    public: explicit HtmlDumper(
        LogWriter* const pLogWriter);

    public: virtual ~HtmlDumper() {}

    // [D]
    protected: virtual void Dump(const BBlock* const pBBlock);
    protected: virtual void Dump(const CfgEdge* const pBBlock);
    protected: virtual void Dump(const Function* const pFunction);
    public: virtual void Dump(const Module* const pModule);
    protected: virtual void Dump(const Instruction* const pInstruction);
    public: virtual void Dump(const Method* const pMethod);
    protected: virtual void Dump(const Output* const pOutput);
    protected: virtual void Dump(const Variable* const pVariable);

    private: void DumpModule(const Module* const pModule);

    // [G]
    public: LogWriter* GetWriter() const { return m_pWriter; }

    // [W]
    public: virtual void WriteAnchor(const Output* const pOutput);
    public: virtual void WriteRef(const SourceInfo* const pSourceInfo);

    DISALLOW_COPY_AND_ASSIGN(HtmlDumper);
}; // HtmlDumper

} // Io
} // Il

#endif // !defined(INCLUDE_Il_Ir_HtmlDumper_h)
