/*
Leidich Message Digest
http://leidich-message-digest.blogspot.com
Copyright 2013 Russell Leidich
March 2, 2013

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

  LMD8_A0==(2^0x100-LMD8_D0)
  LMD8_A1==(2^0x100-LMD8_D1)

These numbers have been selected because they are easy to multiply by, and:

  LMD8_A0*2^0x100-1 is prime,
  LMD8_A1*2^0x100-1 is prime,
  LMD8_A0*2^0xFF-1 is prime,
  and LMD8_A1*2^0xFF-1 is prime.

which implies that the cycle lengths of the iterations (x,c)-->((LMD8_A0*x+c) MOD 2^0x200) and (x,c)-->((LMD8_A1*x+c) MOD 2^0x200) are the last 2 numbers above, respectively.

The exponents above are defined as follows:
*/
#define LMD8_D0 0x9C20E342
#define LMD8_D1 0xD28CC42E
#define LMD8_BIT_COUNT (1<<(LMD8_SIZE_LOG2+U8_BITS_LOG2))
#define LMD8_BLOCK_SIZE 0x1000
#define LMD8_BLOCK_U32_COUNT (LMD8_BLOCK_SIZE>>U32_SIZE_LOG2)
#define LMD8_BLOCK_U64_COUNT (LMD8_BLOCK_SIZE>>U64_SIZE_LOG2)
#define LMD8_SIZE (1<<LMD8_SIZE_LOG2)
#define LMD8_SIZE_LOG2 6
#define LMD8_U32_COUNT (LMD8_SIZE>>U32_SIZE_LOG2)
#define LMD8_U64_COUNT (LMD8_SIZE>>U64_SIZE_LOG2)
