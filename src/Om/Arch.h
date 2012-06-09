// @(#)$Id$
//
// Evita Om - Memory Manager
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Om_Arch_h)
#define INCLUDE_Om_Arch_h

namespace Om {

class Arch32 {
  public: static size_t const Align_Object = 8;
  public: static size_t const AllocUnit = 1 << 16;
  public: static int const CharCodeLimit = 1 << 16;
};

typedef Arch32 Arch;

} // Om

#endif // !defined(INCLUDE_Om_Arch_h)
