#include "precomp.h"
// @(#)$Id$
//
// Evita Compiler - CompileSession
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./CompileSession.h"

#include "./Functor.h"
#include "./Ir/NamespaceDef.h"

namespace Compiler {

#if defined(_DEBUG)
namespace {
class Verifier : public Il::Ir::Functor {
  private: int hash_code_;

  public: Verifier() : hash_code_(0) {}
  public: virtual ~Verifier() {}

  public: virtual void Process(FunctionType* const funty) override {
    hash_code_ = funty->ComputeHashCode();
  }

  public: static void Run() {
    Verifier visitor;
    FunctionType::MapAll(visitor);
  }

  DISALLOW_COPY_AND_ASSIGN(Verifier);
};
} // namesapce
#endif

// ctor
CompileSession::CompileSession(const String& output_file_name) 
    : global_ns_def_(
          NamespaceDef::CreateGlobalNamespaceDef(memory_zone_)),
      output_file_name_(output_file_name) {
  #if defined(_DEBUG)
    Verifier::Run();
  #endif
}

CompileSession::~CompileSession() {
  DEBUG_PRINTF("%p %ls\n", this, output_file_name_.value());
  delete &global_ns_def_;

  while (!compilation_units_.IsEmpty()) {
    delete compilation_units_.Pop();
  }

  while (!errors_.IsEmpty()) {
    delete errors_.Pop();
  }

  #if defined(_DEBUG)
    Verifier::Run();
  #endif
}

Namespace& CompileSession::global_namespace() const {
  return global_ns_def_.namespaze();
}

NamespaceDef& CompileSession::global_namespace_def() const {
  return global_ns_def_;
}

MemoryZone& CompileSession::memory_zone() const {
  return const_cast<CompileSession*>(this)->memory_zone_;
}

const String& CompileSession::output_file_name() const { 
  return output_file_name_;
}

// [A]
void CompileSession::Add(CompilationUnit& unit) {
  compilation_units_.Append(&unit);
}

void CompileSession::AddError(const SourceInfo& source_info,
                              CompileError const error_code,
                              Box a) {
  AddError(source_info, error_code, CollectionV_<Box>(a));
}

void CompileSession::AddError(const SourceInfo& source_info,
                              CompileError const error_code,
                              Box a, Box b) {
  AddError(source_info, error_code, CollectionV_<Box>(a, b));
}


void CompileSession::AddError(const SourceInfo& source_info,
                              CompileError const error_code,
                              Box a, Box b, Box c) {
  AddError(source_info, error_code, CollectionV_<Box>(a, b, c));
}

void CompileSession::AddError(
    const SourceInfo& source_info,
    CompileError const error_code,
    const Collection_<Box>& params) {
  RecordErrorInfo(CompileErrorInfo(source_info, error_code, params));
}

void CompileSession::AddErrorInfo(const ErrorInfo& error_info) {
  RecordErrorInfo(error_info.Clone());
}

void CompileSession::AddWarning(const SourceInfo& source_info,
                                CompileError const error_code,
                                Box a) {
  AddWarningInfo(source_info, error_code, CollectionV_<Box>(a));
}

void CompileSession::AddWarning(const SourceInfo& source_info,
                                CompileError const error_code,
                                Box a, Box b) {
  AddWarningInfo(source_info, error_code, CollectionV_<Box>(a, b));
}


void CompileSession::AddWarning(const SourceInfo& source_info,
                                CompileError const error_code,
                                Box a, Box b, Box c) {
  AddWarningInfo(source_info, error_code, CollectionV_<Box>(a, b, c));
}

void CompileSession::AddWarningInfo(
    const SourceInfo& source_info,
    CompileError const error_code,
    const Collection_<Box>& params) {
  RecordWarningInfo(CompileErrorInfo(source_info, error_code, params));
}

void CompileSession::Apply(Functor* const functor) {
  ASSERT(functor != nullptr);
  functor->Process(this);
}

// [R]
void CompileSession::RecordErrorInfo(const ErrorInfo& templ) {
  static bool fContinue = false;

  if (!fContinue && ::IsDebuggerPresent()) {
    __debugbreak();
    fContinue = true;
  }

  if (error_set_.Contains(&templ)) {
    return;
  }

  auto& error_info = templ.Clone();

  error_set_.Add(&error_info);

  auto ref = errors_.GetLast();
  while (ref) {
    if (error_info.source_info() > ref->source_info()) {
      break;
    }
    ref = ref->GetPrev();
  }
  if (ref) {
    errors_.InsertAfter(&error_info, ref);
  } else {
    errors_.Prepend(&error_info);
  }
}

void CompileSession::RecordWarningInfo(const ErrorInfo& error_info) {
  warnings_.Append(&error_info.Clone());
}

} // Compiler
