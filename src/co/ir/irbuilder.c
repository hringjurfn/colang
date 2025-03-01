#include "../common.h"
#include "../util/tmpstr.h"
#include "ir.h"
#include "irbuilder.h"

ASSUME_NONNULL_BEGIN

#if DEBUG
__attribute__((used))
static Str debug_fmtval1(Str s, const IRValue* v, int indent) {
  s = str_appendfmt(s, "v%u(op=%s type=", v->id, IROpName(v->op));
  s = IRTypeStr(v->type, s);
  s = str_appendcstr(s, " args=[");
  if (v->args.len > 0) {
    for (u32 i = 0; i < v->args.len; i++) {
      IRValue* arg = (IRValue*)v->args.v[i];
      s = str_appendfmt(s, "\n  %*s", (indent * 2), "");
      s = debug_fmtval1(s, arg, indent + 1);
    }
    s = str_appendfmt(s, "\n%*s", (indent * 2), "");
  }
  s = str_appendcstr(s, "])");
  return s;
}

__attribute__((used))
static const char* debug_fmtval(u32 bufid, const IRValue* v) {
  static char bufs[6][512];
  assert(bufid < countof(bufs));
  char* buf = bufs[bufid];
  auto s = debug_fmtval1(str_new(32), v, 0);
  memcpy(buf, s, str_len(s) + 1);
  str_free(s);
  return buf;
}
#endif


bool IRBuilderInit(IRBuilder* u, Build* build, IRBuilderFlags flags) {
  if (u->mem) {
    // recycle
    // Note: As heap-allocated data is allocated in u->mem we have to be careful here
    // and avoid reusing certain memory. For example, SymMapClear(&u->vars) would lead to
    // undefined behavior as its pointer is no longer valid after calling MemLinearReset.
    MemLinearReset(u->mem);
  } else {
    // initialize
    u->mem = MemLinearAlloc(1024); // map 4MB up front
    if (!u->mem)
      return false;
  }
  u->build = build;
  u->flags = flags;
  u->pkg = IRPkgNew(u->mem, build->pkg->id);
  PtrMapInit(&u->typecache, 32, u->mem);
  u->b = NULL;
  u->f = NULL;
  u->vars = SymMapNew(8, u->mem);
  ArrayInitWithStorage(&u->defvars, u->defvarsStorage, countof(u->defvarsStorage));
  ArrayInitWithStorage(&u->funstack, u->funstackStorage, countof(u->funstackStorage));
  return true;
}


void IRBuilderDispose(IRBuilder* u) {
  // Note: No need to call PtrMapDispose(&u->typecache) etc since we use a linear allocator
  MemLinearFree(u->mem);
  u->mem = NULL;
}


// sealBlock sets b.sealed=true, indicating that no further predecessors will be added
// (no changes to b.preds)
static void sealBlock(IRBuilder* u, IRBlock* b) {
  assert(!b->sealed); // block not sealed already
  dlog("sealBlock %p", b);
  // TODO: port from co:
  // if (b->incompletePhis != NULL) {
  //   let entries = s.incompletePhis.get(b)
  //   if (entries) {
  //     for (let [name, phi] of entries) {
  //       dlogPhi(`complete pending phi ${phi} (${name})`)
  //       s.addPhiOperands(name, phi)
  //     }
  //     s.incompletePhis.delete(b)
  //   }
  // }
  b->sealed = true;
}

// startBlock sets the current block we're generating code in
static void startBlock(IRBuilder* u, IRBlock* b) {
  assert(u->b == NULL); // err: forgot to call endBlock
  u->b = b;
  dlog("startBlock %p", b);
}

// startSealedBlock is a convenience for sealBlock followed by startBlock
static void startSealedBlock(IRBuilder* u, IRBlock* b) {
  sealBlock(u, b);
  startBlock(u, b);
}

// endBlock marks the end of generating code for the current block.
// Returns the (former) current block. Returns null if there is no current
// block, i.e. if no code flows to the current execution point.
// The block sealed if not already sealed.
static IRBlock* endBlock(IRBuilder* u) {
  auto b = u->b;
  assert(b != NULL); // has current block

  // Move block-local vars to long-term definition data.
  // First we fill any holes in defvars.
  while (u->defvars.len <= b->id) {
    ArrayPush(&u->defvars, NULL, u->mem);
  }
  if (u->vars->len > 0) {
    u->defvars.v[b->id] = u->vars;
    u->vars = SymMapNew(8, u->mem);  // new block-local vars
  }

  u->b = NULL;  // crash if we try to use b before a new block is started
  return b;
}

