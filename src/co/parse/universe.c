#include "../common.h"
#include "parse.h"
#include "universe.h"

// generated by gen_constants at the end of this file.
// TODO: Move this generator to a script.
//       Currently it is really hard to use when e.g. changing types (TypeCode_*).
// Define to run the generator during program initialization:
//
// #define RUN_GENERATOR


static const Node _Type_type = {.kind=NTypeType};
Node* Type_type = (Node*)&_Type_type;


//-- BEGIN gen_constants()

const Sym sym_as = &"\x87\x3D\x7F\xCD\x02\x00\x00\x08""as\0"[8];
const Sym sym_auto = &"\xAD\xF8\x95\xF5\x04\x00\x00\x10""auto\0"[8];
const Sym sym_break = &"\xFF\x55\x83\xD2\x05\x00\x00\x18""break\0"[8];
const Sym sym_continue = &"\xB4\xFF\xAB\xF1\x08\x00\x00\x20""continue\0"[8];
const Sym sym_defer = &"\xAD\x43\x24\x74\x05\x00\x00\x28""defer\0"[8];
const Sym sym_else = &"\xAD\x2A\x2A\x55\x04\x00\x00\x30""else\0"[8];
const Sym sym_enum = &"\x00\xE0\xE4\xCD\x04\x00\x00\x38""enum\0"[8];
const Sym sym_for = &"\x2E\x77\xA4\x49\x03\x00\x00\x40""for\0"[8];
const Sym sym_fun = &"\x1B\xD7\x8A\x8A\x03\x00\x00\x48""fun\0"[8];
const Sym sym_if = &"\xF5\x8D\x92\xCF\x02\x00\x00\x50""if\0"[8];
const Sym sym_import = &"\xF8\xA4\x4A\xF8\x06\x00\x00\x58""import\0"[8];
const Sym sym_in = &"\x74\x9A\xDF\xDD\x02\x00\x00\x60""in\0"[8];
const Sym sym_nil = &"\x8F\x7E\x4F\xEF\x03\x00\x00\x68""nil\0"[8];
const Sym sym_return = &"\xEB\xA6\x08\xA3\x06\x00\x00\x70""return\0"[8];
const Sym sym_struct = &"\x97\xFC\x80\x50\x06\x00\x00\x78""struct\0"[8];
const Sym sym_switch = &"\x37\xE0\x68\x4B\x06\x00\x00\x80""switch\0"[8];
const Sym sym_type = &"\x52\x1E\xB2\xD6\x04\x00\x00\x88""type\0"[8];
const Sym sym_var = &"\x27\xBE\x0A\xFD\x03\x00\x00\x90""var\0"[8];
const Sym sym_const = &"\x0A\x54\xDC\xAD\x05\x00\x00\x98""const\0"[8];

