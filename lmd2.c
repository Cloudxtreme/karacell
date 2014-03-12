/*
Leidich Message Digest
http://leidich-message-digest.blogspot.com
Copyright 2013-2014 Russell Leidich
March 10, 2014

This file is part of the Leidich Message Digest Library.

The Leidich Message Digest Library is free software: you can redistribute it
and/or modify it under the terms of the GNU Limited General Public License as
published by the Free Software Foundation, version 3.

The Leidich Message Digest Library is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Limited
General Public License version 3 for more details.

You should have received a copy of the GNU Limited General Public License
version 3 along with the Leidich Message Digest Library (filename "COPYING").
If not, see http://www.gnu.org/licenses/ .
*/
/*
Use the following macros to compute the LMD2 or LMD3 of a string.
*/
u64
lmd_string_lmd2_custom_get(u32 lmd_c0,u32 lmd_x0,u8 *string_base,ULONG string_idx_min,u32 string_size){
/*
Don't call this directly. Instead, use LMD_STRING_LMD2_GET() or LMD_STRING_LMD3_GET().

Compute the LMD2 or LMD3 of a string.

In:

  lmd_c0 is LMD2_C0 for LMD2, or LMD3_C0 for LMD3.

  lmd_x0 is LMDX_C0 for LMD2, or LMD3_X0 for LMD3.

  *string_base is self-explanatory.

  string_idx_min offsets string_base.

  string_size is the size of the string.

Out:

  Returns the digest of the string_size bytes at &string_base[string_idx_min].
*/
  u64 iterand;
  u64 lmd;
  u32 string_remainder_size;
  ULONG string_idx;
  u32 string_u32;
  u32 string_u32_count;

  string_u32_count=string_size>>U32_SIZE_LOG2;
  string_remainder_size=string_size&3;
  LMD_ITERAND_INIT(lmd_c0,lmd_x0,iterand)
  LMD_ACCUMULATOR_INIT(lmd)
  string_idx=string_idx_min;
  while(string_u32_count--){
/*
We can safely use LMD_ITERATE_NO_ZERO_CHECK() (for speed) because the next X==0 is more than 2^30 (u32)s away, which but string_size is limited to 2^32-1.
*/
    LMD_ITERATE_NO_ZERO_CHECK(LMD2_A,lmd_c0,lmd_x0,iterand)
    memcpy((void *)(&string_u32),(const void *)(&string_base[string_idx]),(size_t)(U32_SIZE));
    string_idx+=U32_SIZE;
    LMD_ACCUMULATE(string_u32,lmd_x0,lmd)
  }
  if(string_remainder_size){
    LMD_ITERATE_NO_ZERO_CHECK(LMD2_A,lmd_c0,lmd_x0,iterand)
    string_u32=string_base[string_idx];
    if(string_remainder_size>1){
      string_u32|=(u32)(string_base[string_idx+1])<<U8_BITS;
      if(string_remainder_size>2){
        string_u32|=(u32)(string_base[string_idx+2])<<U16_BITS;
      }
    }
    LMD_ACCUMULATE(string_u32,lmd_x0,lmd)
  }
  LMD_FINALIZE(LMD2_A,lmd_c0,lmd_x0,iterand,lmd)
  return lmd;
}
