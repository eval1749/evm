// @(#)$Id$
//
// Evita Compiler
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_evc_Attribute_h)
#define INCLUDE_evc_Attribute_h

namespace Compiler {

class Attribute :
        public DoubleLinkedItem_<Attribute> {
}; // Attribute

typedef DoubleLinkedList_<Attribute> Attributes;

} // Compiler

#endif // !defined(INCLUDE_evc_Attribute_h)
