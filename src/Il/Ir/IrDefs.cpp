#include "precomp.h"
// @(#)$Id$
//
// Evita Il - IR (Intermediate Representation)
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

//#include "./IrDefs.h"

#include "./Name.h"
#include "./Operands.h"

#include "../../Om.h"

namespace Il {
namespace Ir {

#define DEFTYPE(mp_mc, mp_mod, mp_outer, mp_name, ...) \
    const Name* Q_ ## mp_name; \
    mp_mc* Ty_ ## mp_name;

#include "./Types.inc"

BoolOutput* False;
BoolOutput* True;
VoidOutput* Useless;
VoidOutput* Void;

#define DEFSYMBOL(name) const Name* Q_## name;
#include "./Symbols.inc"

const Name* QD_cctor;
const Name* QD_ctor;
const Name* QD_value;

Type* Ty_Bool;
const Type* Ty_VoidPtr;

#define Q(mp_name) Name::Intern(# mp_name)

namespace {
static Collection_<const Class*> ComputeBaseSpecs() {
  return CollectionV_<const Class*>();
}

static Collection_<const Class*> ComputeBaseSpecs(Class& a) {
  return CollectionV_<const Class*>(&a);
}

static Collection_<const Class*> ComputeBaseSpecs(Class& a, Class& b) {
  ASSERT(b.IsInterface());
  return CollectionV_<const Class*>(&a, &b);
}

static Collection_<const Class*> ComputeBaseSpecs(Class& a, Class& b,
                                                  Class& c) {
  ASSERT(b.IsInterface());
  ASSERT(c.IsInterface());
  return CollectionV_<const Class*>(&a, &b, &c);
}

static Collection_<const Class*> ComputeBaseSpecs(Class& a, Class& b,
                                                  Class& c, Class& d) {
  ASSERT(b.IsInterface());
  ASSERT(c.IsInterface());
  ASSERT(d.IsInterface());
  return CollectionV_<const Class*>(&a, &b, &c, &d);
}

static Collection_<const Class*> ComputeBaseSpecs(Class& a, Class& b,
                                                  Class& c, Class& d,
                                                  Class& e) {
  ASSERT(b.IsInterface());
  ASSERT(c.IsInterface());
  ASSERT(d.IsInterface());
  ASSERT(e.IsInterface());
  return CollectionV_<const Class*>(&a, &b, &c, &d, &e);
}

static Collection_<const Class*> ComputeBaseSpecs(Class& a, Class& b,
                                                  Class& c, Class& d,
                                                  Class& e, Class& f) {
  ASSERT(b.IsInterface());
  ASSERT(c.IsInterface());
  ASSERT(d.IsInterface());
  ASSERT(e.IsInterface());
  ASSERT(f.IsInterface());
  return CollectionV_<const Class*>(&a, &b, &c, &d, &e, &f);
}

class Helper {
  public: static const ConstructedClass& ConstructClass(
      Class& clazz,
      const Type& tyarg1) {
    return *clazz.StaticCast<GenericClass>()->Construct(tyarg1)
        .StaticCast<const ConstructedClass>();
  }

  private: static const Type& EnsureType(
      const Class& clazz,
      const Type& type) {
    if (auto const typaram = type.DynamicCast<TypeParam>()) {
      return *GetTypeParam(*clazz.StaticCast<GenericClass>());
    }
    return type;
  }

  public: static const TypeParam* GetTypeParam(const Class& clazz) {
    if (auto const gclass = clazz.DynamicCast<GenericClass>()) {
      Collection_<const TypeParam*>::Enum it(gclass->GetTypeParams());
      return it.Get();
    }
    return nullptr;
  }

  public: static void InstallField(
      Class& clazz,
      int const modifiers,
      const Type& type,
      const Name& name) {
    clazz.Add(
        name,
        *new Field(
            clazz,
            modifiers,
            EnsureType(clazz, type),
            name));
  }

