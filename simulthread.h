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
These are parameters are safe to modify at random without endangering correctness (but maybe performance). They police themselves.
*/
/*
SIMULTHREAD_IDX_MAX must be 0 when PTHREAD is not defined. Otherwise, it's is purely a u32 optimization parameter, dictating the maximum number of simultaneous outstanding threads. Experiment. Higher values use more memory and do not necessarily imply greater speed.
*/
#ifdef PTHREAD
  #define KARACELL_SIMULTHREAD_IDX_MAX 0x10
#else
  #define KARACELL_SIMULTHREAD_IDX_MAX 0
#endif
#if U32_MAX<=KARACELL_SIMULTHREAD_IDX_MAX
  #error "KARACELL_SIMULTHREAD_IDX_MAX is too large."
#endif
