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
#define KARACELL_BLOCK_SIZE (1<<KARACELL_BLOCK_SIZE_LOG2)
#define KARACELL_BLOCK_SIZE_LOG2 (KARACELL_BLOCK_U32_COUNT_LOG2+U32_SIZE_LOG2)
#define KARACELL_BLOCK_U16_COUNT (1<<(KARACELL_BLOCK_U32_COUNT_LOG2+1))
#define KARACELL_BLOCK_U32_COUNT (1<<KARACELL_BLOCK_U32_COUNT_LOG2)
#define KARACELL_BLOCK_U32_COUNT_LOG2 10
#define KARACELL_BLOCK_U64_COUNT (1<<KARACELL_BLOCK_U64_COUNT_LOG2)
#define KARACELL_BLOCK_U64_COUNT_LOG2 9
/*
See mathematica.txt for KARACELL_COMPLEXITY_LOG2_LIST.
*/
#define KARACELL_COMPLEXITY_LOG2_LIST \
  15,28,41,53,65,76,87,99,109,120,131,141,152,162,172,182,192,202,212,221, \
  231,241,250,260,269,278,288,297,306,315,324,333,342,351,360,369,378,386, \
  395,404,412,421,430,438,447,455,464,472,481,489,497,506,514
#define KARACELL_COMPLEXITY_LOG2_LIST_U16_COUNT (KARACELL_TUMBLER_COUNT_MAX>>1)
#define KARACELL_HASH_SEED_SIZE_MAX (KARACELL_HASH_SEED_U32_COUNT_MAX<<U32_SIZE_LOG2)
#define KARACELL_HASH_SEED_U32_COUNT_MAX (LMD7_U32_COUNT<<1)
#define KARACELL_HASH_SIZE_MAX (KARACELL_HASH_U32_COUNT_MAX<<U32_SIZE_LOG2)
#define KARACELL_HASH_U32_COUNT_MAX LMD7_U32_COUNT
#define KARACELL_MASTER_KEY_U32_COUNT_MAX 16
#define KARACELL_TABLE_SIZE (1<<KARACELL_TABLE_SIZE_LOG2)
#define KARACELL_TABLE_SIZE_LOG2 (KARACELL_TABLE_U64_COUNT_LOG2+U64_SIZE_LOG2)
#define KARACELL_TABLE_U64_COUNT (1<<KARACELL_TABLE_U64_COUNT_LOG2)
#define KARACELL_TABLE_U64_COUNT_LOG2 (KARACELL_BLOCK_U64_COUNT_LOG2+1)
#define KARACELL_TUMBLER_BIAS_CRYPT 0
#define KARACELL_TUMBLER_BIAS_HASH 0x8000
#define KARACELL_TUMBLER_COUNT_MAX 106
#define KARACELL_TUMBLER_COUNT_MIN 22

TYPEDEF_START
/*
None of these fields are trustworthy, and all of them are required to be indistinguishable from noise. This structure must be a multiple of U64_SIZE for processing efficiency reasons, even though we measure its size in terms of (u32)s for convenience.
*/
  #define KARACELL_HEADER_U32_COUNT 14
  #define KARACELL_HEADER_U16_COUNT (KARACELL_HEADER_U32_COUNT<<1)
  #define KARACELL_HEADER_SIZE (KARACELL_HEADER_U32_COUNT<<U32_SIZE_LOG2)
/*
If the IV field is wrong, then hash_xor_all (the hash footer after decryption, if present) won't work out. (If the hash isn't present, which is legal but potentially unsafe, main.c spits out an error upon decryption.) This field is source from sourced from something analagous to entropy_iv_make(), so it looks like noise.

Everything beyond the IV is encrypted with block_idx==0 (block_idx==1 being the first data block), as though must_decrypt_to_zero were located at offset 0 of block 0, so that takes care of the noise requirement. In order to minimize communication latency, the fields are not covered by hash_xor_all, so code should behave under the assumption that they may have been maliciously modified.
*/
  #define KARACELL_IV_SIZE (IV_U32_COUNT<<U32_SIZE_LOG2)
  #define KARACELL_IV_U32_COUNT 8
  u32 iv[KARACELL_IV_U32_COUNT];
  #define KARACELL_BLOCK0_U32_COUNT (KARACELL_HEADER_U32_COUNT-KARACELL_IV_U32_COUNT)
  #define KARACELL_BLOCK0_SIZE (KARACELL_BLOCK0_U32_COUNT<<U32_SIZE_LOG2)
/*
Obviously, must_decrypt_to_zero must decrypt to 0. It's necessary because the receiver needs some means by which to rapidly determine the sender's identity with “good” probability (with the hash providing a definitive but comparatively latent answer), while maintaining the deniability requirement. Think of it as a hash of the sender's ID, such that if it's hacked in transit, the only cost is a failed transmission.

There is a small chance that 2 or more peers' keys will result in a 0 in this field, in which case the file must be treated as though lost in transit, in which case it could be resent using a new IV, thereby eventually resolving the conflict. Alternatively, the file could be decrypted with multiple keys, hoping for an eventual match on the hash.

*/
  u64 must_decrypt_to_zero;
