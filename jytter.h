/*
Jytter
Version 1
http://jytter.blogspot.com
Copyright 2012 Russell Leidich
August 24, 2012

This file is part of the Jytter Library.

The Jytter Library is free software: you can redistribute it
and/or modify it under the terms of the GNU Limited General Public License as
published by the Free Software Foundation, version 3.

The Jytter Library is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Limited
General Public License version 3 for more details.

You should have received a copy of the GNU Limited General Public License
version 3 along with the Leidich Message Digest Library (filename "COPYING").
If not, see http://www.gnu.org/licenses/ .
*/
typedef struct {
  uint64_t caller_rcx;
  uint64_t caller_rdx;
  uint64_t caller_rbx;
  uint64_t caller_rbp;
  uint64_t caller_rsi;
} jytter_scratch_space_t;

