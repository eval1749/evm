// @(#)$Id$
//
// Evita Compiler
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_evc_CompilationUnit_h)
#define INCLUDE_evc_CompilationUnit_h

#include "./Ir/Attribute.h"
#include "./SourceBuffer.h"

namespace Compiler {

class CompileSession;
class Functor;
class NamespaceBody;
class NamespaceDef;

// This class represents compilation unit, e.g. source code file.
class CompilationUnit : public DoubleLinkedItem_<CompilationUnit> {
  public: typedef DoubleLinkedList_<CompilationUnit> List;

  private: Attributes attributes_;
  private: NamespaceBody& global_ns_body_;
  private: CompileSession& session_;
  private: SourceBuffer source_;
  private: const String source_path_;

  // ctor
  public: CompilationUnit(CompileSession&, const String&);
  public: ~CompilationUnit();

  public: NamespaceBody& namespace_body() const { return global_ns_body_; }
  public: CompileSession& session() const { return session_; }
  public: const String& source_path() const { return source_path_; }

  // [A]
  public: void AddSource(const char16*, size_t);
  public: void Apply(Functor* const pFunctor);

  // [G]
  public: String GetLine(int) const;

  // [F]
  public: void FinishSource();

  DISALLOW_COPY_AND_ASSIGN(CompilationUnit);
}; // CompilationUnit

} // Compiler

#endif // !defined(INCLUDE_evc_CompilationUnit_h)
