#include "precomp.h"
// @(#)$Id$
//
// Evita Il - Targets - X86 Asm
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./X86DisasmTask.h"

namespace Il {
namespace Cg {

using Common::Io::TextWriter;

namespace {

class X86Disasm {
    private: class CodeBuffer {
      private: const uint8* const end_;
      private: const uint8* cur_;
      private: const uint8* const start_;

      public: CodeBuffer(
        const uint8* const start,
        const uint8* end)
        : cur_(start),
          end_(end),
          start_(start) {
        ASSERT(start_ <= end_);
      }

      public: uint8 operator[](uint const ofs) const {
        ASSERT(start_ + ofs < end_);
        return start_[ofs];
      }

      public: uint current_offset() const {
        return static_cast<uint>(cur_ - start_);
      }

      // [A]
      public: bool AtEnd() const { return cur_ < end_; }

      DISALLOW_COPY_AND_ASSIGN(CodeBuffer);
  };

  private: class DisasmInstruction {
    private: int const start_offset
    public: DisasmInstruction(CodeBuffer& code_buffer)
        : 
  };

  private: CodeBuffer code_buffer_;
  private: TextWriter& writer_;

  public: Disasm(
      TextWriter& writer_,
      const uint8* const start,
      const uint8* end)
      : cur_(start),
        end_(end),
        start_(start),
        writer_(writer) {
    ASSERT(start <= end);
  }

  private: void DisasmInst() {
    DisasmInstruction inst(code_buffer_);

    auto const start_ofs = GetOffset();
    auto const opcode = DecodeOpcode();
    auto& format = GetFormat(opcode);
    DecodeOperands(format);
    auto const end_ofs = GetOffset();

    writer_.Write(" %04X", start_ofs);

    // Write code bytes
    auto const K = 6;
    for (auto ofs = start_ofs; ofs < start_ofs + k; ofs++) {
      if (ofs < end_ofs) {
        writer_.Write(" %02X", GetUInt8(ofs));
      } else {
        writer_.Write("   ");
    }

    // Mnemonic and operands
    writer_.Write(" %s", format.mnemonic());
    const char* comma = " ";
    foreach (Context::EnumOperand, operands, context) {
      auto& operand = *operands;
      writer_.Write(comma);
      comma = ", ";
      writer_.Write(operand.ToString());
    }

    // Write code bytes after start_ofs + K
    auto break_ofs = start_ofs + K;
    for (auto ofs = start_ofs + K; ofs < end_ofs; ofset++) {
      if (ofs = break_ofs) {
        writer_.WriteLine();
        writer_.Write("    ");
      }
      writer_.Write(" %02X", GetUInt8(ofs));
    }
    writer_.WriteLine();
  }

  public: void Run() {
    while (!code_buffer_.AtEnd()) {
      DisasmInstruction inst(code_buffer_);
      inst.WriteTo(writer_);
    }
  }

  DISALLOW_COPY_AND_ASSIGN(X86Disasm);
};

} // namespace


} // Cg
} // Il
