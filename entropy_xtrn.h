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
Entropy (extern)al Prototype Declaration. See entropy.c for full interface descriptions.
*/
extern void entropy_iv_make(u32 entropy_list_base[KARACELL_MASTER_KEY_U32_COUNT_MAX],u64 file_idx,u32 *iv_base,ULONG iv_idx_min,karacell_t *karacell_base);
extern void entropy_make(u32 entropy_list_base[KARACELL_MASTER_KEY_U32_COUNT_MAX]);
extern void entropy_thread_execute(spawn_simulthread_context_t *spawn_simulthread_context_base);
extern void entropy_make_flush(spawn_t *spawn_base);
extern spawn_t *entropy_make_spawn(u32 entropy_list_base[KARACELL_MASTER_KEY_U32_COUNT_MAX]);
