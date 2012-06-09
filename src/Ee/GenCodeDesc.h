// @(#)$Id$
//
// Evita Il - IR - IL
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Targets_Generic_CodeDesc_h)
#define INCLUDE_Targets_Generic_CodeDesc_h

#include "./EeDefs.h"
#include "./EeObject.h"
#include "../Common/Collections.h"

namespace Om {
class Thread;
} // Om

namespace Ee {

using namespace Common::Collections;
using namespace Il::Ir;

class CodeDesc : public EeObject {
  public: class Annotation {
    public: enum Kind {
        Kind_None,

        Kind_AbsLabel = 0,
        Kind_Callee = 1,
        Kind_ClosedLit = 2,
        Kind_ClosedVar = 3,
        Kind_DllLink = 4,
        Kind_Float32 = 5,
        Kind_Float64 = 6,
        Kind_Int16 = 7,
        Kind_Int32 = 8,
        Kind_Int64 = 9,
        Kind_Int8 = 10,
        Kind_Method = 11,
        Kind_Object = 12,
        Kind_RelLabel = 13,
        Kind_UInt16 = 14,
        Kind_UInt32 = 15,
        Kind_UInt64 = 16,
        Kind_UInt8 = 17,
        Kind_NativeCallee = 18,
        Kind_FunRef = 19,
        Kind_MethodRef = 20,
    }; // Kind

    private: uint kind_ : 5;
    private: int offset_ : 27;

    public: Annotation(
        Kind const kind = Kind_None,
        int const offset = 0) :
            kind_(kind),
            offset_(offset) {}

    // For Collection_<Annotation>
    public: Annotation(const Annotation& r)
        : kind_(r.kind_),
          offset_(r.offset_) {}

    public: Kind kind() const { return static_cast<Kind>(kind_); }
    public: int offset() const { return offset_; }
  };

  static_assert(
      sizeof(CodeDesc::Annotation) == 4,
      "Sizeof Annotation must be 4 byte.");

  public: class EnumAnnotation : public Collection_<Annotation>::Enum {
    private: typedef Collection_<Annotation>::Enum Base;
    public: EnumAnnotation(const CodeDesc& r) : Base(r.annotations_) {}
    DISALLOW_COPY_AND_ASSIGN(EnumAnnotation);
  };

  private: const Collection_<Annotation> annotations_;
  private: uint32 const code_size_;
  private: void* codes_;
  private: const Function& function_;

  // ctor
  protected: CodeDesc(
      const Function&,
      const Collection_<Annotation>&,
      void*,
      uint32);

  public: int32 code_size() const { return code_size_; }
  public: void* codes() const { return codes_; }
  public: const Function& function() const { return function_; }

  // [F]
  public: void FixAfterMove();

  // [I]
  public: void Invoke(Om::Thread&);

  // [P]
  public: void PrepareForMove();

  // [S]
  public: void SetPointer(const Annotation&, void*);

  DISALLOW_COPY_AND_ASSIGN(CodeDesc);
};

} // Ee

namespace Common {

inline String Stringify(const Ee::CodeDesc::Annotation& annot) {
  char buf[20];
  ::wsprintfA(buf, "%d.%d", annot.kind(), annot.offset());
  return String(buf);
}

} // Common

#endif // !defined(INCLUDE_Targets_Generic_CodeDesc_h)
