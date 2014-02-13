/*
Leidich Message Digest
http://leidich-message-digest.blogspot.com
Copyright 2013 Russell Leidich
February 28, 2013

This file is part of the Leidich Message Digest Library.

The Leidich Message Digest Library is free software: you can redistribute it
and/or modify it under the terms of the GNU Limited General Public License as
published by the Free Software Foundation, version 3.

The Leidich Message Digest Library is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Limited
General Public License version 3 for more details.

You should have received a copy of the GNU Limited General Public License
version 3 along with the Leidich Message Digest Library (filename "COPYING").
If not, see http://www.gnu.org/licenses/ .
*/
/*
Coefficient definitions for documentation purposes:

  LMD7_A0==(2^0x200-LMD7_D0)
  LMD7_A1==(2^0x200-LMD7_D1)

These numbers have been selected because they are easy to multiply by, and:

  LMD7_A0*2^0x200-1 is prime,
  LMD7_A1*2^0x200-1 is prime,
  LMD7_A0*2^0x1FF-1 is prime,
  and LMD7_A1*2^0x1FF-1 is prime.

which implies that the cycle lengths of the iterations (x,c)-->((LMD7_A0*x+c) MOD 2^0x400) and (x,c)-->((LMD7_A1*x+c) MOD 2^0x400) are the last 2 numbers above, respectively.

The exponents above are defined as follows:
*/
#define LMD7_D0 0xD1AEF329
#define LMD7_D1 0xE5467E8F
#define LMD7_BIT_COUNT (1<<(LMD7_SIZE_LOG2+U8_BITS_LOG2))
#define LMD7_BLOCK_SIZE 0x1000
#define LMD7_BLOCK_U32_COUNT (LMD7_BLOCK_SIZE>>U32_SIZE_LOG2)
#define LMD7_BLOCK_U64_COUNT (LMD7_BLOCK_SIZE>>U64_SIZE_LOG2)
#define LMD7_SIZE (1<<LMD7_SIZE_LOG2)
#define LMD7_SIZE_LOG2 7
#define LMD7_U32_COUNT (LMD7_SIZE>>U32_SIZE_LOG2)
#define LMD7_U64_COUNT (LMD7_SIZE>>U64_SIZE_LOG2)
