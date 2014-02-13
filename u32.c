/*
Code required only for u32 list cryption.
*/
u8
karacell_u32_list_crypt(u32 entropy_list_base[KARACELL_MASTER_KEY_U32_COUNT_MAX],u64 file_idx,karacell_t *karacell_base,u8 *hash_type_base,ULONG *u32_count_base,ULONG u32_idx_min,u32 *u32_list_base,u8 *unauthenticated_status_base){
/*
Out:

  Returns 1 on failure, else 0.

  *unauthenticated_status_base is only defined for decryption (In:hash_type==0). In this case, it's 1 for an unrecognized hash, or 0 otherwise. In combination with the hash_type, the caller can discern whether (1) the hash employed was of acceptable strength and (2) whether or not Karacell actually recognized (and equivalently, verified) it. Recognized hash algos which fail verification will always result in a nonzero return value from this function.
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
This code was adapted from os.c, basically by removing all the OS calls and redundant code comments. The multithreading stuff does involve OS calls, but can be shunted to a monothreaded model (see PTHREAD_OFF in flag.h).
*/
  hash_type=*hash_type_base;
  status=karacell_string_crypt_prepare(&decrypt_status,hash_type,karacell_base);
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
      karacell_header_get(header_base,u32_idx_min,u32_list_base);
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
If it's not obvious, this cast of payload_size is not a truncation because by definition the string being decrypted fits inside the address space of the machine, and karacell_header_decrypt() has already ensured that it was consistent with its hull size (including header and footer and perhaps some overtransmission).
*/
      *u32_count_base=(ULONG)(payload_size)>>U32_SIZE_LOG2;
    }else{
      status=karacell_string_encrypt_prepare(entropy_list_base,file_idx,&hash_size,hash_type,karacell_base,payload_size);
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
These shifts won't overflow because we know that the entire string fits in memory.
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
      unauthenticated_status=karacell_hash_check(hash_type_base,karacell_base);
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
