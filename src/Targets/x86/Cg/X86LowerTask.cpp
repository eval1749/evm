#include "precomp.h"
// @(#)$Id$
//
// Evita Il - Targets - X86 Asm
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./X86LowerTask.h"

#include "./X86Defs.h"
#include "./X86OptimizeTasklet.h"

#include "../../../Il/Tasks/CodeEmitter.h"
#include "../../../Il/Tasks/Tasklet.h"
#include "../../../Om.h"
#include "../../../Om/Thread.h"
#include "../../../Om/Frame.h"

namespace Il {
namespace Cg {

using namespace Il::Tasks;

namespace {

using namespace X86;

struct ObjArray {
  void* type_;
  int32 length_;
};

struct ObjClass {
  enum { ClassAlign = sizeof(void*) * 2 };
  void* type_;
};

class Static {
  public: static int ComputeAllocSize(const Type& type) {
    if (auto const pClass = type.DynamicCast<Class>()) {
      auto iSize = sizeof(ObjClass);
      foreach (Class::EnumField, oEnum, pClass) {
        auto const pField = oEnum.Get();

        auto const iFieldSize =
            Static::ComputeAllocSize(pField->storage_type());

        iSize = RoundUp(iSize, iFieldSize);
        iSize += iFieldSize;
      }
      return RoundUp(iSize, ObjClass::ClassAlign);
    }
    return sizeof(void*);
  }

  public: static int ComputeAllocSizeInStack(const Type& type) {
    if (auto const bit_size = type.bit_width()) {
      return bit_size / 8;
    }

    if (type.IsSubtypeOf(*Ty_Value) == Subtype_Yes) {
      return ComputeAllocSize(type);
    }

    return sizeof(void*);
  }

  public: static bool IsArrayClass(const Class& clazz) {
    if (auto const cclass = clazz.DynamicCast<ConstructedClass>()) {
      return cclass->generic_class() == *Ty_Array;
    }
    return false;
  }

  public: static Method* IsLengthMethod(const Operand& callee) {
    if (auto const method = callee.DynamicCast<Method>()) {
      return &method->name() == Q_get_Length ? method : nullptr;
    }
    return false;
  }
};

class X86CodeEmitter : public CodeEmitter {
  private: CgSession& session_;
  // ctor
  public: X86CodeEmitter(
      CgSession& session,
      const Instruction& ref_inst)
      : CodeEmitter(session, ref_inst), session_(session) {}

  // [A]
  // Emit activate frame codes for most outer frame:
  //   thread.frame_ = &frame.outer_
  //    LEA void** r1 <= %frame offsetof(Frame, outer_)
  //    LEA void** q3 <= %thread offsetof(Thread.Context, frame_)
  //    STORE %q3 r1
  public: void ActivateFrame(const FrameReg& frame_reg) {
    if (ref_inst().output().Is<FrameReg>()) {
      DEBUG_FORMAT("Don't activate frame for interleaved %s", ref_inst());
      return;
    }
    auto& r1 = NewRegister();
    Lea(*Ty_Void, r1, frame_reg, offsetof(Om::Frame, outer_));
    auto& r2 = session_.target().thread_reg();
    auto const offset = offsetof(Om::Thread::Context, frame_);
    auto& q3 = AddressRef(*Ty_Void, r2, offset);
    Store(q3, r1);
  }

  public: const PseudoOutput& AddressRef(
      const Type& elemty,
      const Output& base_reg,
      int const offset) {
    auto& ptrty = PointerType::Intern(elemty);
    auto& out = NewPseudoRegister();
    Emit(*new(zone()) x86LeaI(ptrty, out, base_reg, offset));
    return out;
  }

  // [C]
  private: FrameReg* GetOuterFrame(const FrameReg& frame_reg) {
    auto const curr_fun = bblock().GetFunction();
    for (
        auto runner = frame_reg.GetOuter();
        runner;
        runner = runner->GetOuter()) {
      if (auto const def_inst = runner->GetDefI()) {
        return def_inst->GetBBlock()->GetFunction() == curr_fun
            ? runner
            : nullptr;
      }
    }
    return nullptr;
  }

  // [F]
  public: void FrameLoad(
      const Type& outy,
      const SsaOutput& out,
      const Output& frame_reg,
      int const offset) {
    auto& ptr = AddressRef(outy, frame_reg, offset);
    Emit(*new(zone()) LoadI(outy, out, ptr));
  }

  public: void FrameStore(
      const Output& base_reg,
      int const offset,
      const Operand& src) {
    auto& ptr = AddressRef(src.type(), base_reg, offset);
    Store(ptr, src);
  }