  public: static void InstallKnownMethods(
      Class& clazz,
      const ConstructedClass& cclass) {
    auto& gen_class = cclass.generic_class();

    if (gen_class == *Ty_Comparable) {
      InstallMethod(clazz, Modifier_Public, *Ty_Boolean, Q(CompareTo),
          clazz, Q(another));
      return;
    }

    if (gen_class == *Ty_Enumerable) {
      InstallMethod(clazz, Modifier_Public,
        ConstructClass(*Ty_Enumerator, cclass.GetTypeArg()),
        Q(GetEnumerator));
      return;
    }

    if (gen_class == *Ty_Equatable) {
      InstallMethod(clazz, Modifier_Public, *Ty_Boolean, Q(Equals),
          clazz, Q(another));
      return;
    }

    if (gen_class == *Ty_Hashable) {
      InstallKnownMethods(clazz, ConstructClass(*Ty_Equatable, clazz));
      InstallMethod(clazz, Modifier_Public, *Ty_Int32, Q(ComputeHashcode));
      return;
    }

    DEBUG_FORMAT("Unsupported class: %s", cclass);
  }

  private: static MethodGroup& InternMethodGroup(
      Class& clazz,
      const Name& name) {
    if (auto const present = clazz.Find(name)->DynamicCast<MethodGroup>()) {
      return *present;
    }

    auto& method_group = *new MethodGroup(clazz, name);
    clazz.Add(method_group.name(), method_group);
    return method_group;
  }

  public: static Method& InstallMethod(
      Class& clazz,
      int const modifiers,
      const Name& name,
      const FunctionType& funty) {
    auto& method_group = InternMethodGroup(clazz, name);
    auto& method = *new Method(method_group, modifiers, funty);
    method_group.Add(method);
    return method;
  }

  public: static void InstallMethod(
      Class& clazz,
      int const modifiers,
      const Type& rety,
      const Name& name) {
    InstallMethod(
        clazz,
        modifiers,
        name,
        FunctionType::Intern(
            EnsureType(clazz, rety),
            ValuesType::Intern()));
  }

  public: static void InstallMethod(
      Class& clazz,
      int const modifiers,
      const Type& rety,
      const Name& name,
      const Type& paramty1,
      const Name& name1) {
    auto& method = InstallMethod(
        clazz,
        modifiers,
        name,
        FunctionType::Intern(
            EnsureType(clazz, rety),
            ValuesType::Intern(EnsureType(clazz, paramty1))));
    method.SetParamName(0, name1);
  }

  public: static void InstallMethod(
      Class& clazz,
      int const modifiers,
      const Type& rety,
      const Name& name,
      const Type& paramty1,
      const Name& name1,
      const Type& paramty2,
      const Name& name2) {
    ValuesTypeBuilder builder(2);
    builder.Append(EnsureType(clazz, paramty1));
    builder.Append(EnsureType(clazz, paramty2));
    auto& method = InstallMethod(
        clazz,
        modifiers,
        name,
        FunctionType::Intern(
            EnsureType(clazz, rety),
            builder.GetValuesType()));
    method.SetParamName(0, name1);
    method.SetParamName(1, name2);
  }

  public: static void InstallPropertyGet(
      Class& clazz,
      int const modifiers,
      const Type& type,
      const Name& name) {
    auto& property = *new Property(clazz, modifiers, type, name);
    clazz.Add(property.name(), property);
    auto& method_group = *new MethodGroup(
        clazz,
        Name::Intern(String::Format("get_%s", name)));
    clazz.Add(method_group.name(), method_group);
    auto& method = *new Method(
        method_group,
        modifiers | Modifier_SpecialName,
        FunctionType::Intern(EnsureType(clazz, type), ValuesType::Intern()));
    method_group.Add(method);
    property.Add(Q(get), method);
  }

  public: static const ValuesType& MakeValuesType() {
    return ValuesType::Intern();
  }

  private: static void RealizeClass(
      Class& clazz,
      const Collection_<const Class*>& base_specs) {
    DEBUG_FORMAT("%s", clazz);
    clazz.RealizeClass(base_specs);
    foreach (Collection_<const Class*>::Enum, it, base_specs) {
      if (auto const cclass = it.Get()->DynamicCast<ConstructedClass>()) {
        InstallKnownMethods(clazz, *cclass);
      }
    }
  }