/*
size_following is the number of bytes after itself, which is 0 for a null payload, and may take any value (so watch out for overflow). If it doesn't equal at least the size received from the IO stack, after adding space for hash_xor_all, then discard the file. hash_xor_all is (indirectly) sensitive to every bit in this field by virtue of karacell_tail_crypt(). This means that the hash of files with different lengths but the same nonzero content have radically different values for hash_xor_all. So malicious modification of this field should only be able to cause us to deem the file untrustworthy, if not corrupt.
*/
  u64 size_following;
  #define KARACELL_BLOCK0_SIZE_AFTER_SIZE_FOLLOWING (KARACELL_BLOCK0_SIZE-(U64_SIZE<<1))
/*
build_number_of_creator is the Karacell build number used to encrypt the file. It's just a guideline so that the user can be informed if an upgrade appears to be available, with the understanding that it isn't trustworthy.
*/
  u16 build_number_of_creator;
/*
build_number_needed_to_decode is the minimum Karacell build number which can decode this file. Because it's untrustworthy, it might be erroneously greater than build_number_of_creator, or 0, or some wrong number. It's merely designed to save time, in that the receiver can more quickly know that the file can't be decoded, without going to the trouble of evaluating the hash.
*/
  u16 build_number_needed_to_decode;
/*
reserved_zero must be ignored by the receiver, except for bits which the receiver understands. It's not trustworthy, which future definitions must take into account. Performance hints would be a good use.
*/
  u16 reserved_zero;
/*
KARACELL_HASH_TYPE_UNKNOWN is reserved for internal use. Violation of this policy must result in rejection of the Karacell file. It is possible, however, that a valid but unrecognized hash_type may be present, in which case the file can be decrypted, but the user must be warned of the authentication failure. To add new hash types, please contact Tigerspike at the address in the file header; otherwise we will have code collisions (which would not be a security risk, to the extent that hashes are strong, but would indeed be a mess).
*/
  u8 hash_type;
  #define KARACELL_HASH_TYPE_UNKNOWN 0
  #define KARACELL_HASH_TYPE_NONE 1
  #define KARACELL_HASH_TYPE_LMD8 2
  #define KARACELL_HASH_TYPE_LMD7 3
  #define KARACELL_HASH_TYPE_MAX 3
/*
hash_u32_count_minus_1 is 1 less than the number of (u32)s in the encrypted hash footer. The exception is when hash_type==KARACELL_HASH_TYPE_NONE, in which case this field must be 0 and actually means that there is no hash. This policy ensures that (1) power-of-2 hash sizes can be supported up to a kilobyte (which in theory at some point could allow for some short metainfo as well) and (2) there is only one well-defined way to not have a hash.
*/
  u8 hash_u32_count_minus_1;
TYPEDEF_END(karacell_header_t)

TYPEDEF_START
  u8 tumbler_collision_bitmap[KARACELL_TABLE_SIZE];
  u32 hash_seed[KARACELL_HASH_SEED_U32_COUNT_MAX];
  u32 hash[KARACELL_HASH_U32_COUNT_MAX];
  u32 hash_xor[KARACELL_HASH_U32_COUNT_MAX];
  u32 iterand[KARACELL_MASTER_KEY_U32_COUNT_MAX];
  u16 tumbler_list[KARACELL_TUMBLER_COUNT_MAX];
TYPEDEF_END(karacell_thread_t)

TYPEDEF_START
  u64 crypt_block_idx_min_absolute;
  u32 hash_xor_all[KARACELL_HASH_U32_COUNT_MAX];
  u32 master_key[KARACELL_MASTER_KEY_U32_COUNT_MAX];
  u32 **block_base_list_base;
  u32 *crypt_u32_list_base;
  karacell_header_t *header_base;
  spawn_t *spawn_base;
  karacell_thread_t *thread_list_base;
  ULONG block_idx_max;
  ULONG crypt_block_count;
  ULONG crypt_u32_idx_min;
  u32 crypt_thread_idx_max;
  u32 hash_seed_u32_count;
  u8 crypt_mode;
    #define KARACELL_MODE_DECRYPT_SLOW 1
    #define KARACELL_MODE_ENCRYPT_SLOW 2
    #define KARACELL_MODE_PREFAB 3
    #define KARACELL_MODE_DECRYPT_FAST 4
    #define KARACELL_MODE_ENCRYPT_FAST 5
  u8 hash_type_unencrypted;
  u8 hash_u32_count_minus_1_unencrypted;
  u8 tumbler_idx_max;
TYPEDEF_END(karacell_t)
