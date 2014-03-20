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
Karacell 3 (extern)al Prototype Declaration. See karacell.c for full interface descriptions.
*/
extern void karacell_u32_list_zero(u32 u32_count_minus_1,ULONG u32_idx_min,u32 *u32_list_base);
extern void karacell_u32_list_copy(ULONG in_u32_idx_min,u32 *in_u32_list_base,ULONG out_u32_idx_min,u32 *out_u32_list_base,u32 u32_count_minus_1);
extern void karacell_marsaglia_iterate(u32 iterand_base[KARACELL_MASTER_KEY_U32_COUNT_MAX]);
extern void karacell_tumbler_list_make(u64 block_idx,karacell_t *karacell_base,karacell_thread_t *thread_base);
extern void karacell_subblock_crypt(karacell_t *karacell_base,karacell_thread_t *thread_base,u16 tumbler_bias,u32 u32_count_minus_1,ULONG u32_idx_min,u32 *u32_list_base);
extern void karacell_hash_get(u8 hash_seed_ready_status,karacell_t *karacell_base,karacell_thread_t *thread_base,ULONG u32_idx_min,u32 *u32_list_base,u32 u32_list_size);
extern void karacell_hash_xor(u32 hash_base[KARACELL_HASH_U32_COUNT_MAX],u8 hash_u32_count_minus_1,u32 hash_xor_base[KARACELL_HASH_SEED_U32_COUNT_MAX]);
extern void karacell_tail_crypt(u8 decrypt_status,karacell_t *karacell_base,u64 tail_block_idx_minus_1,u32 tail_size,ULONG u32_idx_min,u32 *u32_list_base);
extern u8 karacell_hash_type_get(karacell_t *karacell_base);
extern u8 karacell_hash_xor_all_compare(u32 hash_xor_all_base[KARACELL_HASH_U32_COUNT_MAX],karacell_t *karacell_base);
extern void karacell_hash_xor_all_get(u32 hash_xor_all_base[KARACELL_HASH_U32_COUNT_MAX],karacell_t *karacell_base);
extern void karacell_thread_flush(karacell_t *karacell_base);
extern void karacell_thread_execute(spawn_simulthread_context_t *spawn_simulthread_context_base);
extern void karacell_spawn(ULONG block_count_minus_1,karacell_t *karacell_base);
extern void karacell_crypt_spawn(ULONG block_count_minus_1,u64 block_idx_min_minus_1,u8 decrypt_status,karacell_t *karacell_base,ULONG u32_idx_min,u32 *u32_list_base);
extern void karacell_prefab_spawn(ULONG block_count_minus_1,u64 block_idx_min_minus_1,karacell_t *karacell_base);
extern void karacell_xor_spawn(ULONG block_count_minus_1,u64 block_idx_min_minus_1,u8 decrypt_status,karacell_t *karacell_base,ULONG u32_idx_min,u32 *u32_list_base);
extern void karacell_header_crypt(karacell_t *karacell_base);
extern u8 karacell_header_make(u32 *hash_size_base,u8 hash_type,karacell_t *karacell_base,u64 payload_size);
extern void karacell_iv_set(karacell_t *karacell_base,u32 iv_base[KARACELL_IV_U32_COUNT],ULONG iv_idx_min);
extern u8 karacell_header_decrypt(u64 *file_size_base,u32 *hash_size_base,karacell_t *karacell_base);
extern u8 karacell_header_size_check(u64 file_size);
extern void karacell_master_key_set(karacell_t *karacell_base,u32 master_key_base[KARACELL_MASTER_KEY_U32_COUNT_MAX],ULONG master_key_idx_min,u8 tumbler_idx_max);
extern void karacell_master_key_xor(karacell_t *karacell_base,u32 master_key_xor_base[KARACELL_MASTER_KEY_U32_COUNT_MAX],ULONG master_key_xor_idx_min);
extern u8 karacell_tumbler_idx_max_get(u32 master_key_base[KARACELL_MASTER_KEY_U32_COUNT_MAX]);
extern void *karacell_free(void *base);
extern void *karacell_malloc(ULONG size_minus_1);
extern void *karacell_realloc(void *base,ULONG size_minus_1);
extern void karacell_block_list_free(karacell_t *karacell_base);
extern ULONG karacell_list_size_get(u64 idx_max,u32 item_size_minus_1);
extern ULONG karacell_block_list_realloc(u64 block_idx_max_new,karacell_t *karacell_base);
extern karacell_t *karacell_free_all(karacell_t *karacell_base);
extern karacell_thread_t *karacell_thread_list_make(void);
extern u8 karacell_verify(void);
extern void karacell_rewind(karacell_t *karacell_base);
extern karacell_t *karacell_init(void);