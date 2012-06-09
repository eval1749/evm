// @(#)$Id$
//
// Evita Compiler
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_evc_Keyword_h)
#define INCLUDE_evc_Keyword_h

#include "../Ir/Entity.h"

namespace Compiler {

class Keyword : public Entity_<Keyword> {
  CASTABLE_CLASS(Keyword);

  private: int const m_iModifier;
  private: const Name& name_;
  private: Type* const m_pType;

  // ctor
  private: Keyword(
      const Name& name,
      Type* const pType = nullptr,
      int const iModifier = 0);

  // [G]
  public: int GetModifier() const { return m_iModifier; }
  public: const Name& name() const { return name_; }
  public: Type* GetTy() const { return m_pType; }

  // [I]
  public: static Keyword* Find(const Name&);
  public: static void Init();

  // [T]
  public: virtual String ToString() const override;

  DISALLOW_COPY_AND_ASSIGN(Keyword);
}; // Keyword

} // Compiler

#endif // !defined(INCLUDE_evc_Keyword_h)