const Sym sym_bool = &"\x70\x6D\x7D\x3D\x04\x00\x00\x00""bool\0"[8];
const Sym sym_i8 = &"\x9D\xE2\x63\xDB\x02\x00\x00\x00""i8\0"[8];
const Sym sym_u8 = &"\xEF\xCA\xBF\x76\x02\x00\x00\x00""u8\0"[8];
const Sym sym_i16 = &"\xC2\xF3\x4A\x52\x03\x00\x00\x00""i16\0"[8];
const Sym sym_u16 = &"\xF8\x6B\xDE\x1D\x03\x00\x00\x00""u16\0"[8];
const Sym sym_i32 = &"\x4A\x53\xEC\xE0\x03\x00\x00\x00""i32\0"[8];
const Sym sym_u32 = &"\x61\x13\x38\xD1\x03\x00\x00\x00""u32\0"[8];
const Sym sym_i64 = &"\xA1\x93\xF3\x6C\x03\x00\x00\x00""i64\0"[8];
const Sym sym_u64 = &"\xBA\x1C\x93\x2D\x03\x00\x00\x00""u64\0"[8];
const Sym sym_f32 = &"\x9E\x9C\x5E\xFD\x03\x00\x00\x00""f32\0"[8];
const Sym sym_f64 = &"\x90\x7B\xEB\x9C\x03\x00\x00\x00""f64\0"[8];
const Sym sym_int = &"\xCD\x9E\x65\xA5\x03\x00\x00\x00""int\0"[8];
const Sym sym_uint = &"\xFA\x0A\xDC\xFB\x04\x00\x00\x00""uint\0"[8];
const Sym sym_isize = &"\x8E\x0D\x8E\xB3\x05\x00\x00\x00""isize\0"[8];
const Sym sym_usize = &"\xBD\x68\xE2\x7D\x05\x00\x00\x00""usize\0"[8];
const Sym sym_str = &"\x12\xAE\x4D\xED\x03\x00\x00\x00""str\0"[8];
const Sym sym_ideal = &"\x5B\xB4\x1C\xC8\x05\x00\x00\x00""ideal\0"[8];
const Sym sym__ = &"\xE7\x52\x89\xD0\x01\x00\x00\x00""_\0"[8];
const Sym sym_true = &"\xA2\x11\x64\xDB\x04\x00\x00\x00""true\0"[8];
const Sym sym_false = &"\xAC\x1C\xED\xD5\x05\x00\x00\x00""false\0"[8];
const Sym sym_b = &"\xBA\x97\x7C\xF4\x01\x00\x00\x00""b\0"[8];
const Sym sym_1 = &"\xC4\xD0\xFC\x8C\x01\x00\x00\x00""1\0"[8];
const Sym sym_2 = &"\xBA\xB3\xD5\x74\x01\x00\x00\x00""2\0"[8];
const Sym sym_3 = &"\x3B\x29\x53\x12\x01\x00\x00\x00""3\0"[8];
const Sym sym_4 = &"\x22\xEF\xF2\x1D\x01\x00\x00\x00""4\0"[8];
const Sym sym_5 = &"\x5C\xCE\x89\xEE\x01\x00\x00\x00""5\0"[8];
const Sym sym_6 = &"\xBE\x2F\xCC\x8D\x01\x00\x00\x00""6\0"[8];
const Sym sym_7 = &"\x3F\xD4\xD9\xA7\x01\x00\x00\x00""7\0"[8];
const Sym sym_8 = &"\x37\x74\x69\x1E\x01\x00\x00\x00""8\0"[8];
const Sym sym_f = &"\xE9\x46\xFD\x9C\x01\x00\x00\x00""f\0"[8];
const Sym sym_F = &"\x5C\x05\x48\xBA\x01\x00\x00\x00""F\0"[8];
const Sym sym_i = &"\x3A\x16\x35\x80\x01\x00\x00\x00""i\0"[8];
const Sym sym_u = &"\x32\x6A\x3E\x41\x01\x00\x00\x00""u\0"[8];
const Sym sym_I = &"\xDA\x70\xE1\x33\x01\x00\x00\x00""I\0"[8];
const Sym sym_U = &"\x4D\x07\x4A\xEB\x01\x00\x00\x00""U\0"[8];
const Sym sym_s = &"\xC0\xB7\x96\x15\x01\x00\x00\x00""s\0"[8];
const Sym sym_a = &"\x60\xE5\x1B\x67\x01\x00\x00\x00""a\0"[8];
const Sym sym_$x2a = &"\x9C\x1C\x9F\x3C\x01\x00\x00\x00""*\0"[8];
const Sym sym_0 = &"\xFE\xED\xFA\xA1\x01\x00\x00\x00""0\0"[8];

static const Node _Type_bool = { NBasicType,NodeFlagsNone,0,0,(Node*)&_Type_type,NULL,
  {.t={sym_b,TypeKindInteger,.basic={TypeCode_bool,sym_bool}}} };
static const Node _Type_i8 = { NBasicType,NodeFlagsNone,0,0,(Node*)&_Type_type,NULL,
  {.t={sym_1,TypeKindInteger,.basic={TypeCode_i8,sym_i8}}} };
static const Node _Type_u8 = { NBasicType,NodeFlagsNone,0,0,(Node*)&_Type_type,NULL,
  {.t={sym_2,TypeKindInteger,.basic={TypeCode_u8,sym_u8}}} };
static const Node _Type_i16 = { NBasicType,NodeFlagsNone,0,0,(Node*)&_Type_type,NULL,
  {.t={sym_3,TypeKindInteger,.basic={TypeCode_i16,sym_i16}}} };
