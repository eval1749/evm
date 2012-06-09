#include "precomp.h"
// @(#)$Id: /proj/evcl3/boot/precomp.h 13 2006-07-29 01:55:00 yosi $
//
// Evia Executor - CommandManager
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)
//

#include "./CommandManager.h"

#include "./Command.h"
#include "./CommandContext.h"

#include "../ExecuteSession.h"
#include "../Io/EeHtmlDumper.h"
#include "../Io/EeTextDumper.h"

namespace Executor {

extern void TargetCommandInit(CommandManager&);

namespace {

class HtmlDumpCommand : public Command {
  public: HtmlDumpCommand() : Command("html") {}
  public: virtual ~HtmlDumpCommand() {}

  public: virtual void Execute(CommandContext& context) override {
    auto const operand = context.EnsureOperand();
    if (!operand) {
      return;
    }

    EeHtmlDumper dumper(context.stdout());
    if (auto const clazz = operand->DynamicCast<const Class>()) {
      dumper.DumpClass(*clazz);

    } else if (auto const mt = operand->DynamicCast<const Method>()) {
      dumper.DumpMethod(*mt);

    } else if (auto const mtg = operand->DynamicCast<const MethodGroup>()) {
      dumper.DumpMethodGroup(*mtg);

    } else {
      context.session().AddErrorInfo(
          ExecuteErrorInfo(
              SourceInfo(),
              ExecuteError_Dump_NotSupported,
              CollectionV_<Box>(operand)));
    }
  }

  DISALLOW_COPY_AND_ASSIGN(HtmlDumpCommand);
};

class ListCommandsCommand : public Command {
  private: const CommandManager& manager_;

  public: ListCommandsCommand(const CommandManager& manager)
      : Command("list"), manager_(manager) {}

  public: virtual ~ListCommandsCommand() {}

  public: virtual void Execute(CommandContext& context) override {
    foreach (Collection_<const Command*>::Enum, commands, manager_.commands()) {
      context.stdout().WriteLine(commands.Get()->name());
    }
  }

  DISALLOW_COPY_AND_ASSIGN(ListCommandsCommand);
};

class TextDumpCommand : public Command {
  public: TextDumpCommand() : Command("text") {}
  public: virtual ~TextDumpCommand() {}

  public: virtual void Execute(CommandContext& context) override {
    auto const operand = context.EnsureOperand();
    if (!operand) {
      return;
    }

    EeTextDumper dumper(context.stdout());
    if (auto const clazz = operand->DynamicCast<const Class>()) {
      dumper.DumpClass(*clazz);

    } else if (auto const mt = operand->DynamicCast<const Method>()) {
      dumper.DumpMethod(*mt);

    } else if (auto const mtg = operand->DynamicCast<const MethodGroup>()) {
      dumper.DumpMethodGroup(*mtg);

    } else {
      context.session().AddErrorInfo(
          ExecuteErrorInfo(
              SourceInfo(),
              ExecuteError_Dump_NotSupported,
              CollectionV_<Box>(operand)));
    }
  }

  DISALLOW_COPY_AND_ASSIGN(TextDumpCommand);
};

} // namespace

CommandManager::CommandManager() {
  DEBUG_PRINTF("%p\n", this);
  TargetCommandInit(*this);
  Register(*new HtmlDumpCommand());
  Register(*new ListCommandsCommand(*this));
  Register(*new TextDumpCommand());
}

CommandManager::~CommandManager() {
  DEBUG_PRINTF("%p\n", this);
  foreach (CommandMap::Enum, entries, command_map_) {
    auto entry = entries.Get();
    delete entry.GetValue();
  }
}

const Collection_<const Command*> CommandManager::commands() const {
  ArrayList_<const Command*> commands;
  foreach (CommandMap::Enum, entries, command_map_) {
    commands.Add(entries.Get().GetValue());
  }
  return commands;
}

void CommandManager::Execute(CommandContext& context, const String& name) {
  if (auto const command = command_map_.Get(name)) {
    command->Execute(context);
    return;
  }

  context.session().AddErrorInfo(
    ExecuteErrorInfo(
        SourceInfo(),
        ExecuteError_Command_NotFound,
        CollectionV_<Box>(name)));
}

void CommandManager::Register(const Command& command) {
  command_map_.Add(command.name(), &const_cast<Command&>(command));
}

} // Executor
