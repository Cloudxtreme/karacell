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
u8 List Cryption for Karacell.

The functions in this file are for convenience only, and are not part of the Karacell specification.
*/
void
listcrypt_u8_to_u32_list_copy(u32 u8_count_minus_1,ULONG u8_idx_min,u8 *u8_list_base,ULONG u32_idx_min,u32 *u32_list_base){
/*
Convert a list of (u8)s to a list of (u32)s. This is a patent waste of time, necessitated solely by virtue of type enforcement in C and other porting targets. memcpy() is not used for ease of facilitating the latter.

In:

  u8_count_minus_1 is 1 less than the number of (u8)s to copy.

  u8_idx_min offsets u8_list_base.

  *u8_list_base contains (u8_count_minus_1+1) (u8)s to copy to u32_list_base[u32_idx_min].

  u32_idx_min offsets u32_list_base.

  *u32_list_base is undefined but writable for ceil((u8_count_minus_1+1)/4.0) (u32)s starting at index u32_idx_min.

Out:

  *u32_list_base[u32_idx_min] matches u8_list_base[u8_idx_min] for the first (u8_count_minus_1+1) (u8)s, but is otherwise unchanged.
*/
  u8 shift;
  ULONG u32_idx;
  ULONG u32_idx_post;
  u32 u32_item;
  u32 u32_item_new;
  ULONG u8_count;
  ULONG u8_idx;

  u32_idx_post=u32_idx_min+(((ULONG)(u8_count_minus_1)+1)>>U32_SIZE_LOG2);
  u8_count=u8_count_minus_1;
  u8_count++;
  u8_idx=u8_idx_min;
  for(u32_idx=u32_idx_min;u32_idx<u32_idx_post;u32_idx++){
    u32_list_base[u32_idx]=u8_list_base[u8_idx]+((u32)(u8_list_base[u8_idx+1])<<U8_BITS)+((u32)(u8_list_base[u8_idx+2])<<U16_BITS)+((u32)(u8_list_base[u8_idx+3])<<(U8_BITS+U16_BITS));
    u8_idx+=U32_SIZE;
  }
  if(u8_count&U32_BYTE_MAX){
    u32_item=u32_list_base[u32_idx];
    u32_item_new=0;
    shift=0;
    if(u8_count&U16_SIZE){
      u32_item_new=((u32)(u8_list_base[u8_idx+1])<<U8_BITS)+u8_list_base[u8_idx];
      u8_idx+=U16_SIZE;
      shift=(u8)(shift+U16_BITS);
    }
    if(u8_count&U8_SIZE){
      u32_item_new+=(u32)(u8_list_base[u8_idx])<<shift;
      shift=(u8)(shift+U8_BITS);
    }
    u32_item>>=shift;
    u32_item<<=shift;
    u32_item+=u32_item_new;
    u32_list_base[u32_idx]=u32_item;
  }
  return;
}

void
listcrypt_u32_to_u8_list_copy(u32 u8_count_minus_1,ULONG u8_idx_min,u8 *u8_list_base,ULONG u32_idx_min,u32 *u32_list_base){
/*
Perform the inverse of listcrypt_u8_to_u32_list_copy().

In:

  u8_count_minus_1 is 1 less than the number of (u8)s to copy.

  u8_idx_min offsets u8_list_base.

  *u8_list_base is undefined but writable for (u8_count_minus_1+1) (u8)s starting at index u8_idx_min.

  u32_idx_min offsets u32_list_base.

  *u32_list_base is writable for (u8_count_minus_1+1) (u8)s starting at index u32_list_base u32_idx_min.

Out:

  *u32_list_base[u32_idx_min] matches u8_list_base[u8_idx_min] for the first (u8_count_minus_1+1) (u8)s, but is otherwise unchanged.
*/
  ULONG u32_idx;
  ULONG u32_idx_post;
  u32 u32_item;
  ULONG u8_count;
  ULONG u8_idx;

  u32_idx_post=u32_idx_min+(((ULONG)(u8_count_minus_1)+1)>>U32_SIZE_LOG2);
  u8_count=u8_count_minus_1;
  u8_count++;
  u8_idx=u8_idx_min;
  for(u32_idx=u32_idx_min;u32_idx<u32_idx_post;u32_idx++){
    u32_item=u32_list_base[u32_idx];
    u8_list_base[u8_idx]=(u8)(u32_item);
    u8_list_base[u8_idx+U8_SIZE]=(u8)(u32_item>>U8_BITS);
    u8_list_base[u8_idx+U16_SIZE]=(u8)(u32_item>>U16_BITS);
    u8_list_base[u8_idx+U16_SIZE+U8_SIZE]=(u8)(u32_item>>(U16_BITS+U8_BITS));
    u8_idx+=U32_SIZE;
  }
  if(u8_count&U32_BYTE_MAX){
    u32_item=u32_list_base[u32_idx];
    if(u8_count&U16_SIZE){
      u8_list_base[u8_idx]=(u8)(u32_item);
      u8_list_base[u8_idx+1]=(u8)(u32_item>>U8_BITS);
      u8_idx+=U16_SIZE;
      u32_item>>=U16_BITS;
    }
    if(u8_count&U8_SIZE){
      u8_list_base[u8_idx]=(u8)(u32_item);
    }
  }
  return;
}

