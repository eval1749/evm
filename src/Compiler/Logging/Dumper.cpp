#include "precomp.h"
// @(#)$Id$
//
// Evita Compiler - Dumper
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./Dumper.h"

#include "../CompilationUnit.h"
#include "../CompileSession.h"

#include "../Ir/ClassDef.h"
#include "../Ir/MethodDef.h"
#include "../Ir/MethodGroupDef.h"
#include "../Ir/NamespaceBody.h"
#include "../Ir/NamespaceDef.h"
#include "../Ir/PropertyDef.h"

#include "../../Common/Io/TextWriter.h"

namespace Compiler {

namespace {
static const char kStyle[] =
  "body { font-size: 10pt; }"
  "h1 { font-size: 140%; }"
  "h2 { font-size: 120%; }"
  "h3 { font-size: 110%; }"
  "h4 { font-size: 100%; }"
  "a.io { color: #800080; }"
  "a.il { color: #800000; }"
  "a.ix { color: #800080 }"
  "a.i {"
  "   font-family: Consolas;"
  "}"
  "div.is {"
  "   color: #008000;"
  "   margin-left: 10pt;"
  "}"
  "i.m { color: #808080; font-style: normal; }"
  "i.p { color: #808080; font-style: normal; }"
  "i.t { color: #808080; }"
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
Dumper::Dumper(TextWriter& writer, const CompilationUnit& cm_unit) 
    : cm_unit_(cm_unit), writer_(writer) {}

// [D]
void Dumper::Dump() {
  writer_.WriteLine("<html>");
  writer_.WriteLine("<head>");
  writer_.WriteLine("<meta charset='utf-8'>");
  writer_.WriteLine("<title>%s</title>", escape(cm_unit_.source_path()));
  writer_.WriteLine("<style type='text/css'>%s</style>", kStyle);
  writer_.WriteLine("</head>");
  writer_.WriteLine("<body>");
  writer_.WriteLine("<h1>%s</h1>", escape(cm_unit_.source_path()));

  class Local {
    public: static void CollectClassDef(
        const NamespaceBody& nsb,
        ArrayList_<ClassDef*>& list) {
      for (auto& member: nsb.members()) {
        if (auto const class_def = member.DynamicCast<ClassDef>()) {
          list.Add(class_def);
        } else if (auto const inner = member.DynamicCast<NamespaceBody>()) {
          CollectClassDef(*inner, list);
        }
      }
    }
  };

  ArrayList_<ClassDef*> class_list;
  Local::CollectClassDef(cm_unit_.namespace_body(), class_list);

  writer_.WriteLine("<h2>Classes</h2>");
  writer_.WriteLine("<ol>");
  for (auto const class_def: class_list) {
    writer_.WriteLine("<li><a href='#c%p'>%s</a></li>",
        *class_def,
        escape(class_def->ToString()));
  }
  writer_.WriteLine("</ol>");

  for (auto const class_def: class_list) {
    DumpClass(*class_def);
  }

  writer_.WriteLine("</body>");
  writer_.WriteLine("</html>");
}

void Dumper::DumpClass(const ClassDef& class_def) {
  writer_.WriteLine("<h2 id='c%p'>%sclass %s</h2>",
    &class_def,
    ModifiersToString(class_def.GetModifiers()),
    escape(class_def.ToString()));

  ArrayList_<const FieldDef*> fields;
  ArrayList_<const MethodGroupDef*> method_groups;
  ArrayList_<const PropertyDef*> properties;

  for (auto& member: class_def.members()) {
    if (auto const field = member.DynamicCast<FieldDef>()) {
      fields.Add(field);
    } else if (auto const mg = member.DynamicCast<MethodGroupDef>()) {
      method_groups.Add(mg);
    } else if (auto const prop = member.DynamicCast<PropertyDef>()) {
      properties.Add(prop);
    }
  }

  if (!fields.IsEmpty()) {
    writer_.WriteLine("<h3>Fields</h3>");
    writer_.WriteLine("<ol>");
    for (auto const field: fields) {
      writer_.WriteLine("<li>%s%s <b>%s</b></li>",
          ModifiersToString(field->GetModifiers()),
          escape(field->GetTy()->ToString()),
          field->name());
    }
    writer_.WriteLine("</ol>");
  }


  if (!properties.IsEmpty()) {
    writer_.WriteLine("<h3>Properties</h3>");
    writer_.WriteLine("<ol>");
    for (auto const prop: properties) {
      writer_.WriteLine("<li>%s %s <b>%s</b>",
          ModifiersToString(prop->modifiers()),
          prop->property_type(),
          prop->name());
      writer_.WriteLine("<ol>");
      for (auto const entry: prop->entries()) {
        auto& method = *entry.value();
        auto& propty = *entry.key();
        writer_.Write("<li>%s ", escape(propty.ToString()));
        WriteMethodRef(method);
        writer_.WriteLine("</li>");
      }
      writer_.WriteLine("</ol>");
    }
    writer_.WriteLine("</ol>");
  }

  if (!method_groups.IsEmpty()) {
    writer_.WriteLine("<h3>Method Groups</h3>");
    writer_.WriteLine("<ol>");
    for (auto const method_group: method_groups) {
      writer_.WriteLine("<li>%s", escape(method_group->name()));
      writer_.WriteLine("<ol>");
      foreach (MethodGroupDef::EnumMethodDef, methods, *method_group) {
        auto& method = *methods.Get();
        writer_.Write("<li>");
        WriteMethodRef(method);
        writer_.WriteLine("</li>");
      }
      writer_.WriteLine("</ol>");
      writer_.WriteLine("</li>");
    }
    writer_.WriteLine("</ol>");

    for (auto const method_group: method_groups) {
      foreach (MethodGroupDef::EnumMethodDef, methods, *method_group) {
        auto& method = *methods.Get();
        if (method.function()) {
          DumpMethod(method);
        }
      }
    }
  }
}

void Dumper::DumpFunction(const Function& fun) {
  writer_.WriteLine("<h4 id='f%p'>%s %s%s</h4>",
      &fun,
      escape(fun.function_type().return_type().ToString()),
      escape(fun.name()),
      escape(fun.function_type().params_type().ToString()));

  auto index = 0;
  for (auto& bb: fun.bblocks()) {
    auto last_line = -1;
    writer_.WriteLine("<a id='b%p'>%s</a>:<br>", bb, bb);
    foreach (BBlock::EnumI, insts, bb) {
      auto& inst = *insts.Get();

      auto& source_info = inst.source_info();
      if (source_info.HasInfo() && source_info.line() != last_line) {
        auto source = cm_unit_.GetLine(source_info.GetLine());
        if (!source.IsEmpty()) {
          last_line = source_info.line();
          writer_.WriteLine("<div class=is>// %d: %s</div>",
            source_info.line() + 1, escape(source));
        }
      }

      writer_.Write("L%04d%c ", index, source_info.HasInfo() ? ':' : '-');
      DumpInstruction(inst);
      writer_.Write("<br>");
      ++index;
    }
    writer_.WriteLine("<br>");
  }
}

void Dumper::DumpInstruction(const Instruction& inst) {
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

void Dumper::DumpLiteral(const Literal& lit) {
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
        writer_.Write("'\\''");
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

void Dumper::DumpMethod(const MethodDef& method) {
  writer_.WriteLine("<h2 id='m%p'>method %s</h2>",
      &method,
      escape(method.ToString()));
  if (!method.function()) {
    return;
  }
  for (auto& fun: method.module().functions())
    DumpFunction(fun);
}

void Dumper::WriteMethodRef(const MethodDef& method) {
  writer_.Write("<i class='m'>%s</i>"
    " <i class='t'>%s</i>"
    " <a href='#m%p'>%s</a>(",
    ModifiersToString(method.modifiers()),
    escape(method.return_type().ToString()),
    &method,
    escape(method.name()));
  const char* comma = "";
  foreach (MethodDef::EnumParam, params, method) {
    auto& param = *params.Get();
    writer_.Write("%s<i class='t'>%s</i> <i class='p'>%s</i>",
        comma,
        escape(param.GetTy()->ToString()),
        escape(param.name().ToString()));
    comma = ", ";
  }
  writer_.Write(")");
}

} // Compiler