  public: static void RealizeClass(Class& clazz) {
    if (clazz == *Ty_Void) {
      RealizeClass(clazz, CollectionV_<const Class*>(Ty_Value));
      return;
    }

    if (clazz.Is<PrimitiveType>()) {
      CollectionV_<const Class*> base_specs(
          Ty_Value,
          &ConstructClass(*Ty_Comparable, clazz),
          &ConstructClass(*Ty_Hashable, clazz));
      RealizeClass(clazz, base_specs);
      InstallMethod(clazz, Modifier_Public, *Ty_String, Q(ToString));
      return;
    }

    if (clazz.IsInterface() || clazz == *Ty_Object) {
      RealizeClass(clazz, CollectionV_<const Class*>());
      return;
    }

    RealizeClass(clazz, CollectionV_<const Class*>(Ty_Object));
  }

  public: static void RealizeClass(Class& clazz, const Class& base1) {
    RealizeClass(
        clazz,
        clazz.IsInterface() || !base1.IsInterface()
            ? CollectionV_<const Class*>(&base1)
            : CollectionV_<const Class*>(Ty_Object, &base1));
  }

  public: static void RealizeClass(
      Class& clazz,
      const Class& base1,
      const Class& base2) {
    RealizeClass(
        clazz,
        clazz.IsInterface() || !base1.IsInterface()
            ? CollectionV_<const Class*>(&base1, &base2)
            : CollectionV_<const Class*>(Ty_Object, &base1, &base2));
  }

  public: static void RealizeClass(
      Class& clazz,
      const Class& base1,
      const Class& base2,
      const Class& base3) {
    RealizeClass(
        clazz,
        clazz.IsInterface() || !base1.IsInterface()
            ? CollectionV_<const Class*>(&base1, &base2, &base3)
            : CollectionV_<const Class*>(Ty_Object, &base1, &base2, &base3));
  }

  public: static void RealizeClass(
      Class& clazz,
      const Class& base1,
      const Class& base2,
      const Class& base3,
      const Class& base4) {
    RealizeClass(
        clazz,
        clazz.IsInterface() || !base1.IsInterface()
            ? CollectionV_<const Class*>(&base1, &base2, &base3, &base4)
            : CollectionV_<const Class*>(Ty_Object, &base1, &base2, &base3,
                                         &base4));
  }
};

#define Abstract Modifier_Abstract
#define Interface Modifier_ClassVariation_Interface
#define Private Modifier_Private
#define Public Modifier_Public
#define Final Modifier_Final
#define Static Modifier_Static
#define Ty_Common Namespace::Common
#define Virtual Modifier_Virtual

static void InstallClasses() {
  #define DEFINE_CLASS(mp_outer, mp_name, mp_mod, ...) { \
    Q_ ## mp_name = &Q(mp_name); \
    Ty_ ## mp_name = new Class(*Ty_ ## mp_outer, mp_mod, *Q_ ## mp_name); \
  }

  #define DEFINE_GENERIC_CLASS_1(mp_outer, mp_name, mp_mod, ...) { \
    Q_ ## mp_name = &Q(mp_name); \
    auto& typaram1 = *new TypeParam(Q(T)); \
    Ty_ ## mp_name = new GenericClass( \
        *Ty_ ## mp_outer, \
        mp_mod, \
        *Q_ ## mp_name, \
        CollectionV_<const TypeParam*>(&typaram1)); \
  }

  #define DEFINE_PRIMITIVE_TYPE(mp_name, mp_rc, mp_size, m_sign) { \
    Q_ ## mp_name = &Q(mp_name); \
    Ty_ ## mp_name = new PrimitiveType( \
        *Q_ ## mp_name, \
        RegClass_ ## mp_rc, \
        mp_size, \
        m_sign); \
  }

  #define DEFTYPE(mp_mc, mp_mod, mp_outer, mp_name, ...) \
    UNSUPPORTED_TYPE_DEFINITION(mp_outer, mp_name)

  #include "./Types.inc"

  ASSERT(Ty_Object->GetNamespace() == Namespace::Common);
}