typedef struct FunBuildState {
  IRFun*   f;
  IRBlock* b;
} FunBuildState;

static void startFun(IRBuilder* u, IRFun* f) {
  if (u->f) {
    // save current function generation state
    dlog("startFun suspend building %p", u->f);
    auto fbs = (FunBuildState*)memalloct(u->mem, FunBuildState);
    fbs->f = u->f;
    fbs->b = u->b;
    ArrayPush(&u->funstack, fbs, u->mem);
    u->b = NULL;
  }
  u->f = f;
  dlog("startFun %p", u->f);
}

static void endFun(IRBuilder* u) {
  assert(u->f != NULL); // no current function
  dlog("endFun %p", u->f);
  if (u->funstack.len > 0) {
    // restore function generation state
    auto fbs = (FunBuildState*)ArrayPop(&u->funstack);
    u->f = fbs->f;
    u->b = fbs->b;
    memfree(u->mem, fbs);
    dlog("endFun resume building %p", u->f);
  } else {
    #if DEBUG
    u->f = NULL;
    #endif
  }
}


static IRValue* TODO_Value(IRBuilder* u) {
  return IRValueNew(u->f, u->b, OpNil, IRType_void, NoPos);
}


// ———————————————————————————————————————————————————————————————————————————————————————————————
// Phi & variables

#define dlogvar(format, ...) dlog("VAR " format, ##__VA_ARGS__)


static void var_write(IRBuilder* u, Sym name, IRValue* value, IRBlock* b) {
  if (b == u->b) {
    dlogvar("write %s in current block", name);
    auto oldv = SymMapSet(u->vars, name, value);
    if (oldv != NULL) {
      dlogvar("new value replaced old value: %p", oldv);
    }
  } else {
    dlogvar("TODO write %s in defvars", name);
  }
}


static IRValue* var_read(IRBuilder* u, Sym name, Node* typeNode, IRBlock* b/*null*/) {
  if (b == u->b) {
    // current block
    dlogvar("var_read %s in current block", name);
    auto v = SymMapGet(u->vars, name);
    if (v != NULL) {
      return v;
    }
  } else {
    dlogvar("TODO var_read %s in defvars", name);
  //   let m = u.defvars[b.id]
  //   if (m) {
  //     let v = m.get(name)
  //     if (v) {
  //       return v
  //     }
  //   }
  }

  dlogvar("var_read %.*s not found -- falling back to readRecursive", (int)symlen(name), name);

  // // global value numbering
  // return u.var_readRecursive(name, t, b)

  dlog("TODO");
  return TODO_Value(u);
}


// ———————————————————————————————————————————————————————————————————————————————————————————————
// types


inline static const IRType* nullable typecache_get(IRBuilder* u, Type* ast_type) {
  return (const IRType*)PtrMapGet(&u->typecache, ast_type);
}

inline static void typecache_add(IRBuilder* u, Type* ast_type, const IRType* t) {
  PtrMapSet(&u->typecache, ast_type, (void*)t);
}


static const IRType* get_type(IRBuilder* u, Type* ast_type);


static const IRType* get_basic_type(IRBuilder* u, Type* ast_type) {
  asserteq_debug(ast_type->kind, NBasicType);
  switch (ast_type->t.basic.typeCode) {
    // primitive types
    //
    #define I_ENUM(NAME, TYPECODE) case TypeCode_##TYPECODE: return IRType_##NAME;
    IR_PRIMITIVE_TYPES(I_ENUM)
    #undef  I_ENUM
    // integers are sign agnostic
    case TypeCode_u8:  return IRType_i8;
    case TypeCode_u16: return IRType_i16;
    case TypeCode_u32: return IRType_i32;
    case TypeCode_u64: return IRType_i64;

    // aliases
    // TODO: make this configurable
    case TypeCode_int:
    case TypeCode_uint:  return IRType_i32;

    default:
      assertf(0, "invalid TypeCode %s", TypeCodeName(ast_type->t.basic.typeCode));
      UNREACHABLE;
  }
}


static const IRType* get_array_type(IRBuilder* u, Type* ast_type) {
  asserteq_debug(ast_type->kind, NArrayType);
  auto t = typecache_get(u, ast_type);
  if (t)
    return t;
  auto newt = (IRType*)memalloc(u->mem, sizeof(IRType) + sizeof(void*));
  newt->code = TypeCode_array;
  newt->count = ast_type->t.array.size;
  newt->elemv = (const IRType**)(((u8*)newt) + sizeof(IRType));
  newt->elemv[0] = get_type(u, ast_type->t.array.subtype);
  typecache_add(u, ast_type, newt);
  return newt;
}


