//
// This file contains mostly auto-generated information about IR operations.
// The gen_ops.py script is run to update this file, its implementation counterpart
// and ast-ir.c based on information in arch_*.lisp definition files as well as types.h
// and, for ast-ir, parse/parse.h.
//
// The approach used here was inspired by the Go compiler.
//
#pragma once

// Op___
typedef enum IROp {
  // generated by gen_ops.py from arch_base.lisp
  // Format: (op (inputs) -> (outputs) flags...)
  //
  OpNil,
  OpNoOp,	// passthrough "no operation"
  OpPhi,	// select an argument based on which predecessor block we came from
  OpCopy,	// alias, usually with a different logical but technically equivalent type
  OpFun,	// auxint = IRFun* address
  OpArg,
  //
  // Function calls
  // ["Call", 1, Call, t.mem, {aux: "SymOff"}], ; auxint=arglen, arg0=mem, returns mem
  OpCall,	// auxint=arglen, arg0=mem
  //
  // Constant values. Stored in IRValue.aux
  OpConstBool,	// aux is 0=false, 1=true
  OpConstI8,	// aux is sign-extended 8 bits
  OpConstI16,	// aux is sign-extended 16 bits
  OpConstI32,	// aux is sign-extended 32 bits
  OpConstI64,	// aux is Int64
  OpConstF32,
  OpConstF64,
  //
  // ---------------------------------------------------------------------
  // 2-input arithmetic. Types must be consistent.
  // Add for example must take two values of the same type and produce that same type.
  //
  // arg0 + arg1 ; sign-agnostic addition
  OpAddI8,
  OpAddI16,
  OpAddI32,
  OpAddI64,
  OpAddF32,
  OpAddF64,
  //
  // arg0 - arg1 ; sign-agnostic subtraction
  OpSubI8,
  OpSubI16,
  OpSubI32,
  OpSubI64,
  OpSubF32,
  OpSubF64,
  //
  // arg0 * arg1 ; sign-agnostic multiplication
  OpMulI8,
  OpMulI16,
  OpMulI32,
  OpMulI64,
  OpMulF32,
  OpMulF64,
  //
  // arg0 / arg1 ; division
  OpDivS8,	// signed (result is truncated toward zero)
  OpDivU8,	// unsigned (result is floored)
  OpDivS16,
  OpDivU16,
  OpDivS32,
  OpDivU32,
  OpDivS64,
  OpDivU64,
  OpDivF32,
  OpDivF64,
  //
  // arg0 % arg1 ; remainder
  // [1] For Mod16, Mod32 and Mod64, AuxInt non-zero means that
  //     the divisor has been proved to be not -1.
  OpModS8,
  OpModU8,
  OpModS16,	// [1]
  OpModU16,
  OpModS32,	// [1]
  OpModU32,
  OpModS64,	// [1]
  OpModU64,
  //
  // arg0 & arg1 ; bitwise AND
  OpAnd8,
  OpAnd16,
  OpAnd32,
  OpAnd64,
  //
  // arg0 | arg1 ; bitwise OR
  OpOr8,
  OpOr16,
  OpOr32,
  OpOr64,
  //
  // arg0 ^ arg1 ; bitwise XOR
  OpXor8,
  OpXor16,
  OpXor32,
  OpXor64,
  //
  // For shifts, AxB means the shifted value has A bits and the shift amount has B bits.
  // Shift amounts are considered unsigned.
  // If arg1 is known to be less than the number of bits in arg0, then aux may be set to 1.
  // According to the Go assembler, this enables better code generation on some platforms.
  //
  // arg0 << arg1 ; sign-agnostic shift left
  OpShLI8x8,
  OpShLI8x16,
  OpShLI8x32,
  OpShLI8x64,
  OpShLI16x8,
  OpShLI16x16,
  OpShLI16x32,
  OpShLI16x64,
  OpShLI32x8,
  OpShLI32x16,
  OpShLI32x32,
  OpShLI32x64,
  OpShLI64x8,
  OpShLI64x16,
  OpShLI64x32,
  OpShLI64x64,
  //
  // arg0 >> arg1 ; sign-replicating (arithmetic) shift right
  OpShRS8x8,
  OpShRS8x16,
  OpShRS8x32,
  OpShRS8x64,
  OpShRS16x8,
  OpShRS16x16,
  OpShRS16x32,
  OpShRS16x64,
  OpShRS32x8,
  OpShRS32x16,
  OpShRS32x32,
  OpShRS32x64,
  OpShRS64x8,
  OpShRS64x16,
  OpShRS64x32,
  OpShRS64x64,
  //
  // arg0 >> arg1 (aka >>>) ; zero-replicating (logical) shift right
  OpShRU8x8,
  OpShRU8x16,
  OpShRU8x32,
  OpShRU8x64,
  OpShRU16x8,
  OpShRU16x16,
  OpShRU16x32,
  OpShRU16x64,
  OpShRU32x8,
  OpShRU32x16,
  OpShRU32x32,
  OpShRU32x64,
  OpShRU64x8,
  OpShRU64x16,
  OpShRU64x32,
  OpShRU64x64,
  //
  // ---------------------------------------------------------------------
  // 2-input comparisons
  //
  // arg0 == arg1 ; sign-agnostic compare equal
  OpEqI8,
  OpEqI16,
  OpEqI32,
  OpEqI64,
  OpEqF32,
  OpEqF64,
  //
  // arg0 != arg1 ; sign-agnostic compare unequal
  OpNEqI8,
  OpNEqI16,
  OpNEqI32,
  OpNEqI64,
  OpNEqF32,
  OpNEqF64,
  //
  // arg0 < arg1 ; less than
  OpLessS8,	// signed
  OpLessU8,	// unsigned
  OpLessS16,
  OpLessU16,
  OpLessS32,
  OpLessU32,
  OpLessS64,
  OpLessU64,
  OpLessF32,
  OpLessF64,
  //
  // arg0 > arg1 ; greater than
  OpGreaterS8,	// signed
  OpGreaterU8,	// unsigned
  OpGreaterS16,
  OpGreaterU16,
  OpGreaterS32,
  OpGreaterU32,
  OpGreaterS64,
  OpGreaterU64,
  OpGreaterF32,
  OpGreaterF64,
  //
  // arg0 <= arg1 ; less than or equal
  OpLEqS8,	// signed
  OpLEqU8,	// unsigned
  OpLEqS16,
  OpLEqU16,
  OpLEqS32,
  OpLEqU32,
  OpLEqS64,
  OpLEqU64,
  OpLEqF32,
  OpLEqF64,
  //
  // arg0 <= arg1 ; greater than or equal
  OpGEqS8,	// signed
  OpGEqU8,	// unsigned
  OpGEqS16,
  OpGEqU16,
  OpGEqS32,
  OpGEqU32,
  OpGEqS64,
  OpGEqU64,
  OpGEqF32,
  OpGEqF64,
  //
  // boolean op boolean
  OpAndB,	// arg0 && arg1 (not shortcircuited)
  OpOrB,	// arg0 || arg1 (not shortcircuited)
  OpEqB,	// arg0 == arg1
  OpNEqB,	// arg0 != arg1
  //
  // ---------------------------------------------------------------------
  // 1-input
  //
  // !arg0 ; boolean not
  OpNotB,
  //
  // -arg0 ; negation
  OpNegI8,
  OpNegI16,
  OpNegI32,
  OpNegI64,
  OpNegF32,
  OpNegF64,
  //
  // ^arg0 ; bitwise complement
  // m ^ x  with m = "all bits set to 1" for unsigned x
  //        and  m = -1 for signed x
  OpCompl8,
  OpCompl16,
  OpCompl32,
  OpCompl64,
  //
  // ---------------------------------------------------------------------
  // Conversions
  // Note: For the generic "base" arch, all conversion ops must start with "Conv" in
  //       order for the gen_opts.py program to map them in the conversion table.
  // Note: Sign conversion, i.e. i32 -> u32 happens by simply interpreting the number
  //       differently. There are no signed conversion ops.
  //
  // extensions
  OpConvS8to16,
  OpConvS8to32,
  OpConvS8to64,
  OpConvU8to16,
  OpConvU8to32,
  OpConvU8to64,
  OpConvS16to32,
  OpConvS16to64,
  OpConvU16to32,
  OpConvU16to64,
  OpConvS32to64,
  OpConvU32to64,
  //
  // truncations
  OpConvI16to8,
  OpConvI32to8,
  OpConvI32to16,
  OpConvI64to8,
  OpConvI64to16,
  OpConvI64to32,
  //
  // conversions
  OpConvS32toF32,
  OpConvS32toF64,
  OpConvS64toF32,
  OpConvS64toF64,
  OpConvU32toF32,
  OpConvU32toF64,
  OpConvU64toF32,
  OpConvU64toF64,
  OpConvF32toF64,
  OpConvF32toS32,
  OpConvF32toS64,
  OpConvF32toU32,
  OpConvF32toU64,
  OpConvF64toF32,
  OpConvF64toS32,
  OpConvF64toS64,
  OpConvF64toU32,
  OpConvF64toU64,

  Op_GENERIC_END, // ---------------------------------------------

  Op_MAX
} IROp;


