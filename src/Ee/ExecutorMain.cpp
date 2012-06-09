#include "precomp.h"
// @(#)$Id: /proj/evcl3/boot/precomp.h 13 2006-07-29 01:55:00 yosi $
//
// Evia Executor - Main
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)
//

#include "./ExecuteSession.h"

#include "../Common/Collections.h"
#include "../Common/String.h"
#include "../Common/Io.h"

#include "../Il.h"
#include "../Il/Fasl/FastLoader.h"

#include "./CodeDesc.h"
#include "./CodeManager.h"

#include "./Commands/CommandContext.h"
#include "./Commands/CommandManager.h"

#include "../Om.h"
#include "../Om/Object.h"
#include "../Om/Thread.h"

using namespace Common;
using namespace Common::Collections;
using namespace Common::Io;
using namespace Executor;
using namespace Il::Fasl;

namespace {

char16 const k_wszTitle[] = L"Evita Executor";

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

}

namespace Executor {

void Init() {
  Common::Init();
  Om::Init();
  Il::Init();
  Il::Cg::CgTargetInit();
  Ee::Init();
  Ee::RuntimeInit();
}

TextWriter* StdErr;
TextWriter* StdOut;

} // Executor

using namespace Executor;

class Static {
  // [I]
  public: static int Invoke(
      CommandContext& context,
      Collection_<String> args) {
    auto const module = context.EnsureModule();
    if (!module) {
      context.session().AddErrorInfo(
          ExecuteErrorInfo(
              SourceInfo(),
              ExecuteError_Invoke_NoModule,
              CollectionV_<Box>(context.name())));
      return 2;
    }

    Module::EnumFunction funs(*module);
    if (funs.AtEnd()) {
      context.session().AddErrorInfo(
          ExecuteErrorInfo(
              SourceInfo(),
              ExecuteError_Invoke_NoFunction,
              CollectionV_<Box>(context.name())));
      return 2;
    }

    auto const code_desc = funs.Get()->code_desc();
    if (!code_desc) {
      context.session().AddErrorInfo(
          ExecuteErrorInfo(
              SourceInfo(),
              ExecuteError_Invoke_NoCodeDesc,
              CollectionV_<Box>(context.name())));
      return 2;
    }

    Om::Thread thread;
    auto& funty = code_desc->function().function_type();
    if (funty.params_type().Count() == 1
        && funty.params_type().Get(0) == ArrayType::Intern(*Ty_String)) {
      auto& vec = thread.AllocVector(*Om::Ty_StringVector, args.Count())
          .StaticCast<Om::StringVector>();
      auto index = 0;
      foreach (Collection_<String>::Enum, en, args) {
        vec[index] = &thread.NewString(*en);
        ++index;
      }
      thread.value(0) = reinterpret_cast<intptr_t>(&vec);
    }
    code_desc->Invoke(thread);
    return funty.return_type() == *Ty_Int32
        ? thread.value(0)
        : 0;
  }

  public: static bool IsSucceeded(ExecuteSession& session) {
    if (!session.HasError()) {
      return true;
    }

    StringBuilder oBuilder;
    foreach (ExecuteSession::EnumError, errors, &session) {
      auto& error = *errors.Get();
      StdErr->WriteLine("%s", error);
    }
    return false;
  }

  // [L]
  public: static void LoadFasl(
      ExecuteSession& session,
      const String& file_name,
      FastLoader::Callback* callback = nullptr) {

    Il::Fasl::FastLoader loader(
        session, *Namespace::Global, file_name,  callback);

    {
      FileStream input(
        file_name,
        FileMode_Open,
        FileAccess_Read,
        FileShare_Read);

      uint8 buf[1024];
      while (auto const count = input.Read(buf, sizeof(buf))) {
        loader.Feed(buf, count);
        if (session.HasError()) {
          return;
        }
      }
    }
  }

  // [U]
  public: static void Usage() {
    StdErr->WriteLine("Usage: evm option... file...");
    StdErr->WriteLine("Options:");
    StdErr->WriteLine(" -get name");
    StdErr->WriteLine(" -r reference-module-file");
  }
}; // Static

// Main driver
static int ExecutorMain() {
  ExecuteSession session(Il::Cg::CgTarget::Get());
  CommandContext context(session, *StdOut, *StdErr);
  CommandManager cmd_man;
  String command_name;
  ArrayList_<String> args;

  enum State {
    State_Args,
    State_File,
    State_Name,
    State_Pass,
    State_Ref,
    State_Start,
  };

  bool invoke = true;
  auto state = State_Start;

  foreach (EnumParam, params, ::GetCommandLineW()) {
    auto const param = String(params.Get());

    switch (state) {
      case State_Args:
        args.Add(param);
        break;

      case State_File:
        if (param == "--") {
          state = State_Args;
          break;
        }

        if (param == "-disasm") {
          invoke = false;
          cmd_man.Execute(context, "disasm");
          break;
        }

        if (param == "-help") {
          Static::Usage();
          return 0;
        }

        if (param == "-html") {
          invoke = false;
          cmd_man.Execute(context, "html");
          break;
        }

        if (param == "-list") {
          cmd_man.Execute(context, "list");
          return 0;
        }

        if (param == "-name") {
          state = State_Name;
          break;
        }

        if (param == "-pass") {
          state = State_Pass;
          break;
        }

        if (param == "-r" || param == "-ref") {
          state = State_Ref;
          break;
        }

        if (param == "-text") {
          invoke = false;
          cmd_man.Execute(context, "text");
          break;
        }

        if (param.EndsWith(".evo")) {
          Static::LoadFasl(session, param, &context);
          break;
        }

        StdErr->WriteLine("Unsupported file type: %s", param);
        return 1;

      case State_Name:
        context.set_name(param);
        state = State_File;
        break;

      case State_Pass:
        cmd_man.Execute(context, param);
        state = State_File;
        break;

      case State_Ref:
        Static::LoadFasl(session, param);
        state = State_File;
        break;

      case State_Start:
        command_name = param;
        state = State_File;
        break;

      default:
        CAN_NOT_HAPPEN();
    }

    if (!Static::IsSucceeded(session)) {
      return 1;
    }
  }

  if (state != State_Args && state != State_File) {
    Static::Usage();
    return 1;
  }

  if (!invoke) {
    return 0;
  }

  cmd_man.Execute(context, "codegen");
  if (!Static::IsSucceeded(session)) {
    return 1;
  }

  Static::Invoke(context, args);
  if (!Static::IsSucceeded(session)) {
    return 1;
  }

  return 0;
}

static void CrtMain() {
  Executor::Init();

  StdOut = new StreamWriter(
      *new HandleStream(::GetStdHandle(STD_OUTPUT_HANDLE)));

  StdErr = new StreamWriter(
      *new HandleStream(::GetStdHandle(STD_ERROR_HANDLE)));

  auto const exit_code = ExecutorMain();

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
extern "C" int __cdecl wmain(int, wchar_t**) {
  CrtMain();
}
#endif
