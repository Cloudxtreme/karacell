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
/*
IV Management for Karacell

The functions in this file are not part of the Karacell specification, but they do offer a suggested safe way of managing initial values (nonces).
*/
extern u32 true_random_get(void);

void
entropy_iv_make(u32 entropy_list_base[KARACELL_MASTER_KEY_U32_COUNT_MAX],u64 file_idx,u32 *iv_base,ULONG iv_idx_min,karacell_t *karacell_base){
/*
Make an IV which appears to be random, but actually derives deterministically from an entropy list. (The intent is to preempt attacks which seek to learn the sequence in which files were encrypted, or by whom, by looking at the IV fields therein.)

This function is threadsafe, as it does not call true_random_get(). However, it cannot be used in parallel with other clients of *karacell_base.

In:

  *entropy_list_base is as defined in in entropy_make():Out. Do not use entropy from an untrusted source. You can always modify true_random_get() to your satisfaction.

  file_idx is the index of the file whose IV is to be derived from *entropy_list. It is suggested that, in the event that a file must be resent due to error recovery, it be encrypted with a different value in this field. In not, then there is a small chance that the file was received intact, but its karacell_header.must_decrypt_to_zero just happened to work out with the master key of the wrong peer, leaving the hash_xor_all invalid, thereby creating the appearance of bitrot, when in fact the wrong master key was tried. (Trying master keys in order of recentcy can mitigate the incidence of this eventuality.) In such a case, resending the file without changing its IV could result in the persistent inability to complete the transfer. This whole problem stems from the requirement that a Karacell file be indistinguishable from noise, and the desire to keep the header as small as possible. (The intent of this field is to allow a sender to send the entropy list to a receiver, then send many files with IVs derived therefrom. That way, the receiver can perform speculative cryption mask prefabrication in order to accelerate the decryption process, even across a stream consisting of many different files. But if you do this, then remember that the Karacell file which transports the entropy list itself must have a unique file_idx. For example, the entropy list file could have file_idx==0, and the first data file could have file_idx==1.) In practice, receiving a file with an unrecognized (unforeseen) IV should prompt the receiver to treat it as a special file containing an entropy pool for the sake of renewing the handshake. If it fails to authenticate as such, then it should be ignored because it could be a replay attack, and in any case the receiver should send back a newly generated (i.e. not read from storage) encrypted entropy pool. Ultimately, the actual entropy pool used to generate IVs should be the xor of the freshly generated local pool with one received from the peer, which is immune to replay, even in the event of a power failure on the local end. (It's possible that sending many entropy pools in succession could cause confusion as to which is the most recent -- and might even provide for a crude denial-of-service attack -- given the vagaries of out-of-order packet arrival. But this paradox is easily resolved by educated guessing, starting with the assumption that the arrival was in-order.) Note that replay immunity implies that IVs (or encrypted block numbers of some proprietary form) must be blacklisted between handshakes, so as to not be receivable more than once. This is extremely important, if you consider the implications for debiting the same bank account multiple times by sending the same Karacell file repeatedly (without even knowning the key!), for example. Finally, note that denial-of-service is neither preventable nor deterministically detectable, so to the extent that this proposed handshaking mechanism allows for such attacks, only higher level reasoning can assist, but nonetheless cannot completely eliminate the problem.

  *iv_base is the hull which will contain an IV consisting of KARACELL_IV_U32_COUNT (u32)s.

  iv_idx_min offsets iv_base.

  *karacell_base is as defined in karacell_master_key_set():Out.

Out:

  *iv_base is the IV to be stored in the Karacell header of the file with index file_idx. A single bit difference in the IV from one file to another is sufficiently secure insofar as encryption is concerned. (However, there are other risks beyond the scope of Karacell, such as an attacker knowing which files were generated in what order by a given author, assuming that the IV is merely incremented from one file to the next. Hence this function.) See also notes on file_idx in In.

  *karacell_base is changed:

    header_base->iv is undefined. Normally, this function will be followed by karacell_iv_set() (but not if the caller is merely generating an IV list for future precryption.)

    master_key_backup is undefined.

    thread_list_base[0].iterand is undefined.

    thread_list_base[0].tumbler_collision_bitmap is unchanged, but was modified during the execution of this function and is therefore unsuitable for sharing among threads.

    thread_list_base[0].tumbler_list is undefined.
*/
  karacell_thread_t *thread_base;

  #if (KARACELL_MASTER_KEY_U32_COUNT_MAX<=KARACELL_IV_U32_COUNT)
    #error "entropy_iv_make() does not have enough entropy to safely generate an IV."
  #endif
/*
Set the fake IV to 0. This fake IV is to be used to generate a real IV. We already have enough entropy at entropy_list_base to do the job. Leaving *iv_base undefined would be unwise, since it would mask the bias of true_random_get() with the pseudorandomness of memory contents. So in the spirit of exposing weakness wherever possible, we set it to 0.
*/
  karacell_u32_list_zero(KARACELL_IV_U32_COUNT-1,iv_idx_min,iv_base);
  karacell_iv_set(karacell_base,iv_base,iv_idx_min);
/*
Use *entropy_list_base xored to the master key as a temporary master key for producing a cryption mask. The cryption mask will then serve as the IV. We mix in the master key in this process because, otherwise, if the TRNG were weak, then the same IVs would occur more often in the wild, which would violate the requirement that Karacell files be indistinguishable from noise. (This is to ameliorate the case where the TRNG is weak, but not weak enough that the above IV zeroing would cause the weakness to be detected, in which case the biased IVs would go public, and presumably be compared to other IVs from other users at a future time.)
*/
  karacell_master_key_xor(karacell_base,entropy_list_base,0);
/*
Make a tumbler list in preparation for crpytion. Set block_idx==file_idx. Implicitly, use the same karacell_base->tumbler_idx_max as with normal block cryption, because there is no advantage to using more tumblers, but it might not be safe to use fewer. Borrow the first item at thread_list_base because we're monothreaded at the moment, and karacell_init() and karacell_block_list_realloc() both guarantee that at least the first one will be there.
*/
  thread_base=&karacell_base->thread_list_base[0];
  karacell_tumbler_list_make(file_idx,karacell_base,thread_base);
/*
Encrypt the 0s at iv_base using the modified master key in order to yield a (real) IV.
*/
  karacell_subblock_crypt(karacell_base,thread_base,KARACELL_TUMBLER_BIAS_CRYPT,KARACELL_IV_U32_COUNT-1,0,iv_base);
/*
Restore the master key.
*/
  karacell_master_key_xor(karacell_base,entropy_list_base,0);
  return;
}