static const Node _Type_u16 = { NBasicType,NodeFlagsNone,0,0,(Node*)&_Type_type,NULL,
  {.t={sym_4,TypeKindInteger,.basic={TypeCode_u16,sym_u16}}} };
static const Node _Type_i32 = { NBasicType,NodeFlagsNone,0,0,(Node*)&_Type_type,NULL,
  {.t={sym_5,TypeKindInteger,.basic={TypeCode_i32,sym_i32}}} };
static const Node _Type_u32 = { NBasicType,NodeFlagsNone,0,0,(Node*)&_Type_type,NULL,
  {.t={sym_6,TypeKindInteger,.basic={TypeCode_u32,sym_u32}}} };
static const Node _Type_i64 = { NBasicType,NodeFlagsNone,0,0,(Node*)&_Type_type,NULL,
  {.t={sym_7,TypeKindInteger,.basic={TypeCode_i64,sym_i64}}} };
static const Node _Type_u64 = { NBasicType,NodeFlagsNone,0,0,(Node*)&_Type_type,NULL,
  {.t={sym_8,TypeKindInteger,.basic={TypeCode_u64,sym_u64}}} };
static const Node _Type_f32 = { NBasicType,NodeFlagsNone,0,0,(Node*)&_Type_type,NULL,
  {.t={sym_f,TypeKindF32,.basic={TypeCode_f32,sym_f32}}} };
static const Node _Type_f64 = { NBasicType,NodeFlagsNone,0,0,(Node*)&_Type_type,NULL,
  {.t={sym_F,TypeKindF64,.basic={TypeCode_f64,sym_f64}}} };
static const Node _Type_int = { NBasicType,NodeFlagsNone,0,0,(Node*)&_Type_type,NULL,
  {.t={sym_i,TypeKindInteger,.basic={TypeCode_int,sym_int}}} };
static const Node _Type_uint = { NBasicType,NodeFlagsNone,0,0,(Node*)&_Type_type,NULL,
  {.t={sym_u,TypeKindInteger,.basic={TypeCode_uint,sym_uint}}} };
static const Node _Type_isize = { NBasicType,NodeFlagsNone,0,0,(Node*)&_Type_type,NULL,
  {.t={sym_I,TypeKindInteger,.basic={TypeCode_isize,sym_isize}}} };
static const Node _Type_usize = { NBasicType,NodeFlagsNone,0,0,(Node*)&_Type_type,NULL,
  {.t={sym_U,TypeKindInteger,.basic={TypeCode_usize,sym_usize}}} };
static const Node _Type_str = { NBasicType,NodeFlagsNone,0,0,(Node*)&_Type_type,NULL,
  {.t={sym_s,TypeKindPointer,.basic={TypeCode_str,sym_str}}} };
static const Node _Type_auto = { NBasicType,NodeFlagsNone,0,0,(Node*)&_Type_type,NULL,
  {.t={sym_a,TypeKindVoid,.basic={TypeCode_auto,sym_auto}}} };
static const Node _Type_ideal = { NBasicType,NodeFlagsNone,0,0,(Node*)&_Type_type,NULL,
  {.t={sym_$x2a,TypeKindVoid,.basic={TypeCode_ideal,sym_ideal}}} };
static const Node _Type_nil = { NBasicType,NodeFlagsNone,0,0,(Node*)&_Type_type,NULL,
  {.t={sym_0,TypeKindVoid,.basic={TypeCode_nil,sym_nil}}} };

static const Node _Const_true =
  {NBoolLit,NodeFlagsNone,0,0,(Node*)&_Type_bool,NULL,{.val={CType_bool,.i=1}}};
static const Node _Const_false =
  {NBoolLit,NodeFlagsNone,0,0,(Node*)&_Type_bool,NULL,{.val={CType_bool,.i=0}}};
static const Node _Const_nil =
  {NNil,NodeFlagsNone,0,0,(Node*)&_Type_nil,NULL,{.val={CType_nil,.i=0}}};

