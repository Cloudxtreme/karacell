/*
Karacell 3 Library
Copyright 2014 Tigerspike Ltd
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
LMD (extern)al Prototype Declaration. See lmd2.c, lmd7.c, and lmd8.c for full interface descriptions.
*/
extern u64 lmd_string_lmd2_custom_get(u32 lmd_c0,u32 lmd_x0,u8 *string_base,ULONG string_idx_min,u32 string_size);
extern void lmd_block_lmd7_get(u32 block_base[LMD7_BLOCK_U32_COUNT],ULONG block_idx_min,u32 p_base[LMD7_U32_COUNT<<1],u32 z_base[LMD7_U32_COUNT]);
extern void lmd_block_lmd8_get(u32 block_base[LMD8_BLOCK_U32_COUNT],ULONG block_idx_min,u32 p_base[LMD8_U32_COUNT<<1],u32 z_base[LMD8_U32_COUNT]);