u8
listcrypt_u8_list_crypt(u32 entropy_list_base[KARACELL_MASTER_KEY_U32_COUNT_MAX],u64 file_idx,karacell_t *karacell_base,u8 *hash_type_base,ULONG *u8_count_base,ULONG u8_idx_min,u8 *u8_list_base,u8 *unauthenticated_status_base){
/*
Crypt a list of (u8)s.

In:

  *entropy_list_base is as defined in entropy_make():Out.

  file_idx is as defined in entropy_iv_make():In.

  *karacell_base is as defined in karacell_init():Out.

  *hash_type_base is as defined in listcrypt_prepare():In:hash_type.

  *u8_count_base is the number of (u8)s to crypt. In the case of decryption, this value includes the number of (u8)s in the header and the hash; if it turns out to be insufficient once the hash_type is decrypted, then an error shall be returned. In any case, in no event shall memory be accessed beyond what this value permits.

  u8_idx_min offsets u8_list_base.

  *u8_list_base contains the list to crypt, starting at index u8_idx_min.

  *unauthenticated_status_base is as defined in listcrypt_u8_list_crypt():In.

Out:

  Returns 1 on failure, else 0. Failure may result from a malformed or inconsistent header, an inconsistent hash.

  *karacell_base is defined in listcrypt_u32_list_crypt():Out.

  *hash_type_base is defined in listcrypt_u32_list_crypt():Out.

  *u8_count_base is the number of decrypted (u8)s at u8_list_base[u8_idx_min+KARACELL_HEADER_SIZE] in the case of decryption, or the (unchanged) number of encrypted (u8)s at u8_list_base[u8_idx_min] in the case of encryption. In either case, this value excludes the size of the hash.

  *u8_list_base contains *u8_count_base crypted (u8)s starting at index (u8_idx_min+KARACELL_HEADER_SIZE) in the case of decryption, or index u8_idx_min in the case of encryption. In both cases: (1) the unencrypted header appears at karacell_base->header_base and (2)  the hash is not returned unencrypted.

  *unauthenticated_status_base is as defined in listcrypt_u32_list_crypt():Out.
*/
  u32 block_base[KARACELL_BLOCK_U32_COUNT];
  u8 decrypt_status;
  u32 *hash_base;  
  u32 hash_size;
  u8 hash_type;
  u32 hash_u8_count_minus_1;
  u32 hash_u32_count_minus_1;
  u32 *hash_xor_all_base;
  karacell_header_t *header_base;
  u8 header_status;
  u32 header_u32_list_base[KARACELL_HEADER_U32_COUNT];
  ULONG payload_block_idx;
  ULONG payload_block_idx_max;
  u64 payload_size;
  u8 status;
  u32 tail_u8_count;
  u32 tail_u8_count_minus_1;
  karacell_thread_t *thread_base;
  u8 unauthenticated_status;
/*
This code is equivalent to listcrypt_u32_list_crypt() with adaptations for converting u8 to u32.
*/
  hash_type=*hash_type_base;
  status=listcrypt_prepare(&decrypt_status,hash_type,karacell_base);
  do{
    if(status){
      break;
    }
    payload_size=*u8_count_base;
    header_base=karacell_base->header_base;
    if(decrypt_status){
      status=karacell_header_size_check(payload_size);
      if(status){
        break;
      }
      listcrypt_u8_to_u32_list_copy(KARACELL_HEADER_SIZE-1,u8_idx_min,u8_list_base,0,header_u32_list_base);
      listcrypt_header_get(header_base,0,header_u32_list_base);
      u8_idx_min+=KARACELL_HEADER_SIZE;
      header_status=karacell_header_decrypt(&payload_size,&hash_size,karacell_base);
      if(1<header_status){
        status=1;
        break;
      }
      *u8_count_base=(ULONG)(payload_size);
    }else{
      status=listcrypt_encrypt_prepare(entropy_list_base,file_idx,&hash_size,hash_type,karacell_base,payload_size);
      if(status){
        break;
      }
    }
    thread_base=&karacell_base->thread_list_base[0];
    hash_xor_all_base=&karacell_base->hash_xor_all[0];
    hash_base=&thread_base->hash[0];
    hash_u32_count_minus_1=(hash_size>>U32_SIZE_LOG2)-1;
    payload_block_idx_max=0;
    if(payload_size){
      payload_block_idx_max=((ULONG)(payload_size)-1)>>KARACELL_BLOCK_SIZE_LOG2;
      payload_block_idx=0;
      while(payload_block_idx<payload_block_idx_max){
        payload_block_idx++;
        listcrypt_u8_to_u32_list_copy(KARACELL_BLOCK_SIZE-1,u8_idx_min,u8_list_base,0,block_base);
        karacell_tumbler_list_make(payload_block_idx,karacell_base,thread_base);
        if(KARACELL_HASH_TYPE_NONE<hash_type){
          karacell_u32_list_zero(hash_u32_count_minus_1,0,hash_base);
          if(!decrypt_status){
            karacell_hash_get(0,karacell_base,thread_base,0,block_base,KARACELL_BLOCK_SIZE);
          }
        }
        u8_idx_min+=KARACELL_BLOCK_SIZE;
        karacell_subblock_crypt(karacell_base,thread_base,KARACELL_TUMBLER_BIAS_CRYPT,KARACELL_BLOCK_U32_COUNT-1,0,block_base);
        if(KARACELL_HASH_TYPE_NONE<hash_type){
          if(decrypt_status){
            karacell_hash_get(0,karacell_base,thread_base,0,block_base,KARACELL_BLOCK_SIZE);
          }
          karacell_hash_xor(hash_base,(u8)(hash_u32_count_minus_1),hash_xor_all_base);
        }
      }
      payload_size-=(ULONG)(payload_block_idx_max)<<KARACELL_BLOCK_SIZE_LOG2;
    }
    tail_u8_count=(u32)(payload_size);
    tail_u8_count_minus_1=tail_u8_count-1;
    if(tail_u8_count){
      listcrypt_u8_to_u32_list_copy(tail_u8_count_minus_1,u8_idx_min,u8_list_base,0,block_base);
    }
    karacell_tail_crypt(decrypt_status,karacell_base,payload_block_idx_max,(u32)(payload_size),0,block_base);
    if(tail_u8_count){
      listcrypt_u32_to_u8_list_copy(tail_u8_count_minus_1,u8_idx_min,u8_list_base,0,block_base);
    }
    u8_idx_min+=tail_u8_count;
    if(decrypt_status){
      unauthenticated_status=listcrypt_hash_check(hash_type_base,karacell_base);
      *unauthenticated_status_base=unauthenticated_status;
      if(!unauthenticated_status){
        hash_u8_count_minus_1=hash_size-1;
        listcrypt_u8_to_u32_list_copy(hash_u8_count_minus_1,u8_idx_min,u8_list_base,0,hash_xor_all_base);
        status=karacell_hash_xor_all_compare(hash_xor_all_base,karacell_base);
      }
    }
  }while(0);
  return status;
}
