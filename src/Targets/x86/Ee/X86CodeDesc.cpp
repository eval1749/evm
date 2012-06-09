#include "precomp.h"
// @(#)$Id$
//
// Evita Ee X86 - CodeManager
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./X86CodeDesc.h"

#include "../../../Om.h"
#include "../../../Om/Frame.h"
#include "../../../Om/Thread.h"

namespace Ee {

namespace {

class CodeDescEditor {
  private: const CodeDesc::Annotation annotation_;
  private: const CodeDesc& code_desc_;

  public: CodeDescEditor(
      const CodeDesc& code_desc,
      const CodeDesc::Annotation annotation)
      : annotation_(annotation),
        code_desc_(code_desc) {}

  // properties
  private: uint8* code_pointer(int const offset) const {
    return reinterpret_cast<uint8*>(code_desc_.codes()) + offset;
  }

  public: CodeDesc::Annotation::Kind kind() const {
    return annotation_.kind();
  }

  public: int offset() const {
    return annotation_.offset();
  }

  // [G]
  public: Function& GetFunction() const {
    return *reinterpret_cast<Function*>(GetInt32());
  }

  public: int32 GetInt32() const {
    return GetInt32(annotation_.offset());
  }

  private: int32 GetInt32(int const offset) const {
    auto const p = code_pointer(offset);
    return p[0] | (p[1] << 8) | (p[2] << 16) | (p[3] << 24);
  }

  public: Method& GetMethod() const {
    return *reinterpret_cast<Method*>(GetInt32());
  }

  // [S]
  public: void SetInt32(int32 const value) {
    SetInt32(annotation_.offset(), value);
  }

  private: void SetInt32(int const offset, int32 const value) {
    auto const p = code_pointer(offset);
    p[0] = static_cast<uint8>(value & 0xFF);
    p[1] = static_cast<uint8>((value >> 8) & 0xFF);
    p[2] = static_cast<uint8>((value >> 16) & 0xFF);
    p[3] = static_cast<uint8>((value >> 24) & 0xFF);
  }

  public: void SetPointer(void* const pv) {
    SetPointer(offset(), pv);
  }

  private: void SetPointer(int const offset, void* const pv) {
    SetInt32(offset, reinterpret_cast<int32>(pv));
  }

  public: void SetRelative(int const target) {
    auto const p = code_pointer(annotation_.offset());
    SetInt32(target - (reinterpret_cast<int>(p) + 4));
  }

  DISALLOW_COPY_AND_ASSIGN(CodeDescEditor);
};

typedef Om::Thread::Context Context;

enum FrameType {
  FrameType_FromNative = Om::FrameType_FromNative,
};

// ecx = context
static void __declspec(naked) __fastcall 
CallManagedFunction(Om::Thread::Context*) {
  __asm {
    // Allocate save area
    sub     esp, 24     // 24 = caller save(16) + from_foreign(8)

    // Save caller save registers for C
    mov     [esp+8],  ebx
    mov     [esp+12], ebp
    mov     [esp+16], esi
    mov     [esp+20], edi

    // ebp <- pContext
    mov     ebp, ecx

    // Make FromForeign Frame
    mov     eax, [ebp] Context.frame_
    mov     [ebp] Context.frame_, esp
    mov     [esp+0], eax
    mov     dword ptr [esp+4], FrameType_FromNative

    // Set callee entry point
    mov     eax, [ebp] Context.param_
    mov     [esp-4], eax

    // Set arguments
    mov     eax, [ebp] Context.values_[0*4]
    mov     edx, [ebp] Context.values_[1*4]
    mov     ebx, [ebp] Context.values_[2*4]
    mov     esi, [ebp] Context.values_[3*4]
    mov     edi, [ebp] Context.values_[4*4]

    // Call lisp function
    call    dword ptr [esp-4]

    // Set values
    mov     [ebp] Context.values_[0*4], eax
    mov     [ebp] Context.values_[1*4], edx
    mov     [ebp] Context.values_[2*4], ebx
    mov     [ebp] Context.values_[3*4], esi
    mov     [ebp] Context.values_[4*4], edi

    // Restore m_fp
    mov     eax, [esp+0]
    mov     [ebp] Context.frame_, eax

    // Restore primary value
    mov     eax, [ebp] Context.values_[0*4]

    // Restore caller save registers
    mov     ebx, [esp+8]
    mov     ebp, [esp+12]
    mov     esi, [esp+16]
    mov     edi, [esp+20]

    add     esp, 24
    ret
  }
}

} // namespace

// ctor
X86CodeDesc::X86CodeDesc(
    const Function& fun,
    const Collection_<Annotation>& annots,
    void* codes,
    int32 code_size)
    : CodeDesc(fun, annots, codes, code_size) {}

// [F]
void CodeDesc::FixAfterMove() {
  foreach (EnumAnnotation, annots, *this) {
    CodeDescEditor editor(*this, annots.Get());
    switch (editor.kind()) {
      case Annotation::Kind_AbsLabel:
        break;

      case Annotation::Kind_Callee: {
        auto& fun = editor.GetFunction();
        auto const target = reinterpret_cast<int32>(
            fun.code_desc()->codes());
        editor.SetRelative(target);
        break;
      }

      case Annotation::Kind_ClosedLit:
      case Annotation::Kind_ClosedVar:
      case Annotation::Kind_DllLink:
      case Annotation::Kind_Float32:
      case Annotation::Kind_Float64:
      case Annotation::Kind_Int16:
      case Annotation::Kind_Int32:
      case Annotation::Kind_Int64:
      case Annotation::Kind_Int8:
        break;

      case Annotation::Kind_FunRef:
        break;

      case Annotation::Kind_Method: {
        auto& method = editor.GetMethod();
        // TODO(yosi) 2012-03-17 Handle when method doesn't have a function.
        auto& fun = *method.GetFunction();
        auto const target = reinterpret_cast<int32>(
            fun.code_desc()->codes());
        editor.SetRelative(target);
        break;
      }

      case Annotation::Kind_MethodRef:
        break;

      case Annotation::Kind_NativeCallee: {
        auto const nativefn = editor.GetInt32();
        editor.SetRelative(nativefn);
        break;
      }

      case Annotation::Kind_Object:
        break;

      case Annotation::Kind_RelLabel:
      case Annotation::Kind_UInt16:
      case Annotation::Kind_UInt32:
      case Annotation::Kind_UInt64:
      case Annotation::Kind_UInt8:
      default:
        Debugger::Fail("Unknown CodeDesc::Annotation::Kind %d", 
            editor.kind());
   }
  }
}

// [I]
void CodeDesc::Invoke(Om::Thread& thread) {
  auto const context = thread.context();
  context->param_ = codes_;
  CallManagedFunction(context);
}

// [S]
void CodeDesc::SetPointer(const Annotation& annot, void* const pv) {
  CodeDescEditor editor(*this, annot);
  editor.SetPointer(pv);
}

} // Ee
