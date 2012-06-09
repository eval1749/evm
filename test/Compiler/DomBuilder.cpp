#include "precomp.h"
// @(#)$Id$
//
// Dom
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./DomBuilder.h"

#include "./DomNode.h"

#include "Common.h"

namespace Dom {

Builder::Builder()
    : document_(Document::New()),
      element_(nullptr) {
}

Builder::~Builder() {
}

// [E]
Element& Builder::EmptyElement(
    S name,
    S attr1_name,
    S attr1_value) {
  auto& elem = StartElement(name, attr1_name, attr1_value);
  EndElement(name);
  return elem;
}

Element& Builder::EmptyElement(
    S name,
    S attr1_name,
    S attr1_value,
    S attr2_name,
    S attr2_value) {
  auto& elem = StartElement(
      name,
      attr1_name, attr1_value,
      attr2_name, attr2_value);
  EndElement(name);
  return elem;
}

void Builder::EndElement(const String& name) {
  ASSERT(element_->node_name() == name);
  auto const parent = element_->parent_node();
  element_ = parent 
    && parent->node_type() == ELEMENT_NODE
    && element_->node_name() == name
      ? static_cast<Element*>(parent)
      : nullptr;
}

// [F]
Document& Builder::Finish() {
  ASSERT(element_ == nullptr);
  return *document_;
}

// [I]
String Builder::InternName(S name) {
  return document_->InternName(name);
}

// [S]
Element& Builder::SetAttribute(
    S const name,
    int const int_value) {
  ASSERT(element_ != nullptr);
  char value[20];
  ::wsprintfA(value, "%d", int_value);
  element_->SetAttribute(
      document_->InternName(name), document_->SaveString(value));
  return *element_;
}

Element& Builder::SetAttribute(S const name, S const value) {
  ASSERT(element_ != nullptr);
  element_->SetAttribute(
      document_->InternName(name), document_->SaveString(value));
  return *element_;
}

Element& Builder::StartElement(S const name) {
  ASSERT(!name.IsEmpty());
  auto& elem = *document_->CreateElement(name);
  if (element_) {
    element_->AppendChild(&elem);
  } else {
    document_->AppendChild(&elem);
  }
  element_ = &elem;
  return *element_;
}

Element& Builder::StartElement(
    S const name,
    S const attr_name,
    S const attr_value) {
  StartElement(name);
  element_->SetAttribute(
      document_->InternName(attr_name), 
      document_->SaveString(attr_value));
  return *element_;
}

Element& Builder::StartElement(
    S const name,
    S const attr1_name,
    S const attr1_value,
    S const attr2_name,
    S const attr2_value) {
  StartElement(name);
  element_->SetAttribute(
      document_->InternName(attr1_name), 
      document_->SaveString(attr1_value));
  element_->SetAttribute(
      document_->InternName(attr2_name), 
      document_->SaveString(attr2_value));
  return *element_;
}

} // Dom