// get_type returns the IR type corresponding to the ast_type
static const IRType* get_type(IRBuilder* u, Type* ast_type) {
  switch (ast_type->kind) {
    case NBasicType:
      return get_basic_type(u, ast_type);

    case NArrayType:
      //
      // TODO: consider adding "user" types to the IRPkg
      //
      return get_array_type(u, ast_type);

    // case NStructType:
    case NTupleType:
    case NFunType:
      panic("TODO %s", NodeKindName(ast_type->kind));

    default:
      assertf(0, "unexpected type %s", fmtnode(ast_type));
      UNREACHABLE;
  }
}


// ———————————————————————————————————————————————————————————————————————————————————————————————
// values (AST nodes)

static IRValue* nullable ast_add_expr(IRBuilder* u, Node* n);
static bool ast_add_toplevel(IRBuilder* u, Node* n);
static IRFun* ast_add_fun(IRBuilder* u, Node* n);


static IRValue* ast_add_intconst(IRBuilder* u, Node* n) { // n->kind==NIntLit
  assert(n->type->kind == NBasicType);
  auto t = get_type(u, n->type);
  auto v = IRFunGetConstInt(u->f, t, n->val.i);
  return v;
}


static IRValue* ast_add_boolconst(IRBuilder* u, Node* n) { // n->kind==NBoolLit
  assert(n->type->kind == NBasicType);
  assert(n->type->t.basic.typeCode == TypeCode_bool);
  return IRFunGetConstBool(u->f, (bool)n->val.i);
}


static IRValue* ast_add_id(IRBuilder* u, Node* n) { // n->kind==NId
  assert(n->id.target != NULL); // never unresolved
  // dlog("ast_add_id \"%s\" target = %s", n->id.name, fmtnode(n->id.target));
  if (n->id.target->kind == NVar) {
    // variable
    return var_read(u, n->id.name, n->type, u->b);
  }
  // else: type or builtin etc
  return ast_add_expr(u, (Node*)n->id.target);
}


inline static bool is_zerocost_typecast(const IRType* srcType, const IRType* dstType) {
  auto fl = TypeCodeFlags(srcType->code);
  if ((fl & TypeCodeFlagInt) &&
      (fl & ~TypeCodeFlagSigned) == (TypeCodeFlags(dstType->code) & ~TypeCodeFlagSigned))
  {
    // integers which differ only by sign
    return true;
  }
  return false;
}


static IRValue* ast_add_array(IRBuilder* u, Node* n) { // n->kind==NArray
  // array is sequential memory of size known at compile time.
  // arrays are stored either:
  // - in global constant data (global immutable arrays)
  // - in global writable data (global mutable arrays)
  // - on the stack (local arrays)
  // arrays can only live on the heap when allocated implicitly.
  // An array is concretely represented by a pointer.

  auto t = get_type(u, n->type);
  auto v = IRValueAlloc(u->mem, OpAlloca, t, n->pos);

  for (u32 i = 0; i < n->array.a.len; i++) {
    Node* cn = (Node*)n->array.a.v[i];
    auto cv = ast_add_expr(u, cn);
    IRValueAddArg(v, u->mem, cv);
  }

  IRBlockAddValue(u->b, v);
  return v;
}


static IRValue* ast_add_index(IRBuilder* u, Node* n) { // n->kind==NIndex
  auto recv = ast_add_expr(u, n->index.operand);
  auto indexexpr = ast_add_expr(u, n->index.indexexpr);
  // See https://llvm.org/docs/LangRef.html#getelementptr-instruction
  // See https://llvm.org/docs/GetElementPtr.html

  auto v = IRValueNew(u->f, u->b, OpGEP, IRType_void, n->pos);
  IRValueAddArg(v, u->mem, recv);
  IRValueAddArg(v, u->mem, indexexpr);

  // set result type
  switch (recv->type->code) {
    case TypeCode_array:
      assertnotnull_debug(recv->type->elemv); // must have subtype
      v->type = recv->type->elemv[0];
      break;
    default:
      panic("TODO index of type %s", fmtirtype(recv->type));
  }

  return v;
}