  // [L]
  public: void Lea(
      const Type& elemty,
      const SsaOutput& out,
      const Output& base_reg,
      int const offset) {
    auto& ptrty = PointerType::Intern(elemty);
    Emit(*new(zone()) x86LeaI(ptrty, out, base_reg, offset));
  }

  // Emit open frame codes. There are two patterns, most outer frame and
  // inner frame.
  //
  // Most outer frame:
  //  frame.outer_ = thread.frame_
  //    LEA void** %r1 <= $thread offsetof(Context.frame_)
  //    LOAD void* %r2 %r1
  //    LEA void** %q3 <= %frame offsetof(Frame.outer_)
  //    STORE %q3 %r1
  //  frame.type_ = frame_type
  //    LEA int* %q4 <= %frame offsetof(Frame.type_)
  //    STORE %q4 frame_type
  //
  // Inner frame:
  //   frame.outer_ = &outer.outer_
  //    LEA void** %r1 <= %frame offsetof(Frame.outer_)
  public: void OpenFrame(
      const FrameReg& frame_reg,
      Om::FrameType const frame_type) {
    auto& r1 = NewRegister();
    if (auto const outer_frame_reg = GetOuterFrame(frame_reg)) {
      Lea(*Ty_VoidPtr, r1, *outer_frame_reg, offsetof(Om::Frame, outer_));
    } else {
      auto& r2 = session_.target().thread_reg();
      FrameLoad(*Ty_VoidPtr, r1, r2, offsetof(Om::Thread::Context, frame_));
    }

    FrameStore(frame_reg, offsetof(Om::Frame, outer_), r1);

    FrameStore(
        frame_reg,
        offsetof(Om::Frame, type_),
        NewLiteral(*Ty_Int32, frame_type));
  }

  // [R]
  const Register& ReinterpretCast(
      const Type& outy,
      const Operand& src) {
    auto& out = NewRegister();
    Emit(*new(zone()) ReinterpretCastI(outy, out, src));
    return out;
  }

