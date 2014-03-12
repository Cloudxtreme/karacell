extern void entropy_iv_make(u32 entropy_list_base[KARACELL_MASTER_KEY_U32_COUNT_MAX],u64 file_idx,u32 *iv_base,ULONG iv_idx_min,karacell_t *karacell_base);
extern void entropy_make(u32 entropy_list_base[KARACELL_MASTER_KEY_U32_COUNT_MAX]);
extern void entropy_thread_execute(spawn_simulthread_context_t *spawn_simulthread_context_base);
extern void entropy_make_flush(spawn_t *spawn_base);
extern spawn_t *entropy_make_spawn(u32 entropy_list_base[KARACELL_MASTER_KEY_U32_COUNT_MAX]);
