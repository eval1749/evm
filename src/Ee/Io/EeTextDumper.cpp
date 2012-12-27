#include "precomp.h"
// @(#)$Id$
//
// Evita Compiler - EeTextDumper
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./EeTextDumper.h"

#include "../../Common/Io/TextWriter.h"

namespace Executor {

// ctor
EeTextDumper::EeTextDumper(TextWriter& writer) : writer_(writer) {}

// [D]
void EeTextDumper::DumpClass(const Class& clazz) {
  Stack_<ClassOrNamespace*> outer_stack;
  foreach (Class::EnumOuterAndSelf, outers, clazz) {
    auto& class_or_ns = outers.Get();
    if (class_or_ns.IsGlobalNamespace()) {
      break;
    }
    outer_stack.Push(&class_or_ns);
  }

  while (!outer_stack.IsEmpty()) {
    auto& outer =* outer_stack.Pop();
    if (auto const ns = outer.DynamicCast<Namespace>()) {
      writer_.WriteLine("namespace %s {", ns->name());
    } else if (auto const cl = outer.DynamicCast<Class>()) {
      writer_.Write("class %s", cl->name());
      if (auto const gcl = cl->DynamicCast<GenericClass>()) {
        writer_.Write('<');
        const char* comma = "";
        foreach (
            Collection_<const TypeParam*>::Enum, en, gcl->GetTypeParams()) {
          writer_.Write("%s%s", comma, en.Get()->name());
          comma = ", ";
        }
        writer_.Write('>');
      }
      const char* comma = " :";
      foreach (Class::EnumBaseSpec, base_specs, *cl) {
        writer_.Write("%s %s", comma, *base_specs);
        comma = ",";
      }
      writer_.WriteLine(" {");
    } else {
      CAN_NOT_HAPPEN();
    }
  }

  ArrayList_<const Field*> fields;
  ArrayList_<const MethodGroup*> method_groups;
  ArrayList_<const Property*> properties;

  foreach (Class::EnumMember, members, clazz) {
    auto const member = members.Get();
    if (auto const field = member->DynamicCast<Field>()) {
      fields.Add(field);
    } else if (auto const mg = member->DynamicCast<MethodGroup>()) {
      method_groups.Add(mg);
    } else if (auto const property = member->DynamicCast<Property>()) {
      properties.Add(property);
    }
  }

  if (!fields.IsEmpty()) {
    foreach (ArrayList_<const Field*>::Enum, en, fields) {
      auto& field = *en.Get();
      writer_.WriteLine("field %s%s %s;",
          ModifiersToString(field.modifiers()),
          field.field_type(),
          field.name());
    }
  }

  if (!properties.IsEmpty()) {
    foreach (ArrayList_<const Property*>::Enum, en, properties) {
      auto& property = *en.Get();
      writer_.WriteLine("property %s%s %s {",
        ModifiersToString(property.modifiers()),
        property.property_type(),
        property.name());
      foreach (Property::EnumMember, members, property) {
        auto entry = members.Get();
        auto& name = *entry.GetKey();
        auto& method = *entry.GetValue();
        writer_.WriteLine("  %s: %s;", name, method);
      }
      writer_.WriteLine("}");
    }
  }

  if (!method_groups.IsEmpty()) {
    foreach (ArrayList_<const MethodGroup*>::Enum, en, method_groups) {
      auto& method_group = *en.Get();
      foreach (MethodGroup::EnumMethod, methods, method_group) {
        auto& method = *methods.Get();
        DumpMethod(method);
      }
    }
  }

  foreach (Class::EnumOuterAndSelf, outers, clazz) {
    auto& class_or_ns = outers.Get();
    if (class_or_ns.IsGlobalNamespace()) {
      break;
    }
    writer_.WriteLine("} // %s", class_or_ns.name());
  }
}

void EeTextDumper::DumpFunction(const Function& fun) {
  writer_.WriteLine("function %s %s%s {",
      fun.function_type().return_type(),
      fun.name(),
      fun.function_type().params_type());

  auto index = 0;
  foreach (Function::EnumBBlock, bblocks, fun) {
    auto& bb = *bblocks.Get();
    writer_.WriteLine("%s:", bb);
    foreach (BBlock::EnumI, insts, bb) {
      auto& inst = *insts.Get();
      writer_.Write("L%04d: ", index);
      DumpInstruction(inst);
      writer_.WriteLine();
      ++index;
    }
  }
  writer_.WriteLine('}');
}

void EeTextDumper::DumpInstruction(const Instruction& inst) {
  writer_.Write(inst.GetMnemonic());
  if (inst.output_type() != *Ty_Void) {
    writer_.Write(" %s %s =",
        inst.output_type(),
        inst.GetOutput());
  }

  foreach (Instruction::EnumOperand, operands, inst) {
    const char* close_paren = "";
    if (auto const phi_box =
            operands.GetBox()->DynamicCast<PhiOperandBox>()) {
      writer_.Write(" (%s", phi_box->GetBB());
      close_paren = ")";

    } else if (auto const sw_box =
            operands.GetBox()->DynamicCast<SwitchOperandBox>()) {
      writer_.Write(" (%s", sw_box->GetBB());
      close_paren = ")";
    }

    auto& operand = *operands.Get();
    if (auto const label = operand.DynamicCast<Label>()) {
      writer_.Write(" %s", label->GetBB());
    } else if (auto const literal = operand.DynamicCast<Literal>()) {
      writer_.Write(' ');
      DumpLiteral(*literal);
    } else {
      writer_.Write(" %s", operand.ToString());
    }
    writer_.Write(close_paren);
  }
}

void EeTextDumper::DumpLiteral(const Literal& lit) {
  if (auto const p = lit.DynamicCast<BooleanLiteral>()) {
    writer_.Write(p->value() ? "true" : "false");
    return;
  }

  if (auto const p = lit.DynamicCast<CharLiteral>()) {
    auto const ch = p->value();
    switch (ch) {
      case '\'':
        writer_.Write("'\\''");
        break;

      case '\\':
        writer_.Write("'\\\\'");
        break;

      default:
        if (ch < ' ' || ch > 0x7E) {
          writer_.Write("'\\u%04X'", ch);
        } else {
          writer_.Write("'%c'", ch);
        }
        break;
    }
    return;
  }

  if (auto const p = lit.DynamicCast<Int32Literal>()) {
    writer_.Write("%d", p->value());
    return;
  }

  if (auto const p = lit.DynamicCast<StringLiteral>()) {
    writer_.Write('"');
    foreach (String::EnumChar, chars, p->value()) {
      auto const ch = *chars;
      switch (ch) {
        case '"':
          writer_.Write("\\\"");
          break;

        case '\\':
          writer_.Write("\\\\");
          break;

        default:
          if (ch < ' ' || ch > 0x7E) {
            writer_.Write("\\u%04X", ch);
          } else {
            writer_.Write(ch);
          }
          break;
       }
    }
    writer_.Write('"');
    return;
  }

  writer_.Write(lit.ToString());
}

void EeTextDumper::DumpMethod(const Method& method) {
  writer_.Write("method %s%s %s%s",
      ModifiersToString(method.modifiers()),
      method.function_type().return_type(),
      method.name(),
      method.function_type().params_type());

  if (!method.GetFunction()) {
    writer_.WriteLine(";");
    return;
  }

  writer_.WriteLine(" {");

  for (auto& fun: method.module().functions())
    DumpFunction(fun);

  writer_.WriteLine('}');
}

void EeTextDumper::DumpMethodGroup(const MethodGroup& mtg) {
  foreach (MethodGroup::EnumMethod, methods, mtg) {
    auto& method = *methods.Get();
    DumpMethod(method);
  }
}

} // Executor
