/*
Karacell 3 Library
Copyright 2013 Tigerspike Ltd
http://karacell.info

This collection of files constitutes the Karacell 3 Library. (This is a
library in the abstact sense; it's not intended to compile to a ".lib"
file.)

The Karacell 3 Library is free software: you can redistribute it and/or
modify it under the terms of the GNU Limited General Public License as
published by the Free Software Foundation, version 3.

The Karacell 3 Library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
Limited General Public License version 3 for more details.

You should have received a copy of the GNU Limited General Public
License version 3 along with the Karacell 3 Library (filename
"COPYING"). If not, see http://www.gnu.org/licenses/ .
*/
/*
Stuff you can't change, and just adds documentation value.
*/
#define BIT_CLEAR(base,idx) (((base)[(idx)>>U8_BITS_LOG2])&=(~(1<<((idx)&U8_BIT_MAX))))
#define BIT_SET(base,idx) (((base)[(idx)>>U8_BITS_LOG2])|=(1<<((idx)&U8_BIT_MAX)))
#define BIT_TEST(base,idx) ((((base)[(idx)>>U8_BITS_LOG2])>>((idx)&U8_BIT_MAX))&1)
#define i16 int16_t
#define I16_BIT_MAX 15
#define I16_BITS 16
#define I16_BITS_LOG2 4
#define I16_BYTE_MAX 1 
#define I16_MAX 0x7FFF
#define I16_MIN ((-I16_MAX)-1)
#define I16_SIZE 2
#define I16_SIZE_LOG2 1
#define I16_SPAN 0x10000
#define i32 int32_t
#define I32_BIT_MAX 31
#define I32_BITS 32
#define I32_BITS_LOG2 5
#define I32_BYTE_MAX 3
#define I32_MAX 0x7FFFFFFF
#define I32_MIN ((-I32_MAX)-1)
#define I32_SIZE 4
#define I32_SIZE_LOG2 2
#define I32_SPAN 0x100000000ULL
#define i64 int64_t
#define I64_BIT_MAX 63
#define I64_BITS 64
#define I64_BITS_LOG2 6
#define I64_BYTE_MAX 7
#define I64_MAX 0x7FFFFFFFFFFFFFFFULL
#define I64_MIN ((-I64_MAX)-1)
#define I64_SIZE 8
#define I64_SIZE_LOG2 3
#define i8 signed char
#define I8_BIT_MAX 7
#define I8_BITS 8
#define I8_BITS_LOG2 3
#define I8_MAX 0x7F
#define I8_MIN ((-I8_MAX)-1)
#define I8_SIZE 1
#define I8_SIZE_LOG2 0
#define I8_SPAN 0x100
#define MAX(m,n) (((m)>=(n))?(m):(n))
#define MIN(m,n) (((m)<=(n))?(m):(n))
#define MSB_SMALL_GET(n,msb) \
  msb=0; \
  while(((n)>>msb)>>1){ \
    msb++; \
  }
#define REVERSE_U64(n) \
  n=(((n)&0x5555555555555555ULL)<<1)+(((n)&0xAAAAAAAAAAAAAAAAULL)>>1); \
  n=(((n)&0x3333333333333333ULL)<<2)+(((n)&0xCCCCCCCCCCCCCCCCULL)>>2); \
  n=(((n)&0x0F0F0F0F0F0F0F0FULL)<<4)+(((n)&0xF0F0F0F0F0F0F0F0ULL)>>4); \
  n=(((n)&0x00FF00FF00FF00FFULL)<<8)+(((n)&0xFF00FF00FF00FF00ULL)>>8); \
  n=(((n)&0x0000FFFF0000FFFFULL)<<16)+(((n)&0xFFFF0000FFFF0000ULL)>>16); \
  n=((n)>>32)+((n)<<32)
#define SHIFT_RIGHT_DOUBLE(bit_count,u32_lo,u32_hi) (((u32_lo)>>(bit_count))+((u32_hi)<<(U32_BITS-(bit_count))))
#define TYPEDEF_END(typedef_struct_name) }typedef_struct_name;
#define TYPEDEF_START typedef struct __attribute__ ((packed)) {
#define u16 uint16_t
#define U16_BIT_MAX 15
#define U16_BITS 16
#define U16_BITS_LOG2 4
#define U16_BYTE_MAX 1
#define U16_MAX 0xFFFF
#define U16_SIZE 2
#define U16_SIZE_LOG2 1
#define U16_SPAN 0x10000
#define u32 uint32_t
#define U32_BIT_MAX 31
#define U32_BITS 32
#define U32_BITS_LOG2 5
#define U32_BYTE_MAX 3
#define U32_MAX 0xFFFFFFFF
#define U32_SIZE 4
#define U32_SIZE_LOG2 2
#define U32_SPAN 0x100000000ULL
#define u64 uint64_t
#define U64_BIT_MAX 63
#define U64_BITS 64
#define U64_BITS_LOG2 6
#define U64_BYTE_MAX 7
#define U64_MAX 0xFFFFFFFFFFFFFFFFULL
#define U64_SIZE 8
#define U64_SIZE_LOG2 3
#define u8 uint8_t
#define U8_BIT_MAX 7
#define U8_BITS 8
#define U8_BITS_LOG2 3
#define U8_MAX 0xFF
#define U8_SIZE 1
#define U8_SIZE_LOG2 0
#define U8_SPAN 0x100
#ifdef _64_
  #define ULONG uint64_t
  #define ULONG_MAX U64_MAX
  #define ULONG_SIZE 8
  #define ULONG_SIZE_LOG2 3
#else
  #define ULONG uint32_t
  #define ULONG_MAX U32_MAX
  #define ULONG_SIZE 4
  #define ULONG_SIZE_LOG2 2
#endif
