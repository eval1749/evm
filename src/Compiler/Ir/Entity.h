// @(#)$Id$
//
// Evita Compiler
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_evc_Entity_h)
#define INCLUDE_evc_Entity_h

#include "./IrObject.h"
#include "../../Il/Ir/WithSourceInfo.h"

namespace Compiler {

using Il::Ir::WithSourceInfo;

class Entity
    : public WithCastable_<Entity, IrObject>,
      public WithSourceInfo {
  CASTABLE_CLASS(Entity);

  protected: Entity(const SourceInfo* source_info = nullptr)
    : WithSourceInfo(source_info) {}

  // [A]
  public: virtual void Apply(Functor*) = 0;

  DISALLOW_COPY_AND_ASSIGN(Entity);
}; // Entity

template<typename T, typename B = Entity>
class Entity_ : public WithCastable1_<T, B, const SourceInfo*> {

  private: typedef WithCastable1_<T, B, const SourceInfo*> TemplateBase;

  protected: typedef Entity_<T, B> Base;

  protected: Entity_(const SourceInfo* source_info = nullptr)
    : TemplateBase(source_info) {}

  public: virtual void Apply(Functor* const functor) override {
    functor->Process(static_cast<T*>(this));
  } // Apply

  DISALLOW_COPY_AND_ASSIGN(Entity_);
}; // WithEntity

} // Compiler

#endif // !defined(INCLUDE_evc_Entity_h)
