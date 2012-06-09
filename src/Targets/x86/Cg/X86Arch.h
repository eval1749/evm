// @(#)$Id$
//
// Evita Il - IR - IL
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#if !defined(INCLUDE_Il_Cg_X86Arch_h)
#define INCLUDE_Il_Cg_X86Arch_h

namespace Il {
namespace X86 {

enum Op
{
  #define DEFFORMAT_0(mp_o, mp_m) \
      op_ ## mp_m = mp_o,

  #define DEFFORMAT_1(mp_o, mp_m, mp_1) \
      op_ ## mp_m ## _ ## mp_1 = mp_o,

  #define DEFFORMAT_2(mp_o, mp_m, mp_1, mp_2) \
      op_ ## mp_m ## _ ## mp_1 ## _ ## mp_2  = mp_o,

  #define DEFFORMAT_3(mp_o, mp_m, mp_1, mp_2, mp_3) \
      op_ ## mp_m ## _ ## mp_1 ## _ ## mp_2 ## _ ## mp_3  = mp_o,

  #define DEFFORMAT_EXT_1(mp_o, mp_e, mp_m, mp_1) \
      op_ ## mp_m ## _ ## mp_1  = mp_o,

  #define DEFFORMAT_EXT_2(mp_o, mp_e, mp_m, mp_1, mp_2) \
      op_ ## mp_m ## _ ## mp_1 ## _ ## mp_2  = mp_o,

  #define DEFFORMAT_EXT_3(mp_o, mp_e, mp_m, mp_1, mp_2, mp_3) \
      op_ ## mp_m ## _ ## mp_1 ## _ ## mp_2 ## _ ## mp_3  = mp_o,

  #include "./X86OpCode.inc"
}; // Op

enum Opext
{
  #define DEFFORMAT_EXT_1(mp_o, mp_e, mp_m, mp_1) \
      opext_ ## mp_m ## _ ## mp_1  = mp_e,

  #define DEFFORMAT_EXT_2(mp_o, mp_e, mp_m, mp_1, mp_2) \
      opext_ ## mp_m ## _ ## mp_1 ## _ ## mp_2 = mp_e,

  #define DEFFORMAT_EXT_3(mp_o, mp_e, mp_m, mp_1, mp_2, mp_3) \
      opext_ ## mp_m ## _ ## mp_1 ## _ ## mp_2 ## _ ## mp_3 = mp_e,

  #include "./X86OpCode.inc"
}; // Opext

enum Reg {
  // GPR 8bit
  $AL = 0x00, $CL = 0x01, $DL = 0x02, $BL = 0x03,
  $AH = 0x04, $CH = 0x05, $DH = 0x06, $BH = 0x07,

  // GPR 16bit
  $AX = 0x08, $CX = 0x09, $DX = 0x0A, $BX = 0x0B,
  $SP = 0x0C, $BP = 0x0D, $SI = 0x0E, $DI = 0x0F,

  // GPR 32bit
  $EAX = 0x10, $ECX = 0x11, $EDX = 0x12, $EBX = 0x13,
  $ESP = 0x14, $EBP = 0x15, $ESI = 0x16, $EDI = 0x17,

  // MM 64bit
  $MM0 = 0x18, $MM1 = 0x19, $MM2 = 0x1A, $MM3 = 0x1B,
  $MM4 = 0x1C, $MM5 = 0x1D, $MM6 = 0x1E, $MM7 = 0x1F,

  // XMM 128bit
  $XMM0 = 0x20, $XMM1 = 0x21, $XMM2 = 0x22, $XMM3 = 0x23,
  $XMM4 = 0x24, $XMM5 = 0x25, $XMM6 = 0x26, $XMM7 = 0x27,

  // Register arguments and values
  $r0 = $EAX, $r1 = $EDX, $r2 = $EBX,
  $r3 = $ESI, $r4 = $EDI,
  $rn = $ECX,

  // Kernel pointers
  $tcb = $EBP,
  $sp  = $ESP,

  // True value of generalized boolean
  $true = $tcb,