// IROpFlag indicates characteristics of an IROp, documented via IROpDescr.
typedef enum IROpFlag {
  // Do not edit. Generated by gen_ops.py
  IROpFlagNone = 0,
  IROpFlagZeroWidth         = 1 << 0 ,// dummy op; no actual I/O.
  IROpFlagConstant          = 1 << 1 ,// true if the value is a constant. Value in aux
  IROpFlagCommutative       = 1 << 2 ,// commutative on its first 2 arguments (e.g. addition; x+y==y+x)
  IROpFlagResultInArg0      = 1 << 3 ,// output of v and v.args[0] must be allocated to the same register.
  IROpFlagResultNotInArgs   = 1 << 4 ,// outputs must not be allocated to the same registers as inputs
  IROpFlagRematerializeable = 1 << 5 ,// register allocator can recompute value instead of spilling/restoring.
  IROpFlagClobberFlags      = 1 << 6 ,// this op clobbers flags register
  IROpFlagCall              = 1 << 7 ,// is a function call
  IROpFlagNilCheck          = 1 << 8 ,// this op is a nil check on arg0
  IROpFlagFaultOnNilArg0    = 1 << 9 ,// this op will fault if arg0 is nil (and aux encodes a small offset)
  IROpFlagFaultOnNilArg1    = 1 << 10,// this op will fault if arg1 is nil (and aux encodes a small offset)
  IROpFlagUsesScratch       = 1 << 11,// this op requires scratch memory space
  IROpFlagHasSideEffects    = 1 << 12,// for "reasons", not to be eliminated. E.g., atomic store.
  IROpFlagGeneric           = 1 << 13,// generic op
  IROpFlagLossy             = 1 << 14,// operation may be lossy. E.g. converting i32 to i16.
} IROpFlag;

