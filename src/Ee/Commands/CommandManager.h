// @(#)$Id$
//
// Evita Il - IR - IL
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Evita_Ee_Commands_CommandManager_h)
#define INCLUDE_Evita_Ee_Commands_CommandManager_h

namespace Executor {

class Command;
class CommandContext;

class CommandManager {
  private: typedef HashMap_<String, Command*> CommandMap;
  private: CommandMap command_map_;
  public: CommandManager();
  public: ~CommandManager();
  public: const Collection_<const Command*> commands() const;
  public: void Execute(CommandContext&, const String&);
  public: void Register(const Command&);
  DISALLOW_COPY_AND_ASSIGN(CommandManager);
};

} // Executor

#endif // !defined(INCLUDE_Evita_Ee_Commands_CommandManager_h)