static IRValue* ast_add_typecast(IRBuilder* u, Node* n) { // n->kind==NTypeCast
  assert(n->call.receiver != NULL);
  assert(NodeIsType(n->call.receiver));
  assert(n->call.args != NULL);

  // generate rvalue
  auto srcValue = ast_add_expr(u, n->call.args);

  // source and destination types
  auto srcType = srcValue->type;
  auto dstType = get_type(u, n->call.receiver);

  if (R_UNLIKELY(n->call.receiver->kind != NBasicType)) {
    build_errf(u->build, NodePosSpan(n),
      "invalid type %s in type cast", fmtnode(n->call.receiver));
    return TODO_Value(u);
  }

  // if the conversion if "free" (e.g. int32 -> uint32), short circuit
  if (dstType == srcType || is_zerocost_typecast(srcType, dstType))
    return srcValue;
  //
  // Variant with a Copy op in between:
  // // if the conversion if "free" (e.g. int32 -> uint32), short circuit
  // if (is_zerocost_typecast(srcType, dstType)) {
  //   auto v = IRValueNew(u->f, u->b, OpCopy, dstType, n->pos);
  //   IRValueAddArg(v, u->mem, srcValue);
  //   return v;
  // }

  // select conversion operation
  IROp convop = IROpConvertType(srcType->code, dstType->code);
  if (R_UNLIKELY(convop == OpNil)) {
    build_errf(u->build, NodePosSpan(n),
      "invalid type conversion %s to %s",
      TypeCodeName(srcType->code), TypeCodeName(dstType->code));
    return TODO_Value(u);
  }

  // build value for convertion op
  auto v = IRValueNew(u->f, u->b, convop, dstType, n->pos);
  IRValueAddArg(v, u->mem, srcValue);
  return v;
}


static IRValue* ast_add_binop(IRBuilder* u, Node* n) { // n->kind==NBinOp
  dlog("ast_add_binop %s %s = %s",
    TokName(n->op.op),
    fmtnode(n->op.left),
    n->op.right != NULL ? fmtnode(n->op.right) : "nil"
  );
  assertnotnull_debug(n->op.left->type);
  assertnotnull_debug(n->op.right->type);

  // gen left operand
  auto left  = ast_add_expr(u, n->op.left);
  auto right = ast_add_expr(u, n->op.right);

  dlog("[BinOp] left:  %s", debug_fmtval(0, left));
  dlog("[BinOp] right: %s", debug_fmtval(0, right));
  assertnotnull(left->type);
  assertnotnull(right->type);

  // lookup IROp
  IROp op = IROpFromAST(n->op.op, left->type->code, right->type->code);
  assert(op != OpNil);

  // read result type
  asserteq(n->type, n->op.left->type); // we assume binop type == op1 type.
  auto restype = left->type;

  // [debug] check that the type we think n will have is actually the type of the resulting value
  #if DEBUG
  TypeCode optype = IROpInfo(op)->outputType;
  if (optype > TypeCode_NUM_END) {
    // result type is parametric; is the same as an input type.
    assert(optype == TypeCode_param1 || optype == TypeCode_param2);
    optype = (optype == TypeCode_param1) ? left->type->code : right->type->code;
  }
  asserteq(optype, restype->code);
  #endif

  auto v = IRValueNew(u->f, u->b, op, restype, n->pos);
  IRValueAddArg(v, u->mem, left);
  IRValueAddArg(v, u->mem, right);
  return v;
}


static IRValue* ast_add_assign(IRBuilder* u, Sym name /*nullable*/, IRValue* value) {
  assert(value != NULL);
  if (name == NULL) {
    // dummy assignment to "_"; i.e. "_ = x" => "x"
    return value;
  }

  // instead of issuing an intermediate "copy", simply associate variable
  // name with the value on the right-hand side.
  var_write(u, name, value, u->b);

  if (u->flags & IRBuilderComments)
    IRValueAddComment(value, u->mem, name, symlen(name));

  return value;
}


static IRValue* nullable ast_add_var(IRBuilder* u, Node* n) { // n->kind==NVar
  if (n->var.nrefs == 0 && !NodeIsParam(n)) {
    // unused, unreferenced; ok to return bad value
    dlog("skip unused %s", fmtnode(n));
    return NULL;
  }
  assertnotnull(n->var.init); // TODO: support default-initializer (NULL)
  assertnotnull(n->type);
  assert(n->type != Type_ideal);
  dlog("ast_add_var %s %s = %s",
    n->var.name ? n->var.name : "_",
    fmtnode(n->type),
    n->var.init ? fmtnode(n->var.init) : "nil"
  );
  auto v = ast_add_expr(u, n->var.init); // right-hand side
  return ast_add_assign(u, n->var.name, v);
}


