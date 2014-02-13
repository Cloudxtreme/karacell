/*
Karacell 3 Library
Copyright 2013 Tigerspike Ltd
http://karacell.info
October 17, 2013

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

GCC is required for all except Windows, for which you need MinGW (http://mingw.org) for the CMD command shell. Cygwin is not recommended.

NASM 2.09.10 or better is also required (http://nasm.us). Supports only X86 and X64 architectures. On Linux:

  sudo apt-get install nasm

Porting Karacell to other CPUs should be fairly easy. You need to consider at least: (1) "-mtune=native" as opposed to "-m32" or "-m64", (2) jytter.asm, and (3) the interaction between FTELLO, FSEEKO, and _FILE_OFFSET_BITS.

RUNNING

We don't supply executables because no one sophisticated enough to download this code would (1) trust us to provide them with a precompiled binary and (2) not know how to compile their own C code. We're at least smart enough to realize that success in this industry does not come from distributing closed-source DRMed garbage. On the contrary, it starts with a reputation for openness.

The Karacell source code supplied herein isn't really intended to be a command line utility, although you could certainly use it that way. Rather, it's just to provide the cryptography community with reference code, to surface any bugs, weaknesses, or opportunities for acceleration. Yes, we're fully aware that the command line isn't exactly the latest technology. And for that matter, it's not a great idea to be injecting crypto keys into the command history buffer. It's assumed that any practical application would merely use this source code as a reference model. That said, while the comment verbosity leaves a lot to be desired, the code has been tested, reviewed, fed into various automated bug finders, and cross-checked against our Wolfram Mathematica model (mathematica.txt). If there's a bug somewhere, please tell us at the address above so we can fix it.

COMPILING

MacOS hasn't really been tested, so let us know if you find a problem.

Run one of the build* files (see below). The output, karacell and demo executables, are to the temp subfolder of the local path. You can then run demo(.exe) to make sure things seem to be functioning properly, or karacell(.exe) for syntax info.

FILE SUMMARY

ascii.c

ASCII conversion function for key importation from the command line.

build_a_clean.sh
build_b_clean.bat

Cleaning files which delete everything in temp. Be careful to run them from within the karacell folder, and not from somewhere else!
 
build_c_linux32.sh
build_d_linux64.sh
build_e_mac_os32.sh
build_f_mac_os64.sh
build_g_win32.bat

Build files for various OSes, named such that minimal command line tabbing is required. If building with MinGW under Windows, you'll see some innocuous warnings related to the .asm files. If you can't seem to run them in Linux, then either use chmod to add the executable attribute, or run via "/bin/sh [filename]".

Build flags:

-DDEBUG(_OFF)

Toggles the debugger. Enabling the debugger will enable corrupt-on-allocate with malloc() in order to reveal dependencies on uninitialized memory.

-DPIPELINE(_OFF)

Toggles IO pipelining. The idea is to allow disk (or network) IO in parallel with xor mask fabrication, ahead of plaintext or ciphertext arrival. As coded, it offers no performance advantages, but reworking the code for a specific situation could result in substantial acceleration.

-DPTHREAD(_OFF)

Toggles multithreading support. Multithreading provides performance advantages, but may not be acceptable under some security regimes.

constant.h

Boring constants.

debug.c
debug.h

Debug infrastructure (compile with -DDEBUG).

demo.c

Demo encryption xor mask construction. When you run temp/demo, you should get a pile of text ending in "Yay! It's correct."

entropy.c

Demo of entropy factory for the purpose of generating initial values for Karacell. NOT part of the Karacell spec.

file_sys.c
file_sys.h

Boring file IO stuff.

flag.h

Build control.

jytter.asm
jytter.h

True random number generator for X86 and X64. http://jytter.blogspot.com

karacell.c
karacell.h

Karacell 3 core functions and data structures. We don't provide a fully encapsulated encrypt_file() function because cryption should be divided into various threads across various processing stages, in the interest of performance. So the caller (os.c) does need to know a few things about how to (dis)assemble a Karacell file.

lmd2.c
lmd2.h

Error detection library used to find accidental flaws in critical data structures.

lmd7.c
lmd7.h
lmd8.c
lmd8.h

Secure hash libraries. http://leidich-message-digest.blogspot.com

mathematica.txt

Cut-and-pastable Wolfram Mathematica Kernel source code which emulates demo.c using huge integers. Hopefully this will make our algo more accessible to mathematically minded hackers. Karacell only looks complicated because we're forced to use machine-sized integers and deal with annoying OS issues and exogenous hashes. It's mainly just a Marsaglia oscillator which selects rotations of an integer to add together, in order to form xor masks. So relax, already!

os.c

OS interface which connects karacell.c to the command line. It knows only the minimum possible amount of information about Karacell file construction in order to do so.

print.c

Stupid text printing functions.

spawn.c
spawn.h

Multithreading library. http://spawnthread.blogspot.com

table.h

The Karacell Table.

temp

Folder for emitted build files and executables.

timestamp.asm

X86 and X64 CPU timestamp reader.

trng.c

Abstract interface from entropy.c to your true random number generator of choice.

tweak.h

Performance tweaks.

unix_include.h

Standard UNIXy stuff.
