// @(#)$Id$
//
// Evita Il - IR - IL
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Evita_Ee_Commands_Command_h)
#define INCLUDE_Evita_Ee_Commands_Command_h

namespace Executor {

class CommandContext;

class Command {
  private: const String name_;
  public: Command(const String& name) : name_(name) {}
  public: virtual ~Command() {}
  public: virtual const String& name() const { return name_; }
  public: virtual void Execute(CommandContext&) = 0;
  DISALLOW_COPY_AND_ASSIGN(Command);
};

} // Executor

#endif // !defined(INCLUDE_Evita_Ee_Commands_Command_h)
