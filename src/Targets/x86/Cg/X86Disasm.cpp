#include "precomp.h"
// @(#)$Id$
//
// Evita Il - Targets - X86 Asm
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./X86Disasm.h"

#include "../../../Common.h"
#include "../../../Common/Collections.h"
#include "../../../Common/Io/TextWriter.h"

#include "../../../Ee/CodeDesc.h"
#include "../../../Ee/CodeManager.h"

#include "../../../Om.h"
#include "../../../Om/Object.h"

namespace Common {
namespace Collections {
inline int ComputeHashCode(int n) { return n; }
} // Collections
} // Common

namespace Il {
namespace X86 {

using Common::Io::TextWriter;

namespace {

enum Extend {
  Extend_ModRm,
  Extend_None,
  Extend_TwoByte,
};

enum OpdFmt {
  OpdFmt_None = 0,

  OpdFmt_1,  // SHL Ev, 1

  OpdFmt_Ib, // imm8
  OpdFmt_Iw, // imm16
  OpdFmt_Iq, // imm64
  OpdFmt_Is, // imm8 with sign extend
  OpdFmt_Iv, // imm16, imm32 or imm64
  OpdFmt_Iz, // imm32

  OpdFmt_Jb,
  OpdFmt_Jv,

  OpdFmt_Eb,
  OpdFmt_Ew,
  OpdFmt_Ed,
  OpdFmt_Eq,
  OpdFmt_Ev,

  OpdFmt_Gb,
  OpdFmt_Gd,
  OpdFmt_Gq,
  OpdFmt_Gv,

  OpdFmt_Ob,
  OpdFmt_Ov,

  OpdFmt_M,
  OpdFmt_Md,
  OpdFmt_Mp,
  OpdFmt_Mpd,
  OpdFmt_Mps,
  OpdFmt_Mq,

  OpdFmt_Nq,   // mm

  OpdFmt_Pd,   // mm
  OpdFmt_Pq,   // mm

  OpdFmt_Qd,   // mm or Mq
  OpdFmt_Qq,   // mm or Mq
  OpdFmt_Qdq,

  OpdFmt_Udq,  // ModRm:rm = xmm
  OpdFmt_Upd,  // ModRm:rm = xmm
  OpdFmt_Ups,  // ModRm:rm = xmm

  OpdFmt_Vdq,  // ModRm:reg = xmm
  OpdFmt_Vpd,  // ModRm:reg = xmm
  OpdFmt_Vpq,  // ModRm:reg = xmm
  OpdFmt_Vps,  // ModRm:reg = xmm
  OpdFmt_Vq,   // ModRm:reg = xmm
  OpdFmt_Vsd,  // ModRm:reg = xmm
  OpdFmt_Vss,  // ModRm:reg = xmm

  OpdFmt_Wdq,  // xmm or Mdq
  OpdFmt_Wpd,  // xmm or Mdq
  OpdFmt_Wps,  // xmm or Mdq
  OpdFmt_Wq,   // xmm or Mdq
  OpdFmt_Wsd,  // xmm or Mdq
  OpdFmt_Wss,  // xmm or Mdq

  OpdFmt_eAX,  // 0
  OpdFmt_eCX,  // 1
  OpdFmt_eDX,  // 2
  OpdFmt_eBX,  // 3
  OpdFmt_eSP,  // 4
  OpdFmt_eBP,  // 5
  OpdFmt_eSI,  // 6
  OpdFmt_eDI,  // 7

  OpdFmt_rAX,  // 0  PUSH/POP
  OpdFmt_rCX,  // 1  PUSH/POP
  OpdFmt_rDX,  // 2  PUSH/POP
  OpdFmt_rBX,  // 3  PUSH/POP
  OpdFmt_rSP,  // 4  PUSH/POP
  OpdFmt_rBP,  // 5  PUSH/POP
  OpdFmt_rSI,  // 6  PUSH/POP
  OpdFmt_rDI,  // 7  PUSH/POP

