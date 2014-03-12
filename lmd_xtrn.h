extern u64 lmd_string_lmd2_custom_get(u32 lmd_c0,u32 lmd_x0,u8 *string_base,ULONG string_idx_min,u32 string_size);
extern void lmd_block_lmd7_get(u32 block_base[LMD7_BLOCK_U32_COUNT],ULONG block_idx_min,u32 p_base[LMD7_U32_COUNT<<1],u32 z_base[LMD7_U32_COUNT]);
extern void lmd_block_lmd8_get(u32 block_base[LMD8_BLOCK_U32_COUNT],ULONG block_idx_min,u32 p_base[LMD8_U32_COUNT<<1],u32 z_base[LMD8_U32_COUNT]);
