#include "precomp.h"
// @(#)$Id$
//
// Evita Compiler IR- Class
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./Class.h"

#include "./ConstructedClass.h"
#include "./GenericClass.h"
#include "./Method.h"
#include "./MethodGroup.h"
#include "./Namespace.h"
#include "./PrimitiveType.h"
#include "./PointerType.h"
#include "./TypeArgs.h"

#include "../Name.h"

namespace Il {
namespace Ir {

Class::EnumClassTree::EnumClassTree(const Class& clazz) {
  queue_.Give(&const_cast<Class&>(clazz));
  QueueBaseSpecs(clazz);
}

bool Class::EnumClassTree::AtEnd() const {
  return queue_.IsEmpty();
}

const Class& Class::EnumClassTree::Get() const {
  ASSERT(!AtEnd());
  return *queue_.Get();
}

void Class::EnumClassTree::Next() {
  ASSERT(!AtEnd());
  QueueBaseSpecs(*queue_.Take());
}

void Class::EnumClassTree::QueueBaseSpecs(const Class& clazz) {
  foreach (EnumBaseSpec, base_specs, clazz) {
    queue_.Give(&base_specs.Get());
  }
}

// ctor
Class::Class(
    const ClassOrNamespace& outer,
    int const modifiers,
    const Name& name)
    : Base(outer, name),
      WithModifiers(modifiers) {}

Class::Class(const GenericClass& generic_class)
    : Base(*Namespace::Global, generic_class.name()),
      WithModifiers(generic_class.modifiers()) {}

Class::~Class() {
  class Collector : public Functor {
    private: typedef HashSet_<Type*> TypeSet;
    private: Class& class_;
    private: bool matched_;
    private: TypeSet tyset_;

    public: Collector(Class& clazz) : class_(clazz), matched_(false) {}
    public: virtual ~Collector() {}

    public: void Clean() {
      foreach (TypeSet::Enum, types, tyset_) {
        auto& ty = *types.Get();
        if (auto const arrty = ty.DynamicCast<ArrayType>()) {
          arrty->Unintern();

        } else if (auto const funty = ty.DynamicCast<FunctionType>()) {
          funty->Unintern();

        } else if (auto const ptrty = ty.DynamicCast<PointerType>()) {
          ptrty->Unintern();

        } else if (auto const valsty = ty.DynamicCast<ValuesType>()) {
          valsty->Unintern();

        } else {
          CAN_NOT_HAPPEN();
        }
      }

      foreach (TypeSet::Enum, types, tyset_) {
        auto& type = *types.Get();
        type.Release();
      }
    }

    public: virtual void Process(ArrayType* arrty) override {
      matched_ = false;
      const_cast<Type&>(arrty->element_type()).Apply(this);
      if (matched_) {
        tyset_.Add(arrty);
      }
    }

    public: virtual void Process(Class* clazz) override {
      if (clazz == &class_) {
        matched_ = true;
      }
    }

    public: virtual void Process(FunctionType* funty) override {
      matched_ = false;
      const_cast<Type&>(funty->return_type()).Apply(this);
      const_cast<ValuesType&>(funty->params_type()).Apply(this);
      if (matched_) {
        tyset_.Add(funty);
      }
    }

    public: virtual void Process(PointerType* ptrty) override {
      matched_ = false;
      const_cast<Type&>(ptrty->pointee_type()).Apply(this);
      if (matched_) {
        tyset_.Add(ptrty);
      }
    }

    public: virtual void Process(ValuesType* valsty) override {
      matched_ = false;
      foreach (ValuesType::Enum, types, valsty) {
        const_cast<Type&>(types.Get()).Apply(this);
        if (matched_) {
          tyset_.Add(valsty);
          return;
        }
      }
    }
    DISALLOW_COPY_AND_ASSIGN(Collector);
  };

  Collector collector(*this);
  ArrayType::MapAll(collector);
  FunctionType::MapAll(collector);
  PointerType::MapAll(collector);
  ValuesType::MapAll(collector);
  collector.Clean();
}

// [A]
void Class::Add(const Name& name, Operand& operand) {
  Base::Add(name, operand);
  if (auto const field = operand.DynamicCast<Field>()) {
    fields_.Append(field);
  }
}

// [C]
int Class::ComputeHashCode() const {
  return Common::ComputeHashCode(name().ComputeHashCode(), 'C');
}

Type& Class::Construct(const TypeArgs&) const {
  return *const_cast<Class*>(this);
}

// [G]
const Class* Class::GetBaseClass() const {
  Class::Collection::Enum bases(base_specs_);
  return bases.AtEnd() ? nullptr : bases.Get();
}

Namespace* Class::GetNamespace() const {
  if (auto const pNamespace = outer().DynamicCast<Namespace>()) {
    return pNamespace;
  }
  return GetOuterClass()->GetNamespace();
}

const Class* Class::GetOuterClass() const {
  return outer().DynamicCast<Class>();
}

// [I]
bool Class::IsClass() const {
  return (GetModifiers() & Modifier_Virtual) == 0;
}

bool Class::IsInterface() const {
  return (GetModifiers() & Modifier_Mask_ClassVariation)
      == Modifier_ClassVariation_Interface;
}

bool Class::IsStruct() const {
  return (GetModifiers() & Modifier_Mask_ClassVariation)
      == Modifier_ClassVariation_Struct;
}

Subtype Class::IsSubtypeOf(const Type& r) const {
  if (this == Ty_Void) {
    return r.Is<ValuesType>() ? Subtype_No : Subtype_Yes;
  }

  if (r == *Ty_Void) {
    return Subtype_No;
  }

  if (*this == r || r == *Ty_Object) {
    return Subtype_Yes;
  }

  if (this == Ty_Object) {
    return Subtype_No;
  }

  if (auto const that = r.DynamicCast<Class>()) {
    foreach (EnumClassTree, classes, *this) {
      auto& clazz = classes.Get();
      if (clazz == *that) {
        return Subtype_Yes;
      }
    }

    foreach (EnumClassTree, classes, *that) {
      auto& clazz = classes.Get();
      if (clazz == *this) {
        return clazz.IsInterface() ? Subtype_Yes : Subtype_No;
      }
    }
  }

  if (auto const arrty = r.DynamicCast<ArrayType>()) {
    return this->IsSubtypeOf(Ty_Array->Construct(arrty->element_type()));
  }

  return Subtype_Unknown;
}

// [R]

void Class::RealizeClass(const Collection_<const Class*>& base_specs) {
  ASSERT(base_specs_.IsEmpty());

  base_specs_ = base_specs;

  if (!Is<ConstructedClass>()) {
    outer().Add(name(), *this);
  }

  if (IsInterface()) {
    foreach (EnumBaseSpec, base_specs, *this) {
      auto& base = base_specs.Get();
      ASSERT(this != &base);
      if (!base.IsInterface()) {
        DEBUG_FORMAT("%s: %s %d must be an interface.",
          name(), base, base.modifiers());
        CAN_NOT_HAPPEN();
      }
    }
  } else {
    EnumBaseSpec base_specs(*this);
    if (!base_specs.AtEnd()) {
      auto& base = base_specs.Get();
      ASSERT(this != &base);
      if (base.IsInterface()) {
        DEBUG_FORMAT("%s: %s %d must not be an interface.",
          name(), base, base.modifiers());
        CAN_NOT_HAPPEN();
      }
      base_specs.Next();
    }

    while (!base_specs.AtEnd()) {
      auto& base = base_specs.Get();
      ASSERT(this != &base);
      if (!base.IsInterface()) {
        DEBUG_FORMAT("%s: %s %d must be an interface.",
          name(), base, base.modifiers());
        CAN_NOT_HAPPEN();
      }
      base_specs.Next();
    }
  }
}

} // Ir
} // Il
