#include "precomp.h"
// @(#)$Id: /proj/evcl3/boot/precomp.h 13 2006-07-29 01:55:00 yosi $
//
// Evia Compiler - Main
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)
//

#include "../Common/GlobalMemoryZone.h"
#include "../Common/Collections.h"
#include "../Common/String.h"
#include "../Common/Io.h"

#include "./CompilerGlobal.h"
#include "./CompilePass.h"
#include "./CompileSession.h"
#include "./FileCompiler.h"
#include "./VisitFunctor.h"

#include "./Ir/ClassDef.h"
#include "./Ir/MethodDef.h"

#include "./Logging/Dumper.h"


#include "../Il/Ir.h"

#include "../Il/Fasl/FastWriter.h"
#include "../Il/Fasl/FastLoader.h"

#include "../Il/Tasks/Cfg2SsaTask.h"
#include "../Il/Tasks/CleanTask.h"
#include "../Il/Tasks/ClosureTask.h"
#include "../Il/Tasks/FinalTask.h"
#include "../Il/Tasks/VerifyTask.h"

using namespace Common;
using namespace Common::Collections;
using namespace Common::Io;
using namespace Compiler;
using namespace Il::Fasl;
using namespace Il::Ir;
using namespace Il::Tasks;

namespace {

char16 const k_wszTitle[] = L"Evita Compiler";

// EnumParam enumerates command line arguments.
class EnumParam {
  private: StringBuilder builder_;
  private: const char16* cur_;

  public: EnumParam(const char16* const pwsz) : cur_(pwsz) {
    if (!AtEnd()) {
      Next();
    }
  }

  public: bool AtEnd() const {
    return *cur_ == 0 && builder_.IsEmpty();
  }

  public: String Get() const {
    ASSERT(!AtEnd());
    ASSERT(!builder_.IsEmpty());
    return builder_.ToString();
  }

  private: static bool IsSpace(char16 const wch) {
    return wch == ' ' || wch == '\t';
  }

  public: void Next() {
    ASSERT(!AtEnd());

    enum State {
      State_DoubleQuote,
      State_Start,
      State_Word,
    } state = State_Start;

    builder_.Clear();
    for (;;) {
      auto ch = *cur_;
      if (ch == 0) {
        break;
      }
      cur_++;

      switch (state) {
        case State_DoubleQuote:
          if (ch == '"') {
            return;
          }
          builder_.Append(ch);
          break;

        case State_Start:
          if (ch == '"') {
            state = State_DoubleQuote;
          } else if (!IsSpace(ch)) {
            builder_.Append(ch);
            state = State_Word;
          }
          break;

       case State_Word:
         if (IsSpace(ch)) {
           return;
         }
         builder_.Append(ch);
         break;

       default:
         CAN_NOT_HAPPEN();
      }
    }
  }

  DISALLOW_COPY_AND_ASSIGN(EnumParam);
};

class TypeCollector : public VisitFunctor {
  private: FastWriter& writer_;

  public: TypeCollector(FastWriter& writer) : writer_(writer) {}

  public: virtual void Process(ClassDef* const class_def) override {
    DEBUG_FORMAT("add ClassDef %s", class_def);
    writer_.Add(class_def->GetClass());
    VisitFunctor::Process(class_def);
  }

  DISALLOW_COPY_AND_ASSIGN(TypeCollector);
}; // TypeCollector

}

using namespace Compiler;

static TextWriter* StdErr;
static TextWriter* StdOut;

struct PassEntry {
  const char* name_;
  ModuleTask::NewFunction ctor_;
};

class TimeReporter {
  private: String name_;
  private: uint start_;
  public: TimeReporter(const String name)
    : name_(name), start_(::GetTickCount()) {}
  public: ~TimeReporter() {
    auto const end = ::GetTickCount();
    StdErr->WriteLine("  %s %dms", name_, static_cast<uint32>(end - start_));
  }
};

