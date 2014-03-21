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
Debugger Prototype Declaration. See debug.c for full interface descriptions.
*/
extern void debug_print(char *string_base);
extern void debug_print_if(u8 status,char *string_base);
extern void debug_ptr(char *name_base,void *base);
extern void debug_list_print(char *context_string_base,u32 chunk_count,u8 *list_base,u8 chunk_size_log2);
extern void debug_u8(char *name_base,u8 value);
extern void debug_u16(char *name_base,u16 value);
extern void debug_u32(char *name_base,u32 value);
extern void debug_u64(char *name_base,u64 value);
