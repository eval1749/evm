// @(#)$Id$
//
// Evita Compiler
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Compiler_Ir_NamespaceBody_h)
#define INCLUDE_Compiler_Ir_NamespaceBody_h

#include "./NamespaceMember.h"

#include "./AliasDef.h"
#include "./NameDef.h"
#include "./ClassOrNamespaceDef.h"
#include "./UsingNamespace.h"

#include "../../Common/Collections/HashMap_.h"
#include "../../Common/Collections/PtrKey_.h"

namespace Compiler {

class AliasDef;
class CompilationUnit;
class CompileSession;
class NamespaceDef;
class Using;
class Functor;

/// <summary>
///  The class NamespaceBody contains alias definitions, namespace
///  defintion and type definitions.
/// </summary>
class NamespaceBody : public Entity_<NamespaceBody, NamespaceMember> {
  CASTABLE_CLASS(NamespaceBody);

  // NameTable contains
  //    alias declaration
  //    namespace
  //    type
  // Note: namespace and type are also found in NamespaceDef::Find.
  private: typedef HashMap_<const Name*, NameDef*> NameTable;

  public: class EnumOuterAndSelf {
    private: NamespaceBody* cur_;

    public: EnumOuterAndSelf(const NamespaceBody& r)
        : cur_(&const_cast<NamespaceBody&>(r)) {}

    public: bool AtEnd() const { return cur_ == nullptr; }
    public: NamespaceBody& Get() const { ASSERT(!AtEnd()); return *cur_; }
    public: void Next() { ASSERT(!AtEnd()); cur_ = cur_->outer(); }
  };

  public: class EnumUsingNamespace : public UsingNamespace::List::Enum {
    public: EnumUsingNamespace(const NamespaceBody& r)
        : UsingNamespace::List::Enum(&r.usings_) {}
  };

  private: CompilationUnit& compilation_unit_;
  private: NamespaceMember::List members_;
  private: NamespaceDef& namespace_def_;
  private: NameTable name_table_;
  private: NamespaceBody* const outer_;
  private: UsingNamespace::List usings_;

  public: NamespaceBody(
      CompilationUnit& compliation_unit,
      NamespaceBody* const outer,
      NamespaceDef& namespace_def,
      const SourceInfo* const source_info = nullptr);

  // properties
  public: CompilationUnit& compilation_unit() const {
    return compilation_unit_;
  }

  public: const NamespaceMember::List& members() const { return members_; }
  public: NamespaceDef& namespace_def() const { return namespace_def_;  }
  public: NamespaceBody* outer() const { return outer_;  }

  // [A]
  public: void Add(NameDef&);
  public: void Add(UsingNamespace&);

  // [F]
  public: NameDef* Find(const Name&) const;

  DISALLOW_COPY_AND_ASSIGN(NamespaceBody);
}; // NamespaceBody

} // Compiler

#endif // !defined(INCLUDE_Compiler_Ir_NamespaceBody_h)
