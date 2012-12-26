// @(#)$Id$
//
// Evita Compiler
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Compiler_CompilePass_h)
#define INCLUDE_Compiler_CompilePass_h

#include "../Il/Tasks/Task.h"

namespace Common {
    namespace Io {
        class StreamWriter;
    } // Io
} // Common

namespace Compiler {

class CompilationUnit;
enum CompileError : int;
class CompileSession;
class MethodDef;

using Common::Io::StreamWriter;

class CompilePass : public Il::Tasks::Task {
  public: typedef CompilePass* (LibExport *NewFunction)(
      CompileSession* const session);

  private: CompileSession& compile_session_;
  private: StreamWriter* writer_;

  // ctor
  protected: CompilePass(const String&, CompileSession*);
  public: virtual ~CompilePass();

  // properties
  public: CompileSession& compile_session() const { return compile_session_; }

  // [G]
  public: static Collection_<NewFunction> LibExport GetPasses();

  // [V]
  public: bool Verify();

  DISALLOW_COPY_AND_ASSIGN(CompilePass);
}; // CompilePass

template<class Self, class Parent = CompilePass>
class CompilePass_ : public Parent {
  protected: typedef CompilePass_<Self, Parent> Base;

  protected: CompilePass_(
      const char16* name,
      CompileSession* const session)
      : Parent(name, session) {
  }

  public: static CompilePass* LibExport Create(
      CompileSession* const session) {
    return new Self(session);
  }

  DISALLOW_COPY_AND_ASSIGN(CompilePass_);
};

template<class Self, class Parent, typename Param1>
class CompilePass1_ : public Parent {
  protected: typedef CompilePass1_<Self, Parent, Param1> Base;

  protected: CompilePass1_(
      const char16* name,
      CompileSession* const session,
      Param1 const param1)
      : Parent(name, session, param1) {
  }

  public: static CompilePass* LibExport Create(
      CompileSession* const session) {
    return new Self(session);
  }

  DISALLOW_COPY_AND_ASSIGN(CompilePass1_);
};

} // Compiler

#endif // !defined(INCLUDE_Compiler_CompilePass_h)
