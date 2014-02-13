/*
Karacell 3 Library
Copyright 2013 Tigerspike Ltd
http://karacell.info
May 1, 2013

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
#ifdef DEBUG
  #define DEBUG_PRINT(string_base) debug_print(string_base);
  #define DEBUG_PTR(name_base,value) debug_ptr(name_base,value);
  #define DEBUG_STRING_PRINT(context_string_base,chunk_count,string_base,chunk_size_log2) debug_string_print(context_string_base,chunk_count,string_base,chunk_size_log2);
  #define DEBUG_U16(name_base,value) debug_u16(name_base,value);
  #define DEBUG_U32(name_base,value) debug_u32(name_base,value);
  #define DEBUG_U64(name_base,value) debug_u64(name_base,value);
  #define DEBUG_U8(name_base,value) debug_u8(name_base,value);
#else
  #define DEBUG_PRINT(string_base)
  #define DEBUG_PTR(name_base,value)
  #define DEBUG_STRING_PRINT(context_string_base,chunk_count,string_base,chunk_size_log2)
  #define DEBUG_U16(name_base,value)
  #define DEBUG_U32(name_base,value)
  #define DEBUG_U64(name_base,value)
  #define DEBUG_U8(name_base,value)
#endif