Node* Type_bool = (Node*)&_Type_bool;
Node* Type_i8 = (Node*)&_Type_i8;
Node* Type_u8 = (Node*)&_Type_u8;
Node* Type_i16 = (Node*)&_Type_i16;
Node* Type_u16 = (Node*)&_Type_u16;
Node* Type_i32 = (Node*)&_Type_i32;
Node* Type_u32 = (Node*)&_Type_u32;
Node* Type_i64 = (Node*)&_Type_i64;
Node* Type_u64 = (Node*)&_Type_u64;
Node* Type_f32 = (Node*)&_Type_f32;
Node* Type_f64 = (Node*)&_Type_f64;
Node* Type_int = (Node*)&_Type_int;
Node* Type_uint = (Node*)&_Type_uint;
Node* Type_isize = (Node*)&_Type_isize;
Node* Type_usize = (Node*)&_Type_usize;
Node* Type_str = (Node*)&_Type_str;
Node* Type_auto = (Node*)&_Type_auto;
Node* Type_ideal = (Node*)&_Type_ideal;
Node* Type_nil = (Node*)&_Type_nil;
Node* Const_true = (Node*)&_Const_true;
Node* Const_false = (Node*)&_Const_false;
Node* Const_nil = (Node*)&_Const_nil;

static SymRBNode n_u16 = { sym_u16, true, NULL, NULL };
static SymRBNode n_bool = { sym_bool, true, NULL, NULL };
static SymRBNode n_u64 = { sym_u64, false, &n_u16, &n_bool };
static SymRBNode n_switch = { sym_switch, false, NULL, NULL };
static SymRBNode n_for = { sym_for, true, &n_u64, &n_switch };
static SymRBNode n_i16 = { sym_i16, true, NULL, NULL };
static SymRBNode n_i64 = { sym_i64, true, NULL, NULL };
static SymRBNode n_else = { sym_else, false, &n_i16, &n_i64 };
static SymRBNode n_struct = { sym_struct, false, &n_for, &n_else };
static SymRBNode n_u8 = { sym_u8, true, NULL, NULL };
static SymRBNode n_usize = { sym_usize, false, &n_u8, NULL };
static SymRBNode n_f64 = { sym_f64, false, NULL, NULL };
static SymRBNode n_fun = { sym_fun, true, &n_usize, &n_f64 };
static SymRBNode n_int = { sym_int, true, NULL, NULL };
static SymRBNode n_isize = { sym_isize, true, NULL, NULL };
static SymRBNode n_const = { sym_const, false, &n_int, &n_isize };
static SymRBNode n_return = { sym_return, false, &n_fun, &n_const };
static SymRBNode n_defer = { sym_defer, false, &n_struct, &n_return };
static SymRBNode n_enum = { sym_enum, false, NULL, NULL };
static SymRBNode n__ = { sym__, true, NULL, NULL };
static SymRBNode n_u32 = { sym_u32, false, &n__, NULL };
static SymRBNode n_if = { sym_if, true, &n_enum, &n_u32 };
static SymRBNode n_false = { sym_false, true, NULL, NULL };
static SymRBNode n_type = { sym_type, false, &n_false, NULL };
static SymRBNode n_break = { sym_break, false, &n_if, &n_type };
static SymRBNode n_true = { sym_true, true, NULL, NULL };
static SymRBNode n_in = { sym_in, false, &n_true, NULL };
static SymRBNode n_str = { sym_str, false, NULL, NULL };
static SymRBNode n_i32 = { sym_i32, false, &n_in, &n_str };
static SymRBNode n_i8 = { sym_i8, true, &n_break, &n_i32 };
static SymRBNode n_continue = { sym_continue, false, NULL, NULL };
static SymRBNode n_import = { sym_import, true, NULL, NULL };
static SymRBNode n_uint = { sym_uint, false, &n_import, NULL };
static SymRBNode n_auto = { sym_auto, true, &n_continue, &n_uint };
static SymRBNode n_f32 = { sym_f32, false, NULL, NULL };
static SymRBNode n_var = { sym_var, false, &n_auto, &n_f32 };
static SymRBNode n_nil = { sym_nil, false, &n_i8, &n_var };
static SymRBNode n_as = { sym_as, false, &n_defer, &n_nil };

static SymRBNode* _symroot = &n_as;

