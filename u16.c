/*
Code required only for u16 list cryption.
*/
void
karacell_u16_to_u32_list_copy(ULONG u16_idx_min,u16 *u16_list_base,ULONG u32_idx_min,u32 *u32_list_base,u32 u16_count_minus_1){
/*
This function is derived from karacell_u32_list_copy, so look there for comments, if this code is not blatheringly obvious to begin with.
*/
  ULONG u16_idx;
  ULONG u32_idx;
  ULONG u32_idx_post;

  u32_idx_post=u32_idx_min+(((ULONG)(u16_count_minus_1)+1)>>1);
  u16_idx=u16_idx_min;
  for(u32_idx=u32_idx_min;u32_idx<u32_idx_post;u32_idx++){
    u32_list_base[u32_idx]=u16_list_base[u16_idx]+((u32)(u16_list_base[u16_idx+1])<<U16_BITS);
    u16_idx+=2;
  }
  if(!(u16_count_minus_1&1)){
    u32_list_base[u32_idx]=((u32_list_base[u32_idx]>>U16_BITS)<<U16_BITS)+u16_list_base[u16_idx];
  }
  return;
}

void
karacell_u32_to_u16_list_copy(ULONG u16_idx_min,u16 *u16_list_base,ULONG u32_idx_min,u32 *u32_list_base,u32 u16_count_minus_1){
  ULONG u16_idx;
  ULONG u32_idx;
  ULONG u32_idx_post;
  u32 u32_item;

  u32_idx_post=u32_idx_min+(((ULONG)(u16_count_minus_1)+1)>>1);
  u16_idx=u16_idx_min;
  for(u32_idx=u32_idx_min;u32_idx<u32_idx_post;u32_idx++){
    u32_item=u32_list_base[u32_idx];
    u16_list_base[u16_idx]=(u16)(u32_item);
    u16_list_base[u16_idx+1]=(u16)(u32_item>>U16_BITS);
    u16_idx+=2;
  }
  if(!(u16_count_minus_1&1)){
    u16_list_base[u16_idx]=(u16)(u32_list_base[u32_idx]);
  }
  return;
}

u8
karacell_u16_list_crypt(u32 entropy_list_base[KARACELL_MASTER_KEY_U32_COUNT_MAX],u64 file_idx,karacell_t *karacell_base,u8 *hash_type_base,ULONG *u16_count_base,ULONG u16_idx_min,u16 *u16_list_base,u8 *unauthenticated_status_base){
  u32 block_base[KARACELL_BLOCK_U32_COUNT];
  u8 decrypt_status;
  u32 *hash_base;  
  u32 hash_size;
  u8 hash_type;
  u32 hash_u16_count_minus_1;
  u8 hash_u32_count_minus_1;
  u32 *hash_xor_all_base;
  karacell_header_t *header_base;
  u8 header_status;
  u32 header_u32_list_base[KARACELL_HEADER_U32_COUNT];
  ULONG payload_block_idx;
  ULONG payload_block_idx_max;
  u64 payload_size;
  u8 status;
  u32 tail_u16_count;
  u32 tail_u16_count_minus_1;
  karacell_thread_t *thread_base;
  u8 unauthenticated_status;
/*
This code is equivalent to karacell_u32_list_crypt() with adaptations for converting u16 to u32.
*/
  hash_type=*hash_type_base;
  status=karacell_string_crypt_prepare(&decrypt_status,hash_type,karacell_base);
  do{
    if(status){
      break;
    }
    payload_size=*u16_count_base;
    payload_size<<=U16_SIZE_LOG2;
    header_base=karacell_base->header_base;
    if(decrypt_status){
      status=karacell_header_size_check(payload_size);
      if(status){
        break;
      }
      karacell_u16_to_u32_list_copy(u16_idx_min,u16_list_base,0,header_u32_list_base,KARACELL_HEADER_U16_COUNT-1);
      karacell_header_get(header_base,0,header_u32_list_base);
      u16_idx_min+=KARACELL_HEADER_U16_COUNT;
      header_status=karacell_header_decrypt(&payload_size,&hash_size,karacell_base);
      if((1<header_status)||(payload_size&U16_BYTE_MAX)){
        status=1;
        break;
      }
      *u16_count_base=(ULONG)(payload_size)>>U16_SIZE_LOG2;
    }else{
      status=karacell_string_encrypt_prepare(entropy_list_base,file_idx,&hash_size,hash_type,karacell_base,payload_size);
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
        karacell_u16_to_u32_list_copy(u16_idx_min,u16_list_base,0,block_base,KARACELL_BLOCK_U16_COUNT-1);
        karacell_tumbler_list_make(payload_block_idx,karacell_base,thread_base);
        if(KARACELL_HASH_TYPE_NONE<hash_type){
          karacell_u32_list_zero(hash_u32_count_minus_1,0,hash_base);
          if(!decrypt_status){
            karacell_hash_get(0,karacell_base,thread_base,0,block_base,KARACELL_BLOCK_SIZE);
          }
        }
        u16_idx_min+=KARACELL_BLOCK_U16_COUNT;
        karacell_subblock_crypt(karacell_base,thread_base,KARACELL_TUMBLER_BIAS_CRYPT,KARACELL_BLOCK_U32_COUNT-1,0,block_base);
        if(KARACELL_HASH_TYPE_NONE<hash_type){
          if(decrypt_status){
            karacell_hash_get(0,karacell_base,thread_base,0,block_base,KARACELL_BLOCK_SIZE);
          }
          karacell_hash_xor(hash_base,hash_u32_count_minus_1,hash_xor_all_base);
        }
      }
      payload_size-=(ULONG)(payload_block_idx_max)<<KARACELL_BLOCK_SIZE_LOG2;
    }
    tail_u16_count=(u32)(payload_size)>>U16_SIZE_LOG2;
    if(tail_u16_count){
      tail_u16_count_minus_1=tail_u16_count-1;
      karacell_u16_to_u32_list_copy(u16_idx_min,u16_list_base,0,block_base,tail_u16_count_minus_1);
    }
    karacell_tail_crypt(decrypt_status,karacell_base,payload_block_idx_max,(u32)(payload_size),0,block_base);
    if(tail_u16_count){
      karacell_u32_to_u16_list_copy(u16_idx_min,u16_list_base,0,block_base,tail_u16_count_minus_1);
    }
    u16_idx_min+=tail_u16_count;
    if(decrypt_status){
      unauthenticated_status=karacell_hash_check(hash_type_base,karacell_base);
      *unauthenticated_status_base=unauthenticated_status;
      if(!unauthenticated_status){
        hash_u16_count_minus_1=(hash_size>>U16_SIZE_LOG2)-1;
        karacell_u16_to_u32_list_copy(u16_idx_min,u16_list_base,0,hash_xor_all_base,hash_u16_count_minus_1);
        status=karacell_hash_xor_all_compare(hash_xor_all_base,karacell_base);
      }
    }
  }while(0);
  return status;
}
