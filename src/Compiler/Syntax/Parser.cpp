#include "precomp.h"
// @(#)$Id$
//
// Evita Parser
//
// Copyright (C) 2010 by Project Vogue.
// Written by Yoshifumi "VOGUE" INOUE. (yosi@msn.com)

#include "./Parser.h"

#include "./Keyword.h"
#include "./Lexer.h"
#include "./NameLexer.h"
#include "./NameScope.h"
#include "./Operator.h"

#include "../CompileSession.h"

#include "../CompilerGlobal.h"

#include "../Ir/AliasDef.h"
#include "../Ir/ClassDef.h"
#include "../Ir/EnumFieldDef.h"
#include "../Ir/FieldDef.h"
#include "../Ir/LabelDef.h"
#include "../Ir/LocalVarDef.h"
#include "../Ir/MethodDef.h"
#include "../Ir/MethodGroupDef.h"
#include "../Ir/MethodParamDef.h"
#include "../Ir/NamespaceBody.h"
#include "../Ir/NamespaceDef.h"
#include "../Ir/PropertyDef.h"
#include "../Ir/TypeDef.h"
#include "../Ir/TypeParamDef.h"
#include "../Ir/UsingNamespace.h"

#include "../../Il/Ir/Instructions.h"
#include "../../Il/Ir/Operands.h"

namespace Compiler {

using namespace Il::Ir;

namespace {

static const char* const k_rgpszState[] = {
  #define DEFSTATE(mp_name) # mp_name,
  #include "./Parser_State.inc"
}; // k_rgpszState

static bool IsNameRefI(const Operand& callee, const Name& name) {
  class Local {
    public: static const NameRef::SimpleName* ToSimpleName(
        const Instruction& inst) {
      if (!inst.Is<NameRefI>()) {
        return nullptr;
      }
      auto const name_ref = inst.op1().DynamicCast<NameRef>();
      return name_ref ? name_ref->GetSimpleName() : nullptr;
    }
  };

  if (auto const callee_def_inst = callee.def_inst()) {
    if (auto const load_inst = callee_def_inst->DynamicCast<LoadI>()) {
      if (auto const ptr_def_inst = load_inst->op0().def_inst()) {
        DEBUG_FORMAT("ptr_def_inst=%s", ptr_def_inst);
        auto const simple_name = Local::ToSimpleName(*ptr_def_inst);
        return simple_name && simple_name->name() == name;
      } else {
        DEBUG_FORMAT("load_inst=%s", load_inst);
      }
    } else {
      DEBUG_FORMAT("callee_def_inst=%s", callee_def_inst);
    }
  }
  return false;
}

static const Operand& NormalizeCaseLabel(const Operand& operand) {
  if (auto const def_inst = operand.def_inst()) {
    if (def_inst->Is<LoadI>()) {
      return NormalizeCaseLabel(def_inst->op0());
    }

    if (def_inst->Is<NameRefI>()) {
      return def_inst->op1();
    }

    if (def_inst->Is<StaticCastI>()) {
      return NormalizeCaseLabel(def_inst->op0());
    }
  }
  return operand;
}

} // namespace

SwitchOperandBox* Parser::SwitchControl::FindCase(
    const Operand& operand) const {
  const Operand& label = NormalizeCaseLabel(operand);
  foreach (SwitchI::EnumCase, operands, m_pSwitchI) {
    auto& box = *operands.Get();
    auto& present = NormalizeCaseLabel(*box.GetOperand());
    DEBUG_FORMAT("%s %s", label, present);
    if (present == label) {
      return &box;
    }

    auto const present_name_ref = present.DynamicCast<NameRef>();
    auto const label_name_ref = label.DynamicCast<NameRef>();
    if (present_name_ref
        && label_name_ref
        && *present_name_ref == *label_name_ref) {
      return &box;
    }
  }
  DEBUG_FORMAT("not found %s(%s)", label, operand);
  return nullptr;
}

Parser::Parser(
  CompileSession& session,
  CompilationUnit& compilation_unit)
  : builder_(nullptr),
    state_(State_CompilationUnit),
    modifiers_(0),
    m_iUnique(0),
    class_def_(nullptr),
    compilation_unit_(compilation_unit),
    current_type_(nullptr),
    enum_field_(nullptr),
    expression_(nullptr),
    frame_reg_(nullptr),
    last_enum_field_(nullptr),
    local_var_def_(nullptr),
    memory_zone_(session.memory_zone()),
    method_def_(nullptr),
    name_ref_(nullptr),
    // Create NameScope for global namespace.
    name_scope_(new NameScope(local_zone_, nullptr)),
    namespace_body_(&compilation_unit.namespace_body()),
    outer_ns_member_(&compilation_unit.namespace_body().namespace_def()),
    property_def_(nullptr),
    session_(session),
    typaram_def_(nullptr),

