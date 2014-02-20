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
ASCII-to-Binary Conversion for Karacell

The functions in this file are not part of the Karacell specification. They are provided as an illustration of how to safely convert ASCII to binary, given the requirements of Karacell.
*/
u8
ascii_master_key_get(u32 master_key_base[KARACELL_MASTER_KEY_U32_COUNT_MAX],u8 *master_key_text_base){
/*
Derive a binary master key from its ASCII equivalent in a safe and case-insentive manner.

In:

  *master_key_text_base is the master key in big endian ASCII, in any mix of letter cases. It must be readable through the lesser 0-based index of: (1) the index at which a null (ASCII 0) character appears and (2) index (KARACELL_MASTER_KEY_U32_COUNT_MAX<<3). Provided that the string meets these requirements, then any manner of corruption is safely dealt with, including a string which begins with a null.

Out:

  Returns 1 on failure, in which case all other outputs are undefined, or 0 on success. Failure can occur because (1) the first character was a null, (2) no null was found as high as index (KARACELL_MASTER_KEY_U32_COUNT_MAX<<3), or (3) nonhexadecimal characters were discovered prior to the null.

  *master_key_base is the master key in binary, padded high with 0s over its writable domain. Pass it to karacell_tumbler_idx_max_get().
*/
  u32 digit;
  u8 digit_shift;
  u32 master_key_idx;
  u32 master_key_u32;
  u32 u8_idx;
  u8 status;

  status=1;
  karacell_u32_list_zero(KARACELL_MASTER_KEY_U32_COUNT_MAX-1,0,master_key_base);
  u8_idx=0;
  while(master_key_text_base[u8_idx]&&(u8_idx<=((KARACELL_MASTER_KEY_U32_COUNT_MAX<<3)-1))){
    u8_idx++;
  }
  if(u8_idx&&(!master_key_text_base[u8_idx])){
    master_key_idx=0;
    master_key_u32=0;
    digit_shift=0;
    status=0;
    do{
      u8_idx--;
/*
Load the digit and, if it's a letter, convert it to lower case.
*/
      digit=master_key_text_base[u8_idx]|0x20;
      if((digit>='0')&&(digit<='9')){
        digit-='0';
      }else if((digit>='a')&&(digit<='f')){
        digit-='a'-10;
      }else{
        status=1;
      }
      master_key_u32|=digit<<digit_shift;
      digit_shift+=4;
      digit_shift&=U32_BIT_MAX;
      if(!(digit_shift&&u8_idx)){
        master_key_base[master_key_idx]=master_key_u32;
        master_key_idx++;
        master_key_u32=0;
      }
    }while(u8_idx);
  }
  return status;
}
