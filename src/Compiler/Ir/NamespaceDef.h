// @(#)$Id$
//
// Evita Compiler
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Compiler_Ir_NamespaceDef_h)
#define INCLUDE_Compiler_Ir_NamespaceDef_h

#include "./ClassOrNamespaceDef.h"

namespace Compiler {


// Add and Find methods are used for multiple definition during compilation.
class NamespaceDef : public NameDef_<NamespaceDef, ClassOrNamespaceDef> {
  CASTABLE_CLASS(NamespaceDef);

  private: Namespace* const real_namespace_;

  // ctor
  public: NamespaceDef(
      NamespaceDef&, const Name&, const SourceInfo&);

  // for Global compilation namespace
  private: NamespaceDef(Namespace&, Namespace&);

  public: Namespace& namespaze() const;
  public: NamespaceDef* outer() const;

  // [C]
  public: static NamespaceDef& CreateGlobalNamespaceDef(MemoryZone&);

  // [F]
  public: ClassOrNamespace* FindObject(const Name&) const;

  // [R]
  public: void Realize(Namespace& ns) { RealizeInternal(ns); }

  // [T]
  public: virtual String ToString() const override;

  DISALLOW_COPY_AND_ASSIGN(NamespaceDef);
}; // NamespaceDef

} // Compiler

#endif // !defined(INCLUDE_Compiler_Ir_NamespaceDef_h)
