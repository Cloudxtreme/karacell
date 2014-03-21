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
Listcrypt (extern)al Prototype Declaration. See listcrypt.c for full interface descriptions.
*/
extern void listcrypt_header_get(karacell_header_t *header_base,ULONG u32_idx_min,u32 *u32_list_base);
extern u8 listcrypt_prepare(u8 *decrypt_status_base,u8 hash_type,karacell_t *karacell_base);
extern u8 listcrypt_encrypt_prepare(u32 entropy_list_base[KARACELL_MASTER_KEY_U32_COUNT_MAX],u64 file_idx,u32 *hash_size_base,u8 hash_type,karacell_t *karacell_base,u64 payload_size);
extern u8 listcrypt_hash_check(u8 *hash_type_base,karacell_t *karacell_base);
extern void listcrypt_u8_to_u32_list_copy(u32 u8_count_minus_1,ULONG u8_idx_min,u8 *u8_list_base,ULONG u32_idx_min,u32 *u32_list_base);
extern void listcrypt_u32_to_u8_list_copy(u32 u8_count_minus_1,ULONG u8_idx_min,u8 *u8_list_base,ULONG u32_idx_min,u32 *u32_list_base);
extern u8 listcrypt_u8_list_crypt(u32 entropy_list_base[KARACELL_MASTER_KEY_U32_COUNT_MAX],u64 file_idx,karacell_t *karacell_base,u32 *hash_size_base,u8 *hash_type_base,ULONG *u8_count_base,ULONG u8_idx_min,u8 *u8_list_base,u8 *unauthenticated_status_base);
extern void listcrypt_u16_to_u32_list_copy(u32 u16_count_minus_1,ULONG u16_idx_min,u16 *u16_list_base,ULONG u32_idx_min,u32 *u32_list_base);
extern void listcrypt_u32_to_u16_list_copy(u32 u16_count_minus_1,ULONG u16_idx_min,u16 *u16_list_base,ULONG u32_idx_min,u32 *u32_list_base);
extern u8 listcrypt_u16_list_crypt(u32 entropy_list_base[KARACELL_MASTER_KEY_U32_COUNT_MAX],u64 file_idx,karacell_t *karacell_base,u32 *hash_size_base,u8 *hash_type_base,ULONG *u16_count_base,ULONG u16_idx_min,u16 *u16_list_base,u8 *unauthenticated_status_base);
extern u8 listcrypt_u32_list_crypt(u32 entropy_list_base[KARACELL_MASTER_KEY_U32_COUNT_MAX],u64 file_idx,karacell_t *karacell_base,u32 *hash_size_base,u8 *hash_type_base,ULONG *u32_count_base,ULONG u32_idx_min,u32 *u32_list_base,u8 *unauthenticated_status_base);
