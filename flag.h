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
Build control. If possible, change the build using gcc command switches, and not by changing this file.
*/
#define BUILD_NUMBER 168
#define BUILD_NUMBER_NEEDED_TO_DECODE 131
#if !(defined(_32_)||defined(_64_))
  #error "Use 'gcc -D_64_' for 64-bit or 'gcc -D_32_' for 32-bit code."
#elif defined(_32_)&&defined(_64_)
  #error "You have defined both _32_ and _64_. Chose one only."
#endif
#if !(defined(DEBUG)||defined(DEBUG_OFF))
  #error "Use 'gcc -DDEBUG' for debugging or 'gcc -DDEBUG_OFF' for release."
#elif defined(DEBUG)&&defined(DEBUG_OFF)
  #error "You have defined both DEBUG and DEBUG_OFF. Chose one only."
#endif
#if !(defined(PIPELINE)||defined(PIPELINE_OFF))
  #error "Use 'gcc -DPIPELINE' to enable IO pipelining or 'gcc -DPIPELINE_OFF' not to."
#elif defined(PIPELINE)&&defined(PIPELINE_OFF)
  #error "You have defined both PIPELINE and PIPELINE_OFF. Chose one only."
#endif
#if !(defined(PTHREAD)||defined(PTHREAD_OFF))
  #error "Use 'gcc -DPTHREAD' for multithreading or 'gcc -DPTHREAD_OFF' for monothreading."
#elif defined(PTHREAD)&&defined(PTHREAD_OFF)
  #error "You have defined both PTHREAD and PTHREAD_OFF. Chose one only."
#endif
#ifndef O_BINARY
  #define O_BINARY 0
#endif
#if defined(WINDOWS)
  #undef WINDOWS
#endif
#if defined(_WIN32)||defined(__WIN32__)||defined(_WIN64)||defined(__WIN64__)
  #define WINDOWS
#endif