// TODO: port this old NParam code into ast_add_var
// static IRValue* ast_add_param(IRBuilder* u, Node* n) {
//   if (R_UNLIKELY(n->type->kind != NBasicType)) {
//     // TODO add support for NTupleType et al
//     build_errf(u->build, NodePosSpan(n), "invalid argument type %s", fmtnode(n->type));
//     return TODO_Value(u);
//   }
//   auto t = get_type(u, n->type);
//   auto v = IRValueNew(u->f, u->b, OpArg, t, n->pos);
//   v->auxInt = n->var.index;
//   if (u->flags & IRBuilderComments)
//     IRValueAddComment(v, u->mem, n->var.name, symlen(n->var.name));
//   return v;
// }


// ast_add_if reads an "if" expression, e.g.
//  (If (Op > (Ident x) (Int 1)) ; condition
//      (Var x (Int 1))          ; then block
//      (Var x (Int 2)) )        ; else block
// Returns a new empty block that's the block after the if.
static IRValue* ast_add_if(IRBuilder* u, Node* n) { // n->kind==NIf
  //
  // if..end has the following semantics:
  //
  //   if cond b1 b2
  //   b1:
  //     <then-block>
  //   goto b2
  //   b2:
  //     <continuation-block>
  //
  // if..else..end has the following semantics:
  //
  //   if cond b1 b2
  //   b1:
  //     <then-block>
  //   goto b3
  //   b2:
  //     <else-block>
  //   goto b3
  //   b3:
  //     <continuation-block>
  //
  // TODO

  // generate control condition
  auto control = ast_add_expr(u, n->cond.cond);
  if (R_UNLIKELY(control->type != TypeCode_bool)) {
    // AST should not contain conds that are non-bool
    build_errf(u->build, NodePosSpan(n->cond.cond),
      "invalid non-bool type in condition %s", fmtnode(n->cond.cond));
  }

  assert(n->cond.thenb != NULL);

  // [optimization] Early optimization of constant boolean condition
  if ((u->flags & IRBuilderOpt) != 0 && (IROpInfo(control->op)->aux & IRAuxBool)) {
    dlog("[ir/builder if] short-circuit constant cond");
    if (control->auxInt != 0) {
      // then branch always taken
      return ast_add_expr(u, n->cond.thenb);
    }
    // else branch always taken
    if (n->cond.elseb == NULL) {
      dlog("TODO ir/builder produce nil value of the approprite type of the pointer");
      return IRValueNew(u->f, u->b, OpNil, IRType_void, n->pos);
    }
    return ast_add_expr(u, n->cond.elseb);
  }

  // end predecessor block (leading up to and including "if")
  auto ifb = endBlock(u);
  ifb->kind = IRBlockIf;
  IRBlockSetControl(ifb, control);

  // create blocks for then and else branches
  auto thenb = IRBlockNew(u->f, IRBlockCont, n->cond.thenb->pos);
  auto elsebIndex = u->f->blocks.len; // may be used later for moving blocks
  auto elseb = IRBlockNew(u->f, IRBlockCont, n->cond.elseb == NULL ? n->pos : n->cond.elseb->pos);
  ifb->succs[0] = thenb;
  ifb->succs[1] = elseb; // if -> then, else

  // begin "then" block
  dlog("[if] begin \"then\" block");
  thenb->preds[0] = ifb; // then <- if
  startSealedBlock(u, thenb);
  auto thenv = ast_add_expr(u, n->cond.thenb);  // generate "then" body
  thenb = endBlock(u);

  IRValue* elsev = NULL;

  if (n->cond.elseb != NULL) {
    // "else"

    // allocate "cont" block; the block following both thenb and elseb
    auto contbIndex = u->f->blocks.len;
    auto contb = IRBlockNew(u->f, IRBlockCont, n->pos);

    // begin "else" block
    dlog("[if] begin \"else\" block");
    elseb->preds[0] = ifb; // else <- if
    startSealedBlock(u, elseb);
    elsev = ast_add_expr(u, n->cond.elseb);  // generate "else" body
    elseb = endBlock(u);
    elseb->succs[0] = contb; // else -> cont
    thenb->succs[0] = contb; // then -> cont
    contb->preds[0] = thenb;
    contb->preds[1] = elseb; // cont <- then, else
    startSealedBlock(u, contb);

    // move cont block to end (in case blocks were created by "else" body)
    IRFunMoveBlockToEnd(u->f, contbIndex);

    assertf(thenv->type == elsev->type,
      "branch type mismatch %s, %s", fmtirtype(thenv->type), fmtirtype(elsev->type));

    if (elseb->values.len == 0) {
      // "else" body may be empty in case it refers to an existing value. For example:
      //   x = 9 ; y = if true x + 1 else x
      // This compiles to:
      //   b0:
      //     v1 = const 9
      //     v2 = const 1
      //   if true -> b1, b2
      //   b1:
      //     v3 = add v1 v2
      //   cont -> b3
      //   b2:                    #<-  Note: Empty
      //   cont -> b3
      //   b3:
      //     v4 = phi v3 v1
      //
      // The above can be reduced to:
      //   b0:
      //     v1 = const 9
      //     v2 = const 1
      //   if true -> b1, b3     #<- change elseb to contb
      //   b1:
      //     v3 = add v1 v2
      //   cont -> b3
      //                         #<- remove elseb
      //   b3:
      //     v4 = phi v3 v1      #<- phi remains valid; no change needed
      //
      ifb->succs[1] = contb;  // if -> cont
      contb->preds[1] = ifb;  // cont <- if
      IRBlockDiscard(elseb);
      elseb = NULL;
    }

    if (u->flags & IRBuilderComments) {
      thenb->comment = str_fmt("b%u.then", ifb->id);
      if (elseb)
        elseb->comment = str_fmt("b%u.else", ifb->id);
      contb->comment = str_fmt("b%u.end", ifb->id);
    }

  } else {
    // no "else" block
    thenb->succs[0] = elseb; // then -> else
    elseb->preds[0] = ifb;
    elseb->preds[1] = thenb; // else <- if, then
    startSealedBlock(u, elseb);

    // move cont block to end (in case blocks were created by "then" body)
    IRFunMoveBlockToEnd(u->f, elsebIndex);

    if (u->flags & IRBuilderComments) {
      thenb->comment = str_fmt("b%u.then", ifb->id);
      elseb->comment = str_fmt("b%u.end", ifb->id);
    }

    // Consider and decide what semantics we want for if..then expressions without else.
    // There are at least three possibilities:
    //
    //   A. zero initialized value of the then-branch type:
    //
    //      "x = if y 3"                 typeof(x) => int       If false: 0
    //      "x = if y Account{ id: 1 }"  typeof(x) => Account   If false: Account{id:0}
    //
    //   B. zero initialized basic types, higher-level types become optional:
    //
    //      "x = if y 3"                 typeof(x) => int       If false: 0
    //      "x = if y Account{ id: 1 }"  typeof(x) => Account?  If false: nil
    //
    //   C. any type becomes optional:
    //
    //      "x = if y 3"                 typeof(x) => int?      If false: nil
    //      "x = if y Account{ id: 1 }"  typeof(x) => Account?  If false: nil
    //
    // Discussion:
    //
    //   C implies that the language has a concept of pointers beyond reference types.
    //   i.e. is an int? passed to a function copy-by-value or not? Probably not because then
    //   "fun foo(x int)" vs "fun foo(x int?)" would be equivalent, which doesn't make sense.
    //   Okay, so C is out.
    //
    //   B is likely the best choice here, assuming the language has a concept of optional.
    //   To implement B, we need to:
    //
    //   - Add support to resolve_type so that the effective type of the the if expression is
    //     optional for higher-level types (but not for basic types.)
    //
    //   - Decide on a representation of optional. Likely actually null; the constant 0.
    //     In that case, we have two options for IR branch block generation:
    //
    //       1. Store 0 to the result before evaluating the condition expression, or
    //
    //       2. generate an implicit "else" block that stores 0 to the result.
    //
    //     Approach 1 introduces possibly unnecessary stores while the second approach introduces
    //     Phi joints always. Also, the second approach introduces additional branching in the
    //     final generated code.
    //
    //     Because of this, approach 1 is the better one. It has optimization opportunities as
    //     well, like for instance: if we know that the storage for the result of the
    //     if-expression is already zero (e.g. from calloc), we can skip storing zero.
    //
    // Conclusion:
    //
    // - B. zero-initialized basic types, higher-level types become optional.
    // - Store zero before branch, rather than generating implicit "else" branches.
    // - Introduce "optional" as a concept in the language.
    // - Update resolve_type to convert higher-order types to optional in lieu of an "else" branch.
    //

    // zero constant in place of "else" block, sized to match the result type
    elsev = IRFunGetConstInt(u->f, thenv->type, 0);
  }

  // make Phi, joining the two branches together
  auto phi = IRValueNew(u->f, u->b, OpPhi, thenv->type, n->pos);
  assertf(u->b->preds[0] != NULL, "phi in block without predecessors");
  IRValueAddArg(phi, u->mem, thenv);
  IRValueAddArg(phi, u->mem, elsev);
  return phi;
}