  OpdFmt_AL,   // 0
  OpdFmt_CL,   // 1
  OpdFmt_DL,   // 2
  OpdFmt_BL,   // 3
  OpdFmt_AH,   // 4
  OpdFmt_CH,   // 5
  OpdFmt_DH,   // 6
  OpdFmt_BH,   // 7

  OpdFmt_DX, // For IN, OUT

  OpdFmt_Sw, // reg of modrm selects segment register

  OpdFmt_EvF64,  // CALL/JMP Ev
  OpdFmt_EvD64,  // PUSH Ev

  OpdFmt_CS,
  OpdFmt_DS,
  OpdFmt_ES,
  OpdFmt_SS,
};

enum RegClass {
  RegClass_Gpr,
  RegClass_Mmx,
  RegClass_Xmm,
};

struct Format {
  int num_operands_;
  Extend extend_;
  int opcode_;
  const char* mnemonic_;
  OpdFmt operand_formats_[3];

  public: class Iterator {
    private: const Format* format_;
    private: int index_;
    public: Iterator(const Format* format, int index)
        : format_(format), index_(index) {}
    public: OpdFmt operator*() const {
      DCHECK_LT(index_, format_->num_operands_);
      return format_->operand_formats_[index_];
    }
    public: bool operator==(const Iterator& another) const {
      DCHECK_EQ(format_, another.format_);
      return index_ == another.index_;
    }
    public: bool operator!=(const Iterator& another) const {
      return !operator==(another);
    }
    public: Iterator& operator++() {
      DCHECK_LT(index_, format_->num_operands_);
      ++index_;
      return *this;
    }
  };

  public: Iterator begin() const { return Iterator(this, 0); }
  public: Iterator end() const { return Iterator(this, num_operands_); }