  DISALLOW_COPY_AND_ASSIGN(X86CodeEmitter);
};

class LowerTasklet :
  public Functor,
  public Tasklet {

  private: const Module& module_;
  private: X86OptimizeTasklet normalizer_;
  private: CgSession& session_;

  // ctor
  public: LowerTasklet(CgSession& session, const Module& module) 
      : Tasklet("LowerTasklet", session),
        module_(module),
        normalizer_(session, module),
        session_(session) {}

  public: virtual ~LowerTasklet() {}

  // [P][B]
  //    BOX Box<ty> %r0 <= %s1
  // =>
  //    VALUES (ty) %v2 <= %s1
  //    CALL Box<ty> %r0 <= Box<ty>::.ctor %v2
  public: virtual void Process(BoxI* const pI) override {
    auto& box_inst = *pI;
    DEBUG_FORMAT("process %s", box_inst);
    auto& box_class = *box_inst.output_type().StaticCast<Class>();
    auto& src = box_inst.op0();
    auto& opty = src.type();
    auto& ctor = *box_class
        .Find(*QD_ctor)->StaticCast<MethodGroup>()
        ->Find(FunctionType::Intern(box_class, ValuesType::Intern(opty)));
    X86CodeEmitter emitter(session_, box_inst);
    auto& r2 = emitter.Call(box_class, ctor, src);
    ReplaceAll(r2, *box_inst.output().StaticCast<Register>());
    normalizer_.Add(box_inst);
  }

  // [P][C]
  public: virtual void Process(CallI* const pI) override {
    auto& call_inst = *pI;
    auto& callee = call_inst.op0();
    if (auto const method = Static::IsLengthMethod(callee)) {
      if (Static::IsArrayClass(method->owner_class())) {
        ReplaceToLoadField(call_inst, offsetof(ObjArray, length_));
      }
    }
  }

  // We only emit restoring $tcb.frame_ for most outer CLOSE instruction.
  public: virtual void Process(CloseI* const pI) override {
    auto& close_inst = *pI;
    if (close_inst.GetNext()->Is<CloseI>()) {
      DEBUG_FORMAT("Skip interleaved %s", close_inst);
      return;
    }

    X86CodeEmitter emitter(session_, close_inst);
    auto& r1 = module_.NewRegister();
    auto& frame_reg = *close_inst.op0().StaticCast<FrameReg>();
    auto const offset = offsetof(Om::Frame, outer_);
    emitter.FrameLoad(*Ty_VoidPtr, r1, frame_reg, offset);
    auto& r2 = session_.target().thread_reg();
    emitter.FrameStore(r2, offsetof(Om::Thread::Context, frame_), r1);
    normalizer_.Add(close_inst);
  }

  // [P][E]
  //    ELTREF ty* %r0 <= %r1 %s2
  // =>
  //    MUL int32 %r3 <= %s2 sizeof(ty)
  //    REINTERPRETCAST int32 %r4 <= %r3 %r1
  //    ADD int32 %r5 <= %r3 %r4
  //    X86LEA ty* %q6 <= %r5 sizeof(ArrayHeader)
  public: virtual void Process(EltRefI* const pI) override {
    auto& eltref_inst = *pI;

    X86CodeEmitter emitter(session_, eltref_inst);
    auto& elemty = eltref_inst.output_type().StaticCast<PointerType>()
        ->pointee_type();
    auto const size = Static::ComputeAllocSizeInStack(elemty);
    auto& elem_size = module_.NewLiteral(*Ty_Int32, size);
    auto& r1 = emitter.Mul(*Ty_Int32, eltref_inst.op1(), elem_size);
    auto& r2 = emitter.ReinterpretCast(*Ty_Int32, eltref_inst.op0());
    auto& r3 = emitter.Add(*Ty_Int32, r1, r2);
    auto& q4 = emitter.AddressRef(elemty, r3,  sizeof(ObjArray));
    ReplaceAll(q4, *eltref_inst.GetRd());
    normalizer_.Add(*r1.GetDefI());
    normalizer_.Add(eltref_inst);
  }

  //    OPENFINALLY %x0 <= fun %v1
  // =>
  //    X86LEA %q2 <= %x0 offsetof(FinallyFrame, finfun_)
  //    STORE %r1 FunLit(fun)
  //    X86LEA %q3 <= %x0 offsetof(FinallyFrame, num_params_)
  //    STORE %q3 num_params
  //    ... store arguments for finally function
  //    ... open frame
  //    ... activate frame
  public: void Process(OpenFinallyI* pI) {
    auto& open_inst = *pI;
    auto& ref_inst = *open_inst.GetNext();
    auto& frame_reg = *open_inst.output().StaticCast<FrameReg>();

    X86CodeEmitter emitter(session_, ref_inst);

    emitter.FrameStore(
        frame_reg,
        offsetof(Om::FinallyFrame, finfun_),
        *new(module_.zone()) FunLit(
            *open_inst.op0().StaticCast<Function>()));

    auto& args_inst = *open_inst.op1().StaticCast<Values>()->GetDefI();
    auto& num_params_lit = module_.NewLiteral(
        *Ty_Int32,
        args_inst.CountOperands());
    emitter.FrameStore(
        frame_reg,
        offsetof(Om::FinallyFrame, num_params_),
        num_params_lit);

    auto offset = sizeof(Om::FinallyFrame);
    foreach (Instruction::EnumOperand, operands, args_inst) {
      auto& operand = *operands.Get();
      auto const size = Static::ComputeAllocSizeInStack(operand.type());
      offset += offset % size;
      emitter.FrameStore(frame_reg, offset, operand);
      offset += size;
    }

    emitter.OpenFrame(frame_reg, Om::FrameType_Finally);
    emitter.ActivateFrame(frame_reg);
  }

  // [R]
  // VALUES %v1 <= %r0
  // CALL int32 %r2 <= Array<T>.get_Length %v1
  // =>
  // x86Lea %q1 <= %r0 offst
  // Load int32 %r2 <= %q1
  private: void ReplaceToLoadField(CallI& call_inst, int const offset) {
    DEBUG_FORMAT("Replace %s to LOAD offset %d", call_inst, offset);
    X86CodeEmitter emitter(session_, call_inst);
    auto& object = *call_inst.op1().StaticCast<Values>()
        ->GetDefI()->op0().StaticCast<Register>();
    auto& q1 = emitter.AddressRef(*Ty_Int32, object, offset);
    auto& r2 = emitter.Load(q1);
    ReplaceAll(r2, *call_inst.GetRd());
    normalizer_.Add(call_inst);
    call_inst.set_output(*Useless);
  }

  public: void Run(const Function& fun) {
    foreach (Function::EnumBBlock, oEnum, fun) {
      auto const pBB = oEnum.Get();
      BBlock::EnumI oEnumI(pBB);
      while (!oEnumI.AtEnd()) {
        auto const pI = oEnumI.Get();
        oEnumI.Next();
        pI->Apply(this);
      }
    }
    normalizer_.Start();
  }

  DISALLOW_COPY_AND_ASSIGN(LowerTasklet);
};

} // namespace

// ctor
X86LowerTask::X86LowerTask(Session& session, Module& module) 
  : Base("X86LowerTask", session, module) {}

// [P]
void X86LowerTask::ProcessFunction(Function& fun) {
  LowerTasklet tasklet(static_cast<CgSession&>(session()), fun.module());
  tasklet.Run(fun);
}

} // Cg
} // Il
