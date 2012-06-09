#include "precomp.h"
// @(#)$Id$
//
// Evita Il - Cg - Code Generator
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "../../../Ee/CodeDesc.h"
#include "../../../Ee/Commands/Command.h"
#include "../../../Ee/Commands/CommandContext.h"
#include "../../../Ee/Commands/CommandManager.h"
#include "../../../Ee/ExecuteErrorInfo.h"
#include "../../../Ee/ExecuteSession.h"

#include "../../../Cg.h"
#include "../../../Cg/Tasks/RegAllocPass.h"

#include "../Cg/X86Defs.h"
#include "../Cg/X86AsmTask.h"
#include "../Cg/X86Disasm.h"
#include "../Cg/X86EnsureTask.h"
#include "../Cg/X86LowerTask.h"

#include "../../../Il/Tasks/CleanTask.h"
#include "../../../Il/Tasks/FinalTask.h"
#include "../../../Il/Tasks/VerifyTask.h"

namespace Executor {

namespace {

using Il::Cg::X86LowerTask;
using Il::Cg::X86EnsureTask;
using Il::Cg::RegAllocPass;
using Il::Cg::X86AsmTask;

using namespace Il::Tasks;

typedef Collection_<Command*> CommandList;

class DisasmCommand : public Command {
  private: Command& generator_;

  public: DisasmCommand(Command& generator)
    : Command("disasm"), generator_(generator) {}

  public: virtual ~DisasmCommand() {}

  public: virtual void Execute(CommandContext& context) override {
    generator_.Execute(context);
    if (context.HasError()) {
      return;
    }

    auto const module = context.EnsureModule();
    if (!module) {
      return;
    }

    foreach (Module::EnumFunction, en, *module) {
      auto& fun = *en.Get();
      if (auto code_desc = fun.code_desc()) {
        auto& fun = code_desc->function();
        if (auto const method = fun.GetMethod()) {
          context.stdout().WriteLine("// %s", *method);
        } else {
          context.stdout().WriteLine("// %s", fun);
        }
        Il::X86::X86Disassemble(context.stdout(), *code_desc);
      }
    }
  }

  DISALLOW_COPY_AND_ASSIGN(DisasmCommand);
};

// TODO(yosi) 2012-02-08 NYI X86VerifyTask
class X86VerifyTask : public FunctionTask_<X86VerifyTask> {
  public: X86VerifyTask(Session& session, Module& module)
    : Base("X86VerifyTask", session, module) {}
  public: void ProcessFunction(Function&) override {}
  DISALLOW_COPY_AND_ASSIGN(X86VerifyTask);
};

class TaskCommand : public Command {
  private: ModuleTask::NewFunction ctor_;
  private: const String name_;
  private: ModuleTask::NewFunction verify_ctor_;

  public: TaskCommand(
      String name,
      ModuleTask::NewFunction ctor,
      ModuleTask::NewFunction verify_ctor)
      : Command(name),
        ctor_(ctor),
        verify_ctor_(verify_ctor) {}

  public: virtual ~TaskCommand() {}

  public: virtual void Execute(CommandContext& context) override {
    auto const module = context.EnsureModule();
    if (!module) {
      return;
    }

    auto& session = context.session();

    {
      ScopedPtr_<ModuleTask> task(&ctor_(session, *module));
      task->Start();
    }

    if (session.HasError()) {
      return;
    }

    {
      ScopedPtr_<ModuleTask> task(&verify_ctor_(session, *module));
      task->Start();
    }
  }

  DISALLOW_COPY_AND_ASSIGN(TaskCommand);
};

class CodeGenCommand : public Command {
  private: CommandList command_list_;

  public: CodeGenCommand(const CommandList& command_list)
      : Command("codegen"), command_list_(command_list) {}

  public: virtual void Execute(CommandContext& context) override {
    foreach (CommandList::Enum, en, command_list_) {
      auto& command = *en.Get();
      command.Execute(context);
      if (context.HasError()) {
        break;
      }
    }
  }

  DISALLOW_COPY_AND_ASSIGN(CodeGenCommand);
};

} // namespace

void TargetCommandInit(CommandManager& manager) {
  ArrayList_<Command*> codegen_commands;

  #define REGISTER_COMMAND(name, verify) { \
    auto& command = *new TaskCommand( \
        #name, name :: Create, verify :: Create); \
    manager.Register(command); \
    codegen_commands.Add(&command); \
  }

  REGISTER_COMMAND(X86LowerTask, VerifyTask);
  REGISTER_COMMAND(X86EnsureTask, VerifyTask);
  REGISTER_COMMAND(RegAllocPass, X86VerifyTask);
  REGISTER_COMMAND(CleanTask, X86VerifyTask);
  REGISTER_COMMAND(X86AsmTask, X86VerifyTask);
  REGISTER_COMMAND(FinalTask, X86VerifyTask);

  auto& codegen_command = *new CodeGenCommand(codegen_commands);
  manager.Register(codegen_command);
  manager.Register(*new DisasmCommand(codegen_command));
}

} // Executor