Node* const _TypeCodeToTypeNodeMap[TypeCode_CONCRETE_END] = {
  (Node*)&_Type_bool,    // TypeCode_bool
  (Node*)&_Type_i8,      // TypeCode_i8
  (Node*)&_Type_u8,      // TypeCode_u8
  (Node*)&_Type_i16,     // TypeCode_i16
  (Node*)&_Type_u16,     // TypeCode_u16
  (Node*)&_Type_i32,     // TypeCode_i32
  (Node*)&_Type_u32,     // TypeCode_u32
  (Node*)&_Type_i64,     // TypeCode_i64
  (Node*)&_Type_u64,     // TypeCode_u64
  (Node*)&_Type_f32,     // TypeCode_f32
  (Node*)&_Type_f64,     // TypeCode_f64
  (Node*)&_Type_int,     // TypeCode_int
  (Node*)&_Type_uint,    // TypeCode_uint
  (Node*)&_Type_isize,   // TypeCode_isize
  (Node*)&_Type_usize,   // TypeCode_usize
  NULL,                  // TypeCode_NUM_END
  (Node*)&_Type_str,     // TypeCode_str
  (Node*)&_Type_nil,     // TypeCode_nil
  (Node*)&_Type_auto,    // TypeCode_auto
};
#ifndef NDEBUG
__attribute__((used)) static const char* const debugSymCheck =
  "as#101 auto#102 break#103 continue#104 defer#105 else#106 enum#107 "
  "for#108 fun#109 if#10a import#10b in#10c nil#10d return#10e struct#10f "
  "switch#110 type#111 var#112 const#113 bool i8 u8 i16 u16 i32 u32 i64 "
  "u64 f32 f64 int uint isize usize str auto ideal nil true:bool=1 false:bool=0 "
  "nil:nil=0 _ ";
#endif

//-- END gen_constants()



const SymPool* universe_syms() {
  static SymPool p = {};
  if (p.root == NULL) {
    //init_type_nodes();
    sympool_init(&p, NULL, MemInvalid(), _symroot);
  }
  return &p;
}


// ---------------------------------------------------------------------------------------
// test

// R_TEST(universe) {
//   auto s = sympool_repr(universe_syms(), str_new(0));
//   dlog("%s", s);
//   str_free(s);
// }


// ---------------------------------------------------------------------------------------
// sym constant data generator

#if defined(RUN_GENERATOR)

// red-black tree implementation used for interning
// RBKEY must match that in sym.c
#define RBKEY      Sym
#define RBUSERDATA Mem _Nonnull
#include "../util/rbtree.c.h"

static RBNode* RBAllocNode(Mem mem) {
  return (RBNode*)memalloct(mem, RBNode);
}

static void RBFreeNode(RBNode* node, Mem mem) {
}

static int RBCmp(Sym a, Sym b, Mem mem) {
  if (symhash(a) < symhash(b))
    return -1;
  if (symhash(a) > symhash(b))
    return 1;
  int cmp = (int)symlen(a) - (int)symlen(b);
  if (cmp == 0) {
    // hash is identical and length is identical; compare bytes
    cmp = memcmp(a, b, symlen(a));
  }
  return cmp;
}



inline static Str fmt_nodes(const RBNode* n, Str s) {
  // descent first
  if (n->left) {
    s = fmt_nodes(n->left, s);
  }
  if (n->right) {
    s = fmt_nodes(n->right, s);
  }

  s = str_appendcstr(s, "static SymRBNode n_");
  s = str_append(s, n->key, symlen(n->key));
  s = str_appendcstr(s, " = { ");

  // { key, isred, left, right }

  s = str_appendfmt(s, "sym_%s, ", n->key);
  // s = str_appendcstr(s, "NULL, ");

  s = str_appendcstr(s, n->isred ? "true, " : "false, ");
  if (n->left) {
    s = str_appendfmt(s, "&n_%s, ", n->left->key);
  } else {
    s = str_appendcstr(s, "NULL, ");
  }
  if (n->right) {
    s = str_appendfmt(s, "&n_%s ", n->right->key);
  } else {
    s = str_appendcstr(s, "NULL ");
  }
  s = str_appendcstr(s, "};\n");
  return s;
}

