/*
Spawn Library
http://spawnthread.blogspot.com
Copyright 2013 Tigerspike Ltd
http://tigerspike.com
April 18, 2013

This collection of files constitutes the Spawn Library. (This is a
library in the abstact sense; it's not intended to compile to a ".lib"
file.)

The Spawn Library is free software: you can redistribute it and/or
modify it under the terms of the GNU Limited General Public License as
published by the Free Software Foundation, version 3.

The Spawn Library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
Limited General Public License version 3 for more details.

You should have received a copy of the GNU Limited General Public
License version 3 along with the Spawn Library (filename
"COPYING"). If not, see http://www.gnu.org/licenses/ .
*/
TYPEDEF_START
  u8 *readonly_string_base;
  ULONG thread_idx;
  u32 simulthread_idx;
TYPEDEF_END(spawn_simulthread_context_t)

TYPEDEF_START
  spawn_simulthread_context_t context;
#ifdef PTHREAD
  pthread_t pthread;
#endif
TYPEDEF_END(spawn_simulthread_t)

TYPEDEF_START
  void (*function_base)(spawn_simulthread_context_t *);
  spawn_simulthread_t *simulthread_list_base;
  u32 simulthread_idx_max;
  u32 simulthread_launch_idx;
  u32 simulthread_retire_idx;
  u8 simulthread_active_status;
TYPEDEF_END(spawn_t)