  public: bool IsPrefix() const { return num_operands_ == -1; }
}; // Format

static Format s_format_db = { 1, Extend_None, -1, "DB", OpdFmt_Ib };
static Format s_format_dd = { 1, Extend_None, -1, "DD", OpdFmt_Iz };
static HashMap_<int, const Format*>* s_format_map;

static const Format k_formats[] = {
    #define DEFFORMAT_0(mp_o, mp_m) \
        { 0, Extend_None, mp_o, # mp_m },

    #define DEFFORMAT_1(mp_o, mp_m, mp_1) \
        { 1, Extend_None, mp_o, # mp_m, OpdFmt_ ## mp_1 },

    #define DEFFORMAT_2(mp_o, mp_m, mp_1, mp_2) \
        { 2, Extend_None, mp_o, # mp_m, OpdFmt_ ## mp_1, OpdFmt_ ## mp_2 },

    #define DEFFORMAT_3(mp_o, mp_m, mp_1, mp_2, mp_3) \
        { 3, Extend_None, mp_o, # mp_m, \
          OpdFmt_ ## mp_1, OpdFmt_ ## mp_2, OpdFmt_ ## mp_3 },

    #define DEFFORMAT_EXT_1(mp_o, mp_e, mp_m, mp_1) \
        { 1, Extend_None, (mp_o << 8) | mp_e, # mp_m, OpdFmt_ ## mp_1 },

    #define DEFFORMAT_EXT_2(mp_o, mp_e, mp_m, mp_1, mp_2) \
        { 2, Extend_None, (mp_o << 8) | mp_e, # mp_m, \
          OpdFmt_ ## mp_1, OpdFmt_ ## mp_2 },

    #define DEFFORMAT_EXT_3(mp_o, mp_e, mp_m, mp_1, mp_2, mp_3) \
        { 3, Extend_None, (mp_o << 8) | mp_e, # mp_m, \
          OpdFmt_ ## mp_1, OpdFmt_ ## mp_2, OpdFmt_ ## mp_3 },

    #define DEFFORMAT_X(mp_opcode, mp_extend) \
        { 0, Extend_ ## mp_extend, mp_opcode, NULL },

    #define DEFFORMAT_P(mp_o, mp_m) \
        { -1, Extend_None, mp_o, # mp_m },

    #include "./X86OpCode.inc"
};

const char* const k_gpr8_names[8] = {
  "AL", "CL", "DL", "BL", "AH", "CH", "DH", "BH"
};

const char* const k_gpr16_names[8] = {
  "AX", "CX", "DX", "BX", "SP", "BP", "SI", "DI"
};

const char* const k_gpr32_names[8] = {
  "EAX", "ECX", "EDX", "EBX", "ESP", "EBP", "ESI", "EDI"
};

const char* const k_mmx_names[8] = {
  "MM0", "MM1", "MM2", "MM3", "MM4", "MM5", "MM6", "MM7"
};

const char* const k_xmm_names[8] = {
  "XMM0", "XMM1", "XMM2", "XMM3", "XMM4", "XMM5", "XMM6", "XMM7"
};

static int ldb(int const size, int const posn, int const val) {
  return (val >> posn) & ((1 << size) - 1);
}

class CodeBuffer {
  private: const Ee::CodeDesc& code_desc_;
  private: const uint8* cur_;
  private: const uint8* const end_;
  private: Ee::CodeDesc::EnumAnnotation enum_annot_;
  private: const uint8* const start_;

  public: CodeBuffer(const Ee::CodeDesc& code_desc)
    : code_desc_(code_desc),
      cur_(static_cast<const uint8*>(code_desc.codes())),
      end_(static_cast<const uint8*>(code_desc.codes()) 
              + code_desc.code_size()),
      enum_annot_(code_desc),
      start_(static_cast<const uint8*>(code_desc.codes())) {}

  public: uint8 operator[](uint const ofs) const {
    ASSERT(start_ + ofs < end_);
    return start_[ofs];
  }

  public: const Ee::CodeDesc& code_desc() const { return code_desc_; }

  public: uint current_offset() const {
    return static_cast<uint>(cur_ - start_);
  }

  public: const uint8* relative_address(int const offset) const {
    return cur_ + offset;
  }

  public: const uint8* start() const { return start_; }

  // [A]
  public: bool AtEnd() const { return cur_ == end_; }

  // [G]
  public: Ee::CodeDesc::Annotation GetAnnotation() {
    auto offset = int(current_offset());
    while (!enum_annot_.AtEnd()) {
      auto annot = enum_annot_.Get();
      if (annot.offset() == offset) {
        return annot;
      }

      if (annot.offset() > offset) {
        break;
      }

      enum_annot_.Next();
    }
    return Ee::CodeDesc::Annotation();;
  }

  // [R]
  public: int16 ReadInt16() {
    return static_cast<int16>(ReadUInt16());
  }

  public: int32 ReadInt32() {
    return static_cast<int32>(ReadUInt32());
  }

  public: int8 ReadInt8() {
    return static_cast<int8>(ReadUInt8());
  }

  public: uint16 ReadUInt16() {
    uint16 u16 = ReadUInt8();
    u16 |= ReadUInt8() << 8;
    return u16;
  }

  public: uint8 ReadUInt8() {
    return cur_ < end_ ? *cur_++ : 0;
  }

  public: uint32 ReadUInt32() {
    uint32 u32 = ReadUInt8();
    u32 |= ReadUInt8() << 8;
    u32 |= ReadUInt8() << 16;
    u32 |= ReadUInt8() << 24;
    return u32;
  }

  DISALLOW_COPY_AND_ASSIGN(CodeBuffer);
};

// Instruction object model.
class Instruction {
  private: int end_offset_;
  private: const Format* format_;
  private: int opcode_;
  private: Collection_<String> operands_;
  private: Collection_<String> prefixes_;
  private: int start_offset_;

  public: Instruction(
    int start_offset,
    int end_offset,
    const Collection_<String> prefixes,
    const Format& format,
    int opcode,
    const Collection_<String> operands)
    : end_offset_(end_offset),
      format_(&format),
      opcode_(opcode),
      operands_(operands),
      prefixes_(prefixes),
      start_offset_(start_offset) {}

  public: Instruction()
    : end_offset_(0),
      format_(&s_format_db),
      opcode_(-1),
      start_offset_(0) {}

  // properties
  public: int end_offset() const { return end_offset_; }
  public: String mnemonic() const { return format_->mnemonic_; }
  public: int start_offset() const { return start_offset_; }
  public: const Collection_<String> operands() const { return operands_; }
};

class Decoder {
  private: int addr_size_;
  private: CodeBuffer& code_buffer_;
  private: int modrm_;
  private: int operand_size_;
  private: ArrayList_<String> operands_;
  private: ArrayList_<String> prefixes_;
  private: int const start_offset_;

  public: Decoder(CodeBuffer& code_buffer)
      : addr_size_(32),
        code_buffer_(code_buffer),
        modrm_(0x100),
        operand_size_(32),
        start_offset_(code_buffer.current_offset()) {}

  // [D]
  public: Instruction Decode() {
    // TODO(yosi) 2012-03-25 Check code annotation for data part.
    auto const opcode = DecodeOpcode();
    if (opcode < 0) {
      return Instruction();
    }
    auto& format = DecodeFormat(opcode);
    DecodeOperands(format);

    return Instruction(
        start_offset_,
        code_buffer_.current_offset(),
        prefixes_,
        format,
        opcode,
        operands_);
  }

  private: String DecodeDisp32() {
    return String::Format("0x%X", code_buffer_.ReadInt32());
  }

  private: String DecodeEa(
      int const opdsiz,
      RegClass const reg_class = RegClass_Gpr) {

    auto const modrm = GetModRm();
    auto const mod = static_cast<Mod>(modrm & 0xC0);
    auto const rm = ldb(3, 0, modrm);
    switch (mod) {
      case Mod_Disp0:
        switch (rm) {
          case Rm_Sib: // [base+index]
            return DecodeOpdSiz(DecodeEaSib(mod), opdsiz);

          case Rm_Disp32: // [disp32]
            return DecodeOpdSiz(DecodeDisp32(), opdsiz);

          default:
            return DecodeOpdSiz(DecodeRb(rm), opdsiz);
        }

      case Mod_Disp8:
        if (rm == Rm_Sib) { // [base+index+disp8]
          auto sib = DecodeEaSib(mod);
          auto const disp8 = code_buffer_.ReadInt8();
          return DecodeOpdSiz(String::Format("%s%+d", sib, disp8), opdsiz);
        }

        {
          auto rb = DecodeRb(rm);
          auto const disp8 = code_buffer_.ReadInt8();
          return DecodeOpdSiz(String::Format("%s%+d", rb , disp8), opdsiz);
        }

      case Mod_Disp32:
       if (rm == Rm_Sib) { // [base+index+disp32]
         auto sib = DecodeEaSib(mod);
         auto disp32 = DecodeIv();
         return DecodeOpdSiz(String::Format("%s%+d", sib, disp32), opdsiz);
       }
       {
         auto rb = DecodeRb(rm);
         auto disp32 = DecodeIv();
         return DecodeOpdSiz(String::Format("%s+%d", rb, disp32), opdsiz);
       }

     case Mod_Reg:
       return DecodeReg(reg_class, opdsiz, rm);

     default:
       CAN_NOT_HAPPEN();
    }
  }

  private: String DecodeEaSib(Mod const mod) {
    auto const sib = code_buffer_.ReadUInt8();
    auto const scale = static_cast<Scale>(sib & 0xC0);
    auto const index = static_cast<Reg>(ldb(3, 3, sib));
    auto const base = static_cast<Reg>(ldb(3, 0, sib));
    auto rb = DecodeRb(base);
    if (index != Rm_Sib) {
      auto ri = DecodeEaSibIndex(scale, index);
      if (mod == Mod_Disp0 && base == Rm_Disp32) {
        return String::Format("%s %s", ri, DecodeDisp32());
      }
      return String::Format("%s+%s", rb, ri);

    } else if (base == Rm_Disp32) {
      return String();
    }
    return rb;
  }

  private: String DecodeEaSibIndex(
      Scale const scale,
      Reg const index) const {
    auto rx = DecodeReg(RegClass_Gpr, addr_size_, index);
    switch (scale) {
      case Scale_1:
        return rx;

      case Scale_2:
        return String::Format("%s*2", rx);

      case Scale_4:
        return String::Format("%s*4", rx);

      case Scale_8:
        return String::Format("%s*8", rx);

      case Scale_None:
          return String();

      default:
        CAN_NOT_HAPPEN();
    }
  }

  private: const Format& DecodeFormat(uint const opcode) {
    if (!s_format_map) {
      auto const N = ARRAYSIZE(k_formats);
      s_format_map = new HashMap_<int,const Format*>(N);
      for (auto p = k_formats; p < k_formats + N; p++) {
        s_format_map->Add(p->opcode_, p);
      }
    }

    auto const format = s_format_map->Get(opcode);
    if (format == nullptr) {
      return s_format_db;
    }

    switch (format->extend_) {
      case Extend_ModRm:
        return DecodeFormat((opcode << 8) | ldb(3, 3, GetModRm()));

      case Extend_None:
        return *format;

      case Extend_TwoByte:
        return DecodeFormat((opcode << 8) | code_buffer_.ReadUInt8());

      default:
        CAN_NOT_HAPPEN();
    }
  }

  private: String DecodeGx(const char* const* ppsz) {
    return DecodeReg(ppsz, ldb(3, 3, GetModRm()));
  }

  private: String DecodeIv() {
    auto const annot = code_buffer_.GetAnnotation();
    auto const i32 = code_buffer_.ReadInt32();
    switch (annot.kind()) {
      case Ee::CodeDesc::Annotation::Kind_AbsLabel:
      case Ee::CodeDesc::Annotation::Kind_Callee:
      case Ee::CodeDesc::Annotation::Kind_ClosedLit:
      case Ee::CodeDesc::Annotation::Kind_ClosedVar:
      case Ee::CodeDesc::Annotation::Kind_DllLink:
      case Ee::CodeDesc::Annotation::Kind_Float32:
      case Ee::CodeDesc::Annotation::Kind_Float64:
        break;

      case Ee::CodeDesc::Annotation::Kind_FunRef: {
        auto const target = reinterpret_cast<void*>(i32);
        if (auto const code_desc = Ee::CodeManager::Get()->Find(target)) {
          auto& fun = code_desc->function();
          if (auto const method = fun.GetMethod()) {
            return fun.flavor() == Function::Flavor_Toplevel
              ? method->ToString()
              : String::Format("%s.%s", *method, fun);
          }
          return fun.ToString();
        }
        break;
      }

      case Ee::CodeDesc::Annotation::Kind_MethodRef: {
        auto const obj = reinterpret_cast<Object*>(i32);
        return obj->ToString();
      }

      case Ee::CodeDesc::Annotation::Kind_Int16:
      case Ee::CodeDesc::Annotation::Kind_Int32:
      case Ee::CodeDesc::Annotation::Kind_Int64:
      case Ee::CodeDesc::Annotation::Kind_Int8:
      case Ee::CodeDesc::Annotation::Kind_Method:
        break;

      case Ee::CodeDesc::Annotation::Kind_Object: {
        auto& obj = *reinterpret_cast<Om::Object*>(i32);
        if (auto const str = obj.DynamicCast<Om::String>()) {
          StringBuilder builder;
          builder.Append('"');
          foreach (Om::String::EnumChar, chars, *str) {
            auto const ch = chars.Get();
            switch (ch) {
              case '"':
              case '\\':
                builder.Append("\\%c", ch);
                break;
  
              default:
                if (ch >= ' ' && ch <= 0x7F) {
                  builder.Append(ch);
                } else {
                  builder.Append("\\u%04X", ch);
                }
                break;
            }
          }
          builder.Append('"');
          return builder.ToString();
        }
        return String::Format("Object@%08X", i32);
      }

      case Ee::CodeDesc::Annotation::Kind_RelLabel:
      case Ee::CodeDesc::Annotation::Kind_UInt16:
      case Ee::CodeDesc::Annotation::Kind_UInt32:
      case Ee::CodeDesc::Annotation::Kind_UInt64:
      case Ee::CodeDesc::Annotation::Kind_UInt8:
      case Ee::CodeDesc::Annotation::Kind_NativeCallee:
        break;
    }
    return String::Format("%d", i32);
  }

  private: String DecodeLabel(int offset) {
    auto const target = code_buffer_.relative_address(offset);
    if (auto const code_desc = Ee::CodeManager::Get()->Find(target)) {
      if (code_desc == &code_buffer_.code_desc()) {
        return String::Format("L%04X", target - code_buffer_.start());
      }

      if (auto const mt = code_desc->function().GetMethod()) {
        return mt->ToString();
      }
      return code_desc->function().ToString();
    }
    return String::Format("L%08X", reinterpret_cast<uintptr_t>(target));
  }

  private: String DecodeOpdSiz(
      const String& operand,
      int const opdsiz) const {
    switch (opdsiz) {
      case 8:
        return String::Format("BYTE PTR [%s]",  operand);

      case 16:
        return String::Format("WORD PTR [%s]",  operand);

      case 32:
        return String::Format("[%s]", operand);

      case 64:
        return String::Format("QWORD PTR [%s]",  operand);

      case 80:
        return String::Format("TBYTE PTR [%s]",  operand);

      case 128:
        return String::Format("DQWORD PTR [%s]",  operand);

      default:
        CAN_NOT_HAPPEN();
    }
  }

  private: int DecodeOpcode() {
    auto has_repne = false;
    auto has_rep = false;

    enum State {
      State_0F,
      State_Normal,
    } state = State_Normal;

    while (!code_buffer_.AtEnd()) {
      auto const u8 = code_buffer_.ReadUInt8();
      switch (state) {
        case State_0F:
          if (operand_size_ == 16) {
            auto const opcode = u8 | 0x0F00;
            if (DecodeFormat(opcode | 0x660000).opcode_ >= 0) {
              operand_size_ = 32;
              return opcode | 0x660000;
            }
            return opcode;
          }

          if (has_repne) {
            RemovePrefix("REPNE");
            return u8 | 0xF20F00;
          }

          if (has_rep) {
            RemovePrefix("REP");
            return u8 | 0xF30F00;
          }

          return u8 | 0x0F00;

        case State_Normal:
          switch (u8) {
            case 0x0F:
              state = State_0F;

            case 0x66:
              operand_size_ = 16;
              break;

            case 0x67:
              addr_size_ = 16;
              break;

            case 0xF2:
              has_repne = true;
              prefixes_.Add("REPNE");
              break;

            case 0xF3:
              has_rep = true;
              prefixes_.Add("REP");
              break;

            default: {
              auto& format = DecodeFormat(u8);
              if (format.IsPrefix()) {
                prefixes_.Add(format.mnemonic_);
                break;
              }
              return u8;
            }
          }
          break;

        default:
          CAN_NOT_HAPPEN();
      }
    }
    return -1;
  }

  private: void DecodeOperands(const Format& format) {
    for (auto const operand: format) {
      operands_.Add(DecodeOperand(operand));
    }
  }

  private: String DecodeOperand(OpdFmt const opdfmt) {
    switch (opdfmt) {
      case OpdFmt_1: // SHL Ev:, 1
        return "1";

      case OpdFmt_AL:     // 0
      case OpdFmt_CL:     // 1
      case OpdFmt_DL:     // 2
      case OpdFmt_BL:     // 3
      case OpdFmt_AH:     // 4
      case OpdFmt_CH:     // 5
      case OpdFmt_DH:     // 6
      case OpdFmt_BH:     // 7
          return DecodeReg(k_gpr8_names, opdfmt - OpdFmt_AL);

      case OpdFmt_DX: // For IN: OUT
          return DecodeReg(k_gpr16_names, 2);

      case OpdFmt_eAX:    // 0
      case OpdFmt_eCX:    // 1
      case OpdFmt_eDX:    // 2
      case OpdFmt_eBX:    // 3
      case OpdFmt_eSP:    // 4
      case OpdFmt_eBP:    // 5
      case OpdFmt_eSI:    // 6
      case OpdFmt_eDI:    // 7
          if (operand_size_ == 16) {
              return DecodeReg(k_gpr16_names, opdfmt - OpdFmt_eAX);
          }
          return DecodeReg(k_gpr32_names, opdfmt - OpdFmt_eAX);

      case OpdFmt_Eb:
        return DecodeEa(8);

      case OpdFmt_Ed:
      case OpdFmt_Eq:
        return DecodeEa(32);

      case OpdFmt_Ew:
        return DecodeEa(16);

      case OpdFmt_Ev:
        return DecodeEa(operand_size_);

      case OpdFmt_EvF64:  // CALL/JMP Ev
      case OpdFmt_EvD64:  // PUSH Ev
          return DecodeEa(32);

      case OpdFmt_Gb:
        return DecodeGx(k_gpr8_names);

      case OpdFmt_Gd:
        return DecodeGx(k_gpr32_names);

      case OpdFmt_Gq:
      case OpdFmt_Gv:
        return DecodeReg(
            RegClass_Gpr, 
            operand_size_, 
            ldb(3, 3, GetModRm()));

      case OpdFmt_Ib: // imm8
      case OpdFmt_Is: // imm8 w/ sign extended
        return String::Format("%d", code_buffer_.ReadInt8());

      case OpdFmt_Iw: // imm16
        return String::Format("%d", code_buffer_.ReadInt16());

      case OpdFmt_Iq:
      case OpdFmt_Iv: // imm16, imm32, or imm64
        return operand_size_ == 16
            ? String::Format("%d", code_buffer_.ReadInt16())
            : DecodeIv();

      case OpdFmt_Iz: // imm32
        return DecodeIv();

      case OpdFmt_Jb:
        return DecodeLabel(code_buffer_.ReadInt8());

      case OpdFmt_Jv:
        return DecodeLabel(code_buffer_.ReadInt32());

      case OpdFmt_M:
        return DecodeEa(operand_size_);

      case OpdFmt_Md:
        return DecodeEa(64, RegClass_Mmx);

      case OpdFmt_Mp:
        return DecodeEa(64, RegClass_Mmx);

      case OpdFmt_Mpd:
        return DecodeEa(128, RegClass_Xmm);

      case OpdFmt_Mps:
        return DecodeEa(128, RegClass_Xmm);

      case OpdFmt_Mq:
      case OpdFmt_Nq:
      case OpdFmt_Pd:
      case OpdFmt_Pq:
        return DecodeReg(RegClass_Mmx, 64, ldb(3, 0, GetModRm()));

      case OpdFmt_Ob:
        return DecodeOpdSiz(DecodeDisp32(), 8);

      case OpdFmt_Ov:
        return DecodeOpdSiz(DecodeDisp32(),32);

      case OpdFmt_Qq:
      case OpdFmt_Qd:
      case OpdFmt_Qdq:
        return DecodeEa(64, RegClass_Mmx);

      case OpdFmt_rAX:    // 0    PUSH/POP
      case OpdFmt_rCX:    // 1    PUSH/POP
      case OpdFmt_rDX:    // 2    PUSH/POP
      case OpdFmt_rBX:    // 3    PUSH/POP
      case OpdFmt_rSP:    // 4    PUSH/POP
      case OpdFmt_rBP:    // 5    PUSH/POP
      case OpdFmt_rSI:    // 6    PUSH/POP
      case OpdFmt_rDI:    // 7    PUSH/POP
          return DecodeReg(k_gpr32_names, opdfmt - OpdFmt_rAX);

      case OpdFmt_Udq:
      case OpdFmt_Upd:
      case OpdFmt_Ups:
        return DecodeReg(RegClass_Xmm, 128, ldb(3, 0, GetModRm()));

      case OpdFmt_Vdq:
      case OpdFmt_Vpd:
      case OpdFmt_Vpq:
      case OpdFmt_Vps:
      case OpdFmt_Vq:
      case OpdFmt_Vsd:
      case OpdFmt_Vss:
        return DecodeReg(RegClass_Xmm, 128, ldb(3, 3, GetModRm()));

      case OpdFmt_Wdq:    // xmm or Mdq
      case OpdFmt_Wpd:    // xmm or Mdq
      case OpdFmt_Wps:    // xmm or Mdq
      case OpdFmt_Wq:     // xmm or Mdq
        return DecodeEa(128, RegClass_Xmm);

      case OpdFmt_Wsd:    // xmm or Mdq
        return DecodeEa(64, RegClass_Xmm);

      case OpdFmt_Wss:    // xmm or Mdq
        return DecodeEa(32, RegClass_Xmm);

      case OpdFmt_Sw: // reg of modrm selects segment register
        return String();

      case OpdFmt_ES:
      case OpdFmt_CS:
      case OpdFmt_DS:
      case OpdFmt_SS:
      case OpdFmt_None:
      default:
        CAN_NOT_HAPPEN();
    }
  }

  private: String DecodeRb(int const rb) const {
    return DecodeReg(RegClass_Gpr, addr_size_, rb);
  }

  private: String DecodeReg(const char* const* ppsz, uint n) const {
    return ppsz[n];
  }

  private: String DecodeReg(RegClass reg_class, uint size, uint n) const {
    switch (reg_class) {
      case RegClass_Gpr:
        switch (size) {
          case 8:
            return DecodeReg(k_gpr8_names, n);
          case 16:
            return DecodeReg(k_gpr16_names, n);
          case 32:
            return DecodeReg(k_gpr32_names, n);
          default:
            CAN_NOT_HAPPEN();
        }

      case RegClass_Mmx:
        return DecodeReg(k_mmx_names, n);

      case RegClass_Xmm:
        return DecodeReg(k_xmm_names, n);

      default:
        CAN_NOT_HAPPEN();
     }
  }

  // [G]
  private: uint GetModRm() {
    if (modrm_ == 0x100) {
      modrm_ = code_buffer_.ReadUInt8();
    }
    return modrm_;
  }

  // [R]
  private: void RemovePrefix(const String& pattern) {
    ArrayList_<String> copies;
    for (auto prefix: prefixes_) {
      if (prefix != pattern)
        copies.Add(prefix);
    }

    prefixes_.Clear();
    for (auto prefix: copies)
      prefixes_.Add(prefix);
  }

  DISALLOW_COPY_AND_ASSIGN(Decoder);
};

class X86Disasm {
  private: CodeBuffer code_buffer_;
  private: TextWriter& writer_;

  public: X86Disasm(TextWriter& writer, const Ee::CodeDesc& code_desc)
    : code_buffer_(code_desc),
      writer_(writer) {}

  private: void WriteInstruction(const Instruction& inst) {
    writer_.Write(" %04X", inst.start_offset());

    // Write code bytes
    auto const K = 6;
    for (
        auto ofs = inst.start_offset();
        ofs < inst.start_offset() + K;
        ofs++) {
      if (ofs < inst.end_offset()) {
        writer_.Write(" %02X", code_buffer_[ofs]);
      } else {
        writer_.Write("   ");
      }
    }

    // Mnemonic and operands
    writer_.Write(" %s", inst.mnemonic());

    const char* comma = " ";
    for (const auto& operand: inst.operands()) {
      writer_.Write("%s%s", comma, operand);
      comma = ", ";
    }

    // Write code bytes after start_ofs + K
    auto break_ofs = inst.start_offset() + K;
    for (
        auto ofs = break_ofs;
        ofs < inst.end_offset();
        ofs++) {
      if (ofs == break_ofs) {
        writer_.WriteLine();
        writer_.Write("    ");
      }
      writer_.Write(" %02X", code_buffer_[ofs]);
    }
    writer_.WriteLine();
  }

  public: void Run() {
    while (!code_buffer_.AtEnd()) {
      Decoder decoder(code_buffer_);
      auto inst = decoder.Decode();
      WriteInstruction(inst);
    }
  }

  DISALLOW_COPY_AND_ASSIGN(X86Disasm);
};

} // namespace

void LibExport X86Disassemble(
    TextWriter& writer,
    const Ee::CodeDesc& code_desc) {
  X86Disasm disasm(writer, code_desc);
  disasm.Run();
}

} // X86
} // Il