  Reg_Limit = 8,
}; // Reg

enum Mod {
  Mod_Disp0  = 0x00,
  Mod_Disp8  = 0x40,
  Mod_Disp32 = 0x80,
  Mod_Reg    = 0xC0,
}; // Mod

enum Rm {
  Rm_Sib      = 4,
  Rm_Disp32   = 5,
}; // Rm

enum Scale {
  Scale_None  = 1,
  Scale_1     = 0x00,
  Scale_2     = 0x40,
  Scale_4     = 0x80,
  Scale_8     = 0xC0,
}; // Scale

enum Tttn {
 tttn_O     = 0,          // overflow
 tttn_NO    = 1,          // no overflow
 tttn_B     = 2,          // below (ULT)
   tttn_C    = tttn_B,    // carry
   tttn_NAE  = tttn_B,    // not above or equal
 tttn_NB    = 3,          // not below (UGE)
   tttn_AE = tttn_NB,     // above or equal
   tttn_NC = tttn_NB,     // no carry
 tttn_E     = 4,          // equal
   tttn_Z = tttn_E,       // zero
 tttn_NE    = 5,          // not equal
   tttn_NZ = tttn_NE,     // not zero
 tttn_BE    = 6,          // below or equal (ULE)
   tttn_NA = tttn_BE,     // not above
 tttn_NBE   = 7,          // not below or equal (UGT)
   tttn_A = tttn_NBE,     // above
 tttn_S     = 8,          // sign
 tttn_NS    = 9,          // not sign
 tttn_P     = 10,         // parity
   tttn_PE = tttn_P,      // parity even
 tttn_NP    = 11,         // not parity
   ttttn_PO = tttn_NP,    // parity odd
 tttn_L     = 12,         // less than
   tttn_NGE = tttn_L,     // not greater than or equal to
 tttn_GE    = 13,         // greater than or equal to
   tttn_NL = tttn_GE,     // not less than
 tttn_LE    = 14,         // less than or equal to
   tttn_NG = tttn_LE,     // not greater than
 tttn_G     = 15,         // greater than
   tttn_NLE = tttn_G,     // not less than
}; // Tttn


__forceinline Tttn FlipTttn(Tttn eTttn)
  { return static_cast<Tttn>(eTttn ^ 1); }


const Reg k_rgeRegVal[5] = { $r0, $r1, $r2, $r3, $r4 };

inline uint8 ModRm(Mod eMod, Reg rx, Reg rb) {
  int iReg = rx & 7;
  int iRm  = rb & 7;
  return static_cast<uint8>(eMod | (iReg << 3) | iRm);
} // ModRm

inline uint8 ModRm(Mod eMod, Reg rx, Rm eRm) {
  int iReg = rx & 7;
  return static_cast<uint8>(eMod | (iReg << 3) | eRm);
} // ModRm

inline uint8 ModRm(Mod eMod, Opext opext, Reg rd) {
  int iRm  = rd & 7;
  return static_cast<uint8>(eMod | (opext << 3) | iRm);
} // ModRm

inline uint8 ModRm(Mod eMod, Opext opext, Rm eRm) {
  return static_cast<uint8>(eMod | (opext << 3) | eRm);
} // ModRm

#if 0
struct ThreadExtra {
  uint32  m_rgn[26];
  uint8*  m_NotFunction;              // [26] -24
  uint8*  m_TypeError;                // [27] -20
  uint8*  m_UndefinedFunction;        // [28] -16
  uint8*  m_AllocConsArea;            // [29] -12
  uint8*  m_ArityError;               // [30] -8
  Val     m_FixnumOne;                // [31] -4
}; // ThreadExtra

CASSERT(sizeof(ThreadExtra) == 128);

#define ThreadExtra_(mp_field) \
  ( offsetof(ThreadExtra, m_ ## mp_field) - 128 )

#define SVC_alloc_cons_area     ThreadExtra_(AllocConsArea)
#define ThreadExtra_ArityError  ThreadExtra_(ArityError)
#define ThreadExtra_FixnumOne   ThreadExtra_(FixnumOne)

CASSERT(ThreadExtra_(FixnumOne) == -4);

}; // X86

// Note:
//  Due to pointer tag scheme, location of m_classd of native-code-object
//  must be second word instead of the first word.
//
// Note:
//  m_nCookie must be slot[2] for stack walking during GC.
struct layout_funobj
{
  size_t  m_cbFunction;
  Val     m_classd;
  Val     m_cookie;
  Val     m_frob;
}; // layout_funobj

CASSERT(16 == sizeof(layout_funobj));

//
//      +-------------------+
//    0 | m_cbFunction      |   +0
//      +-------------------+
//    1 | m_classd          |   +4
//      +-------------------+
//    2 | m_cookie          |   +8
//      +-------------------+
//    3 | m_frob            |   +12
//      +-------------------+
//      |                   |   +16
//      |  ... code ...     |
//      |                   |
//      +-------------------+
//      | padding 4         |
//      +-------------------+
//      | ... annots ...    |
//      +-------------------+
//      | ... gcmap ...     | for compatibility to Evita Common Lisp
//      +-------------------+
//      | desc.m_ofsGcMap   |
//      | desc.m_ofsAnnot   |
//      | desc.m_cbCode     |
//      | desc.m_nFrame     |
//      +-------------------+
//
class FunObj : public Datum, public layout_funobj
{
  public: static uint32 const Cookie = 0x0F0BCEF4;

  public: enum Contant
  {
      Tag     = Arch::Tag_FunObj,
      Align   = 16, // for finding FunObj from return address
  }; // Constant

  public: struct Annot
  {
      enum Kind
      {
          Kind_LispVal        = 0,
          Kind_NamedCallee    = 1,
          Kind_LocalCallee    = 2,
          //Kind_Callee         = 3,    // for discriminator
          Kind_SymFun         = 4,
          Kind_SymSetf        = 5,
          Kind_SymVal         = 6,
          Kind_NotUsed7       = 7,

          Kind_TlvOfs         = -8,   // 8
          Kind_RelLabel       = -7,   // 9
          Kind_ClosedLit      = -6,   // 10   object or fixnum
          Kind_ClosedVar      = -5,   // 11   closed-cell or fixnum
          Kind_DllLink        = -4,   // 12
          Kind_NotUsed13      = 13,   // 13
          Kind_AbsLabel       = -2,   // 14
          Kind_NotUsed15      = 15,   // 15

          Kind_Limit,
      }; // Kind

      Kind    m_eKind : 4;
      uint    m_ofs   : 28;

      Annot(Kind e, uint ofs) : m_eKind(e), m_ofs(ofs) {}
  }; // Annot

  public: enum FrameKind
  {
      FrameKind_Fixed     = 0,
      FrameKind_Restify   = 1,
      FrameKind_2         = 2,
      FrameKind_3         = 3,
  }; // FrameKind

  public: struct FunDesc
  {
      uint32  m_ofsGcMap;
      uint32  m_ofsAnnot;
      uint32  m_cbCode;
      uint32  m_nFrame;
  }; // FunDesc

  // [D]
  public: static Int Decode_(const Datum* const x)
  {
      ASSERT(Is_(x));
      return x->ToInt() - Tag;
  } // Decode_

  // [E]
  public: class EnumAnnot
  {
      private: Annot* m_pEnd;
      private: Annot* m_pRunner;

      public: EnumAnnot(const FunObj* p) :
          m_pEnd(p->GetAnnotStart() + p->GetAnnotSize() / sizeof(Annot)),
          m_pRunner(p->GetAnnotStart()) {}

      public: bool  AtEnd() const { return m_pRunner >= m_pEnd; }
      public: Annot Get()   const { ASSERT(!AtEnd()); return *m_pRunner; }
      public: void Next() { ASSERT(!AtEnd()); m_pRunner++; }

      public: void Set(Annot oAnnot)
      {
          ASSERT(!AtEnd());
          *m_pRunner = oAnnot;
      } // Set
  }; // EnumAnnot

  public: Val Encode() const
  {
      return FromInt<Datum>(ToInt() + Tag);
  } // Encode

  // [F]
  public: Val FetchCallee(uint ofs) const
  {
      Int iRel = FetchS32(ofs);

      Int iCallee = reinterpret_cast<Int>(
          (GetCodeStart() + ofs + 4) + iRel );
      iCallee -= sizeof(FunObj);
      iCallee |= Arch::Tag_FunObj;
      return FromInt<Datum>(iCallee);
  } // FetchCallee

  public: DllEntry* FetchDllEntry(uint ofs) const
      { return reinterpret_cast<DllEntry*>(FetchUn(ofs)); }

  public: int32 FetchS32(uint ofs) const
      { return static_cast<int32>(FetchU32(ofs)); }

  public: Val FetchSymFun(uint ofs) const
  {
      UInt u32 = FetchU32(ofs);
      u32 -= offsetof(Symbol, m_function);
      return reinterpret_cast<Symbol*>(u32)->Encode();
  } // FetchSymFun

  public: Val FetchSymSetf(uint ofs) const
  {
      UInt u32 = FetchU32(ofs);
      u32 -= offsetof(SetfCell, m_function);
      return reinterpret_cast<ValueCell*>(u32)->Encode();
  } // FetchSymSetf

  public: Val FetchSymVal(uint ofs) const
  {
      UInt u32 = FetchU32(ofs);
      u32 -= offsetof(ValueCell, m_value);
      return reinterpret_cast<ValueCell*>(u32)->Encode();
  } // FetchSymVal

  public: uint32 FetchU32(uint ofs) const
  {
      return reinterpret_cast<uint32*>(GetCodeStart() + ofs)[0];
  } // FetchU32

  public: UInt FetchUn(uint ofs) const
      { return static_cast<UInt>(FetchU32(ofs)); }

  public: Val FetchVal(uint ofs) const
  {
      return reinterpret_cast<Val>(static_cast<UInt>(FetchU32(ofs)));
  } // FetchVal

  // [G]
  public: size_t GetAnnotSize() const
  {
      const FunDesc* pDesc = GetFunDesc();
      return pDesc->m_ofsGcMap - pDesc->m_ofsAnnot;
  } // GetAnnotSize

  public: Annot* GetAnnotStart() const
  {
      return reinterpret_cast<Annot*>(
          ToInt() + GetFunDesc()->m_ofsAnnot );
  } // GetAnnotStart

  public: uint8* GetCodeStart() const
  {
      return reinterpret_cast<uint8*>(
          ToInt() + sizeof(FunObj) );
  } // GetCode

  public: uint GetCodeSize() const
  {
      return GetFunDesc()->m_cbCode;
  } // GetCode

  public: FrameKind GetFrameKind() const
  {
      return static_cast<FrameKind>(GetFunDesc()->m_nFrame & 3);
  } // GetFrameKind

  public: size_t GetFixedFrameSize() const
  {
      return GetFunDesc()->m_nFrame >> 2;
  } // GetFixedFrameSize

  public: FunDesc* GetFunDesc() const
  {
      return reinterpret_cast<FunDesc*>(
          ToInt() + m_cbFunction - sizeof(FunDesc) );
  } // GetFunDesc

  // GetGcMap
  public: uint32* GetGcMap() const
  {
      return reinterpret_cast<uint32*>(
          ToInt() + GetFunDesc()->m_ofsGcMap );
  } // GetGcMap

  // GetGcMapSize
  public: uint GetGcMapSize() const
  {
      return static_cast<uint>(
          reinterpret_cast<Int>(GetFunDesc()) -
          reinterpret_cast<Int>(GetGcMap()) );
  } // GetGcMapSize

  // [I]
  public: static bool Is_(const Datum* const x)
      { return Tag == x->GetTag(); }

  // [P]
  public: void PatchCallee(uint ofs, Val fn)
  {
      uint8* pbCallee = fn->StaticCast<CodeObject>()->GetCodeStart();
      uint8* pbCall   = GetCodeStart() + ofs + 4;

      Int iRel = pbCallee - pbCall;
      PatchU32(ofs, static_cast<uint32>(iRel));
  } // PatchCallee

  public: void PatchVal(uint ofs, Val val)
  {
      PatchU32(ofs, static_cast<uint32>(val->ToInt()));
  } // PatchLit

  public: void PatchSymFun(uint ofs, Val cell)
  {
      check_type(cell, symbol);
      Symbol* p = cell->StaticCast<Symbol>();
      PatchU32(ofs, static_cast<uint32>(
          p->ToInt() + offsetof(Symbol, m_function) ) );
  } // PathSymFun

  public: void PatchSymSetf(uint ofs, Val cell)
  {
      check_type(cell, setf_cell);
      SetfCell* p = cell->StaticCast<SetfCell>();
      PatchU32(ofs, static_cast<uint32>(
          p->ToInt() + offsetof(SetfCell, m_function) ) );
  } // PathSymSetf

  public: void PatchSymVal(uint ofs, Val cell)
  {
      check_type(cell, value_cell);
      ValueCell* p = cell->StaticCast<ValueCell>();
      PatchU32(ofs, static_cast<uint32>(
          p->ToInt() + offsetof(ValueCell, m_value) ) );
  } // PathSymVal

  public: void PatchTlvOfs(uint ofs, Val tlvrec)
  {
      check_type(tlvrec, tlv_record);

      Int iIndex = Fixnum::Decode_(
          tlvrec->StaticCast<TlvRecord>()->m_index );

      PatchU32(ofs, static_cast<uint32>(
          offsetof(Thread, mv_tlv) + sizeof(Val) * iIndex ) );
  } // PathTlvOfs

  public: void PatchU32(uint ofs, uint32 u32)
  {
      reinterpret_cast<uint32*>(GetCodeStart() + ofs)[0] = u32;
  } // PatchU32

  public: void PatchUn(uint ofs, UInt un)
      { PatchU32(ofs, static_cast<uint32>(un)); }

  public: void PatchUn(uint ofs, void* pv)
      { PatchU32(ofs, static_cast<uint32>(reinterpret_cast<UInt>(pv))); }
}; // FunObj
#endif

} // X86
} // Il

#endif // !defined(INCLUDE_Il_Cg_X86Arch_h)