    // Lexers
    //ALLOW_THIS_IN_INITIALIZER_LIST(m_pCastLexer(new CastLexer(this, this))),
    //current_type_ArgsLexer(new TypeArgsLexer(m_pCastLexer)),
    ALLOW_THIS_IN_INITIALIZER_LIST(
      name_lexer_(*new NameLexer(*this))),
    lexer_(
        *new Lexer(session, compilation_unit.source_path(), name_lexer_)) {
  ASSERT(namespace_body_ != nullptr);
}

Parser::~Parser() {
  while (!paren_stack_.IsEmpty()) {
    auto& token = *paren_stack_.Pop();
    token.Release();
  }

  {
    auto runner = name_scope_;
    while (runner) {
      auto const outer = runner->GetOuter();
      delete runner;
      runner = outer;
    }
  }

  ReleaseModifierMap();
  delete &name_lexer_;
  delete &lexer_;
}

// [A]
void Parser::AddClassDef(ClassDef& class_def) {
  ASSERT(!!namespace_body_);
  ASSERT(!!outer_ns_member_);

  if (auto const ns_def = outer_ns_member_->DynamicCast<NamespaceDef>()) {
    ASSERT(&namespace_body_->namespace_def() == ns_def);
    namespace_body_->Add(class_def);
  } else {
    outer_ns_member_->Add(class_def);
  }
}

void Parser::AddError(
    const SourceInfo& source_info,
    CompileError const error_code,
    Box a) {
  ASSERT(error_code != CompileError_NoError);
  ASSERT(!a.Is<Token>());
  session_.AddError(source_info, error_code, CollectionV_<Box>(a));
}

void Parser::AddError(
    const SourceInfo& source_info,
    CompileError const error_code,
    Box a,
    Box b) {
  ASSERT(error_code != CompileError_NoError);
  ASSERT(!a.Is<Token>());
  ASSERT(!b.Is<Token>());
  session_.AddError(source_info, error_code, CollectionV_<Box>(a, b));
}

void Parser::AddError(
    const SourceInfo& source_info,
    CompileError const error_code,
    Box a,
    Box b,
    Box c) {
  ASSERT(error_code != CompileError_NoError);
  ASSERT(!a.Is<Token>());
  ASSERT(!b.Is<Token>());
  ASSERT(!c.Is<Token>());
  session_.AddError(source_info, error_code, CollectionV_<Box>(a, b));
}

void Parser::AddError(CompileError const error_code, const Token& token) {
  AddError(token.source_info(), error_code, token.ToString());
}

void Parser::AddError(CompileError const error_code, const Token& token,
                      Box a) {
  AddError(token.source_info(), error_code, token.ToString(), a);
  token.AddRef();
}

void Parser::AddError(CompileError const error_code,  const Token& token,
                      Box a, Box b) {
  AddError(token.source_info(), error_code, token.ToString(), a, b);
}

void Parser::AddVarDef(const Name& name, const SourceInfo& source_info) {
  ASSERT(current_type_ != nullptr);

  if (auto const present = name_scope_->Find(name)) {
    if (present->Is<VarDef>()) {
      AddWarning(source_info, CompileError_Parse_RedefineSame, name);
    }
  }

  local_var_def_ = new LocalVarDef(
      *method_def_,
      *current_type_,
      name,
      *new Variable(name),
      source_info);

  method_def_->Add(*local_var_def_);
  name_scope_->Add(*local_var_def_);
}

void Parser::AddWarning(
    const SourceInfo& source_info,
    CompileError const error_code,
    Box a) {
  ASSERT(error_code != CompileError_NoError);
  session_.AddWarning(source_info, error_code, a);
}

void Parser::AddWarning(
    const SourceInfo& source_info,
    CompileError const error_code,
    Box a,
    Box b) {
  ASSERT(error_code != CompileError_NoError);
  session_.AddWarning(source_info, error_code, a, b);
}

void Parser::AddWarning(
    const SourceInfo& source_info,
    CompileError const error_code,
    Box a,
    Box b,
    Box c) {
  ASSERT(error_code != CompileError_NoError);
  session_.AddWarning(source_info, error_code, a, b, c);
}

// [C]
void Parser::CallExpr(const Type& type, State const eContinueState) {
  expect_stack_.Push(&type);
  state_stack_.Push(eContinueState);
  expression_ = nullptr;
  name_lexer_.SetExpressionMode();
  CallState(State_Expr, State_Expr_Expr);
}

void Parser::CallExpr(
    const Type& type,
    const NameRef& name_ref,
    State const continue_state) {
  CallExpr(type, continue_state);
  ScopedRefCount_<const NameRefToken> name_ref_token(
      *new NameRefToken(name_ref));
  ProcessToken(*name_ref_token);
}

void Parser::CallExpr(State const eContinueState) {
  CallExpr(*expect_stack_.GetTop(), eContinueState);
}

void Parser::CallState(State const eNextState) {
  state_stack_.Push(state_);
  GotoState(eNextState);
}

void Parser::CallState(
  State const eNextState,
  State const eContinueState) {
  //ASSERT(eContinueState != eNextState); State_Class_Member

  DEBUG_PRINTF("[%d] push %s\n",
      state_stack_.Count(),
      k_rgpszState[eContinueState]);

  state_stack_.Push(eContinueState);
  GotoState(eNextState);
}

Function& Parser::CloseFinally() {
  ASSERT(frame_reg_->IsFinally());

  auto& finfun = *frame_reg_->GetDefI()
      ->GetSx()->StaticCast<Function>();

  builder_->EmitI(*new CloseI(frame_reg_));

  auto& v1 = builder_->NewValues();

  builder_->EmitI(
      *new ValuesI(
          v1,
          *frame_reg_->GetDefI()->GetVy()->GetDefI()
              ->StaticCast<ValuesI>()));

  builder_->EmitCallI(finfun, v1);

  return finfun;
}

const Operand& Parser::CoerceToBoolean(const Operand& s1) {
  if (auto const b1 = s1.DynamicCast<BoolOutput>()) {
    auto& r2 = builder_->NewRegister();
    builder_->EmitI(
        *new(builder_->zone()) IfI(
            *Ty_Boolean, r2, *b1,
            builder_->NewLiteral(true),
            builder_->NewLiteral(false)));
    return r2;
  }

  if (s1.GetTy() == *Ty_Boolean) {
    return s1;
  }

  auto& r2 = builder_->EmitI(Op_StaticCast, *Ty_Boolean, s1);
  return r2;
}

const BoolOutput& Parser::CoerceToBoolOutput(const Operand& src) {
  if (auto const bool_reg = src.DynamicCast<BoolOutput>()) {
    AddError(builder_->source_info(), CompileError_Other_Error, src);
    return *bool_reg;
  }

  if (src.GetTy() == *Ty_Boolean) {
    return builder_->EmitBool(Op_Eq, src, true);
  }

  auto& r2 = builder_->EmitI(Op_StaticCast, *Ty_Boolean, src);
  return builder_->EmitBool(Op_Eq, r2, true);
}

void Parser::ContinueState(State const eNextState) {
  GotoState(eNextState);
}

// [E]
const Operand& Parser::EmitCast(const Operand& src, const Type& type) {
  if (src.GetTy().IsSubtypeOf(type) == Subtype_Yes) {
      return *expression_;
  }

  if (auto const reg = src.DynamicCast<Register>()) {
    if (auto const null_inst = reg->GetDefI()->DynamicCast<NullI>()) {
      null_inst->set_output_type(type);
      return *reg;
    }
  }

  auto& r1 = builder_->EmitI(Op_StaticCast, type, src);
  return r1;
}

// Load name reference
//  NAMEREF T* %r1 <= %void $NameRef
//  LOAD T %r2 <= %r1
const Register& Parser::EmitLoadNameRef(
    const Operand& base,
    const NameRef& name_ref) {
  auto& r1 = builder_->EmitNameRefI(*new TypeVar(), base, name_ref);
  return builder_->EmitLoadI(r1);
}

// Call static T operator unary(T).
void Parser::EmitUnaryOperator(const NameRef& operator_name_ref) {
  ASSERT(expression_ != nullptr);
  expression_ = &builder_->EmitCallI(
      expression_->type(),
      operator_name_ref,
      *expression_);
}

void Parser::EmitVarDefI(const Variable& var, const Operand& operand) {
  if (auto const r1 = operand.DynamicCast<Register>()) {
      if (!r1->GetVariable()) {
          r1->SetVariable(var);
      }
  }

  builder_->EmitI(
      *new VarDefI(
          Ty_ClosedCell->Construct(operand.GetTy()),
          builder_->NewRegister(),
          var,
          operand));
}

const Register& Parser::EmitVarPtr(const Variable& var) {
  ASSERT(var.GetDefI() != nullptr);
  auto& cell_class = *var.GetDefI()->output_type()
      .StaticCast<ConstructedClass>();
  auto& field = *cell_class.Find(*Q_value)
      ->StaticCast<Field>();
  auto& r1 = builder_->NewRegister();
  builder_->EmitI(*new FieldPtrI(r1, *var.GetRd(), field));
  return r1;
}

void Parser::EndEnum() {
  ASSERT(class_def_ != nullptr);
  ASSERT(enum_field_ != nullptr);
  ASSERT(method_def_ == nullptr);
  builder_->EmitRetI(*Void);
  class_def_ = nullptr;
  enum_field_ = nullptr;
  last_enum_field_ = nullptr;
  method_def_ = nullptr;
  outer_ns_member_ = outer_ns_member_->owner().StaticCast<ClassOrNamespaceDef>();
}

void Parser::EndMethod() {
  ASSERT(class_def_ != nullptr);
  ASSERT(method_def_ != nullptr);

  DEBUG_FORMAT("%s.%s", class_def_, method_def_);

  if (builder_) {
    if (auto const pCurrBB = builder_->GetCurr()) {
      auto const pLastI = pCurrBB->GetLastI();
      if (!pLastI || !pLastI->Is<LastInstruction>()) {
        if (method_def_->return_type() == *Ty_Void) {
            builder_->EmitRetI(*Void);
        } else if (pCurrBB->CountInEdges()) {
            AddError(
                builder_->source_info(),
                CompileError_Parse_Method_NoReturnValue,
                method_def_);
        }
      }
    }
    builder_ = builder_->EndFunction();
  }

  method_def_ = nullptr;
}

void Parser::EndProperty(IndexerOrProperty which) {
  ASSERT(class_def_ != nullptr);
  ASSERT(property_def_ != nullptr);

  auto* getter = property_def_->FindMember(*Q_get);
  auto* setter = property_def_->FindMember(*Q_set);

  if (!getter && !setter) {
    AddError(
        property_def_->source_info(),
        which == IsProperty
            ? CompileError_Parse_Property_Member_None
            : CompileError_Parse_Indexer_Member_None,
        property_def_->name());
    return;
  }

  if (class_def_->IsInterface()) {
    if (getter && getter->function()) {
      AddError(
          getter->source_info(),
          which == IsProperty
              ? CompileError_Parse_Property_Member_Interface_Body
              : CompileError_Parse_Indexer_Member_Interface_Body,
          *Q_get);
    }

    if (setter && setter->function()) {
      AddError(
          getter->source_info(),
          which == IsProperty
              ? CompileError_Parse_Property_Member_Interface_Body
              : CompileError_Parse_Indexer_Member_Interface_Body,
          *Q_set);
    }
    return;
  }

  if (property_def_->IsExtern()) {
    if (getter && getter->function()) {
      AddError(
          getter->source_info(),
          which == IsProperty
              ? CompileError_Parse_Property_Member_Extern_Body
              : CompileError_Parse_Indexer_Member_Extern_Body,
          *Q_get);
    }

    if (setter && setter->function()) {
      AddError(
          getter->source_info(),
          which == IsProperty
              ? CompileError_Parse_Property_Member_Extern_Body
              : CompileError_Parse_Indexer_Member_Extern_Body,
          *Q_set);
    }

    return;
  }

  if (which == IsProperty
      && getter && !getter->function()
      && setter && !setter->function()) {
    // automatic property
    auto& field_def = *new FieldDef(
        *class_def_,
        Modifier_Anonymous | Modifier_Private
            | (property_def_->IsStatic() ? Modifier_Static : 0),
        property_def_->property_type(),
        Name::Intern(
            String::Format(".prop_%s", property_def_->name())),
        property_def_->source_info());

    class_def_->Add(field_def);

    // Auto generated property getter method body.
    //   NAMEREF T* %r1 <= %this NameRef
    //   LOAD T %r2 <= %r1
    //   RET %r2
    {
      builder_ = &Builder::StartMethodFunction(
          builder_,
          *getter,
          getter->source_info());

      auto& r1 = builder_->EmitNameRefI(
          property_def_->property_type(),
          getter->this_operand(),
          *new NameRef(field_def.name(), field_def.source_info()));
      auto& r2 = builder_->EmitLoadI(r1);
      builder_->EmitRetI(r2);
      method_def_ = getter;
      EndMethod();
    }

    // Auto generated property setter.
    //   r1 <= ...
    //   NAMEREF T* %r3 <= %this NameRef
    //   STORE %3, %r2
    //   RET %r2
    {
      builder_ = &Builder::StartMethodFunction(
          builder_,
          *setter, 
          setter->source_info());
      MethodDef::EnumParam params(*setter);
      auto& var_def = *params.Get();
      auto& var = var_def.GetVariable();
      auto& r1 = EmitVarPtr(var);
      auto& r2 = builder_->EmitLoadI(r1);
      auto& r3 = builder_->EmitNameRefI(
          var.GetTy(),
          setter->this_operand(),
          *new NameRef(field_def.name(), field_def.source_info()));
      builder_->EmitStoreI(r3, r2);
      builder_->EmitRetI(*Void);
      method_def_ = setter;
      EndMethod();
    }

    return;
  }

  if (getter && !getter->function()) {
    AddError(
        getter->source_info(),
        which == IsProperty
            ? CompileError_Parse_Property_Member_Missing_Body
            : CompileError_Parse_Indexer_Member_Missing_Body,
        *Q_get);
  }

  if (setter && !setter->function()) {
    AddError(
        setter->source_info(),
        which == IsProperty
            ? CompileError_Parse_Property_Member_Missing_Body
            : CompileError_Parse_Indexer_Member_Missing_Body,
        *Q_set);
  }
}

void Parser::EndUnary(const Operator& op) {
  ASSERT(expression_ != nullptr);
  auto& r1 = builder_->EmitCallI(
      *new TypeVar(),
      *op.GetCallee(),
      *expression_);
  PopState();
  expression_ = &r1;
}

bool Parser::Expect(const Operator& expected, const Token& token) {
  if (token.Is(expected)) {
    return true;
  }

  AddError(CompileError_Parse_Expect, token, expected);
  return false;
}

bool Parser::ExpectCloseParen(const Token& token) {
  if (!Expect(*Op__CloseParen, token)) {
      return false;
  }
  PopParenthesis(token);
  return true;
}

// [F]
NameDef* Parser::Find(const Name& name) const {
  return name_scope_->Find(name);
}

Parser::SwitchControl* Parser::FindSwitchControl() const {
  foreach (EnumControl, oEnum, this) {
      auto const pControl = oEnum.Get();
      if (auto const pSwitch = pControl->DynamicCast<SwitchControl>()) {
          return pSwitch;
      }
  }
  return nullptr;
}

void Parser::Finish() {
  if (!session_.HasError()) {
    lexer_.Finish();
    while (!paren_stack_.IsEmpty()) {
      auto& token = *paren_stack_.Pop();
      AddError(
        CompileError_Parse_Operator_ParenNotClosed,
        token);
      token.Release();
    }
  }

  if (!session_.HasError()) {
    if (NeedMoreFeed()) {
      AddError(
        last_source_info_,
        CompileError_Parse_Unexpected_Eof,
        k_rgpszState[state_]);
    }
  }

  if (!session_.HasError()) {
    if (!bblock_stack_.IsEmpty()) {
      AddError(
          last_source_info_,
          CompileError_Parse_Internal_StackNotEmpty,
          "BBlock statck");
    }

    if (!control_stack_.IsEmpty()) {
      AddError(
          last_source_info_,
          CompileError_Parse_Internal_StackNotEmpty,
          "Control statck");
    }

    if (!expect_stack_.IsEmpty()) {
      AddError(
          last_source_info_,
          CompileError_Parse_Internal_StackNotEmpty,
          "Expect statck");
    }

    if (!instruction_stack_.IsEmpty()) {
      AddError(
          last_source_info_,
          CompileError_Parse_Internal_StackNotEmpty,
          "Integer statck");
    }

    if (!operand_stack_.IsEmpty()) {
      AddError(
          last_source_info_,
          CompileError_Parse_Internal_StackNotEmpty,
          "Operand statck");
    }

    while (!state_stack_.IsEmpty()) {
      auto const state = state_stack_.Pop();
      AddError(
        last_source_info_,
        CompileError_Parse_Internal_StateInStack,
        k_rgpszState[state]);
    }
  }
}

// [G]
Instruction* Parser::GetGetterInstruction(const Operand& operand) {
  if (auto const def_inst = operand.def_inst()) {
    if (def_inst->Is<LoadI>()) {
      return def_inst;
    }

    if (def_inst->Is<CallI>() && IsNameRefI(def_inst->op0(), *Q_get_Item)) {
      return def_inst;
    }

    DEBUG_FORMAT("def_inst=%s", def_inst);
  }
  return nullptr;
}

const NameRef* Parser::GetNameRef(const Token& token) {
  auto const name_ref = GetTypeNameRef(token);
  return !name_ref || name_ref->IsArray() ? nullptr : name_ref;
}

const Name& Parser::GetSimpleName(
    const NameRef& name_ref,
    CompileError error) {
  if (auto const simple_name = name_ref.GetSimpleName()) {
    return simple_name->name();
  }

  AddError(name_ref.source_info(), error, name_ref);
  return Name::Intern(name_ref.ToString());
}

const NameRef* Parser::GetTypeNameRef(const Token& token) {
  if (auto const name_token = token.DynamicCast<NameToken>()) {
    return &name_token->ToNameRef();
  }

  if (auto const qname_token = token.DynamicCast<QualifiedNameToken>()) {
    return &qname_token->ToNameRef();
  }

  if (auto const name_ref_token = token.DynamicCast<NameRefToken>()) {
    return &name_ref_token->name_ref();
  }

  return nullptr;
}

void Parser::GotoState(State const eNextState) {
  state_ = eNextState;
}

// [I]
MethodGroupDef& Parser::InternMethodGroup(
    const Name& method_name,
    const SourceInfo& source_info) {
  ASSERT(class_def_ != nullptr);

  if (auto const present = class_def_->Find(method_name)) {
    if (auto const mtg = present->DynamicCast<MethodGroupDef>()) {
      return *mtg;
    }

    // Current type definition contains name as non-method.
    AddError(
        source_info,
        CompileError_Parse_RedefineOther,
        method_name);
  }

  auto& mtg = *new MethodGroupDef(*class_def_, method_name);
  class_def_->Add(mtg);
  return mtg;
}

// Intern method group name without type parameters.
MethodGroupDef& Parser::InternMethodGroup(const NameRef& name_ref) {
  ASSERT(!name_ref.IsArray());

  if (!name_ref.HasQualifier()) {
    return InternMethodGroup(name_ref.name(), name_ref.source_info());
  }

  StringBuilder builder;
  builder.Append(name_ref.qualifier().ToString());
  builder.Append('.');
  builder.Append(name_ref.name());

  return InternMethodGroup(
      Name::Intern(builder.ToString()),
      name_ref.source_info());
}

NamespaceDef& Parser::InternNamespaceDef(
    NamespaceDef& outer_ns_def,
    const Name& name,
    const SourceInfo& source_info) {
  if (auto const present = outer_ns_def.Find(name)) {
    if (auto const ns = present->DynamicCast<NamespaceDef>()) {
      ASSERT(ns != &outer_ns_def);
      return *ns;
    }

    AddError(source_info, CompileError_Parse_RedefineOther, name);
    // We continue parsing with dummy namespace.
  }

  auto& ns_def = *new NamespaceDef(outer_ns_def, name, source_info);
  outer_ns_def.Add(ns_def);

  auto& outer_ns = outer_ns_def.namespaze();
  if (auto const present = outer_ns.Find(name)) {
    if (auto const ns = present->DynamicCast<Namespace>()) {
      ns_def.Realize(*ns);
    } else {
      AddError(source_info, CompileError_Parse_RedefineOther, name);
      // We continue parsing with dummy namespace.
    }
  } else {
    ns_def.Realize(*new Namespace(outer_ns_def.namespaze(), name));
  }
  return ns_def;
}

// [N]

/// <summary>
///  This method returns true if Parser needs more feed to finish
///  compilation unit.
/// </summary>
bool Parser::NeedMoreFeed() const {
  return state_ != State_CompilationUnit_Member;
}

const Name& Parser::NewTempName(const char* psz) {
  ASSERT(psz != nullptr);
  char sz[100];
  ::wsprintfA(sz, "__%s_%d", psz, ++m_iUnique);
  return Name::Intern(sz);
}

// [O]
// For "try"-"finally" and "using".
Function& Parser::OpenFinally() {
  auto& finfun = method_def_->module().NewFunction(
      method_def_->function(),
      Function::Flavor_Finally,
      K_finally->name());
  finfun.SetUp();
  finfun.SetFunctionType(
      FunctionType::Intern(*Ty_Void, ValuesType::Intern()));

  frame_reg_ = new FrameReg(frame_reg_, FrameReg::Kind_Finally);

  auto& v1 = builder_->EmitValuesI();
  builder_->EmitI(
      *new OpenFinallyI(frame_reg_, &finfun, &const_cast<Values&>(v1)));

  return finfun;
}

// [P]
void Parser::Parse(const char16* const pwchSource, uint cwchSource) {
  ASSERT(pwchSource != nullptr);
  ASSERT(cwchSource > 0);
  lexer_.Feed(pwchSource, cwchSource);
}

// Move undefined label to outer name scope.
void Parser::PopNameScope() {
  DEBUG_PRINTF("~~~~~~~~~~~~~~~~~~~~\n");

  auto const outer = name_scope_->GetOuter();

  foreach (NameScope::EnumMember, oEnum, name_scope_) {
    auto const name_def = oEnum.Get();
    if (auto const label_def = name_def->DynamicCast<LabelDef>()) {
      if (label_def->IsDefined()) {
        if (!label_def->IsUsed()) {
          AddWarning(
              builder_->source_info(),
              CompileError_Parse_Label_NotUsed,
              label_def);
        }

      } else if (!outer) {
        AddWarning(
            builder_->source_info(),
            CompileError_Parse_Label_NotDefined,
            label_def);

      } else {
        outer->Add(*label_def);
      }
    }
  }

  delete name_scope_;

  name_scope_ = outer;
}

void Parser::PopParenthesis(const Token& token) {
  if (paren_stack_.IsEmpty()) {
    AddError(
      CompileError_Parse_Internal_StackIsEmpty,
      token);
  } else {
    auto& token = *paren_stack_.Pop();
    DEBUG_FORMAT("depth %d close for %s at %s:%s",
        paren_stack_.Count() + 1,
        token.ToString(),
        token.source_info().line(),
        token.source_info().column());
    token.Release();
  }
}

void Parser::PopState() {
  auto const eState = state_stack_.Pop();

  DEBUG_PRINTF("[%d] pop %s\n",
      state_stack_.Count(),
      k_rgpszState[eState]);

  GotoState(eState);
}

// owner:
//  Values %v1 <= %r0
//  OpenFinally %f2 <= finally %v1
//
// finally:
//  Prologue %v1 <=
//  Select %r2 <= %v1 0
//  Null %r3 <=
//  Ne %r4 <= %r2 %r3
//  Branch %r4 B1 B2
// B1:
//  RuntimeCast %r5 <= %r2
//  Null %r6 <=
//  Ne %r7 <= %r5 %r6
//  Branch %r7 B3 B1
// B3:
//  Values %v4 <= %r5
//  Call Disposable.Dispose %v4
//  Ret
// B2:
//  Ret
void Parser::ProcessDisposer(const Keyword& keyword, const Register& r0) {
  auto& finfun = OpenFinally();
  auto& enumerator = r0.type();

  {
    auto const pOpenI = frame_reg_->GetDefI();
    auto const pValuesI = pOpenI->GetVy()->GetDefI();
    pValuesI->AppendOperand(r0);
    pValuesI->set_output_type(ValuesType::Intern(enumerator));
  }

  GetControl<UsingControl>()->Add(frame_reg_);

  builder_ = &Builder::StartFunction(builder_, finfun);

  auto const pPrologueI = finfun.GetPrologueI();
  pPrologueI->set_output_type(ValuesType::Intern(enumerator));

  finfun.SetFunctionType(
      FunctionType::Intern(
          *Ty_Void,
          *pPrologueI->output_type().StaticCast<ValuesType>()));

  auto& r2 = builder_->NewRegister();
  builder_->EmitI(*new SelectI(enumerator, r2, *pPrologueI->GetVd(), 0));
  auto& r3 = builder_->EmitNullI(r2.type());
  auto& b4 = builder_->EmitBool(Op_Ne, r2, r3);

  auto const pNonNullBB = &builder_->NewBBlock();
  auto const pNullBB = &builder_->NewBBlock();

  builder_->EmitBranchI(b4, *pNonNullBB, *pNullBB);
  builder_->SetCurr(*pNonNullBB);

  auto& dispose_method = *Ty_Disposable->Find(*Q_Dispose);
  if (keyword == *K_for) {
    auto& r5 = builder_->EmitI(Op_RuntimeCast, *Ty_Disposable, r2);
    auto& r6 = builder_->EmitNullI(*Ty_Disposable);
    auto& b7 = builder_->EmitBool(Op_Ne, r5, r6);

    auto& dispose_block = builder_->NewBBlock();
    builder_->EmitBranchI(b7, dispose_block, *pNullBB);

    builder_->SetCurr(dispose_block);
    builder_->EmitCallI(dispose_method, r5);

  } else if (keyword == *K_using) {
    builder_->EmitCallI(dispose_method, r2);

  } else {
    AddError(
        last_source_info_,
        CompileError_Parse_Internal_Unexpected,
        keyword);
  }

  builder_->EmitRetI(*Void);
  builder_->SetCurr(*pNullBB);
  builder_->EmitRetI(*Void);
  builder_ = builder_->EndFunction();
}

bool Parser::ProcessExprLeft(
    const Token& token,
    Operator::Category const category,
    State const next_state,
    State const return_state) {
  if (expression_ == Void) {
      PopState();
      return false;
  }

  if (auto const op_token = token.DynamicCast<OperatorToken>()) {
    auto& op = op_token->value();
    if (op.GetCategory() == category) {
      builder_->set_source_info(op_token->source_info());
      auto& r1 = builder_->NewRegister();
      auto& v2 = *new Values();
      auto& outy = op.is_bool_output() ? *Ty_Boolean : expression_->type();
      auto& callee = *new NameRef(op.name(), op_token->source_info());
      PushI(*new(builder_->zone()) CallI(outy, r1, callee, v2));
      PushI(*new(builder_->zone()) ValuesI(v2, *expression_));
      CallState(next_state, return_state);
      return true;
    }
  }

  PopState();
  return false;
}

bool Parser::ProcessExprRight(State const next_state) {
  ASSERT(!!expression_);
  if (expression_ == Void) {
      AddError(
          builder_->source_info(),
          CompileError_Parse_Expr_BadExpr,
          expression_);
      return false;
  }

  auto& values_inst = *instruction_stack_.Pop()->StaticCast<ValuesI>();
  ASSERT(values_inst.CountOperands() == 1);
  auto& call_inst = *instruction_stack_.Pop()->StaticCast<CallI>();
  values_inst.AppendOperand(*expression_);
  builder_->EmitI(values_inst);
  values_inst.UpdateOutputType();
  builder_->EmitI(call_inst);
  expression_ = &call_inst.output();

  GotoState(next_state);
  return true;
}

// CurrBB:
//   Jump TestBB
//
// TestBB:
//   Phi %rIndex <= (CurrBB 0) (ContinueBB %r4)
//   Values Array<T> %v1 <= %rArray
//   Call Int32 %r2 <= Array<T>::Length %v0
//   Lt %b4 <= %rIndex %r2
//   Branch LoopBB BreakBB
//
//  LoopBB:
//   EltRef T* %r5 <= %rArray %rIndex
//   Load T %r6 <= %r5
//   FieldPtr %r7 <= %rVar ClosedCell<T>.value
//   Store %r7 %6
//   ...
//   Jump ContinueBB
//
// ContinueBB:
//   Add Int32 %r4 %rIndex 1
//   Jump TestBB
void Parser::ProcessForEachOnArray(const Operand& array) {
  operand_stack_.Pop(); // discard var type

  if (!array.Is<Register>() || !array.type().Is<ArrayType>()) {
    AddError(
        last_source_info_,
        CompileError_Parse_Internal_Unexpected,
        array);
    return;
  }

  auto& array_reg = *array.StaticCast<Register>();
  auto& arrty = *array_reg.type().StaticCast<ArrayType>();
  auto& elemty = arrty.element_type();
  auto& array_class = Ty_Array->Construct(elemty);

  StartFor();
  auto& control = *GetControl<ForControl>();
  auto& curr_block = *builder_->GetCurr();

  // CurrBB
  builder_->EmitJumpI(*control.m_pTestBB);

  // TestBB
  builder_->SetCurr(*control.m_pTestBB);
  auto& phi_inst = builder_->EmitPhiI(*Ty_Int32);
  phi_inst.AddOperand(curr_block, builder_->NewLiteral(*Ty_Int32, 0));
  auto& length_method = *array_class.Find(*Q_get_Length)
      ->StaticCast<MethodGroup>()
      ->Find(FunctionType::Intern(*Ty_Int32, ValuesType::Intern()));
  auto& index_reg = phi_inst.output();
  auto& len_reg = builder_->EmitCallI(*Ty_Int32, length_method, array_reg);
  auto& b4 = builder_->EmitBool(Op_Lt, index_reg, len_reg);
  builder_->EmitBranchI(b4, *control.m_pLoopBB, *control.m_pBreakBB);

  // ContinueBB
  builder_->SetCurr(*control.m_pContinueBB);
  auto& one = builder_->NewLiteral(*Ty_Int32, 1);
  auto& r4 = builder_->EmitI(Op_Add, *Ty_Int32, index_reg, one);
  builder_->EmitJumpI(*control.m_pTestBB);
  phi_inst.AddOperand(*control.m_pContinueBB, r4);

  // LoopBB
  builder_->SetCurr(*control.m_pLoopBB);
  auto& r5 = builder_->NewRegister();
  builder_->EmitI(*new(builder_->zone())
      EltRefI(
        PointerType::Intern(elemty),
        r5,
        array_reg,
        index_reg));
  auto &r6 = builder_->EmitLoadI(r5);
  ProcessVarDef(r6);

  builder_->SetSucc(*control.m_pContinueBB);
  CallState(State_Stmt, State_Stmt_For_Finish);
}

// CurrBB:
//  Values %v1 <= %r0
//  Call Enumerator<T> %rEnum <= Enumerable.GetEnumerator %v1
//  Values %v3 <= %rEnum
//  OpenFinally %r4 <= finally %v3
//  Jump TestBB
//
// BreakBB:
//  Values %r11 <= %rEnum
//  Call finally
//  Close %r4
//
// ContinueBB:
//  Values %v21 <= %rEnum
//  Call Enumerator.MoveNext %v21
//  Jump TestBB
//
// LoopBB:
//  Values %v41 <= %rEnum
//  Call %r42 <= Enumerator.get_Current %v41
//  FieldPtr %r43 <= %rVar ClosedCell<T>.value
//  Store %r43 %r42
//
// TestBB:
//  VALUES %v33 <= %rEnum
//  CALL Boolean %r34 <= Enumerator.HasMore %v33
//  BRANCH %r34 LoopBB BreakBB
void Parser::ProcessForEachOnEnumerator(const Operand& operand) {
  auto& elemty = *operand_stack_.Pop()->StaticCast<Type>();
  auto& enumerable = EmitCast(operand, Ty_Enumerable->Construct(elemty));
  auto& enumerator_class = Ty_Enumerator->Construct(elemty);

  // CurrBB
  auto& rEnum = builder_->EmitCallI(
        enumerator_class,
        *Ty_Enumerable->Construct(elemty).Find(*Q_GetEnumerator),
        enumerable);

  StartFor();
  auto const pControl = GetControl<ForControl>();
  control_stack_.Push(new UsingControl(frame_reg_));
  ProcessDisposer(*K_for, rEnum);
  builder_->EmitJumpI(*pControl->m_pTestBB);

  // LoopBB
  builder_->SetCurr(*pControl->m_pLoopBB);
  auto& rElement = builder_->EmitCallI(
      elemty,
      *enumerator_class.Find(*Q_get_Current),
      rEnum);
  ProcessVarDef(rElement);

  // ContinueBB
  builder_->SetCurr(*pControl->m_pContinueBB);
  builder_->EmitCallI(*enumerator_class.Find(*Q_MoveNext), rEnum);
  builder_->EmitJumpI(*pControl->m_pTestBB);

  // TestBB
  {
    builder_->SetCurr(*pControl->m_pTestBB);
    auto& r4 = builder_->EmitCallI(
        *Ty_Boolean,
         *enumerator_class.Find(*Q_HasMore),
         rEnum);

    auto& b5 = builder_->EmitBool(Op_Eq, r4, true);
    builder_->EmitBranchI(b5, *pControl->m_pLoopBB, *pControl->m_pBreakBB);
  }

  builder_->SetCurrSucc(*pControl->m_pLoopBB, *pControl->m_pContinueBB);
  CallState(State_Stmt, State_Stmt_ForEach_EndLoop);
}

Operand* Parser::ProcessInvoke() {
  auto const pValuesI = instruction_stack_.Pop()->StaticCast<ValuesI>();

  builder_->EmitI(*pValuesI);
  pValuesI->UpdateOutputType();
  auto const pCallI = instruction_stack_.Pop()->StaticCast<CallI>();
  builder_->EmitI(*pCallI);

  if (pCallI->GetSx()->Is<Type>()) {
      // ctor
      return pCallI->GetVy()->GetDefI()->GetSx();
  }

  return pCallI->GetOutput();
}

void Parser::ProcessToken(const Token& token) {
  while (!session_.HasError()) {
    DEBUG_FORMAT("state=%s token=|%s| %s",
        k_rgpszState[state_], token, token.GetKind());

    last_source_info_ = token.source_info();

    switch (state_) {
      #include "./Parser_Class.inc"
      #include "./Parser_Enum.inc"
      #include "./Parser_Expr.inc"
      #include "./Parser_Indexer.inc"
      #include "./Parser_Method.inc"
      #include "./Parser_Property.inc"
      #include "./Parser_Statement.inc"
      #include "./Parser_Type.inc"
      #include "./Parser_Using.inc"

      case State_Block:
        ASSERT(builder_ != nullptr);
        if (token.Is(*Op__CloseBrace)) {
          PopParenthesis(token);
          builder_->set_source_info(token.source_info());
          PopState();
          return;
        }

        CallState(State_Stmt, State_Block);
        continue;

      // CompilationUnit :=
      //      ExternalAliasDirecive(
      //      UsingDirective*
      //      GlobalAttribute*
      //      NamespaceMemberDecl*
      case State_CompilationUnit:
        if (token.Is(*K_using)) {
          CallState(State_Using);
          return;
        }

        GotoState(State_CompilationUnit_Attribute);
        continue;

      case State_CompilationUnit_Attribute:
        GotoState(State_CompilationUnit_Member);
        continue;

      case State_CompilationUnit_Member:
        if (token.Is(*K_namespace)) {
          CallState(State_Namespace);
          return;
        }

        CallState(State_TypeDef);
        continue;

      // Modifier
      case State_Modifier:
        ReleaseModifierMap();
        GotoState(State_Modifier_Modifiers);
        continue;

      case State_Modifier_Modifiers:
        if (auto const keyword_token = token.DynamicCast<KeywordToken>()) {
          auto const& keyword = keyword_token->value();
          if (auto const modifier = keyword.GetModifier()) {
            if (modifierMap_.Contains(&keyword.name())) {
              AddError(CompileError_Parse_Modifier_Duplicate, token);
            } else {
              modifierMap_.Add(&keyword.name(), keyword_token);
              keyword_token->AddRef();
            }
            return;
          }
        }

        PopState();
        continue;

      // Namespace
      case State_Namespace:
        ASSERT(namespace_body_ != nullptr);

        if (auto const name_ref = GetNameRef(token)) {
          StartNamespace(*name_ref);
          GotoState(State_Namespace_Name);
          return;
        }

        AddError(CompileError_Parse_ExpectName, token);
        return;

      case State_Namespace_Body:
        if (token.Is(*K_using)) {
          CallState(State_Using);
          return;
        }

        GotoState(State_Namespace_Member);
        continue;

      case State_Namespace_Member:
        ASSERT(&namespace_body_->namespace_def() == outer_ns_member_);

        if (token.Is(*Op__CloseBrace)) {
          PopParenthesis(token);
          namespace_body_ = namespace_body_->outer();
          outer_ns_member_ = &namespace_body_->namespace_def();
          ASSERT(outer_ns_member_ != nullptr);
          PopState();
          PopNameScope();
          return;
        }

        if (token.Is(*K_namespace)) {
          CallState(State_Namespace);
          return;
        }

        CallState(State_TypeDef);
        continue;

      case State_Namespace_Name:
        ASSERT(namespace_body_ != nullptr);
        ASSERT(outer_ns_member_ == &namespace_body_->namespace_def());
        if (Expect(*Op__OpenBrace, token)) {
          PushParenthesis(token);
          GotoState(State_Namespace_Body);
        }
        return;

      // TypeDef
      //  ClassDef     ::= modifier* "class"
      //  EnumDef      ::= modifier* "enum"
      //  InterfaceDef ::= modifier* "interface"
      //  StructDef    ::= modifier* "struct"
      case State_TypeDef:
        CallState(State_Modifier, State_TypeDef_Modifiers);
        continue;

      case State_TypeDef_Modifiers:
        if (token.Is(*K_class)) {
          modifiers_ = ValidateModifiers(
              CompileError_Parse_Class_BadModifier,
              Modifier_Mask_Class);

          GotoState(State_Class);
          return;
        }

        if (token.Is(*K_enum)) {
          modifiers_ = ValidateModifiers(
              CompileError_Parse_Enum_BadModifier,
              Modifier_Mask_Enum);

          GotoState(State_Enum);
          return;
        }

        if (token.Is(*K_interface)) {
          modifiers_ = ValidateModifiers(
              CompileError_Parse_Interface_BadModifier,
              Modifier_Mask_Interface);

          modifiers_ |= Modifier_ClassVariation_Interface;
          GotoState(State_Class);
          return;
        }

        if (token.Is(*K_struct)) {
          modifiers_ = ValidateModifiers(
              CompileError_Parse_Struct_BadModifier,
              Modifier_Mask_Struct);

          modifiers_ |= Modifier_ClassVariation_Struct;
          GotoState(State_Class);
          return;
        }

        AddError(CompileError_Parse_Namespace_BadMember, token);
        return;

      default:
        CAN_NOT_HAPPEN();
    }
  }
}

const Type& Parser::ProcessTypeName(const NameRef& name_ref) {
  return ResolveTypeName(name_ref);
}

const Variable& Parser::ProcessVarDef(const Operand& value) {
  ASSERT(local_var_def_ != nullptr);

  auto& var = local_var_def_->GetVariable();

  if (auto const pRx = value.DynamicCast<Register>()) {
    if (!pRx->GetVariable()) {
      pRx->SetVariable(var);
    }
  }

  auto& varty = value.GetTy();
  const_cast<Variable&>(var).SetTy(varty);
  auto& cell_class = Ty_ClosedCell->Construct(varty);

  builder_->EmitI(
      *new(builder_->zone()) VarDefI(
          cell_class,
          builder_->NewRegister(),
          var,
          value));

  return var;
}

void Parser::PushNameScope() {
  DEBUG_PRINTF("~~~~~~~~~~~~~~~~~~~~\n");
  name_scope_ = new NameScope(local_zone_, name_scope_);
}

void Parser::PushI(Instruction& inst) {
  inst.SetSourceInfo(&builder_->source_info());
  instruction_stack_.Push(&inst);
}

void Parser::PushParenthesis(const Token& token) {
  ASSERT(paren_stack_.IsEmpty()
      || paren_stack_.GetTop()->source_info() != token.source_info());
  paren_stack_.Push(&token);
  token.AddRef();
  DEBUG_FORMAT("depth %d", paren_stack_.Count());
}

// [R]
void Parser::ReleaseModifierMap() {
  foreach (ModifierMap::Enum, entries, modifierMap_) {
    auto const entry = entries.Get();
    entry.GetValue()->Release();
  }
}

// When we use TypeVar, we failed on below test:
//
// namespace N1.N2 { class A {} }
// namespace N3 { class A {} }
//
// namespace N3 {
//   using A = N1.N2.A;,
//   class B : A {}
// }
const Type& Parser::ResolveTypeName(const NameRef& name_ref) {
  class Local {
    public: static const Type& MakeType(
        const NameScope& name_scope,
        const NameRef& name_ref) {
      if (auto const simple_name = name_ref.GetSimpleName()) {
        auto& name = simple_name->name();
        if (auto const present = name_scope.Find(name)) {
          if (auto const class_def = present->DynamicCast<ClassDef>()) {
            return class_def->GetClass();
          }

          if (auto const typaram_def = present->DynamicCast<TypeParamDef>()) {
            return typaram_def->type_param();
          }
        }
      }
      return *new UnresolvedType(name_ref);
    }
  };

  auto type = &Local::MakeType(*name_scope_, name_ref);

  DEBUG_FORMAT("%s to %s %s", name_ref, type->GetKind(), *type);

  auto end = name_ref.rank_rend();
  for (auto it = name_ref.rank_rbegin(); it != end; ++it) {
    type = &ArrayType::Intern(*type, *it);
  }

  return *type;
}

// [S]
//  1. Create MethodDef object
//  2. Copy parameter from PropertyDef
void Parser::StartAccessor(IndexerOrProperty which, 
    const Name& member_name,
    const SourceInfo& source_info) {
  ASSERT(class_def_ != nullptr);
  ASSERT(!method_def_);
  ASSERT(property_def_ != nullptr);

  PushNameScope();

  StringBuilder builder;
  {
    auto last = static_cast<const NameRef::Item*>(nullptr);
    auto dot = "";
    foreach (NameRef::Enum, it, property_def_->qualified_name()) {
      if (last) {
        builder.Append(dot);
        dot = ".";
        builder.Append(last->ToString());
      }
      last = &*it;
    }
    builder.Append(dot);
    builder.Append(member_name);
    builder.Append('_');
    builder.Append(last->ToString());
  }

  auto& method_name = Name::Intern(builder.ToString());

  auto& method_group_def = InternMethodGroup(
      method_name,
      source_info);

  auto& method_def = *new MethodDef(
      *namespace_body_,
      method_group_def,
      property_def_->modifiers() | Modifier_SpecialName,
      member_name == *Q_set ? *Ty_Void : property_def_->property_type(),
      *new NameRef(method_name, source_info));

  method_group_def.Add(method_def);

  if (member_name == *Q_set) {
    auto& method_param_def = *new MethodParamDef(
        method_def,
        property_def_->property_type(),
        *Q_value,
        *new Variable(*Q_value),
        source_info);
    method_def.Add(method_param_def);
    name_scope_->Add(method_param_def);
  }

  foreach (PropertyDef::EnumParam, it, *property_def_) {
    auto& param_def = *it.Get();
    auto& name = param_def.name();
    ASSERT(!method_def.Find(name));
    auto& method_param_def = *new MethodParamDef(
        method_def,
        param_def.type(),
        name,
        *new Variable(name),
        param_def.source_info());
    method_def.Add(method_param_def);
    name_scope_->Add(method_param_def);
  }

  if (property_def_->FindMember(member_name)) {
    AddError(
        source_info,
        which == IsProperty
            ? CompileError_Parse_Property_Member_Duplicate
            : CompileError_Parse_Indexer_Member_Duplicate,
        property_def_,
        member_name);
  } else {
    property_def_->AddMember(member_name, method_def);
  }

  method_def_ = &method_def;
}

void Parser::StartClass(const NameRef& name_ref) {
  if (name_ref.HasQualifier()) {
    AddError(
        name_ref.source_info(),
        CompileError_Parse_ExpectName, 
        name_ref);
  }

  class_def_ = new(memory_zone_) ClassDef(
      *namespace_body_,
      *outer_ns_member_,
      modifiers_,
      name_ref.name(),
      name_ref.source_info());
  if (auto const present = outer_ns_member_->Find(name_ref.name())) {
    AddError(
        name_ref.source_info(),
        present->Is<ClassDef>()
            ? CompileError_Parse_Class_Redefined_Class
            : CompileError_Parse_Class_Redefined_Different,
        name_ref);
  }

  AddClassDef(*class_def_);
  name_scope_->Add(*class_def_);

  PushNameScope();

  for (auto it = name_ref.arg_begin(); it != name_ref.arg_end(); ++it) {
    if (auto const simple_name = it->GetSimpleName()) {
      auto& name = simple_name->name();
      if (auto const present = class_def_->Find(name)) {
          AddError(
              simple_name->source_info(),
              CompileError_Parse_Type_DuplicateName,
              name,
              present);
      }

      auto& type_param = *new(memory_zone_) TypeParamDef(
          *class_def_,
          name,
          simple_name->source_info());

      class_def_->AddTypeParam(type_param);
      name_scope_->Add(type_param);
    } else {
      AddError(
        it->source_info(),
        CompileError_Parse_Class_BadParam,
        *it);
    }
  }

  class_def_->SetClass();
  GotoState(State_Class_Name);
  return;
}

void Parser::StartEnum(ClassDef& class_def, const Type& base_type) {
  ASSERT(!last_enum_field_);

  // Add dummy static constructor for calculating member values.
  // Static constructor will be empty if all members defined.
  // Otherwise, it is compilation error since members aren't
  // constant, or there are cyclic references.
  auto& method_group_def = *new MethodGroupDef(class_def, *QD_cctor);
  class_def.Add(method_group_def);

  auto& method_def = *new MethodDef(
      *namespace_body_,
      method_group_def,
      Modifier_SpecialName | Modifier_Static,
      *Ty_Void,
      *new NameRef(*QD_cctor, class_def.source_info()));

  method_group_def.Add(method_def);

  builder_ = &Builder::StartMethodFunction(
      builder_,
      method_def,
      class_def.source_info());

  // private const .value = (T) -1;
  last_enum_field_ =
      new(memory_zone_) EnumFieldDef(
          class_def,
          Modifier_Const | Modifier_Private | Modifier_Static,
          base_type,
          *QD_value,
          class_def.source_info());
  last_enum_field_->set_expr(builder_->NewLiteral(base_type, -1));
  class_def.Add(*last_enum_field_);
  auto& r1 = builder_->EmitNameRefI(
      *last_enum_field_->GetTy(),
      *Void,
      *new NameRef(last_enum_field_->name(), class_def.source_info()));
  builder_->EmitStoreI(r1, last_enum_field_->expr());
}

BBlock& Parser::StartFor() {
  auto const pTestBB = &builder_->NewBBlock();
  auto const pLoopBB = &builder_->NewBBlock();
  auto const pContinueBB = &builder_->NewBBlock();
  auto const pBreakBB = &builder_->NewSucc();

  control_stack_.Push(
      new ForControl(
          frame_reg_,
          pLoopBB,
          pTestBB,
          pBreakBB,
          pContinueBB));

  return *pTestBB;
}

void Parser::StartIf(const BoolOutput& bool_src) {
  auto& then_block = builder_->NewBBlock();
  auto& else_block = builder_->NewBBlock();
  auto& end_block = builder_->NewSucc();
  builder_->EmitBranchI(bool_src, then_block, else_block);
  bblock_stack_.Push(&else_block);
  builder_->SetCurrSucc(then_block, end_block);
}

// Note: We can't check method signature here, since type name can be alias
// of forward referened type.
void Parser::StartMethod(const NameRef& method_name_ref) {
  ASSERT(!method_def_);
  ASSERT(class_def_ != nullptr);
  ASSERT(namespace_body_ != nullptr);

  DEBUG_FORMAT("Method %s %s ==============================",
    class_def_,
    method_name_ref);

  if (class_def_->IsInterface()) {
    modifiers_ = ValidateModifiers(
        CompileError_Parse_Interface_Method_Modifier,
        Modifier_New);

  } else {
    // Note: Modifiers in methods in struct are as same as methods in
    // class.
    modifiers_ = ValidateModifiers(
      CompileError_Parse_Class_Method_Modifier,
      Modifier_Mask_Method);
  }

  auto& mtg = InternMethodGroup(method_name_ref);

  method_def_ = new MethodDef(
      *namespace_body_,
      mtg,
      method_name_ref.GetSimpleName()
      && method_name_ref.GetSimpleName()->name() == *QD_ctor
          ? modifiers_ | Modifier_SpecialName
          : modifiers_,
      *current_type_,
      method_name_ref);

  mtg.Add(*method_def_);

  PushNameScope();

  // Populate type parameter list for generic method.
  HashSet_<const Name*> typaram_name_set;
  for (
      auto it = method_name_ref.arg_begin();
      it != method_name_ref.arg_end();
      ++it) {
    auto& name_ref = *it;
    auto const simple_name = name_ref.GetSimpleName();
    if (!simple_name) {
      AddError(
          name_ref.source_info(),
          CompileError_Parse_Method_TypeParam_Invalid,
          name_ref);
      continue;
    }

    auto& name = simple_name->name();
    if (typaram_name_set.Contains(&name)) {
      AddError(
          name_ref.source_info(),
          CompileError_Parse_Method_TypeParam_Dup,
          name);
    } else {
      typaram_name_set.Add(&name);
      auto& typaram_def = *new(memory_zone_) TypeParamDef(
          *method_def_,
          name,
          name_ref.source_info());
      DEBUG_FORMAT("type parameter: %s", typaram_def);
      method_def_->Add(typaram_def);
      name_scope_->Add(typaram_def);
    }
  }
}

void Parser::StartNamespace(const NameRef& ns_name_ref) {
  auto namespace_def = &namespace_body_->namespace_def();
  foreach (NameRef::Enum, it, ns_name_ref) {
    auto const simple_name = it->DynamicCast<NameRef::SimpleName>();
    if (!simple_name) {
      AddError(
        it->source_info(),
        CompileError_Parse_Name_Invalid,
        *it);
      continue;
    }

    namespace_def = &InternNamespaceDef(
        *namespace_def,
        simple_name->name(),
        simple_name->source_info());
  }

  namespace_body_ = new NamespaceBody(
      compilation_unit_,
      namespace_body_,
      *namespace_def,
      &ns_name_ref.source_info());

  outer_ns_member_ = namespace_def;
  PushNameScope();
}

void Parser::StartProperty(
    IndexerOrProperty const which,
    const Type& prop_type,
    const NameRef& prop_name_ref) {
  ASSERT(class_def_ != nullptr);

  DEBUG_FORMAT("Property %s %s ==============================",
    class_def_,
    prop_name_ref);

  // Note: Modifiers in methods in struct are as same as methods in
  // class.
  if (which == IsProperty) {
    modifiers_ = ValidateModifiers(
        CompileError_Parse_Property_Modifier_Invalid,
        class_def_->IsInterface() ? Modifier_New : Modifier_Mask_Property);
  } else {
    modifiers_ = ValidateModifiers(
        CompileError_Parse_Indexer_Modifier_Invalid,
        class_def_->IsInterface() ? Modifier_New : Modifier_Mask_Indexer);
  }

  if (prop_type == *Ty_Void) {
    AddError(
        prop_name_ref.source_info(),
        which == IsProperty
            ? CompileError_Parse_Property_Value_Void
            : CompileError_Parse_Indexer_Value_Void,
        prop_name_ref);
  }

  auto& prop_name = Name::Intern(prop_name_ref.ToString());
  if (auto const present = class_def_->Find(prop_name)) {
      AddError(
          prop_name_ref.source_info(),
          CompileError_Parse_Member_Duplicate,
          prop_name_ref,
          present);
  }

  property_def_ = new PropertyDef(
      *namespace_body_,
      *class_def_,
      modifiers_,
      prop_type == *Ty_Void ? *Ty_Object : prop_type,
      prop_name_ref);

  class_def_->Add(*property_def_);
}

int Parser::ValidateField(const SourceInfo& source_info, const Name& name) {
  ASSERT(class_def_ != nullptr);

  auto const modifiers = ValidateModifiers(
    CompileError_Parse_Field_Modifier_Invalid,
    Modifier_Mask_Field);

  if (class_def_->IsInterface()) {
    AddError(source_info, CompileError_Parse_Interface_Field, name);
  }

  if (auto const present = class_def_->Find(name)) {
    if (!present->Is<FieldDef>()) {
      AddError(source_info, CompileError_Parse_RedefineOther, name);
    }

    if (&present->owner() == class_def_) {
      AddError(source_info, CompileError_Parse_RedefineSame, name);
    }

    if ((modifiers & Modifier_New) == 0) {
      AddError(source_info, CompileError_Parse_NeedNewModifier, name);
    }
  }

  return modifiers;
}

int Parser::ValidateModifiers(
    CompileError const error_code,
    int const mask) {
  ASSERT(mask!= 0);
  int modifiers = 0;
  foreach (ModifierMap::Enum, entries, modifierMap_) {
    auto entry = entries.Get();
    auto& keyword_token = *entry.GetValue();
    auto const modifier = keyword_token.value().GetModifier();
    ASSERT(modifier != 0);
    if ((modifier & ~mask) != 0) {
      AddError(error_code, keyword_token);
    } else {
      modifiers |= modifier;
    }
    keyword_token.Release();
  }

  modifierMap_.Clear();
  return modifiers;
}

} // Compiler