static IRValue* ast_add_type_call(IRBuilder* u, Node* n) {
  asserteq_debug(n->kind, NCall);
  assert_debug(NodeIsType(n->call.receiver));
  Node* recv = n->call.receiver;

  switch (recv->kind) {
    case NBasicType:
      // convert to type cast
      n->kind = NTypeCast;
      dlog("TYPECAST");
      return ast_add_typecast(u, n);

    case NArrayType:
    case NTupleType:
    case NFunType:
      panic("TODO type_call %s %s", NodeKindName(recv->kind), fmtnode(recv));
      break;

    default:
      UNREACHABLE;
  }
}


static IRValue* ast_add_call(IRBuilder* u, Node* n) {
  asserteq_debug(n->kind, NCall);
  Node* recv = n->call.receiver;

  if (NodeIsType(recv))
    return ast_add_type_call(u, n);

  IRFun* fn;
  if (recv->kind == NFun) {
    // target is function directly. e.g. from direct call on function value:
    //   (fun(x int) { ... })(123)
    fn = ast_add_fun(u, recv);
  } else if (recv->kind == NId && recv->id.target && recv->id.target->kind == NFun) {
    // common case of function referenced by name. e.g.
    //   fun foo(x int) { ... }
    //   foo(123)
    fn = ast_add_fun(u, recv->id.target);
  } else {
    // function is a value
    IRValue* fnval = ast_add_expr(u, recv);
    asserteq(fnval->op, OpFun);
    fn = (IRFun*)fnval->auxInt;
  }

  Node* argstuple = n->call.args;
  auto v = IRValueAlloc(u->mem, OpCall, fn->type, n->pos); // preallocate
  v->auxInt = (i64)fn->name;
  if (argstuple) {
    for (u32 i = 0; i < argstuple->array.a.len; i++) {
      Node* argnode = (Node*)argstuple->array.a.v[i];
      IRValue* arg = ast_add_expr(u, argnode);
      IRValueAddArg(v, u->mem, arg);
    }
  }
  IRBlockAddValue(u->b, v);

  u->f->ncalls++;
  u->f->npurecalls += ((u32)IRFunIsPure(fn));

  // TODO: if the function was not directly named, add a recognizable name as a comment
  // if ((u->flags & IRBuilderComments) && fn->name)
  //   IRValueAddComment(v, u->mem, fn->name, symlen(fn->name));

  return v;
}


