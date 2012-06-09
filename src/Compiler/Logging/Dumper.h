// @(#)$Id$
//
// Evita Compiler
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Compiler_Logging_Dumper_h)
#define INCLUDE_Compiler_Logging_Dumper_h

#include "../../Common/Io.h"

namespace Compiler {

using Common::Io::TextWriter;

class Dumper {
  private: const CompilationUnit& cm_unit_;
  private: TextWriter& writer_;

  // ctor
  public: Dumper(TextWriter&, const CompilationUnit&);

  // [D]
  public: void Dump();
  private: void DumpClass(const ClassDef&);
  private: void DumpFunction(const Function&);
  private: void DumpInstruction(const Instruction&);
  private: void DumpLiteral(const Literal&);
  private: void DumpMethod(const MethodDef&);

  // [W]
  private: void WriteMethodRef(const MethodDef&);

  DISALLOW_COPY_AND_ASSIGN(Dumper);
}; // Dumper

} // Compiler

#endif // !defined(INCLUDE_Compiler_Logging_Dumper_h)