// static ConstStr fmtkey(Sym k) { return (ConstStr)k; } // Sym is a type of Str :-)

// is_cident_nth returns true if c is a valid character in a C identifier as the
// 2nd or later character.
static inline bool is_cident_nth(char c) {
  return (
    ('0' <= c && c <= '9') ||
    ('A' <= c && c <= 'Z') ||
    ('a' <= c && c <= 'z') ||
    c == '_' ||
    c == '$'
  );
}

static const char* typecode_cident(char c) {
  static char buf[16];
  buf[0] = c;
  buf[1] = 0;
  if (c == '$' || !is_cident_nth(c))
    snprintf(buf, sizeof(buf), "$x%02x", c);
  return buf;
}

static bool gen_append_symdef(Str* sp, RBNode** rp, Sym sym, const char* name) {
  assert(symlen(sym) < 1000);
  bool added;
  *rp = RBInsert(*rp, sym, &added, MemHeap);
  if (!added)
    return false;

  auto s = *sp;
  s = str_setlen(s, 0); /* reset */
  s = str_appendfmt(s, "const Sym sym_%s = &\"", name);

  auto h = _SYM_HEADER(sym);
  auto hash = (const u8*)&h->hash;
  auto len  = (const u8*)&h->len;

  // hash, len, cap
  s = str_appendfmt(s, "\\x%02X\\x%02X\\x%02X\\x%02X", hash[0], hash[1], hash[2], hash[3]);
  s = str_appendfmt(s, "\\x%02X\\x%02X\\x%02X\\x%02X", len[0], len[1], len[2], len[3]);

  s = str_appendcstr(s, "\"\""); // in case sym starts with a hex digit
  s = str_appendrepr(s, sym, symlen(sym));
  s = str_appendfmt(s, "\\0\"[%d];\n", (int)sizeof(SymHeader));
  *sp = s;
  return true;
}

static bool gen_append_symdef_lit(Str* sp, RBNode** rp, Sym sym, const char* name, u8 flags) {
  sym_dangerously_set_flags(sym, flags);
  return gen_append_symdef(sp, rp, sym, name);
}

static bool gen_append_symdef_typecode_lit(Str* sp, RBNode** rp, SymPool* syms, char typecode_ch) {
  char buf[2];
  buf[0] = typecode_ch;
  buf[1] = 0;
  auto sym = symadd(syms, buf, 1);
  return gen_append_symdef(sp, rp, sym, typecode_cident(typecode_ch));
}

