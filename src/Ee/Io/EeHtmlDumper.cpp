#include "precomp.h"
// @(#)$Id$
//
// Evita Compiler - EeHtmlDumper
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./EeHtmlDumper.h"

#include "../../Common/Io/TextWriter.h"

namespace Executor {

namespace {
static const char kStyle[] =
  "body { font-size: 10pt; }"
  "h1 { font-size: 140%; }"
  "h2 { font-size: 120%; }"
  "h3 { font-size: 110%; }"
  "h4 { font-size: 100%; }"
  "a.io { color: #000080 }"
  "div.i {"
  "   font-family: Consolas;"
  "   margin-left: 10pt;"
  "}"
  "div.is {"
  "   color: #008000;"
  "   margin-left: 10pt;"
  "}"
  "a.il { color: #800000; }"
  "";

static String escape(const String& str) {
  if (str.IndexOf('<') < 0 && str.IndexOf('&')) {
   return str;
  }

  StringBuilder builder;
  foreach (String::EnumChar, chars, str) {
    auto const ch = *chars;
    switch (ch) {
      case '&':
        builder.Append("&amp;");
        break;

      case '<':
        builder.Append("&lt;");
        break;

      default:
        builder.Append(ch);
        break;
    }
  }
  return builder.ToString();
}

} // namespace

// ctor
EeHtmlDumper::EeHtmlDumper(TextWriter& writer) : writer_(writer) {}

// [D]
void EeHtmlDumper::DumpClass(const Class& clazz) {
  writer_.WriteLine("<h2 id='c%p'>%sclass %s</h2>",
    &clazz,
    ModifiersToString(clazz.GetModifiers()),
    escape(clazz.ToString()));

  ArrayList_<const Field*> fields;
  ArrayList_<const MethodGroup*> method_groups;

  foreach (Class::EnumMember, members, clazz) {
    auto const member = members.Get();
    if (auto const field = member->DynamicCast<Field>()) {
      fields.Add(field);
    } else if (auto const mg = member->DynamicCast<MethodGroup>()) {
      method_groups.Add(mg);
    }
  }

  if (!fields.IsEmpty()) {
    writer_.WriteLine("Fields");
    writer_.WriteLine("<ol>");
    foreach (ArrayList_<const Field*>::Enum, en, fields) {
      auto& field = *en.Get();
      writer_.WriteLine("<li>%s%s %s</li>",
          ModifiersToString(field.GetModifiers()),
          field.GetTy(),
          field.name());
    }
    writer_.WriteLine("</ol>");
  }

  if (!method_groups.IsEmpty()) {
    writer_.WriteLine("Method Groups");
    writer_.WriteLine("<ol>");
    foreach (ArrayList_<const MethodGroup*>::Enum, en, method_groups) {
      auto& method_group = *en.Get();
      writer_.WriteLine("<li>%s", escape(method_group.name()));
      writer_.WriteLine("<ol>");
      foreach (MethodGroup::EnumMethod, methods, method_group) {
        auto& method = *methods.Get();
        writer_.WriteLine("<li><a href='#m%p'>%s</a></li>",
            &method, 
            escape(method.ToString()));
      }
      writer_.WriteLine("</ol>");
      writer_.WriteLine("</li>");
    }
    writer_.WriteLine("</ol>");

    foreach (ArrayList_<const MethodGroup*>::Enum, en, method_groups) {
      auto& method_group = *en.Get();
      foreach (MethodGroup::EnumMethod, methods, method_group) {
        auto& method = *methods.Get();
        DumpMethod(method);
      }
    }
  }
}

void EeHtmlDumper::DumpFunction(const Function& fun) {
  writer_.WriteLine("<h3 id='f%p'>%s %s%s</h3>",
      &fun,
      escape(fun.function_type().return_type().ToString()),
      escape(fun.name()),
      escape(fun.function_type().params_type().ToString()));

  auto index = 0;
  foreach (Function::EnumBBlock, bblocks, fun) {
    auto& bb = *bblocks.Get();
    writer_.WriteLine("<a id='b%p'>%s</a>:<br>", bb, bb);
    foreach (BBlock::EnumI, insts, bb) {
      auto& inst = *insts.Get();
      writer_.Write("L%04d: ", index);
      DumpInstruction(inst);
      writer_.Write("<br>");
      ++index;
    }
    writer_.WriteLine("<br>");
  }
}

void EeHtmlDumper::DumpInstruction(const Instruction& inst) {
  writer_.Write("<a class=i>%s", inst.GetMnemonic());
  if (inst.output_type() != *Ty_Void) {
    writer_.Write(" %s <a class=io id='o%p'>%s</a> =",
        escape(inst.output_type().ToString()),
        inst.GetOutput(),
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
      writer_.Write(" <a href='#b%p'>%s</a>", label->GetBB(), operand);
    } else if (auto const literal = operand.DynamicCast<Literal>()) {
      writer_.Write(" <a class=il>");
      DumpLiteral(*literal);
      writer_.Write("</a>");
    } else if (operand.Is<Output>()) {
      writer_.Write(" <a href='#o%p'>%s</a>", 
          operand, escape(operand.ToString()));
    } else {
      writer_.Write(" <a class=ix>%s</a>", escape(operand.ToString()));
    }
    writer_.Write(close_paren);
  }
  writer_.WriteLine("</a>");
}

void EeHtmlDumper::DumpLiteral(const Literal& lit) {
  if (auto const p = lit.DynamicCast<BooleanLiteral>()) {
    writer_.Write(p->value() ? "true" : "false");
    return;
  }

  if (auto const p = lit.DynamicCast<CharLiteral>()) {
    auto const ch = p->value();
    switch (ch) {
      case '&':
        writer_.Write("'&amp;'");
        break;

      case '\'':
        writer_.Write("'\\\''");
        break;

      case '<':
        writer_.Write("'&lt;'");
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

        case '&':
          writer_.Write("&amp;");
          break;

        case '<':
          writer_.Write("&lt;");
          break;

        case '\\':
          writer_.Write("'\\\\'");
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

void EeHtmlDumper::DumpMethod(const Method& method) {
  writer_.WriteLine("<h2 id='m%p'>method %s</h2>",
      &method,
      escape(method.ToString()));
  if (!method.GetFunction()) {
    return;
  }

  for (auto& fun: method.module().functions())
    DumpFunction(fun);
}

void EeHtmlDumper::DumpMethodGroup(const MethodGroup& mtg) {
  writer_.WriteLine("<h2 id='m%p'>%s</h2>", &mtg, mtg);
  foreach (MethodGroup::EnumMethod, methods, mtg) {
    auto& method = *methods.Get();
    DumpMethod(method);
  }
}

} // Executor