// IRAux describes what is in a IRValue's aux value, if anything.
typedef enum IRAux {
  // Do not edit. Generated by gen_ops.py
  IRAuxNone = 0,
  IRAuxBool,
  IRAuxI8,
  IRAuxI16,
  IRAuxI32,
  IRAuxI64,
  IRAuxF32,
  IRAuxF64,
  IRAuxMem,
  IRAuxSym,
} IRAux;


// IROpDescr describes an IROp. Retrieve with IROpInfo(IROp).
typedef struct IROpDescr {
  // Do not edit. Generated by gen_ops.py
  IROpFlag flags;
  TypeCode outputType; // invariant: < TypeCode_NUM_END
  IRAux    aux;        // type of data in IRValue.aux
} IROpDescr;


// maps IROp => name, e.g. "Phi"
extern const char* const IROpNames[Op_MAX];
static inline const char* IROpName(IROp op) {
  assert(op >= 0 && op < Op_MAX);
  return IROpNames[op];
}

// IROpNamesMaxLen = longest name in IROpNames
//!Generated by gen_ops.py -- do not edit.
#define IROpNamesMaxLen 12
//!EndGenerated

// IROpConstFromAST(TypeCode) maps TypeCode => IROp for constants.
extern const IROp _IROpConstMap[TypeCode_NUM_END];
static inline IROp IROpConstFromAST(TypeCode t) {
  assert(t < TypeCode_NUM_END); // t must be a concrete, basic numeric type
  return _IROpConstMap[t];
}

// IROpConvertType returns the IROp for converting fromType -> toType.
// If fromType can not be converted to toType, OpNil is returned.
// Expectations on from and to types:
// - intrinsic, i.e. v < TypeCode_NUM_END. Or else crash.
// - types differ, i.e. int32 and int32 is unexpected and yields OpNil.
// Utilizes _IROpConvMap.
// To determine if a conversion might be lossy, check IROpFlagLossy:
//   IROpInfo(op)->flags & IROpFlagLossy
IROp IROpConvertType(TypeCode fromType, TypeCode toType);
extern const IROp _IROpConvMap[TypeCode_NUM_END][TypeCode_NUM_END];

// IROpInfo(IROp) accesses the description of an IROp
extern const IROpDescr _IROpInfoMap[Op_MAX];
static inline const IROpDescr* IROpInfo(IROp op) {
  assert(op < Op_MAX);
  return &_IROpInfoMap[op];
}
