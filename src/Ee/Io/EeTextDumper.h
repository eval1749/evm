// @(#)$Id$
//
// Evita Compiler
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_evm_Ee_Io_EeTextDumper_h)
#define INCLUDE_evm_Ee_Io_EeTextDumper_h

#include "../../Common/Io.h"

namespace Executor {

using Common::Io::TextWriter;
using namespace Il::Ir;

class EeTextDumper {
  private: TextWriter& writer_;

  // ctor
  public: EeTextDumper(TextWriter&);

  // [D]
  public: void DumpClass(const Class&);
  public: void DumpFunction(const Function&);
  private : void DumpInstruction(const Instruction&);
  public: void DumpMethod(const Method&);
  private: void DumpLiteral(const Literal&);
  public: void DumpMethodGroup(const MethodGroup&);

  DISALLOW_COPY_AND_ASSIGN(EeTextDumper);
};

} // Executor

#endif // !defined(INCLUDE_evm_Ee_Io_EeTextDumper_h)