class Static {
  private: class Optimizer : public VisitFunctor {
    private: const HashSet_<String>& dump_set_;
    private: int pass_counter_;
    private: CompileSession& session_;

    private: static PassEntry s_rgpPass[];

    // ctor
    public: Optimizer(
        CompileSession& session,
        const HashSet_<String>& dump_set)
        : dump_set_(dump_set),
          pass_counter_(0),
          session_(session) {}

    // [D]
    private: void DumpPass(const String& name) {
      Dump(session_, String::Format("%s-%s", name, pass_counter_));
    }

    // [P]
    public: virtual void Process(MethodDef* const method_def) override {
      auto const fun = method_def->function();
      if (fun == nullptr) {
          return;
      }

      auto& module = fun->module();
      for (auto runner = s_rgpPass; runner->name_; runner++) {
        ++pass_counter_;
        {
          TimeReporter timing(runner->name_);
          ScopedPtr_<ModuleTask> task(&runner->ctor_(session_, module));
          task->Start();
        }

        if (session_.HasError()) {
          DEBUG_FORMAT("Abort pass %s", runner->name_);
          DumpPass(runner->name_);
          SourceInfo source_info;
          session_.AddError(
              method_def->source_info(),
              CompileError_Pass_Abort,
              runner->name_);
          break;
        }

        {
          TimeReporter timing("verify");
          VerifyTask task(session_, module);
          task.Start();
        }

        if (session_.HasError()) {
          DEBUG_FORMAT("Verification failed %s", runner->name_);
          DumpPass(runner->name_);
          SourceInfo source_info;
          session_.AddError(
              method_def->source_info(),
              CompileError_Pass_Fail,
              runner->name_);
          break;
        }

        if (dump_set_.Contains(runner->name_)) {
          DumpPass(runner->name_);
        }
      }
    }

    DISALLOW_COPY_AND_ASSIGN(Optimizer);
  }; // Optimizer

  // [D]
  public: static void Dump(CompileSession& session, const String& name) {
    foreach (CompileSession::EnumCompilationUnit, units, session) {
      auto& unit = *units.Get();

      StreamWriter file(
        String::Format("%s.%s.log.html", unit.source_path(), name));

      Dumper dumper(file, unit);
      dumper.Dump();
    }
  }

  // [I]
  public: static bool IsSucceeded(CompileSession& session) {
    if (!session.HasError()) {
      return true;
    }

    StringBuilder oBuilder;
    foreach (CompileSession::EnumError, errors, session) {
      auto& error = *errors.Get();
      StdOut->WriteLine("%s", error);
    }

    StdOut->WriteLine("Compilation failed.");
    return false;
  }

  // [L]
  public: static void LoadFasl(
      CompileSession& session,
      const String& file_name) {

    Il::Fasl::FastLoader loader(session, *Namespace::Global, file_name);

    {
      FileStream input(file_name, FileMode_Open, FileAccess_Read,
                       FileShare_Read);

      uint8 buf[1024];
      while (auto const count = input.Read(buf, sizeof(buf))) {
        loader.Feed(buf, count);
      }
    }

    if (loader.HasError()) {
      StdErr->WriteLine("Failed to load %s", file_name);
      return;
    }
  }

  // [O]
  public: static bool Optimize(
      CompileSession& session, 
      const HashSet_<String>& dump_set) {
    Optimizer optimizer(session, dump_set);
    session.Apply(&optimizer);
    return IsSucceeded(session);
  }

  // [U]
  public: static void Usage() {
    StdErr->WriteLine("Usage: evc option... file...");
    StdErr->WriteLine("Options:");
    StdErr->WriteLine(" -dump pass-name");
    StdErr->WriteLine(" -o output-file");
    StdErr->WriteLine(" -r reference-module-file");
  }

  // [W]
  public: static void WriteToFasl(
      CompileSession& session,
      const String& file_name) {
    ASSERT(!file_name.IsEmpty());

    TimeReporter timing("output");

    FileStream output(file_name, FileMode_Create, FileAccess_Write);

    Il::Fasl::FastWriter writer(
        session,
        output,
        session.global_namespace());

    TypeCollector collector(writer);
    session.Apply(&collector);
    writer.Finish();
  }
}; // Static

#define PASS_ENTRY(name) { #name, name :: Create }

PassEntry Static::Optimizer::s_rgpPass[] = {
  PASS_ENTRY(CleanTask),
  PASS_ENTRY(ClosureTask),
  PASS_ENTRY(Cfg2SsaTask),
  PASS_ENTRY(CleanTask),  // remove Phi instructions created by CFG to SSA.
  PASS_ENTRY(FinalTask),
  nullptr,
}; // s_rgpPass

