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
        for (auto const typaram: gcl->GetTypeParams()) {
          writer_.Write("%s%s", comma, typaram->name());
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

  for (auto const member: clazz.members()) {
    if (auto const field = member->DynamicCast<Field>()) {
      fields.Add(field);
    } else if (auto const mg = member->DynamicCast<MethodGroup>()) {
      method_groups.Add(mg);
    } else if (auto const property = member->DynamicCast<Property>()) {
      properties.Add(property);
    }
  }

  if (!fields.IsEmpty()) {
    for (auto const field: fields) {
      writer_.WriteLine("field %s%s %s;",
          ModifiersToString(field->modifiers()),
          field->field_type(),
          field->name());
    }
  }

  if (!properties.IsEmpty()) {
    for (auto const property: properties) {
      writer_.WriteLine("property %s%s %s {",
        ModifiersToString(property->modifiers()),
        property->property_type(),
        property->name());
      for (auto const entry: property->entries()) {
        auto& name = *entry.key();
        auto& method = *entry.value();
        writer_.WriteLine("  %s: %s;", name, method);
      }
      writer_.WriteLine("}");
    }
  }

  if (!method_groups.IsEmpty()) {
    for (auto const method_group: method_groups) {
      for (auto& method: method_group->methods())
        DumpMethod(method);
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
  for (auto& bb: fun.bblocks()) {
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

  for (const auto& box: inst.operand_boxes()) {
    const char* close_paren = "";
    if (auto const phi_box = box.DynamicCast<PhiOperandBox>()) {
      writer_.Write(" (%s", phi_box->GetBB());
      close_paren = ")";

    } else if (auto const sw_box =box.DynamicCast<SwitchOperandBox>()) {
      writer_.Write(" (%s", sw_box->GetBB());
      close_paren = ")";
    }

    auto& operand = box.operand();
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
  for (auto& method: mtg.methods())
    DumpMethod(method);
}

} // Executor