static IRValue* ast_add_block(IRBuilder* u, Node* n) {  // language block, not IR block.
  assert(n->kind == NBlock);
  IRValue* v = NULL;
  for (u32 i = 0; i < n->array.a.len; i++)
    v = ast_add_expr(u, (Node*)n->array.a.v[i]);
  return v;
}


static IRValue* ast_add_ret(IRBuilder* u, Node* n) { //
  assert(n->kind == NReturn);
  auto retval = ast_add_expr(u, n->op.left);

  // set current block as "ret"
  u->b->kind = IRBlockRet;
  IRBlockSetControl(u->b, retval);

  // Note: ast_add_fun sets up the function block as ret unconditionally
  // for the value of the block, which is the last expression.
  // Because of this we return retval here to make sure the effect is unchanged.
  return retval;
}


static IRValue* ast_add_funexpr(IRBuilder* u, Node* n) {
  IRFun* fn = ast_add_fun(u, n);
  auto v = IRValueNew(u->f, u->b, OpFun, fn->type, n->pos);
  v->auxInt = (i64)fn;
  return v;
}


static IRValue* ast_add_expr(IRBuilder* u, Node* n) {
  // AST should be fully typed
  assertf_debug(NodeIsType(n) || n->type != NULL, "n = %s %s", NodeKindName(n->kind), fmtnode(n));

  if (R_UNLIKELY(n->type == Type_ideal)) {
    // This means the expression is unused. It does not necessarily mean its value is unused,
    // so it would not be accurate to issue diagnostic warnings at this point.
    // For example:
    //
    //   fun foo {
    //     x = 1    # <- the NVar node is unused but its value (NIntLit 3) ...
    //     bar(x)   # ... is used by this NCall node.
    //   }
    //
    dlog("skip unused %s", fmtnode(n));
    return NULL;
  }

  switch (n->kind) {
    case NVar:      return ast_add_var(u, n);
    case NBlock:    return ast_add_block(u, n);
    case NIntLit:   return ast_add_intconst(u, n);
    case NBoolLit:  return ast_add_boolconst(u, n);
    case NBinOp:    return ast_add_binop(u, n);
    case NId:       return ast_add_id(u, n);
    case NIf:       return ast_add_if(u, n);
    case NTypeCast: return ast_add_typecast(u, n);
    case NCall:     return ast_add_call(u, n);
    case NReturn:   return ast_add_ret(u, n);
    case NFun:      return ast_add_funexpr(u, n);
    case NArray:    return ast_add_array(u, n);
    case NIndex:    return ast_add_index(u, n);

    case NFloatLit:
    case NStrLit:
    case NNil:
    case NAssign:
    case NRef:
    case NField:
    case NPrefixOp:
    case NPostfixOp:
    case NTuple:
    case NSelector:
    case NNamedVal:
    case NSlice:
    case NMacro:
    case NBasicType:
    case NRefType:
    case NTupleType:
    case NArrayType:
    case NStructType:
    case NFunType:
    case NTypeType:
      panic("TODO ast_add_expr kind %s", NodeKindName(n->kind));
      break;

    case NFile:
    case NPkg:
    case NNone:
    case NBad:
    case _NodeKindMax:
      build_errf(u->build, NodePosSpan(n), "invalid AST node %s", NodeKindName(n->kind));
      break;
  }
  return TODO_Value(u);
}


