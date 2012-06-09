// @(#)$Id$
//
// Evita Compiler
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_evc_CompileSession_h)
#define INCLUDE_evc_CompileSession_h

#include "./CompileErrorInfo.h"
#include "./CompilationUnit.h"
#include "../Il/Tasks/Session.h"

namespace Compiler {

using Common::Box;
using Common::Collections::Collection_;

class CompilationUnit;
class Functor;
class NamespaceDef;

// TODO(yosi) 2012-01-07 Unintern ArrayType, FunctionType, PointerType.
class CompileSession : public Il::Tasks::Session {
  public: class EnumCompilationUnit :
          public CompilationUnit::List::Enum {
      private: typedef CompilationUnit::List::Enum Base;
      public: EnumCompilationUnit(const CompileSession& r) :
          Base(&const_cast<CompileSession&>(r).compilation_units_) {}
  };

  public: class EnumError : public ErrorInfo::List::Enum {
    private: typedef ErrorInfo::List::Enum Base;
    public: EnumError(const CompileSession& r) : Base(r.errors_) {}
  };

  public: class EnumWarning : public ErrorInfo::List::Enum {
    private: typedef ErrorInfo::List::Enum Base;
    public: EnumWarning(const CompileSession& r) : Base(r.warnings_) {}
  };

  private: HashSet_<HashKey_<const ErrorInfo>> error_set_;
  private: ErrorInfo::List errors_;
  private: LocalMemoryZone memory_zone_;
  private: CompilationUnit::List compilation_units_;
  private: NamespaceDef& global_ns_def_;
  private: const String output_file_name_;
  private: ErrorInfo::List warnings_;

  // ctor
  public: CompileSession(const String&);
  public: virtual ~CompileSession();

  public: Namespace& global_namespace() const;
  public: NamespaceDef& global_namespace_def() const;
  public: MemoryZone& memory_zone() const;
  public: const String& output_file_name() const;

  // [A]
  public: void Add(CompilationUnit&);

  public: void AddError(const SourceInfo&, CompileError);
  public: void AddError(const SourceInfo&, CompileError, Box);
  public: void AddError(const SourceInfo&, CompileError, Box, Box);
  public: void AddError(const SourceInfo&, CompileError, Box, Box, Box);

  public: void CompileSession::AddError(
      const SourceInfo&,
      CompileError,
      const Collection_<Box>&);

  public: virtual void AddErrorInfo(const ErrorInfo&) override;

  private: virtual void AddWarningInfo(
      const SourceInfo&,
      CompileError,
      const Collection_<Box>&);

  public: void AddWarning(const SourceInfo&, CompileError);
  public: void AddWarning(const SourceInfo&, CompileError, Box);
  public: void AddWarning(const SourceInfo&, CompileError, Box, Box);
  public: void AddWarning(const SourceInfo&, CompileError, Box, Box, Box);

  public: void Apply(Functor*);

  // [H]
  public: virtual bool HasError() const override { 
    return !errors_.IsEmpty(); 
  }

  // [R]
  private: void RecordErrorInfo(const ErrorInfo&);
  private: void RecordWarningInfo(const ErrorInfo&);

  DISALLOW_COPY_AND_ASSIGN(CompileSession);
};

} // Compiler

#endif // !defined(INCLUDE_evc_CompileSession_h)
