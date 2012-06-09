// @(#)$Id$
//
// Evita Il - IR - IL
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Evita_Ee_Commands_CommandContext_h)
#define INCLUDE_Evita_Ee_Commands_CommandContext_h

#include "../../Il/Fasl/FastLoader.h"

namespace Executor {

using Common::Collections::ArrayList_;
using Common::Collections::Collection_;
using Common::Io::TextWriter;

using namespace Il::Ir;

class ExecuteSession;

class CommandContext : public Il::Fasl::FastLoader::Callback {
  private: String name_;
  private: ArrayList_<const Class*> class_list_;
  private: Module* module_;
  private: ExecuteSession& session_;
  private: TextWriter& stderr_;
  private: TextWriter& stdout_;

  public: CommandContext(ExecuteSession&, TextWriter&, TextWriter&);
  public: virtual ~CommandContext();

  // properties
  public: const String name() const { return name_; }
  public: ExecuteSession& session() const { return session_; }
  public: TextWriter& stderr() const { return stderr_; }
  public: TextWriter& stdout() const { return stdout_; }

  public: void set_name(const String name) { name_ = name; }

  // [E]
  public: Module* EnsureModule();
  public: const Operand* EnsureOperand();

  private: Collection_<Method*> FindMethods(
      const Name&,
      const Collection_<const FunctionType*>) const;

  // [F]
  public: Class* FindClass(const Name&) const;

  // [H]
  public: virtual void HandleClass(Class& clazz) override;
  public: bool HasError() const;

  DISALLOW_COPY_AND_ASSIGN(CommandContext);
};

} // Executor

#endif // !defined(INCLUDE_Evita_Ee_Commands_CommandContext_h)
