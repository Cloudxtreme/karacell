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
REQUIREMENTS

An X86 or X64 CPU is required. See PORTING for migration to other architectures.

For Linux, you need GCC. For Windows, you need MinGW (http://mingw.org) for the CMD command shell. Cygwin is not supported. For MacOS, you need clang.

NASM 2.09.10 or better is also required (http://nasm.us) for systems with an X86 or X64 CPU. On Linux use:

  sudo apt-get install nasm

PORTING

Porting Karacell to other CPUs should be fairly easy. You need to consider at least: (1) porting jytter.asm or modifying trng.c to use some other true random number generator, (2) compiling with "-mtune=native" as opposed to "-m32" or "-m64", and (3) the interaction between FTELLO, FSEEKO, and _FILE_OFFSET_BITS.

RUNNING

We don't supply executables because no one sophisticated enough to download this code would (1) trust us to provide them with a precompiled binary and (2) not know how to compile their own C code. We're at least smart enough to realize that success in this industry does not come from distributing closed-source DRMed garbage. On the contrary, it starts with a reputation for openness.

The Karacell source code supplied herein isn't really intended to be a command line utility, although you could certainly use it that way. Rather, it's just to provide the cryptography community with reference code, to surface any bugs, weaknesses, or opportunities for acceleration. Yes, we're fully aware that the command line isn't exactly the latest technology. And for that matter, it's not a great idea to be injecting crypto keys into the command history buffer. It's assumed that any practical application would merely use this source code as a reference model. That said, while the comment verbosity leaves a lot to be desired, the code has been tested, reviewed, fed into various automated bug finders, and cross-checked against our Wolfram Mathematica model (mathematica.txt). If there's a bug somewhere, please tell us at the address above so we can fix it.

COMPILING

Compiling with MinGW will give a warning about jytter_true_random_get, which can be safely ignored.

make clean

  Deletes preexisting build files.

make demo

  Produces %TEMP%\demo.exe on Windows or ./temp/demo otherwise. Run it to test the correct behavior of Karacell.

make karacell
make karacell_debug
make karacell_debug_pipeline
make karacell_debug_pipeline_pthread
make karacell_debug_pthread
make karacell_pipeline
make karacell_pipeline_pthread
make karacell_pthread

  Produces a monothreaded or multithreaded ("pthread") commandline version of IO pipelined-or-not ("pipeline") Karacell with or without the debugger ("debug") enabled. IO pipelining is for demo purposes only, and does not result in a performance improvement in this case.

FILE SUMMARY

[ascii.c]

  ASCII conversion function for key importation from the command line.

[constant.h]

  Boring constants.

[COPYING]

  Required for distribution.

[debug.c]
[debug.h]

  Debug infrastructure (compile with -DDEBUG).

[demo.c]

  Demo encryption xor mask construction. When you run temp/demo, you should get a pile of text ending in "Yay! It's correct." This is followed by tests of the list (string) cryption apparatus, which performs Karacell operations on memory regions as opposed to files. These tests are well commented so as to illustrate the use of said list cryption functions.

[entropy.c]

  Demo of entropy factory for the purpose of generating initial values for Karacell. NOT part of the Karacell spec.

[file_sys.c]
[file_sys.h]

  Boring file IO stuff.

[flag.h]

  Build control.

[jytter.asm]
[jytter.h]

  True random number generator for X86 and X64. http://jytter.blogspot.com

[karacell.c]
[karacell.h]

  Karacell 3 core functions and data structures. We don't provide a fully encapsulated encrypt_file() function because Karacell needs to be divorced from TRNG architecture, the latter being hardware dependent. There are other design considerations as well, namely: (1) multithreading (handled transparently via the PTHREAD(_OFF) build constants) and (2) the decision to construct xor masks (a) immediately (JIT) after data arrives (higher latency requiring only a local secret entropy pool) or (b) speculatively (see PIPELINE(_OFF) in flag.h, which is used only for demo purposes but should operate correctly) prior to arrival (lower latency, but requiring an entropy pool shared with all peers and resynchonized after each power failure in such a manner as to prevent replay attacks). Most architectures will probably perform best with monothreading (which keeps other cores available for more urgent tasks) and JIT mask construction.

  However, a simplified JIT cryption interface is provided in u8.c, u16.c, and u32.c.

[listcrypt.c]

  Functions used by u8.c, u16.c, and u32.c.

[lmd2.c]
[lmd2.h]

  Error detection library used to find accidental flaws in critical data structures.

[lmd7.c]
[lmd7.h]
[lmd8.c]
[lmd8.h]

  Secure hash libraries. http://leidich-message-digest.blogspot.com

[main.c]

  OS interface which connects karacell.c to the command line. It knows only the minimum possible amount of information about Karacell file construction in order to do so. The name only refers to its status as the root compilation file for the executable, and is not required if calling the listcrypt functions via u8.c, u16.c, or u32.c.

[makefile]

  Disasterously complex multiplatform build file. Be careful! Tabs and spaces actually matter.

[mathematica.txt]

  Cut-and-pastable Wolfram Mathematica Kernel source code which emulates demo.c using huge integers. Hopefully this will make our algo more accessible to mathematically minded hackers. Karacell only looks complicated because we're forced to use machine-sized integers and deal with annoying OS issues and exogenous hashes. It's mainly just a Marsaglia oscillator which selects rotations of an integer to add together, in order to form xor masks. So relax, already!

[print.c]

  Stupid text printing functions.

[spawn.c]
[spawn.h]

  Multithreading library. http://spawnthread.blogspot.com

[table.h]

  The Karacell Table.

[temp]

  Folder for emitted build files and executables.

[trng.c]

  Abstract interface from entropy.c to your true random number generator of choice.

[tweak.h]

  Performance tweaks.

[u8.c]

  Functions for cryption of byte-granular lists (strings).

[u16.c]

  Functions for cryption of u16-granular lists, for example, UNICODE.

[u32.c]

  Functions for cryption of u32-granular lists.

[unix_include.h]

  Standard UNIXy stuff.
