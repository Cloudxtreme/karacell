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
u32 List Cryption for Karacell.

The functions in this file are for convenience only, and are not part of the Karacell specification.
*/
u8
listcrypt_u32_list_crypt(u32 entropy_list_base[KARACELL_MASTER_KEY_U32_COUNT_MAX],u64 file_idx,karacell_t *karacell_base,u8 *hash_type_base,ULONG *u32_count_base,ULONG u32_idx_min,u32 *u32_list_base,u8 *unauthenticated_status_base){
/*
Crypt a list of (u32)s.

In:

  *entropy_list_base is as defined in entropy_make():Out.

  file_idx is as defined in entropy_iv_make():In.

  *karacell_base is as defined in karacell_init():Out.

  *hash_type_base is as defined in listcrypt_prepare():In:hash_type.

  *u32_count_base is the number of (u32)s to crypt. In the case of decryption, this value includes the number of (u32)s in the header and the hash; if it turns out to be insufficient once the hash_type is decrypted, then an error shall be returned. In any case, in no event shall memory be accessed beyond what this value permits.

  u32_idx_min offsets u32_list_base.

  *u32_list_base contains the list to crypt, starting at index u32_idx_min.

  *unauthenticated_status_base is undefined.

Out:

  Returns 1 on failure, else 0. Failure may result from a malformed or inconsistent header, an inconsistent hash, or a decrypted payload_size which is nonzero and not a multiple of U32_SIZE.

  *karacell_base is in need of refurbishment via karacell_rewind(). karacell_base->header_base is the base of the unencrypted header and karacell_base->hash_xor_all is the encrypted hash.

  *hash_type_base is only changed in the case of decryption, in which case it's the unencrypted hash_type field from the Karacell header. See *unauthenticated_status_base for security considerations.

  *u32_count_base is the number of decrypted (u32)s at u32_list_base[u32_idx_min+KARACELL_HEADER_U32_COUNT] in the case of decryption, or the (unchanged) number of encrypted (u32)s at u32_list_base[u32_idx_min] in the case of encryption. In either case, this value excludes the size of the hash.

  *u32_list_base contains *u32_count_base crypted (u32)s starting at index (u32_idx_min+KARACELL_HEADER_U32_COUNT) in the case of decryption, or index u32_idx_min in the case of encryption. In both cases: (1) the unencrypted header appears at karacell_base->header_base and (2)  the hash is not returned unencrypted.

  *unauthenticated_status_base is only defined for decryption (In:hash_type==0). In this case, it's 1 for an unauthenticated (unverified) hash or KARACELL_HASH_TYPE_NONE, or 0 otherwise. In combination with the *hash_type_base, the caller can discern whether (1) the hash employed was of acceptable strength and (2) whether or not Karacell actually authenticated it. (Recognized hash algos which fail authentication will always result in a nonzero return value from this function, obviating the need to inspect this value.) The idea here is that future versions of Karacell might support new hash types, such that decryption would still be possible with older versions, but authentication would not.
*/
  u8 decrypt_status;
  u32 hash_size;
  u8 hash_type;
  u32 hash_u32_count_minus_1;
  u32 *hash_xor_all_base;
  karacell_header_t *header_base;
  u8 header_status;
  ULONG payload_block_idx_max;
  u64 payload_size;
  u8 status;
  u32 tail_block_base[KARACELL_BLOCK_U32_COUNT];
  u32 tail_u32_count;
  u32 tail_u32_count_minus_1;
  u8 unauthenticated_status;
/*
This code was adapted from main.c, basically by removing all the OS calls and redundant code comments. The multithreading stuff does involve OS calls, but can be shunted to a monothreaded model (see PTHREAD_OFF in flag.h).
*/
  hash_type=*hash_type_base;
  status=listcrypt_prepare(&decrypt_status,hash_type,karacell_base);
  do{
    if(status){
      break;
    }
    payload_size=*u32_count_base;
    payload_size<<=U32_SIZE_LOG2;
    header_base=karacell_base->header_base;
    if(decrypt_status){
      status=karacell_header_size_check(payload_size);
      if(status){
        break;
      }
      listcrypt_header_get(header_base,u32_idx_min,u32_list_base);
      u32_idx_min+=KARACELL_HEADER_U32_COUNT;
      header_status=karacell_header_decrypt(&payload_size,&hash_size,karacell_base);
/*
payload_size is byte-granular, but in this case, u32 granularity is required in order to avoid buffer overflows.
*/
      if((1<header_status)||(payload_size&U32_BYTE_MAX)){
        status=1;
        break;
      }
/*
If it's not obvious, this cast of payload_size is not a truncation because by definition the list being decrypted fits inside the address space of the machine, and karacell_header_decrypt() has already ensured that it was consistent with its hull size (including header and footer and perhaps some overtransmission).
*/
      *u32_count_base=(ULONG)(payload_size)>>U32_SIZE_LOG2;
    }else{
      status=listcrypt_encrypt_prepare(entropy_list_base,file_idx,&hash_size,hash_type,karacell_base,payload_size);
      if(status){
        break;
      }
    }
    payload_block_idx_max=0;
    if(payload_size){
      payload_block_idx_max=((ULONG)(payload_size)-1)>>KARACELL_BLOCK_SIZE_LOG2;
      if(payload_block_idx_max){
        payload_block_idx_max--;
        karacell_crypt_spawn(payload_block_idx_max,0,decrypt_status,karacell_base,u32_idx_min,u32_list_base);
        karacell_thread_flush(karacell_base);
        payload_block_idx_max++;
/*
These shifts won't overflow because we know that the entire list fits in memory.
*/
        u32_idx_min+=(ULONG)(payload_block_idx_max)<<(KARACELL_BLOCK_SIZE_LOG2-U32_SIZE_LOG2);
        payload_size-=(ULONG)(payload_block_idx_max)<<KARACELL_BLOCK_SIZE_LOG2;
      }
    }
    tail_u32_count=(u32)(payload_size)>>U32_SIZE_LOG2;
/*
We need to crypt the tail block at tail_block_base because the hash region granularity is only guaranteed to be at most a multiple of KARACELL_BLOCK_SIZE. Only the first payload_size bytes need be defined.
*/
    if(tail_u32_count){
      tail_u32_count_minus_1=tail_u32_count-1;
      karacell_u32_list_copy(u32_idx_min,u32_list_base,0,tail_block_base,tail_u32_count_minus_1);
    }
/*
Run tail cryption even if payload_size==0 because we need to compute the encrypted hash.
*/
    karacell_tail_crypt(decrypt_status,karacell_base,payload_block_idx_max,(u32)(payload_size),0,tail_block_base);
    if(tail_u32_count){
      karacell_u32_list_copy(0,tail_block_base,u32_idx_min,u32_list_base,tail_u32_count_minus_1);
    }
    u32_idx_min+=tail_u32_count;
    if(decrypt_status){
      unauthenticated_status=listcrypt_hash_check(hash_type_base,karacell_base);
      *unauthenticated_status_base=unauthenticated_status;
      if(!unauthenticated_status){
/*
hash_size is a multiple of U32_SIZE, so we're not losing information here.
*/
        hash_u32_count_minus_1=(hash_size>>U32_SIZE_LOG2)-1;
        hash_xor_all_base=&karacell_base->hash_xor_all[0];
        karacell_u32_list_copy(u32_idx_min,u32_list_base,0,hash_xor_all_base,hash_u32_count_minus_1);
        status=karacell_hash_xor_all_compare(hash_xor_all_base,karacell_base);
      }
    }
  }while(0);
  return status;
}