void
entropy_make(u32 entropy_list_base[KARACELL_MASTER_KEY_U32_COUNT_MAX]){
/*
Populate an entropy list in preparation for entropy_iv_make(). For faster performance, see entropy_make_spawn().

WARNING: See warning in true_random_get().

In:

  *entropy_list_base is undefined.

Out:

  *entropy_list_base contains at least enough entropy to generate a (hopefully) unbiased IV.
*/
  u32 entropy_idx;
  u32 one_count;
  u32 random;
  u32 random_prev;
  u32 random_xor;
/*
There is a not-so-insignificant chance that despite the warning above, the TRNG output has gone into a failure mode where its output is constant, perhaps due to a linker problem or stuck hardware. We can't detect all failure modes, but at least let's look out for this obvious and most dangerous one.

We can do a bit more by way of ensuring safety. We'll look at the xors of successive outputs from true_random_get(), and count the 1s therein. This will allow us to know when we're lacking in sufficient entropy to exit (but of course we can never really know if we have enough, because it's easy to generate entropy that appears to be of high quality, but isn't).

Failing either of the above tests (which is equivalent to failing just the second test) would cause us to retry the TRNG indefinitely before proceeding. In theory, this could hang the thread. Good. The user needs to know if they have a security hole, and a persistent failure of this nature is equivalent to hardware failure.
*/
  do{
    entropy_idx=0;
    one_count=0;
    random_prev=0;   
    do{
      random=true_random_get();
      random_xor=random^random_prev;
      random_prev=random;
      while(random_xor){
        one_count+=random_xor&1;
        random_xor>>=1;
      }
      entropy_list_base[entropy_idx]=random;
      entropy_idx++;
/*
In theory, we only need to generate enough entropy to equal the IV size. But since the *entropy_list is necessarily larger than that, due to the way entropy_iv_make() works, we might as well fully populate it.
*/
    }while(entropy_idx!=KARACELL_MASTER_KEY_U32_COUNT_MAX);
/*
The most entropic IVs have equal numbers of 0s and 1s, namely, (KARACELL_IV_U32_COUNT<<U16_BITS_LOG2) of them. If we have at least this many of each, then we have enough entropy to proceed safely. If not, then try again. (But don't be confused. Just because we have enough entropy here doesn't mean that we can only generate IVs with equal numbers of 0s and 1s.)
*/
  }while((one_count<(KARACELL_IV_U32_COUNT<<U16_BITS_LOG2))||(one_count>((KARACELL_MASTER_KEY_U32_COUNT_MAX<<U32_BITS_LOG2)-(KARACELL_IV_U32_COUNT<<U16_BITS_LOG2))));
  return;
}

void
entropy_thread_execute(spawn_simulthread_context_t *spawn_simulthread_context_base){
/*
Populate *entropy_list in its own thread, so that entropic stuff like OS calls can happen in parallel, thereby increasing throughput due to faster entropy accrual. This function is merely a hook for Spawn, should one wish to include it in the build.

WARNING: See warning in true_random_get().

In:

  *spawn_simulthread_context_base->readonly_string_base is is as defined in entropy_make():In:*entropy_list_base.

Out:

  Returns via Spawn.

  *spawn_simulthread_context_base->readonly_string_base is is as defined in entropy_make():Out:*entropy_list_base. (We can write to *readonly_string_base because we're required to be monothreaded.)
*/
  entropy_make((u32 *)(spawn_simulthread_context_base->readonly_string_base));
  return;
}

void
entropy_make_flush(spawn_t *spawn_base){
/*
Force the entropy_make() thread to finish.

In:

  spawn_base is the nonnull return value of entropy_make_spawn().

Out:

  entropy_make() has finished.

  *spawn_base has been freed.
*/
  SPAWN_RETIRE_ALL(spawn_base);
  SPAWN_FREE(spawn_base);
  return;
}

spawn_t *
entropy_make_spawn(u32 entropy_list_base[KARACELL_MASTER_KEY_U32_COUNT_MAX]){
/*
Spawn entropy_make() into its own thread, which will increase timing entropy and allow it to execute faster.

WARNING: See warning in true_random_get().

In:

  *entropy_list_base is as defined in entropy_make():In.

Out:

  Returns NULL if we're out of memory, else a spawn_t for a follow up call to entropy_make_flush(), which must occur before any other Entropy call.

  *entropy_list_base is as defined in entropy_make():Out.
*/
  spawn_t *spawn_base;

  spawn_base=SPAWN_INIT(&entropy_thread_execute,(u8 *)(entropy_list_base),0);
  if(spawn_base){
    SPAWN_ONE(spawn_base,0);
  }
  return spawn_base;
}
