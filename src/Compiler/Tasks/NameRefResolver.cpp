#include "precomp.h"
// @(#)$Id$
//
// Evita Compiler - TypeUnifier
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./NameRefResolver.h"

#include "../../Il/Tasks/CodeEmitter.h"

#include "../CompileSession.h"
#include "../Ir.h"

namespace Compiler {

using Il::Tasks::CodeEmitter;

namespace {

class ClassMemberResolver {
  private: const Name& name_;
  private: ArrayList_<const Operand*> presents_;
  private: HashSet_<const Class*> visited_set_;

  public: ClassMemberResolver(const Class& clazz, const Name& name)
      : name_(name) {
    Resolve(clazz);
  }

  public: const Operand& operator[](int const index) const {
    return *presents_[index];
  }

  public: int Count() const { return presents_.Count(); }

  private: void Resolve(const Class& clazz) {
    if (visited_set_.Contains(&clazz)) {
      return;
    }

    visited_set_.Add(&clazz);

    if (auto const present = clazz.Find(name_)) {
      presents_.Add(present);
      return;
    }

    foreach (Class::EnumBaseSpec, bases, clazz) {
      Resolve(*bases);
    }
  }

  DISALLOW_COPY_AND_ASSIGN(ClassMemberResolver);
};

} // namespace

// ctor
NameRefResolver::NameRefResolver(
    CompileSession& session,
    const MethodDef& method_def)
    : Tasklet("NameRefResolver", session),
      compile_session_(session),
      method_def_(method_def) {}

void NameRefResolver::FixLastNameRef(
    NameRefI& name_ref_inst,
    const Operand& base_operand,
    const NameRef::Item& name_item,
    const Operand& operand) {
  DEBUG_FORMAT("base_operand=%s name_item=%s:%s %s", 
      base_operand, name_item, operand.GetKind(), operand);
  ASSERT(base_operand.Is<Output>() || base_operand.Is<Literal>());

  if (auto const clazz = operand.DynamicCast<Class>()) {
    // class literal
    if (base_operand != *Void) {
      compile_session_.AddError(
          name_item.source_info(),
          CompileError_Resolve_Name_IsClass,
          name_item);
      return;
    }

    // TODO(yosi) 2012-05-19 NYI: NameRefI ConstructedClass
    foreach (
        Register::EnumUser,
        users,
        name_ref_inst.output().StaticCast<Register>()) {
      auto& user = users->instruction();
      if (user.Is<LoadI>()) {
        ReplaceAll(*clazz, *user.output().StaticCast<Register>());
      } else if (user.Is<StoreI>()) {
        compile_session_.AddError(
            user.source_info(),
            CompileError_Resolve_Name_IsClass,
            name_item);
      } else {
        compile_session_.AddError(
            user.source_info(),
            CompileError_Resolve_Unexpected,
            user);
      }
    }
    return;
  }

  if (auto const field = operand.DynamicCast<Field>()) {
    if (!name_item.Is<NameRef::SimpleName>()) {
      compile_session_.AddError(
        name_item.source_info(),
        CompileError_Resolve_Name_Invalid,
        name_item);
      return;
    }

    CodeEmitter emitter(compile_session_, name_ref_inst);

    auto& this_operand = SelectBaseOperand(
        CompileError_Resolve_Field_InstanceField,
        CompileError_Resolve_Field_StaticField,
        name_item,
        base_operand,
        method_def_.this_operand(),
        field->IsStatic());
    ReplaceAll(
        emitter.FieldPtr(this_operand, *field),
        *name_ref_inst.output().StaticCast<Register>());
    return;
  }

  if (auto const method_group = operand.DynamicCast<MethodGroup>()) {
    // TODO(yosi) 2012-05-19 NYI: NameRefI GenericMethod
    auto& call_inst = *name_ref_inst.GetRd()->GetFirstUser()
        ->GetI()->output().StaticCast<Register>()->GetFirstUser()
        ->GetI()->StaticCast<CallI>();
    auto& args_inst = *call_inst.GetVy()->def_inst()->StaticCast<ValuesI>();

    DEBUG_FORMAT("replace callee of %s to %s", call_inst, *method_group);
    call_inst.GetOperandBox(0)->Replace(*method_group);

    DEBUG_FORMAT("insert %s to %s", base_operand, args_inst);
    args_inst.InsertOperandBefore(&base_operand, args_inst.GetOperandBox(0));
    args_inst.UpdateOutputType();
    return;
  }

  if (operand.Is<Namespace>()) {
    compile_session_.AddError(
        name_item.source_info(),
        CompileError_Resolve_Name_IsNamespace,
        name_item.name());
    return;
  }

  if (auto const property = operand.DynamicCast<Property>()) {
    if (!name_item.Is<NameRef::SimpleName>()) {
      compile_session_.AddError(
        name_item.source_info(),
        CompileError_Resolve_Name_Invalid,
        name_item);
      return;
    }

    auto& this_operand = SelectBaseOperand(
        CompileError_Resolve_Property_IsInstance,
        CompileError_Resolve_Property_IsStatic,
        name_item,
        base_operand,
        method_def_.this_operand(),
        property->IsStatic());

    foreach (
        Register::EnumUser,
        users,
        name_ref_inst.output().StaticCast<Register>()) {
      auto& user = users->instruction();
      CodeEmitter emitter(session(), user);
      if (user.Is<LoadI>()) {
        if (auto const getter = property->Get(*Q_get)) {
          DEBUG_FORMAT("rewrite %s to property getter %s", user, *getter)
          ReplaceAll(
            emitter.Call(getter->return_type(), *getter, this_operand),
            *user.output().StaticCast<Register>());
        } else {
          compile_session_.AddError(
              name_item.source_info(),
              CompileError_Resolve_Property_NoGetter,
              name_item);
        }
      } else if (user.Is<StoreI>()) {
        if (auto const setter = property->Get(*Q_set)) {
          DEBUG_FORMAT("rewrite %s to property setter %s", user, *setter)
          emitter.Call(setter->return_type(), *setter, this_operand, user.op1());
          user.set_output(*Useless);
        } else {
          compile_session_.AddError(
              name_item.source_info(),
              CompileError_Resolve_Property_NoGetter,
              name_item);
        }
      } else {
        compile_session_.AddError(
            user.source_info(),
            CompileError_Resolve_Unexpected,
            user);
      }
    }
    return;
  }

  DEBUG_FORMAT("%s: %s %s", name_item, operand.GetKind(), operand);
  compile_session_.AddError(
      name_item.source_info(),
      CompileError_Resolve_Unexpected,
      name_item);
}

const Operand* NameRefResolver::FixMiddleNameRef(
    const NameRefI& name_ref_inst,
    const Operand& base_operand,
    const NameRef::Item& name_item,
    const Operand& operand) {
  DEBUG_FORMAT("base_operand=%s name_item=%s:%s %s", 
      base_operand, name_item, operand.GetKind(), operand);
  if (auto const clazz = operand.DynamicCast<Class>()) {
    if (base_operand != *Void && base_operand.Is<Output>()) {
      compile_session_.AddError(
          name_item.source_info(),
          CompileError_Resolve_Name_IsClass,
          name_item);
      return nullptr;
    }

    // TODO(yosi): 2012-05-19 NYI: GenericClass
    return clazz;
  }

  if (auto const field = operand.DynamicCast<Field>()) {
    if (!name_item.Is<NameRef::SimpleName>()) {
      compile_session_.AddError(
        name_item.source_info(),
        CompileError_Resolve_Name_Invalid,
        name_item);
      return nullptr;
    }

    if (field->IsStatic()) {
      if (base_operand != *Void && base_operand.Is<Output>()) {
        compile_session_.AddError(
          name_item.source_info(),
          CompileError_Resolve_Field_StaticField,
          name_item);
        return nullptr;
      }
    } else {
      if (base_operand == *Void || !base_operand.Is<Output>()) {
        compile_session_.AddError(
          name_item.source_info(),
          CompileError_Resolve_Field_InstanceField,
          name_item);
        return nullptr;
      }
    }

    CodeEmitter emitter(compile_session_, name_ref_inst);
    return &emitter.Load(emitter.FieldPtr(base_operand, *field));
  }

  if (auto const method_group = operand.DynamicCast<MethodGroup>()) {
    compile_session_.AddError(
        name_item.source_info(),
        CompileError_Resolve_Name_IsMethod,
        name_item);
    return nullptr;
  }

  if (auto const namespaze = operand.DynamicCast<Namespace>()) {
    if (base_operand != *Void && base_operand.Is<Output>()) {
      compile_session_.AddError(
          name_item.source_info(),
          CompileError_Resolve_Name_IsNamespace,
          name_item);
      return nullptr;
    }

    if (!name_item.Is<NameRef::SimpleName>()) {
      compile_session_.AddError(
        name_item.source_info(),
        CompileError_Resolve_Name_Invalid,
        name_item);
      return nullptr;
    }

    return namespaze;
  }

  if (auto const property = operand.DynamicCast<Property>()) {
    if (!name_item.Is<NameRef::SimpleName>()) {
      compile_session_.AddError(
        name_item.source_info(),
        CompileError_Resolve_Name_Invalid,
        name_item);
      return nullptr;
    }

    if (property->IsStatic()) {
      if (base_operand != *Void && base_operand.Is<Output>()) {
        compile_session_.AddError(
          name_item.source_info(),
          CompileError_Resolve_Property_IsStatic,
          name_item);
        return nullptr;
      }
    } else {
      if (base_operand == *Void || !base_operand.Is<Output>()) {
        compile_session_.AddError(
          name_item.source_info(),
          CompileError_Resolve_Property_IsInstance,
          name_item);
        return nullptr;
      }
    }

    auto const getter = property->Get(*Q_get);
    if (!getter) {
      compile_session_.AddError(
        name_item.source_info(),
        CompileError_Resolve_Property_NoGetter,
        name_item);
      return nullptr;
    }

    CodeEmitter emitter(compile_session_, name_ref_inst);
    return &emitter.Call(
        getter->return_type(),
        *getter,
        base_operand);
  }

  compile_session_.AddError(
      name_item.source_info(),
      CompileError_Resolve_Unexpected,
      name_item);
  return nullptr;
}

// [L]
const Operand* NameRefResolver::LookupMostOuterName(
  const NameRef& name_ref,
  const Class& clazz,
  const NamespaceBody& most_inner_nsb) {

  auto& name = name_ref.name();

  // Try class member
  {
    ClassMemberResolver resolver(clazz, name);
    switch (resolver.Count()) {
      case 0:
        break;

      case 1:
        return &resolver[0];

      default:
        compile_session_.AddError(
          name_ref.source_info(),
          CompileError_Resolve_Name_Ambiguous,
          resolver[0],
          resolver[1]);
        return nullptr;
    }
  }

  // Try outer classes and namespaces
  foreach (Class::EnumOuterAndSelf, outers, clazz.outer()) {
    auto const outer = outers.Get().DynamicCast<Class>();
    if (!outer) {
      break;
    }

    if (auto const present = outer->Find(name)) {
      DEBUG_FORMAT("%s.Find(%s)=%s (outer)", outer, name, present);
      return present;
    }
  }

  // Lookup in namespace
  foreach (NamespaceBody::EnumOuterAndSelf, outers, most_inner_nsb) {
    auto& nsb = outers.Get();

    // Find name in current namespace body
    if (auto const present = nsb.Find(name)) {
      ASSERT(present->IsRealized());
      if (auto const alias_def = present->DynamicCast<AliasDef>()) {
        return &present->operand();
      }
      DEBUG_FORMAT("%s.Find(%s)=%s (nsb)", nsb, name, present);
      return &present->operand();
    }

    ArrayList_<Operand*> presents;

    foreach (NamespaceBody::EnumUsingNamespace, uzings, nsb) {
      auto& uzing = *uzings.Get();
      if (name_ref.StartsWith(uzing.name_ref())) {
        return uzing.namezpace();
      }

      if (auto const present = uzing.namezpace()->Find(name)) {
        // Using namespace directive doesn't import members in inner
        // namespaces.
        if (!present->Is<Namespace>()) {
          DEBUG_FORMAT("%s.Find(%s)=%s (%s)", nsb, name, present, uzing);
          presents.Add(present);
        }
      }
    }

    // Find name defined in other namespace bodies.
    auto& ns = nsb.namespace_def().namespaze();
    if (auto const present = ns.Find(name)) {
      DEBUG_FORMAT("%s.Find(%s)=%s (ns)", ns, name, present);
      presents.Add(present);
    }

    switch (presents.Count()) {
      case 0:
        break;

      case 1:
        return presents[0];

      default:
        compile_session_.AddError(
          name_ref.source_info(),
          CompileError_Resolve_Name_Ambiguous,
          const_cast<Name*>(&name));
        return nullptr;
    }
  }

  DEBUG_FORMAT("%s.Find(%s) Not found", most_inner_nsb, name);
  return nullptr;
}

// [R]

void NameRefResolver::Resolve(NameRefI& name_ref_inst) {
  DEBUG_FORMAT("%s", name_ref_inst);
  auto& name_ref = *name_ref_inst.op1().StaticCast<NameRef>();
  const Operand* base_operand = &name_ref_inst.op0();
  NameRef::Enum names(name_ref);
  for (;;) {
    auto& name_item = *names;
    names.Next();

    auto const present = ResolveNameItem(*base_operand, name_ref, name_item);
    if (!present) {
      return;
    }

    if (names.AtEnd()) {
      FixLastNameRef(
          name_ref_inst,
          name_ref_inst.op0() == *Void ? *Void : *base_operand,
          name_item,
          *present);
      return;
    }

    base_operand = FixMiddleNameRef(
        name_ref_inst,
        *base_operand,
        name_item,
        *present);
    if (!base_operand) {
      return;
    }
  }
}

const Operand* NameRefResolver::ResolveNameItem(
    const Operand& base_operand,
    const NameRef& name_ref,
    const NameRef::Item& name_item) {

  class Local {
    public: static const Type& EnsureClass(const Type& type) {
      if (auto const clazz = type.DynamicCast<Class>()) {
        return *clazz;
      }

      if (auto const arrty = type.DynamicCast<ArrayType>()) {
        return Ty_Array->Construct(arrty->element_type());
      }

      // Caller of EnsureClass must not pass type parameter and type variable,
      // even if it is bound.
      return type;
    }
  };

  auto& name = name_item.name();
  if (base_operand == *Void) {
    if (auto const present = method_def_.Find(name)) {
      if (!name_item.Is<NameRef::SimpleName>()) {
        compile_session_.AddError(
            name_item.source_info(),
            CompileError_Resolve_Name_Invalid,
            name_item);
        return nullptr;
      }

      if (auto const typaram = present->DynamicCast<TypeParamDef>()) {
        return &typaram->type_param();
      }

      compile_session_.AddError(
        name_item.source_info(),
        CompileError_Resolve_Unexpected,
        name);
      return nullptr;
    }

    auto const present = LookupMostOuterName(
      name_ref,
      method_def_.owner_class_def().GetClass(),
      method_def_.namespace_body());
    if (!present) {
      compile_session_.AddError(
        name_item.source_info(),
        CompileError_Resolve_Name_NotFound,
        name);
    }
    return present;
  }

  if (auto const namespaze = base_operand.DynamicCast<Namespace>()) {
    return namespaze->Find(name);
  }

  // TODO(yosi) 2012-05-19 If type of base_operand is type variable or
  // type parameter, EnsureClass doesn't work.
  auto& base_type = base_operand.Is<Class>()
      ? *base_operand.StaticCast<const Type>()
      : Local::EnsureClass(base_operand.type());
  if (!base_type.Is<Class>()) {
    DEBUG_FORMAT("unexpected type: %s %s", base_type.GetKind(), base_type);
    compile_session_.AddError(
        name_item.source_info(),
        CompileError_Resolve_Unexpected,
        base_operand.type(),
        name_ref);
    return nullptr;
  }

  ClassMemberResolver resolver(*base_type.StaticCast<Class>(), name);

  switch (resolver.Count()) {
    case 0:
      compile_session_.AddError(
          name_item.source_info(),
          CompileError_Resolve_Name_NotFound,
          name);
      return nullptr;

    case 1:
      return &resolver[0];

    default:
       compile_session_.AddError(
          name_item.source_info(),
          CompileError_Resolve_Name_Ambiguous,
        resolver[0],
        resolver[1]);
      return nullptr;
  }
}

// [S]
const Operand& NameRefResolver::SelectBaseOperand(
    CompileError error_operand_is_an_instance,
    CompileError error_operand_is_a_static,
    const NameRef::Item& name_item,
    const Operand& base_operand,
    const Operand& this_operand,
    bool is_static) {
  if (is_static) {
    if (base_operand != *Void) {
      compile_session_.AddError(
          name_item.source_info(),
          error_operand_is_a_static,
          name_item);
    }
    return *Void;
  }

  if (base_operand != *Void) {
    return base_operand;
  }

  if (this_operand == *Void) {
    compile_session_.AddError(
        name_item.source_info(),
        error_operand_is_an_instance,
        name_item);
    return *Void;
  }

  return this_operand;
}

} // Compiler