static void InstallFields() {
  auto& T = *Helper::GetTypeParam(*Ty_Enumerator);

  Helper::InstallField(*Ty_ClosedCell, Public | Final, T, *Q_value);
  Helper::InstallField(*Ty_LiteralCell, Public | Final, T, *Q_value);
  Helper::InstallField(*Ty_Nullable, Private | Final, T, *Q_value);
  Helper::InstallField(*Ty_StackCell, Public | Final, T, *Q_value);
}

static void InstallMethods() {
  #define ARRAY_(elemty) \
      ArrayType::Intern(*Ty_ ## elemty)

  #define CONSTRUCTED_CLASS_(gclass, elemty) \
      Helper::ConstructClass(*Ty_ ## gclass, *Ty_ ## elemty)

  #define Ty_ARRAY_(elemty) \
    &ARRAY_(elemty)

  #define Ty_CONSTRUCTED_CLASS_(gclass, elemty) \
    &CONSTRUCTED_CLASS_(gclass, elemty)

  #define DEFMETHOD(mp_class, mp_mod, mp_rety, mp_name, ...) { \
    Ty_T = Helper::GetTypeParam(*Ty_ ## mp_class); \
    Helper::InstallMethod( \
        *Ty_ ## mp_class, \
        mp_mod, \
        *Ty_ ## mp_rety, \
        Q(mp_name), \
        __VA_ARGS__); \
  }

  #define DEFPROPERTY_GET(mp_class, mp_mod, mp_ty, mp_name) { \
    Ty_T = Helper::GetTypeParam(*Ty_ ## mp_class); \
    Helper::InstallPropertyGet( \
        *Ty_ ## mp_class, \
        mp_mod, \
        *Ty_ ## mp_ty, \
        Q(mp_name)); \
   }

  #define DEFPROPERTY_SET(mp_class, mp_mod, mp_ty, mp_name) \
    NYI(DEFPROPERTY_SET)

  const Type* Ty_T = nullptr;
  #include "./Methods.inc"

  #undef ARRAY_
  #undef CONSTRUCTED_CLASS_
  #undef T
  #undef Ty_ARRAY_
  #undef Ty_CONSTRUCTED_CLASS_
}

static void RealizeClasses() {
  #define CONSTRUCTED_CLASS_(gclass, elemty) \
    Helper::ConstructClass(*Ty_ ## gclass, *Ty_ ## elemty)

  #define DEFTYPE(mp_mc, mp_mod, mp_outer, mp_name, ...) \
    Ty_T = Helper::GetTypeParam(*Ty_ ## mp_name); \
    Helper::RealizeClass(*Ty_ ## mp_name, __VA_ARGS__);

  const Type* Ty_T = nullptr;
  #include "./Types.inc"

  #undef CONSTRUCTED_CLASS
}

} // namespace

void Init() {
  Name::Init();

  #define DEFSYMBOL(name) Q_ ## name = &Q(name);

  #define DEFOPERATORSYMBOL(name, text) \
      Q_operator_ ## name = &Name::Intern("operator " text);

  #include "./Symbols.inc"

  QD_cctor = &Q(.cctor);
  QD_ctor = &Q(.ctor);
  QD_value = &Q(.value);

  Namespace::Global = &Namespace::CreateGlobalNamespace();
  Namespace::Common = new Namespace(*Namespace::Global, Q(Common));

  ASSERT(!Ty_Object);

  InstallClasses();
  RealizeClasses();
  InstallFields();
  InstallMethods();

  Ty_Bool = new BoolType();
  Ty_VoidPtr = &PointerType::Intern(*Ty_Void);

  // Well known constant operand.
  False = new BoolOutput();
  True = new BoolOutput();
  Useless = new VoidOutput();
  Void = new VoidOutput();

  // Set name to special outputs for FASL.
  False->set_name(1);
  True->set_name(2);
  Void->set_name(3);
}

} // Ir
} // Il
