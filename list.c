/*
Functions used by u8.c, u16.c, and u32.c in order to crypt lists (strings) of such items without OS calls, other than for multithreading, which can be disabled (see flag.h).
*/
void
karacell_header_get(karacell_header_t *header_base,ULONG u32_idx_min,u32 *u32_list_base){  
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
karacell_string_crypt_prepare(u8 *decrypt_status_base,u8 hash_type,karacell_t *karacell_base){
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
karacell_string_encrypt_prepare(u32 entropy_list_base[KARACELL_MASTER_KEY_U32_COUNT_MAX],u64 file_idx,u32 *hash_size_base,u8 hash_type,karacell_t *karacell_base,u64 payload_size){
  u32 *iv_base;

  iv_base=&karacell_base->header_base->iv[0];
  entropy_iv_make(entropy_list_base,file_idx,iv_base,0,karacell_base);
  return karacell_header_make(hash_size_base,hash_type,karacell_base,payload_size);
}

u8
karacell_hash_check(u8 *hash_type_base,karacell_t *karacell_base){
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
