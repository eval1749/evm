#include "precomp.h"
// @(#)$Id$
//
// Evita Virtuam Machine - Targets - X86 - X86ValuesTask
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./X86ValuesTask.h"

#include "./X86Defs.h"

#include "../../../Il/Ir/VmErrorInfo.h"
#include "../../../Il/Tasks/NormalizeTasklet.h"
#include "../../../Il/Tasks/Tasklet.h"

namespace Il {
namespace Cg {

using namespace Il::Tasks;

namespace {

using namespace X86;

/// <summary>
///   Pass for expanding VALUES instruction.
///   <para>
///     Following instructions are related to this expansion.
///     <list>
///       <item><term>CALL</term></item>
///       <item><term>OPENFINALLY</term>
///         <description>Same as CALL.</description>
///       </item>
///       <item><term>MVRESTORE</term>
///         <description>Restore $r0 from $tcb.mv_value[0].</description>
///       </item>
///       <item><term>MVSAVE</term>
///         <description>Save $r0 to $tcb.mv_value[0].</description>
///       </item>
///       <item><term>RET</term></item>
///       <item><term>SELECT</term>
///         <description>We don't need SELECT anymore.</description>
///       </item>
///     </list>
///   </para>
/// </summary>
class ValuesTasklet : public Tasklet, public X86Functor {
  public: ValuesTasklet(Session& session, const Module& module)
      : Tasklet("ValuesTasklet", session),
        module_(module) {}

  public: virtual ~ValuesTasklet() {}

  // [R]
  public: void Run(const Function& fun) {
    foreach (Function::EnumI, insts, fun) {
      Process(insts.Get());
    }
  }

  // Instruction Rules
  #define DefProcI(mp_Name) \
      public: void virtual Process(mp_Name ## I* const pI) override

  DefProcI(Call) { ProcessCall(*pI); }
  DefProcI(OpenFinally) { ProcessCall(*pI); }

  DefProcI(Seelct) {
    pI->GetBBlock()->RemoveI(pI);
  }
};

} // namespace

// ctor
X86ValuesTask::X86ValuesTask(Session& session, Module& module)
    : Base("X86ValuesTask", session, module) {}

// [P]
void X86ValuesTask::ProcessFunction(Function& fn) {
  ValuesTasklet tasklet(session(), fn.module());
  tasklet.Run(fn);
} // ProcessFunction

} // Cg
} // Il
