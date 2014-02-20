/*
Karacell 3 Library
Copyright 2014 Tigerspike Ltd
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
Common functions for list cryption.

The functions in this file are for convenience only, and are not part of the Karacell specification.
*/
void
listcrypt_header_get(karacell_header_t *header_base,ULONG u32_idx_min,u32 *u32_list_base){
/*
Extract a karacell_header_t from a list of (u32)s.

In:

  *header_base is an undefined karacell_header_t.

  u32_idx_min offsets u32_list_base.

  *u32_list_base contains a list of (u32)s, the first KARACELL_HEADER_U32_COUNT starting at index u32_idx_min comprise a karacell_header_t, which may be corrupt.

Out:

  *header_base contains the header at &u32_list_base[u32_idx_min], verbatim, whether or not corrupt.
*/
  #if KARACELL_HEADER_U32_COUNT!=14
    #error "karacell_header_get() is unaware of changes to karacell_header_t."
  #endif
  karacell_u32_list_copy(u32_idx_min,u32_list_base,0,&header_base->iv[0],KARACELL_IV_U32_COUNT-1);
  u32_idx_min+=KARACELL_IV_U32_COUNT;
  header_base->must_decrypt_to_zero=((u64)(u32_list_base[u32_idx_min+1])<<U32_BITS)+u32_list_base[u32_idx_min];
  u32_idx_min+=2;
  header_base->size_following=((u64)(u32_list_base[u32_idx_min+1])<<U32_BITS)+u32_list_base[u32_idx_min];
  u32_idx_min+=2;
  header_base->build_number_of_creator=(u16)(u32_list_base[u32_idx_min]);
  header_base->build_number_needed_to_decode=(u16)(u32_list_base[u32_idx_min]>>U16_BITS);
  u32_idx_min++;
  header_base->reserved_zero=(u16)(u32_list_base[u32_idx_min]);
  header_base->hash_type=(u8)(u32_list_base[u32_idx_min]>>U16_BITS);
  header_base->hash_u32_count_minus_1=(u8)(u32_list_base[u32_idx_min]>>(U16_BITS+U8_BITS));
  return;
}

u8
listcrypt_prepare(u8 *decrypt_status_base,u8 hash_type,karacell_t *karacell_base){
/*
Determine whether to encrypt or decrypt, determine whether the desired hash type is supported, and reset private memory structures.

In:

  *decrypt_status_base is undefined.

  hash_type is 0 if preparing to decrypt. Otherwise one of the nonzero KARACELL_HASH_TYPE constants.

  karacell_base is as defined in karacell_init():Out.

Out:

  Returns 0 if decrypting, or if encrypting and the requested hash type is supported; else 1.

  *decrypt_status_base is 0 if decrypting, else 1.

  karacell_base is as defined in karacell_rewind():Out. 
*/
  u8 decrypt_status;
  u8 status;

  status=0;
  decrypt_status=0;
  switch(hash_type){
  case 0:
    decrypt_status=1;
    break;
  case KARACELL_HASH_TYPE_NONE:
  case KARACELL_HASH_TYPE_LMD8:
  case KARACELL_HASH_TYPE_LMD7:
    break;
  default:
    status=1;
  }
  *decrypt_status_base=decrypt_status;
  karacell_rewind(karacell_base);
  return status;
}

u8
listcrypt_encrypt_prepare(u32 entropy_list_base[KARACELL_MASTER_KEY_U32_COUNT_MAX],u64 file_idx,u32 *hash_size_base,u8 hash_type,karacell_t *karacell_base,u64 payload_size){
/*
Generate an IV from an entropy pool and use it to build a karacell_header_t.

In:

  *entropy_list_base is as defined in entropy_iv_make():In

  file_idx is as defined in entropy_iv_make():In.

  *hash_size_base is as defined in karacell_header_make():In.

  hash_type is as defined in karacell_header_make():In.

  *iv_base is as defined in entropy_iv_make():In.

  *karacell_base is as defined in entropy_iv_make():In.

  payload_size is as defined in karacell_header_make():In.

Out:

  *hash_size_base is as defined in karacell_header_make():Out.

  *iv_base is as defined in entropy_iv_make():Out.

  *karacell_base is as defined in entropy_iv_make():Out (except that header_base->iv is a valid IV) and karacell_header_make():Out. 
*/
  u32 *iv_base;

  iv_base=&karacell_base->header_base->iv[0];
  entropy_iv_make(entropy_list_base,file_idx,iv_base,0,karacell_base);
  return karacell_header_make(hash_size_base,hash_type,karacell_base,payload_size);
}

u8
listcrypt_hash_check(u8 *hash_type_base,karacell_t *karacell_base){
/*
Determine whether a nonnull hash is present which Karacell is able to authenticate.

In:

  *hash_type_base is undefined.

  *karacell_base is as defined in karacell_hash_type_get():In.

Out:

  Returns 0 if a nonnull hash is present which Karacell is able to authenticate, else 1. (This is a statement regarding capability only; this function does not perform authentication.)

  *hash_type_base is the unencrypted hash_type byte from karacell_base->header_base. Its value may or may not match one of the KARACELL_HASH_TYPE constants.
*/
  u8 hash_type;
  u8 unauthenticated_status;

  hash_type=karacell_hash_type_get(karacell_base);
  *hash_type_base=hash_type;
  unauthenticated_status=1;
  switch(hash_type){
  case KARACELL_HASH_TYPE_LMD8:
  case KARACELL_HASH_TYPE_LMD7:
    unauthenticated_status=0;
  }
  return unauthenticated_status;
}