static IRFun* ast_add_fun(IRBuilder* u, Node* n) {
  assert(n->kind == NFun);
  assertnotnull(n->fun.body); // must have a body (not be just a declaration)
  assertnotnull(n->fun.name); // functions must be named

  // const char* name;
  IRFun* f = IRPkgGetFun(u->pkg, n->fun.name);
  if (f) {
    // fun already built or in progress of being built
    return f;
  }

  dlog("ast_add_fun %s", fmtnode(n));

  // allocate a new function and its entry block
  assert(n->type != NULL);
  assert(n->type->kind == NFunType);
  auto params = n->fun.params;
  u32 nparams = 0;
  if (params)
    nparams = params->kind == NTuple ? params->array.a.len : 1;
  f = IRFunNew(u->mem, n->type->t.id, n->fun.name, n->pos, nparams);
  auto entryb = IRBlockNew(f, IRBlockCont, n->pos);

  // Since functions can be self-referential, add the function before we generate its body
  IRPkgAddFun(u->pkg, f);

  // start function
  startFun(u, f);
  startSealedBlock(u, entryb); // entry block has no predecessors, so seal right away.

  // build body
  auto bodyval = ast_add_expr(u, n->fun.body);

  // end last block, if not already ended
  if (u->b) {
    u->b->kind = IRBlockRet;
    if (n->type->t.fun.result != Type_nil)
      IRBlockSetControl(u->b, bodyval);
    endBlock(u);
  }

  // end function and return
  endFun(u);
  return f;
}


static bool ast_add_file(IRBuilder* u, Node* n) { // n->kind==NFile
  #if DEBUG
  auto src = build_get_source(u->build, n->pos);
  dlog("ast_add_file %s", src ? src->filename : "(unknown)");
  #endif
  for (u32 i = 0; i < n->cunit.a.len; i++) {
    if (!ast_add_toplevel(u, (Node*)n->cunit.a.v[i]))
      return false;
  }
  return true;
}


static bool ast_add_pkg(IRBuilder* u, Node* n) { // n->kind==NPkg
  for (u32 i = 0; i < n->cunit.a.len; i++) {
    if (!ast_add_file(u, (Node*)n->cunit.a.v[i]))
      return false;
  }
  return true;
}


static bool ast_add_toplevel(IRBuilder* u, Node* n) {
  switch (n->kind) {
    case NPkg:  return ast_add_pkg(u, n);
    case NFile: return ast_add_file(u, n);
    case NFun:  return ast_add_fun(u, n) != NULL;

    case NVar:
      // top-level var bindings which are not exported can be ignored.
      // All var bindings are resolved already, so they only concern IR if their data is exported.
      // Since exporting is not implemented, just ignore top-level var for now.
      return true;

    default:
      build_errf(u->build, NodePosSpan(n), "invalid top-level AST node %s", NodeKindName(n->kind));
      break;
  }
  return false;
}


bool IRBuilderAddAST(IRBuilder* u, Node* n) {
  return ast_add_toplevel(u, n);
}


ASSUME_NONNULL_END