__attribute__((constructor,used)) static void gen_constants() {
  printf("\n//-- BEGIN gen_constants()\n\n");

  SymPool syms;
  sympool_init(&syms, NULL, MemHeap, NULL);

  // // define temporary runtime symbols
  // #define SYM_DEF1(name)              symaddcstr(&syms, #name);
  // #define SYM_DEF2(str, tok)          symaddcstr(&syms, #str);
  // #define SYM_DEF1_IGN2(name, _t, _v) symaddcstr(&syms, #name);
  // TOKEN_KEYWORDS(SYM_DEF2)
  // TYPE_SYMS(SYM_DEF1)
  // TYPE_SYMS_PRIVATE(SYM_DEF1)
  // PREDEFINED_CONSTANTS(SYM_DEF1_IGN2)
  // PREDEFINED_IDENTS(SYM_DEF1)
  // #undef SYM_DEF1
  // #undef SYM_DEF2
  // #undef SYM_DEF1_IGN2


  // ---------------------------------------------------------------------------------------------
  // generate symbol constants
  Str tmpstr = str_new(512);
  RBNode* root = NULL;
  bool added;

  #define SYM_GEN_NOFLAGS(name, ...)                                        \
    if (gen_append_symdef_lit(&tmpstr, &root, symgetcstr(&syms, #name), #name, 0)) \
      printf("%s", tmpstr);

  #define SYM_GEN_TYPECODE(name, ...)                                                     \
    if (gen_append_symdef_typecode_lit(&tmpstr, &root, &syms, TypeCodeEncoding(TypeCode_##name))) \
      printf("%s", tmpstr);

  #define SYM_GEN_KEYWORD(name, tok, ...) {                          \
    Sym sym = symgetcstr(&syms, #name);                              \
    u8 flags = (tok - TKeywordsStart);                               \
    if (!gen_append_symdef_lit(&tmpstr, &root, sym, #name, flags)) { \
      errlog("duplicate keyword symbol definition: %s", sym);        \
      exit(1);                                                       \
    }                                                                \
    printf("%s", tmpstr);                                            \
  }

  // keyword symbols must be generated first as they use custom Sym flags
  TOKEN_KEYWORDS(SYM_GEN_KEYWORD)
  printf("\n");

  TYPE_SYMS(SYM_GEN_NOFLAGS)
  TYPE_SYMS_PRIVATE(SYM_GEN_NOFLAGS)
  PREDEFINED_IDENTS(SYM_GEN_NOFLAGS)
  PREDEFINED_CONSTANTS(SYM_GEN_NOFLAGS)

  TYPE_SYMS(SYM_GEN_TYPECODE)
  TYPE_SYMS_PRIVATE(SYM_GEN_TYPECODE)


  #undef SYM_GEN_NOFLAGS
  #undef SYM_GEN_TYPECODE
  #undef SYM_GEN_KEYWORD
  str_free(tmpstr);

  RBClear(root, MemHeap);


  // ---------------------------------------------------------------------------------------------
  // generate AST nodes

  // TYPE_SYMS
  // TODO: precompute t.id Sym
  // const Node* type_NAME
  printf("\n");
  #define NODE_GEN(name, typeKind)                                                       \
    printf(                                                                              \
      "static const Node _Type_%s ="                                                     \
      " { NBasicType,NodeFlagsNone,0,0,(Node*)&_Type_type,NULL,\n"                       \
      "  {.t={sym_%s,%s,.basic={TypeCode_%s,sym_%s}}} };\n",                             \
      #name, typecode_cident(TypeCodeEncoding(TypeCode_##name)), #typeKind, #name, #name \
    );
  TYPE_SYMS(NODE_GEN)
  TYPE_SYMS_PRIVATE(NODE_GEN)
  #undef NODE_GEN


  // PREDEFINED_CONSTANTS
  printf("\n");
  #define NODE_GEN(name, type, value)                                \
    printf(                                                          \
      "static const Node _Const_%s =\n"                              \
      "  {%s,NodeFlagsNone,0,0,(Node*)&_Type_%s,NULL,{.val=%s}};\n", \
      #name,                                                         \
      strcmp(#type, "bool") == 0 ? "NBoolLit" :                      \
      strcmp(#type, "nil") == 0 ?  "NNil" :                          \
                                   "NIntLit",                        \
      #type,                                                         \
      /*NVal*/ "{CType_" #type ",.i=" #value "}"                     \
    );
  PREDEFINED_CONSTANTS(NODE_GEN)
  #undef NODE_GEN


  // Generate named pointers
  printf("\n");
  #define NODE_GEN(name, ...) printf("Node* Type_%s = (Node*)&_Type_%s;\n", #name, #name);
  TYPE_SYMS(NODE_GEN)
  TYPE_SYMS_PRIVATE(NODE_GEN)
  #undef NODE_GEN
  #define NODE_GEN(name, ...) printf("Node* Const_%s = (Node*)&_Const_%s;\n", #name, #name);
  PREDEFINED_CONSTANTS(NODE_GEN)
  #undef NODE_GEN


  // // generate function "init_type_nodes" to init type nodes, e.g.
  // //   _Type_i32.type = Type_i32;
  // printf("\n");
  // printf("inline static void init_type_nodes() {\n");
  // #define NODE_GEN(name, ...) \
  //   printf("  _Type_%s.type = Type_%s;\n", #name, #name);
  // TYPE_SYMS(NODE_GEN)
  // TYPE_SYMS_PRIVATE(NODE_GEN)
  // #undef NODE_GEN
  // printf("}\n");


  // ---------------------------------------------------------------------------------------------
  // generate SymPool tree


  root = NULL;

  #define RB_GEN(symname, ...) root = RBInsert(root, symgetcstr(&syms, #symname), &added, MemHeap);
  TOKEN_KEYWORDS(RB_GEN)
  TYPE_SYMS(RB_GEN)
  // Note: TYPE_SYMS_PRIVATE are not exported in the global namespace
  PREDEFINED_CONSTANTS(RB_GEN)
  PREDEFINED_IDENTS(RB_GEN)
  #undef RB_GEN

  printf("\n%s\n", fmt_nodes(root, str_new(0)));

  printf("static SymRBNode* _symroot = &n_%s;\n", root->key);


  // ---------------------------------------------------------------------------------------------
  // generate _TypeCodeToTypeNodeMap

  printf("\n");
  printf("Node* const _TypeCodeToTypeNodeMap[TypeCode_CONCRETE_END] = {\n");

  size_t maxnamelen = 0;
  #define I_ENUM(name, _encoding, _flags) \
    if (TypeCode_##name < TypeCode_CONCRETE_END) \
      maxnamelen = MAX(strlen(#name), maxnamelen);
  TYPE_CODES(I_ENUM)
  #undef I_ENUM

  #define I_ENUM(name, _encoding, _flags) { \
    TypeCode tc = TypeCode_##name; \
    if (tc < TypeCode_CONCRETE_END) { \
      if (tc == TypeCode_NUM_END) { \
        printf("  NULL,                 "); \
      } else { \
        printf("  (Node*)&_Type_" #name ","); \
      } \
      printf("%-*s // TypeCode_" #name "\n", (int)(maxnamelen - strlen(#name)), ""); \
    } \
  }
  TYPE_CODES(I_ENUM)
  #undef I_ENUM
  printf("};\n");


  // ---------------------------------------------------------------------------------------------
  // generate a sort of checksum used in debug mode to make sure the generator is updated
  // when keywords change. See the function debug_check() below as well.
  int col = 0;
  printf(
    "#ifndef NDEBUG\n"
    "__attribute__((used)) static const char* const debugSymCheck =\n"
    "  \"");
  #define KW(str, tok)             \
    printf("%s#%03x ", #str, tok); \
    col += strlen(#str) + 6;       \
    if (col > 70) { col = 0; printf("\"\n  \""); }
  #define CONST(name, type, val)               \
    printf("%s:%s=%s ", #name, #type, #val);   \
    col += strlen(#name #type #val) + 3;      \
    if (col > 70) { col = 0; printf("\"\n  \""); }
  #define NAME(name, ...)          \
    printf("%s ", #name);          \
    col += strlen(#name) + 1;      \
    if (col > 70) { col = 0; printf("\"\n  \""); }
  TOKEN_KEYWORDS(KW)
  TYPE_SYMS(NAME)
  TYPE_SYMS_PRIVATE(NAME)
  PREDEFINED_CONSTANTS(CONST)
  PREDEFINED_IDENTS(NAME)
  #undef KW
  #undef CONST
  #undef NAME
  printf("\";\n#endif\n");


  // ---------------------------------------------------------------------------------------------

  printf("\n//-- END gen_constants()\n\n");
  sympool_dispose(&syms);
  exit(1);
}

#endif /* defined(RUN_GENERATOR) */


// -----------------------------------------------------------------------------------------------


#if !defined(NDEBUG) && !defined(RUN_GENERATOR)
__attribute__((constructor)) static void debug_check() {
  auto s = str_new(0);

  #define KW(str, tok)           s = str_appendfmt(s, "%s#%03x ", #str, tok);
  #define CONST(name, type, val) s = str_appendfmt(s, "%s:%s=%s ", #name, #type, #val);
  #define NAME(name, ...)        s = str_appendcstr(s, #name " ");
  TOKEN_KEYWORDS(KW)
  TYPE_SYMS(NAME)
  TYPE_SYMS_PRIVATE(NAME)
  PREDEFINED_CONSTANTS(CONST)
  PREDEFINED_IDENTS(NAME)
  #undef KW
  #undef CONST
  #undef NAME

  if (strcmp(debugSymCheck, s) != 0) {
    printf("Keywords changed.\n");
    printf("Define RUN_GENERATOR in %s to run code generator.\n", __FILE__);
    printf("\ndebugSymCheck:\n%s\n\ndetected:\n%s\n", debugSymCheck, s);
    exit(1);
  }
}
#endif
