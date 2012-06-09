// @(#)$Id$
//
// Evita Compiler - ResolveClassPass
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Evita_Compiler_Passes_ResolveClassPass_h)
#define INCLUDE_Evita_Compiler_Passes_ResolveClassPass_h

#include "./AbstractResolvePass.h"

namespace Compiler {

class ResolveClassPass
    : public CompilePass_<ResolveClassPass, AbstractResolvePass> {

  private: typedef HashSet_<NameDef*> PendingSet;

  private: PendingSet pendings_;
  private: ArrayList_<const ClassDef*> class_defs_;

  // ctor
  public: ResolveClassPass(CompileSession*);

  // [A]
  private: void AnalyzeError();

  // [F]
  private: bool FixAliasDef(AliasDef&);
  private: bool FixClassDef(ClassDef&);
  private: bool FixNameDef(NameDef&);
  // [I]
  private: bool IsPending(NameDef&) const;

  // [P]
  private: void Postpone(AliasDef&);
  private: void Postpone(ClassDef&);

  private: virtual void Process(AliasDef*) override;
  private: virtual void Process(ClassDef*) override;

  // [S]
  public: virtual void Start() override;

  DISALLOW_COPY_AND_ASSIGN(ResolveClassPass);
}; // ResolveClassPass

} // Compiler

#endif // !defined(INCLUDE_Evita_Compiler_Passes_ResolveClassPass_h)
