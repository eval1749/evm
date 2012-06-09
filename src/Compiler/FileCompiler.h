// @(#)$Id$
//
// Evita Compiler - File Compiler
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Evita_Compiler_FileCompiler_h)
#define INCLUDE_Evita_Compiler_FileCompiler_h

namespace Compiler {

class CompileSession;

class FileCompiler {
  private: CompileSession& session_;

  // ctor
  public: FileCompiler(CompileSession&);


  // [C]
  public: void CompileFile(const String&);

  // [F]
  private: void FatalError(const char*, ...);

  DISALLOW_COPY_AND_ASSIGN(FileCompiler);
}; // FileCompiler

} // Compiler

#endif // !defined(INCLUDE_Evita_Compiler_FileCompiler_h)
