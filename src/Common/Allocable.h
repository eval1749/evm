// @(#)$Id$
//
// Evita Common - Allocable
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Common_Allocable_h)
#define INCLUDE_Common_Allocable_h

namespace Common {

class MemoryZone;

// no matching operator delete found; memory will not be freed if
// initialization throws an exception
#pragma warning(disable: 4291)

class Allocable {
    private: struct Extra {
      static const int kMagic = 12345678;
      int live_;
      size_t const size_;
      MemoryZone& zone_;
      Extra(MemoryZone& zone, size_t cb)
        : live_(kMagic), size_(cb), zone_(zone) {}
      void* operator new(size_t, void* pv) { return pv; }
      DISALLOW_COPY_AND_ASSIGN(Extra);
    }; // Extra

    public: void LibExport operator delete(void*, size_t);
    public: void LibExport operator delete[](void*, size_t);
    public: void* LibExport operator new[](size_t, MemoryZone&);
    public: void* LibExport operator new(size_t, MemoryZone&);
    public: void* LibExport operator new[](size_t, MemoryZone*);
    public: void* LibExport operator new(size_t, MemoryZone*);
    public: void* LibExport operator new[](size_t);
    public: void* LibExport operator new(size_t);

    protected: static void* Alloc(MemoryZone&, size_t const);
}; // Allocable

} // Common

#endif // !defined(INCLUDE_Common_Allocable_h)
