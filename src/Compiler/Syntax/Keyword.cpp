#include "precomp.h"
// @(#)$Id$
//
// Evita Compiler - Parsre - Keyword
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./Keyword.h"

#include "../Ir/NamespaceDef.h"

namespace Compiler {

namespace {
typedef HashMap_<const Name*, Keyword*> KeywordTable;
static KeywordTable* s_KeywordTable;
}

Keyword::Keyword(
    const Name& name,
    Type* const pType,
    int const iModifier)
    : m_iModifier(iModifier),
      name_(name),
      m_pType(pType) {
}

Keyword* Keyword::Find(const Name& name) {
  return s_KeywordTable->Get(&name);
}

void Keyword::Init() {
  #define Modifier_abstract Il::Ir::Modifier_Abstract
  #define Modifier_const Il::Ir::Modifier_Const
  #define Modifier_extern Il::Ir::Modifier_Extern
  #define Modifier_internal Il::Ir::Modifier_Internal
  #define Modifier_new Il::Ir::Modifier_New
  #define Modifier_override Il::Ir::Modifier_Override
  #define Modifier_private Il::Ir::Modifier_Private
  #define Modifier_protected Il::Ir::Modifier_Protected
  #define Modifier_public Il::Ir::Modifier_Public
  #define Modifier_readonly Il::Ir::Modifier_ReadOnly
  #define Modifier_final Il::Ir::Modifier_Final
  #define Modifier_static Il::Ir::Modifier_Static
  #define Modifier_virtual Il::Ir::Modifier_Virtual
  #define Modifier_volatile Il::Ir::Modifier_Volatile

  #define DEFKEYWORD(mp_name) \
    K_ ## mp_name = new Keyword(*Q_ ## mp_name); \
    s_KeywordTable->Add(Q_ ## mp_name, K_ ## mp_name);

  #define DEFMODIFIER(mp_name) \
      K_ ## mp_name = new Keyword( \
          *Q_ ## mp_name, \
          nullptr, \
          Modifier_ ## mp_name); \
      s_KeywordTable->Add(Q_ ## mp_name, K_ ## mp_name);

  #define DEFTYPE(mp_name, mp_Name) \
      K_ ## mp_name = new Keyword( \
          *Q_ ## mp_name, \
          Ty_ ## mp_Name); \
      s_KeywordTable->Add(Q_ ## mp_name, K_ ## mp_name);

  s_KeywordTable = new KeywordTable();
  #include "./keywords.inc"
}

// [T]
String Keyword::ToString() const {
  return name_.ToString();
}

} // Compiler
