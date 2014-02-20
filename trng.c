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
True Random Number Generator

This is the interface to the true random number generator required but not specified by Karacell. See true_random_get() for important security information.
*/
#ifdef _32_
  extern u32 jytter_true_random_get(jytter_scratch_space_t *jytter_scratch_space_base) __attribute__((fastcall));
#else
  extern u32 jytter_true_random_get(jytter_scratch_space_t *jytter_scratch_space_base);
#endif

u32
true_random_get(void){
/*
Return a u32 sourced from unbiased physical noise. We use Jytter, but feel free to replace or enhance it as need be, while preserving the rest of the interface. Note that this is NOT the place where the TRNG output is masked by a trapdoor function in order to generate an opaque IV; see entropy_iv_make() for that.

WARNING: Test this function on your target platform before you trust it! It pays to characterize. (And with Jytter, you might experience linker issues that pass the return value in the wrong register, which could have catastrophic consequences, if you're using a compiler other than GCC or MinGW on X86 or X64, with which there is no known problem.) Also, this function may not be run on more than 1 core at a time, as different cores may make similar measurements of the same physical phenomena -- whether or not you use Jytter. On the other hand, it is strongly recommended that other threads be in flight while this function executes, ideally involving IO, which will create timing entropy and allow us, counterintuitively, to finish sooner. Nonetheless no statistical bias should be visible in the output data if the system is idle. If you change this function, then definitely do not import randomness from beyond the circle of trust.

Out:

  Returns u32 sourced from unbiased (we hope!) physical noise.
*/
  jytter_scratch_space_t jytter_scratch_space;

  return jytter_true_random_get(&jytter_scratch_space);
}