// Main driver
static int CompilerMain() {
  TimeReporter timing("CompilerMain");

  ArrayList_<String> source_files;
  ArrayList_<String> ref_files;
  ArrayList_<String> link_files;

  String command_name;
  HashSet_<String> dump_set;
  String output_file;

  {
    enum State {
      State_Dump,
      State_File,
      State_Output,
      State_Ref,
      State_Start,
    }; // State

    auto state = State_Start;

    foreach (EnumParam, params, ::GetCommandLineW()) {
      auto param = params.Get();

      switch (state) {
        case State_File:
          if (param == "-dump") {
            state = State_Dump;
            break;
          }

          if (param == "-o") {
            state = State_Output;
            break;
          }

          if (param == "-r") {
            state = State_Ref;
            break;
          }

          if (param.EndsWith(".evo")) {
            link_files.Add(param);
            ref_files.Add(param);
            break;
          }

          if (param.EndsWith(".ev")) {
            source_files.Add(param);
            break;
          }

          StdErr->WriteLine("Unsupported file type: %s", param);
          return 1;

        case State_Dump:
          dump_set.Add(param);
          state = State_File;
          break;

        case State_Output:
          output_file = param;
          state = State_File;
          break;

        case State_Ref:
          ref_files.Add(param);
          state = State_File;
          break;

        case State_Start:
          command_name = param;
          state = State_File;
          break;

        default:
          CAN_NOT_HAPPEN();
      }
    }

    if (state != State_File) {
      Static::Usage();
      return 1;
    }
  }

  if (source_files.IsEmpty() && link_files.IsEmpty()) {
    StdErr->WriteLine("No input file.");
    return 1;
  }

  // Set default output file name.
  if (output_file.IsEmpty()) {
    auto const pFirstFile = source_files.IsEmpty()
        ? link_files.Get(0)
        : source_files.Get(0);

    StringBuilder builder;
    auto const iDotPosn = pFirstFile.LastIndexOf('.');
    if (iDotPosn > 0) {
      builder.Append(pFirstFile.value(), iDotPosn);
    } else {
      builder.Append(pFirstFile);
    }

    builder.Append(".evo");
    output_file = builder.ToString();
  }

  if (::IsDebuggerPresent()) {
    dump_set.Add("Parse");
    dump_set.Add("FinalTask");
  }

  CompileSession session(output_file);

  // Compile source files
  foreach (ArrayList_<String>::Enum, files, &source_files) {
    FileCompiler compiler(session);
    TimeReporter timing("compile");
    compiler.CompileFile(*files);
  }

  if (dump_set.Contains("Parse")) {
    Static::Dump(session, "Parse");
  }

  if (!Static::IsSucceeded(session)) {
    return 1;
  }

  // Load reference files
  foreach (ArrayList_<String>::Enum, files, &ref_files) {
    auto const file = files.Get();
    TimeReporter timing(String::Format("load %s", file));
    Static::LoadFasl(session, file);
    if (!Static::IsSucceeded(session)) {
      return 1;
    }
  }

  foreach (
      Collection_<CompilePass::NewFunction>::Enum,
      pass_ctors, 
      CompilePass::GetPasses()) {
    auto const ctor = pass_ctors.Get();
    ScopedPtr_<CompilePass> pass(ctor(&session));
    TimeReporter timing(pass->name());
    pass->Start();

    if (!Static::IsSucceeded(session)) {
      Static::Dump(session, pass->name());
      return 1;
    }

    if (dump_set.Contains(pass->name())) {
      Static::Dump(session, pass->name());
    }
  }

  if (!Static::Optimize(session, dump_set)) {
      return 1;
  }

  Static::WriteToFasl(session, output_file);
  if (!Static::IsSucceeded(session)) {
    Static::Dump(session, "Fasl");
    return 1;
  }

  return 0;
}

static void CrtMain() {
  CompilerGlobal::Init();

  StdOut = new StreamWriter(
      *new HandleStream(::GetStdHandle(STD_OUTPUT_HANDLE)));

  StdErr = new StreamWriter(
      *new HandleStream(::GetStdHandle(STD_ERROR_HANDLE)));

  auto const exit_code = CompilerMain();

  StdOut->Flush();
  StdErr->Flush();

  if (::IsDebuggerPresent()) {
    if (exit_code) {
      char16 wsz[100];
      ::wsprintfW(wsz, L"Exit Code %d", exit_code);
      ::MessageBoxW(nullptr, wsz, k_wszTitle, MB_ICONEXCLAMATION);
    }
  }

  ::ExitProcess(exit_code);
}

#if _NDEBUG
extern "C" int __cdecl mainCRTStartup() {
  CrtMain();
}
#else
int __cdecl wmain(int, wchar_t**) {
CrtMain();
}
#endif
