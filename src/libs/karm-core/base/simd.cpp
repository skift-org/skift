export module Karm.Core:base.simd;

import :base.base;

namespace Karm {

export using i8x2 = i8 __attribute__((vector_size(2)));
export using i8x4 = i8 __attribute__((vector_size(4)));
export using i8x8 = i8 __attribute__((vector_size(8)));
export using i8x16 = i8 __attribute__((vector_size(16)));
export using i8x32 = i8 __attribute__((vector_size(32)));

export using i16x2 = i16 __attribute__((vector_size(4)));
export using i16x4 = i16 __attribute__((vector_size(8)));
export using i16x8 = i16 __attribute__((vector_size(16)));
export using i16x16 = i16 __attribute__((vector_size(32)));

// the asm intrinsics demand chars as  the 8-bit type, and do not allow
// (un-)signed ones to be used
export using c8x2 = char __attribute__((vector_size(2)));
export using c8x4 = char __attribute__((vector_size(4)));
export using c8x8 = char __attribute__((vector_size(8)));
export using c8x16 = char __attribute__((vector_size(16)));
export using c8x32 = char __attribute__((vector_size(32)));

export using i32x2 = i32 __attribute__((vector_size(8)));
export using i32x4 = i32 __attribute__((vector_size(16)));
export using i32x8 = i32 __attribute__((vector_size(32)));

export using i64x2 = i64 __attribute__((vector_size(16)));
export using i64x4 = i64 __attribute__((vector_size(32)));

export using u8x2 = u8 __attribute__((vector_size(2)));
export using u8x4 = u8 __attribute__((vector_size(4)));
export using u8x8 = u8 __attribute__((vector_size(8)));
export using u8x16 = u8 __attribute__((vector_size(16)));
export using u8x32 = u8 __attribute__((vector_size(32)));

export using u16x2 = u16 __attribute__((vector_size(4)));
export using u16x4 = u16 __attribute__((vector_size(8)));
export using u16x8 = u16 __attribute__((vector_size(16)));
export using u16x16 = u16 __attribute__((vector_size(32)));

export using u32x2 = u32 __attribute__((vector_size(8)));
export using u32x4 = u32 __attribute__((vector_size(16)));
export using u32x8 = u32 __attribute__((vector_size(32)));

export using u64x2 = u64 __attribute__((vector_size(16)));
export using u64x4 = u64 __attribute__((vector_size(32)));

export using f16x2 = __fp16 __attribute__((vector_size(4)));
export using f16x4 = __fp16 __attribute__((vector_size(8)));
export using f16x8 = __fp16 __attribute__((vector_size(16)));

export using f32x2 = float __attribute__((vector_size(8)));
export using f32x4 = float __attribute__((vector_size(16)));
export using f32x8 = float __attribute__((vector_size(32)));

export using f64x2 = double __attribute__((vector_size(16)));
export using f64x4 = double __attribute__((vector_size(32)));

} // namespace Karm
