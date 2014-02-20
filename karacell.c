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
Karacell 3
*/
const u16 karacell_complexity_log2_list_base[KARACELL_COMPLEXITY_LOG2_LIST_U16_COUNT]={KARACELL_COMPLEXITY_LOG2_LIST};
const u64 karacell_table_base[KARACELL_TABLE_U64_COUNT]={KARACELL_TABLE};
#ifdef DEBUG
  static u64 karacell_allocation_count=0;
#endif

void
karacell_u32_list_zero(u32 u32_count_minus_1,ULONG u32_idx_min,u32 *u32_list_base){
/*
Zero a contiguous list of (u32)s. Note that this function is intended for zeroing small structures; by design, it is NOT ULONG-abstract with respect to object size (but it is, with respect to object base index).

In:

  u32_count_minus_1 is number of (u32)s to set to 0, less 1, on [0,(ULONG_MAX-1)-u32_idx_min].

  u32_idx_min is the first index to 0.

  *u32_list_base is writable for (u32_count_minus_1+1) (u32)s starting at index u32_idx_min.

Out:

  *u32_list_base is 0 for all indexes its writable domain.
*/
  ULONG u32_idx;
  ULONG u32_idx_max;

  u32_idx_max=u32_idx_min+u32_count_minus_1;
/*
We expect the compiler to unroll this loop and aggregate stores. It's guaranteed to terminate because u32_idx_max!=ULONG_MAX. memset() causes a host of compiler problems and is less portable.
*/
  for(u32_idx=u32_idx_min;u32_idx<=u32_idx_max;u32_idx++){
    u32_list_base[u32_idx]=0;
  }
  return;
}

void
karacell_u32_list_copy(ULONG in_u32_idx_min,u32 *in_u32_list_base,ULONG out_u32_idx_min,u32 *out_u32_list_base,u32 u32_count_minus_1){
/*
Copy a contiguous list of (u32)s. Note that this function is intended for copying small structures; by design, it is NOT ULONG-abstract with respect to object size (but it is, with respect to object base index).

In:

  in_u32_idx_min is the index of the first u32 to read at in_u32_list_base.

  in_u32_list_base is the base of the source u32 list, writable for (u32_count_minus_1+1) (u32)s starting at index in_u32_idx_min.

  out_u32_idx_min is the index of the first u32 to write at out_u32_list_base.

  out_u32_list_base is the base of the target u32 list, writable for (u32_count_minus_1+1) (u32)s starting at index out_u32_idx_min.

  u32_count_minus_1 is number of (u32)s to copy, less 1, on [0,(ULONG_MAX-1)-MAX(in_u32_idx_min,out_u32_idx_min)].

Out:

  *out_u32_list_base[out_u32_idx_min] is identical to in_u32_list_base[in_u32_idx_min] for all (u32_count_minus_1+1) (u32)s.
*/
  ULONG in_u32_idx;
  ULONG out_u32_idx;
  ULONG out_u32_idx_max;

  out_u32_idx_max=out_u32_idx_min+u32_count_minus_1;
  in_u32_idx=in_u32_idx_min;
/*
We expect the compiler to unroll this loop and aggregate stores. It's guaranteed to terminate because out_u32_idx_max!=ULONG_MAX. memcpy() causes a host of compiler problems and is less portable.
*/
  for(out_u32_idx=out_u32_idx_min;out_u32_idx<=out_u32_idx_max;out_u32_idx++){
    out_u32_list_base[out_u32_idx]=in_u32_list_base[in_u32_idx];
    in_u32_idx++;
  }
  return;
}

void
karacell_marsaglia_iterate(u32 iterand_base[KARACELL_MASTER_KEY_U32_COUNT_MAX]){
/*
Iterate the Marsaglia oscillator which is used for various pseudorandom purposes.

Background info:

  http://leidich-message-digest.blogspot.com/2013/02/visualizing-marsaglia-oscillators.html
  http://leidich-message-digest.blogspot.com/2013/02/information-loss-in-marsaglia.html
  http://leidich-message-digest.blogspot.com/2013/02/the-marsaglia-limit-cycle-theorem.html
  http://leidich-message-digest.blogspot.com/2013/02/harder-than-diehard-scintillating.html

In:

  *iterand_base contains I, which is the 512-bit iterand to iterate. It must not be 0 or (((A-1)<<256)+((1<<256)-1)) where:

      A=((2^256)-0xE66FAD12)

    Provided that the system is not subject to outside influence with respect to iterand selection, which would be a primary requirement for encryption to provide any security whatsoever, then there is no point in checking for these ultrarare bad cases, which iterate back to themselves. The reasoning behind the selection of A is explained in Out.

Out:

  *iterand_base contains F(A,I):

      F=((A*(I&((1<<256)-1)))+(I>>256))

    iterated 10X, where A is as defined on In. The value of A was chosen so that:

      P=(((2^255)*A)-1)

    is a Sophie Germain prime:

      http://www.wolframalpha.com/input/?i=isprime%28%28%282%5E255%29*%28%282%5E256%29-3866078482%29%29-1%29
      http://www.wolframalpha.com/input/?i=isprime%28%28%282%5E256%29*%28%282%5E256%29-3866078482%29%29-1%29

    which is sufficient for a 512-bit Marsaglia multiply-with-carry oscillator.

    P is also the period of the oscillator, which, because it's prime, is unaffected by the fact that we take 10 iterations at a time. Note that log2(P) is just about 1 bit less than the maximum key length supported by Karacell, which (hopefully) implies that attacking this oscillator is comparable to the difficulty of trying every key.

    Granted, the high bit of F(A,I) is slightly biased toward 0, with a ratio of about (2^256)/A, which is about (1+(3x10^(-68))). Good luck making use of this bias.
*/
  u32 carry0;
  u32 carry1;
  u32 d;
  u8 i;
  u32 iterand0;
  u32 iterand1;
  u32 iterand2;
  u32 iterand3;
  u32 iterand4;
  u32 iterand5;
  u32 iterand6;
  u32 iterand7;
  u32 iterand8;
  u32 iterand9;
  u32 iterand10;
  u32 iterand11;
  u32 iterand12;
  u32 iterand13;
  u32 iterand14;
  u32 iterand15;
  u64 product;
  u32 x;
  u32 x0;
  u32 x1;
  u32 x2;
  u32 x3;
  u32 x4;
  u32 x5;
  u32 x6;
  u32 x7;

  iterand0=iterand_base[0];
  iterand1=iterand_base[1];
  iterand2=iterand_base[2];
  iterand3=iterand_base[3];
  iterand4=iterand_base[4];
  iterand5=iterand_base[5];
  iterand6=iterand_base[6];
  iterand7=iterand_base[7];
  iterand8=iterand_base[8];
  iterand9=iterand_base[9];
  iterand10=iterand_base[10];
  iterand11=iterand_base[11];
  iterand12=iterand_base[12];
  iterand13=iterand_base[13];
  iterand14=iterand_base[14];
  iterand15=iterand_base[15];
  d=0xE66FAD12UL;
  for(i=0;i<=9;i++){
    product=iterand0*(u64)(d);
    x0=iterand0;
    x=(u32)(product);
    carry0=(iterand8<x);
    carry0+=product>>U32_BITS;
    iterand0=iterand8-x;
    product=(iterand1*(u64)(d))+carry0;
    x1=iterand1;
    x=(u32)(product);
    carry0=(iterand9<x);
    carry0+=product>>U32_BITS;
    iterand1=iterand9-x;
    product=(iterand2*(u64)(d))+carry0;
    x2=iterand2;
    x=(u32)(product);
    carry0=(iterand10<x);
    carry0+=product>>U32_BITS;
    iterand2=iterand10-x;
    product=(iterand3*(u64)(d))+carry0;
    x3=iterand3;
    x=(u32)(product);
    carry0=(iterand11<x);
    carry0+=product>>U32_BITS;
    iterand3=iterand11-x;
    product=(iterand4*(u64)(d))+carry0;
    x4=iterand4;
    x=(u32)(product);
    carry0=(iterand12<x);
    carry0+=product>>U32_BITS;
    iterand4=iterand12-x;
    product=(iterand5*(u64)(d))+carry0;
    x5=iterand5;
    x=(u32)(product);
    carry0=(iterand13<x);
    carry0+=product>>U32_BITS;
    iterand5=iterand13-x;
    product=(iterand6*(u64)(d))+carry0;
    x6=iterand6;
    x=(u32)(product);
    carry0=(iterand14<x);
    carry0+=product>>U32_BITS;
    iterand6=iterand14-x;
    product=(iterand7*(u64)(d))+carry0;
    x7=iterand7;
    x=(u32)(product);
    carry0=(iterand15<x);
    carry0+=product>>U32_BITS;
    iterand7=iterand15-x;
    carry1=(x0<carry0);
    iterand8=x0-carry0;
    carry0=(x1<carry1);
    iterand9=x1-carry1;
    carry1=(x2<carry0);
    iterand10=x2-carry0;
    carry0=(x3<carry1);
    iterand11=x3-carry1;
    carry1=(x4<carry0);
    iterand12=x4-carry0;
    carry0=(x5<carry1);
    iterand13=x5-carry1;
    carry1=(x6<carry0);
    iterand14=x6-carry0;
    iterand15=x7-carry1;
  }
  iterand_base[0]=iterand0;
  iterand_base[1]=iterand1;
  iterand_base[2]=iterand2;
  iterand_base[3]=iterand3;
  iterand_base[4]=iterand4;
  iterand_base[5]=iterand5;
  iterand_base[6]=iterand6;
  iterand_base[7]=iterand7;
  iterand_base[8]=iterand8;
  iterand_base[9]=iterand9;
  iterand_base[10]=iterand10;
  iterand_base[11]=iterand11;
  iterand_base[12]=iterand12;
  iterand_base[13]=iterand13;
  iterand_base[14]=iterand14;
  iterand_base[15]=iterand15;
  return;
}

void
karacell_tumbler_list_make(u64 block_idx,karacell_t *karacell_base,karacell_thread_t *thread_base){
/*
Produce a set of tumblers without any repeats. (Repeated tumblers would undermine Subset Sum, resulting in a simpler problem.) See mathematica.txt for a simulation.

In:

  block_idx is the index of the block whose tumblers to find, where the header starting after the IV is block 0, and the first payload block is 1.

  *karacell_base is as follows:

    header_base->iv contains KARACELL_IV_U32_COUNT (u32)s which are indistinct from true random data, for example, as defined in entropy_iv_make():Out:*iv_base.

    master_key is the master key (as little endian binary, not ASCII), with the unused high bits padded to 0.

    tumbler_idx_max is the number of tumblers to create, less 1, on [0,KARACELL_TUMBLER_COUNT_MAX-1].

  *thread_base is as follows:

    iterand is undefined.

    tumbler_collision_bitmap is all 0s.

    tumbler_list is undefined.

Out:

  *thread_base is as follows:

    iterand is undefined.

    tumbler_collision_bitmap is unchanged. However, it was written during the function, so it cannot be shared among threads.

    tumbler_list contains (tumbler_idx_max+1) unique and effectively unbiased u16 tumblers.
*/
  u32 collision_idx;
  u8 collision_status;
  u8 collision_status_new;
  u32 *iterand_base;
  u32 iterand_idx;
  u32 iterand_u32;
  u32 *iv_base;
  u32 *master_key_base;
  u8 *tumbler_collision_bitmap_base;
  u32 tumbler_idx;
  u8 tumbler_idx_max;
  u16 *tumbler_list_base;
  u32 tumbler_u32;

  iterand_base=&thread_base->iterand[0];
  iv_base=&karacell_base->header_base->iv[0];
  master_key_base=&karacell_base->master_key[0];
  tumbler_collision_bitmap_base=&thread_base->tumbler_collision_bitmap[0];
  tumbler_list_base=&thread_base->tumbler_list[0];
  tumbler_idx_max=karacell_base->tumbler_idx_max;
  #ifdef DEBUG
    for(tumbler_idx=0;tumbler_idx<=(KARACELL_TABLE_SIZE-1);tumbler_idx++){
      if(tumbler_collision_bitmap_base[tumbler_idx]){
        print_error("Tumbler collision bitmap corrupt");
        exit(1);
      }
    }
  #endif
/*
Set *iterand_base as given below with the associated u32 index ranges:

  [0,KARACELL_MASTER_KEY_U32_COUNT_MAX-KARACELL_IV_U32_COUNT-3]: Low (u32)s of *master_key_base.

  [KARACELL_MASTER_KEY_U32_COUNT_MAX-KARACELL_IV_U32_COUNT-2,KARACELL_MASTER_KEY_U32_COUNT_MAX-KARACELL_IV_U32_COUNT-1]: Middle (u32)s of *master_key_base xored with the bitwise reverse of block_idx. This provides for seemless integration of longer block indexes, if needed in the future. It also helps slightly to preserve entropy, as the high bits of the master key are probably more biased than the low bits.

  [KARACELL_MASTER_KEY_U32_COUNT_MAX-KARACELL_IV_U32_COUNT,KARACELL_MASTER_KEY_U32_COUNT_MAX-1]: High (u32)s of *master_key_base xored with *iv_base.
*/
  #if KARACELL_MASTER_KEY_U32_COUNT_MAX<(KARACELL_IV_U32_COUNT+2)
    #error "karacell_tumbler_list_make() has no iterand space left for block_idx."
  #endif
  for(iterand_idx=0;iterand_idx<=(KARACELL_MASTER_KEY_U32_COUNT_MAX-KARACELL_IV_U32_COUNT-3);iterand_idx++){
    iterand_base[iterand_idx]=master_key_base[iterand_idx];
  }
  REVERSE_U64(block_idx);
  iterand_base[KARACELL_MASTER_KEY_U32_COUNT_MAX-KARACELL_IV_U32_COUNT-2]=master_key_base[KARACELL_MASTER_KEY_U32_COUNT_MAX-KARACELL_IV_U32_COUNT-2]^(u32)(block_idx);
  iterand_base[KARACELL_MASTER_KEY_U32_COUNT_MAX-KARACELL_IV_U32_COUNT-1]=master_key_base[KARACELL_MASTER_KEY_U32_COUNT_MAX-KARACELL_IV_U32_COUNT-1]^(u32)(block_idx>>U32_BITS);
  for(iterand_idx=(KARACELL_MASTER_KEY_U32_COUNT_MAX-KARACELL_IV_U32_COUNT);iterand_idx<=(KARACELL_MASTER_KEY_U32_COUNT_MAX-1);iterand_idx++){
    iterand_base[iterand_idx]=master_key_base[iterand_idx]^iv_base[iterand_idx-(KARACELL_MASTER_KEY_U32_COUNT_MAX-KARACELL_IV_U32_COUNT)];
  }
  tumbler_idx=0;
/*
Set iterand_idx so as to force a call to karacell_marsaglia_iterate() before we use *iterand_base as a tumbler generator.
*/
  iterand_idx=KARACELL_MASTER_KEY_U32_COUNT_MAX;
  do{
    if(iterand_idx==KARACELL_MASTER_KEY_U32_COUNT_MAX){
/*
We've taken all the tumblers that the Marsaglia oscillator can provide. It's time to iterate again.
*/
      iterand_idx=0;
      karacell_marsaglia_iterate(iterand_base);
    }
/*
It's kind of annoying that we have an iterator composed of (u32)s, but a tumbler list composed of (u16)s. Deal with this by extracting up to 2 tumblers per loop iteration.
*/
    iterand_u32=iterand_base[iterand_idx];
    iterand_idx++;
    tumbler_u32=(u16)(iterand_u32);
    collision_idx=tumbler_u32>>U8_BITS_LOG2;
    collision_status=tumbler_collision_bitmap_base[collision_idx];
    collision_status_new=(u8)(1<<((u8)(tumbler_u32)&U8_BIT_MAX));
    collision_status_new|=collision_status;
/*
Check for a repeated tumbler, which must be discarded.
*/
    if(collision_status_new!=collision_status){
      tumbler_collision_bitmap_base[collision_idx]=collision_status_new;
      tumbler_list_base[tumbler_idx]=(u16)(tumbler_u32);
      tumbler_idx++;
    }
/*
If we're not done, then try the next tumbler as well.
*/
    if(tumbler_idx<=tumbler_idx_max){
      collision_idx=iterand_u32>>(U16_BITS+U8_BITS_LOG2);
      tumbler_u32=iterand_u32>>U16_BITS;
      collision_status=tumbler_collision_bitmap_base[collision_idx];
      collision_status_new=(u8)(1<<((u8)(tumbler_u32)&U8_BIT_MAX));
      collision_status_new|=collision_status;
      if(collision_status_new!=collision_status){
        tumbler_collision_bitmap_base[collision_idx]=collision_status_new;
        tumbler_list_base[tumbler_idx]=(u16)(tumbler_u32);
        tumbler_idx++;
      }
    }
  }while(tumbler_idx<=tumbler_idx_max);
/*
Clear all the bits we set at *tumbler_collision_bitmap_base, so it's ready to go for next time.
*/
  while(tumbler_idx--){
    collision_idx=tumbler_list_base[tumbler_idx]>>U8_BITS_LOG2;
    tumbler_collision_bitmap_base[collision_idx]=0;
  }
  #ifdef DEBUG
    for(tumbler_idx=0;tumbler_idx<=(KARACELL_TABLE_SIZE-1);tumbler_idx++){
      if(tumbler_collision_bitmap_base[tumbler_idx]){
        print_error("Tumbler collision bitmap corrupt");
        exit(1);
      }
    }
  #endif
  return;
}

void
karacell_subblock_crypt(karacell_t *karacell_base,karacell_thread_t *thread_base,u16 tumbler_bias,u32 u32_count_minus_1,ULONG u32_idx_min,u32 *u32_list_base){
/*
Crypt a subset of a block. See mathematica.txt for a simulation.

In:

  *karacell_base is as follows:

    tumbler_idx_max is the number of tumblers at thread_base->tumbler_list, less 1, on [0,KARACELL_TUMBLER_COUNT_MAX-1].

  *thread_base is as follows:

    *tumbler_list_base is as defined in karacell_tumbler_list_make():Out.

  tumbler_bias is KARACELL_TUMBLER_BIAS_CRYPT if crypting a data string or KARACELL_TUMBLER_BIAS_HASH if making hash seeds. *tumbler_list_base must be identical in either case, for a given block.

  u32_count_minus_1 is the number of (u32)s to crypt, less 1, starting at &u32_list_base[u32_idx_min], on [0,KARACELL_BLOCK_U32_COUNT-1]. If making hash seeds, this value must be karacell_base->hash_seed_u32_count.

  u32_idx_min offsets u32_list_base.

  *u32_list_base is the hull containing the (sub)block to crypt, which is writable for (u32_count_minus_1+1) (u32)s starting at index u32_idx_min.

Out:

  u32_list_base[u32_idx_min] is crypted for (u32_count_minus_1+1) (u32)s.
*/
  u32 carry;
  u64 noise;
  u8 shift_right;
  u64 sum;
  u64 sum_prev;
  u32 table_idx;
  u16 tumbler;
  u32 tumbler_idx;
  u32 tumbler_idx_max;
  u16 *tumbler_list_base;
  ULONG u32_idx;
  ULONG u32_idx_max;

  tumbler_list_base=&thread_base->tumbler_list[0];
  tumbler_idx_max=karacell_base->tumbler_idx_max;
  u32_idx_max=u32_idx_min+u32_count_minus_1;
  u32_idx=u32_idx_min;
  carry=0;
  while(u32_idx<u32_idx_max){
    sum=carry;
    carry=0;
/*
Because the tumbler count is always even, we can pipeline up 2 at a time for better performance.
*/
    for(tumbler_idx=0;tumbler_idx<=tumbler_idx_max;){
      tumbler=tumbler_list_base[tumbler_idx];
      tumbler+=tumbler_bias;
      table_idx=tumbler>>U64_BITS_LOG2;
      noise=karacell_table_base[table_idx];
/*
This increment should be up higher, but the noise load is somewhat likely to result in a cache miss, so we've deferred it until now.
*/
      tumbler_idx++;
      shift_right=tumbler&U64_BIT_MAX;
      if(shift_right){
        table_idx++;
        table_idx&=KARACELL_TABLE_U64_COUNT-1;
        noise>>=shift_right;
        noise+=karacell_table_base[table_idx]<<(U64_BITS-shift_right);
      }
/*
We've computed the noise addition from the first tumbler. Add it to sum, then compute the next noise index ASAP, because it, too, may result in a cache miss. Things are done in a weird order because it increases the probability that the compiler will produce fast code.
*/
      tumbler=tumbler_list_base[tumbler_idx];
      sum_prev=sum;
      tumbler+=tumbler_bias;
      table_idx=tumbler>>U64_BITS_LOG2;
      sum+=noise;
      noise=karacell_table_base[table_idx];
      tumbler_idx++;
      carry+=(sum<sum_prev);
      shift_right=tumbler&U64_BIT_MAX;
      if(shift_right){
        table_idx++;
        table_idx&=KARACELL_TABLE_U64_COUNT-1;
        noise>>=shift_right;
        noise+=karacell_table_base[table_idx]<<(U64_BITS-shift_right);
      }
      sum_prev=sum;
      sum+=noise;
      carry+=(sum<sum_prev);
    }
    u32_list_base[u32_idx]^=sum;
    u32_list_base[u32_idx+1]^=sum>>U32_BITS;
    u32_idx+=2;
    tumbler_bias+=U64_BITS;
  }
  if(!(u32_count_minus_1&1)){
    for(tumbler_idx=0;tumbler_idx<=tumbler_idx_max;tumbler_idx++){
      tumbler=tumbler_list_base[tumbler_idx]+tumbler_bias;
      table_idx=tumbler>>U64_BITS_LOG2;
      noise=karacell_table_base[table_idx];
      shift_right=tumbler&U64_BIT_MAX;
      if(shift_right){
        table_idx++;
        table_idx&=KARACELL_TABLE_U64_COUNT-1;
        noise>>=shift_right;
        noise+=karacell_table_base[table_idx]<<(U64_BITS-shift_right);
      }
      carry+=noise;
    }
    u32_list_base[u32_idx_max]^=carry;
  }
  return;
}

void
karacell_hash_get(u8 hash_seed_ready_status,karacell_t *karacell_base,karacell_thread_t *thread_base,ULONG u32_idx_min,u32 *u32_list_base,u32 u32_list_size){
/*
Compute the hash of a Karacell block, which is considered to consist of 1 or more hash blocks.

In:

  *karacell_base has passed through karacell_header_decrypt() or karacell_header_make().

  hash_seed_ready_status is 1 if the hash seed at thread_base->hash_seed is ready for use, else 0 in order to generate them from thread_base->tumbler_list.

  *thread_base is as defined in karacell_subblock_crypt():In.

  u32_idx_min offsets u32_list_base.

  *u32_list_base is the base of hull containing the block of size KARACELL_BLOCK_SIZE whose hash to evaluate, starting at index u32_idx_min, only the first u32_list_size bytes of which are defined.

  u32_list_size is as defined in *u32_list_base, on [0,KARACELL_BLOCK_SIZE]. Must be KARACELL_BLOCK_SIZE if this is not the last block. 0 is only valid for a null payload.

Out:

  *u32_list_base is unchanged for the first u32_list_size bytes. If u32_list_size!=KARACELL_BLOCK_SIZE, then it's padded with high 0s accordingly.

  *thread_base is as follows:

    hash is xor of all the hashes of the hash blocks at *block_base, such that *block_base is always considered to have size KARACELL_BLOCK_SIZE. The hash consists of (karacell_base->hash_u32_count_minus_1_unencrypted+1) (u32)s.

    hash_seed is undefined.
*/
  u32 *hash_base;
  u32 *hash_seed_base;
  u8 hash_type;
  u32 hash_seed_u32_count;
  u32 hash_seed_u32_count_minus_1;
  u32 pad_u32_count;
  u32 pad_u32_count_minus_1;
  u32 tail;
  u32 tail_mask;
  u8 tail_size;
  ULONG tail_u32_idx;
  ULONG u32_count;

  hash_seed_base=&thread_base->hash_seed[0];
  if(!hash_seed_ready_status){
    hash_seed_u32_count=karacell_base->hash_seed_u32_count;
    if(hash_seed_u32_count){
      hash_seed_u32_count_minus_1=hash_seed_u32_count-1;
/*
The hash seeds are just the crypt of 0, as computed with tumbler_bias==KARACELL_TUMBLER_BIAS_HASH.
*/
      karacell_u32_list_zero(hash_seed_u32_count_minus_1,0,hash_seed_base);
      karacell_subblock_crypt(karacell_base,thread_base,KARACELL_TUMBLER_BIAS_HASH,hash_seed_u32_count_minus_1,0,hash_seed_base);
    }
  }
  if(u32_list_size!=KARACELL_BLOCK_SIZE){
    u32_count=u32_list_size>>U32_SIZE_LOG2;
    tail_u32_idx=u32_idx_min+u32_count;
    tail_size=u32_list_size&U32_BYTE_MAX;
    u32_count+=!!tail_size;
    pad_u32_count=KARACELL_BLOCK_U32_COUNT-(u32)(u32_count);
    if(pad_u32_count){
      pad_u32_count_minus_1=pad_u32_count-1;
      u32_count+=u32_idx_min;
      karacell_u32_list_zero(pad_u32_count_minus_1,u32_count,u32_list_base);
    }
    tail=u32_list_base[tail_u32_idx];
    tail_mask=(1<<(tail_size<<U8_BITS_LOG2))-1;
    tail&=tail_mask;
    u32_list_base[tail_u32_idx]=tail;
  }
  hash_base=&thread_base->hash[0];
  hash_type=karacell_base->hash_type_unencrypted;
/*
Evaluate the requested hash. For hashes with block sizes less than KARACELL_BLOCK_SIZE, the hashes of each hash block (i.e. something smaller than KARACELL_BLOCK_SIZE) would need to be xored together, producing a final output at hash_base. In such case, the seeds should be a combination of *hash_seed_base and the hash block number within the Karacell block; otherwise, we would be xoring lots of equal values together, on account of the 0 padding at the end of the block (at least, for the tail block).
*/
  switch(hash_type){
  case KARACELL_HASH_TYPE_LMD8:
    lmd_block_lmd8_get(u32_list_base,u32_idx_min,hash_seed_base,hash_base);
    break;
  case KARACELL_HASH_TYPE_LMD7:
    lmd_block_lmd7_get(u32_list_base,u32_idx_min,hash_seed_base,hash_base);
    break;
  }
  return;
}

void
karacell_hash_xor(u32 hash_base[KARACELL_HASH_U32_COUNT_MAX],u8 hash_u32_count_minus_1,u32 hash_xor_base[KARACELL_HASH_SEED_U32_COUNT_MAX]){
/*
Accumulate hashes with xor.

In:

  *hash_base contains the hash of a block.

  hash_u32_count_minus_1 is 1 less than the number of (u32)s defined at *hash_base and *hash_xor_base.

  *hash_xor_base contains the xor of the hashes of a set of blocks, initially 0.

Out:

  *hash_xor_base is the xor of the input values, defined for the first (hash_u32_count_minus_1+1) (u32)s.
*/
  do{
    hash_xor_base[hash_u32_count_minus_1]^=hash_base[hash_u32_count_minus_1];
  }while(hash_u32_count_minus_1--);
  return;
}

void
karacell_tail_crypt(u8 decrypt_status,karacell_t *karacell_base,u64 tail_block_idx_minus_1,u32 tail_size,ULONG u32_idx_min,u32 *u32_list_base){
/*
Crypt the last (sub)block of a payload as well as the hash_xor_all footer.

In:

  Do not call this function until all other blocks have been crypted. It's not threadsafe, because, for one thing, it temporarily alters karacell_base->master_key.

  decrypt_status is 0 to encrypt or 1 to decrypt.

  *karacell_base is as defined in karacell_subblock_crypt():In. Also:

    hash_seed_u32_count is as defined in karacell_header_decrypt():Out.

    *hash_xor_all is finalized, except for the hash of this block.

    hash_type_unencrypted is as defined in karacell_header_decrypt():Out.

    hash_u32_count_minus_1_unencrypted is as defined in karacell_header_decrypt():Out.

  *thread_base is as defined in karacell_subblock_crypt():In.

  tail_block_idx_minus_1 is 1 less than the block index of the last payload block, the first payload block being 1.

  tail_size is the size of the tail block, on [0,KARACELL_BLOCK_SIZE]. 0 is only allowed with tail_block_idx_minus_1==0, i.e. a null file, which implies that, otherwise, it's possible that the tail block is a full block. Note that this has nothing to do with the required readable size, as descibed under *u32_list_base.

  u32_idx_min offsets u32_list_base.

  *u32_list_base is the base of the hull containing the payload tail block, which is only defined for tail_size bytes, starting at u32_idx_min, but writable for the full KARACELL_BLOCK_SIZE.

Out:

   Cryption is complete. To crypt a new file, call karacell_rewind() first.

  *u32_list_base is padded, starting at index u32_idx_min, in a manner suitable for size-sensitive hash computation: if tail_size<KARACELL_BLOCK_SIZE, then the tail block has been padded high with 0s.

  karacell_base is as follows:

    *hash_xor_all is its input value xored with the hash of the tail block, then encrypted with a block index of (tail_block_idx_minus_1+2).

    thread_list_base[0].tumbler_list_base is undefined.
*/
  u64 block_idx;
  u32 *hash_base;
  u8 hash_type;
  u8 hash_u32_count_minus_1;
  u32 *hash_xor_all_base;
  karacell_thread_t *thread_base;
  u32 u32_count_minus_1;

  thread_base=&karacell_base->thread_list_base[0];
  block_idx=tail_block_idx_minus_1+1;
  karacell_tumbler_list_make(block_idx,karacell_base,thread_base);
  hash_type=karacell_base->hash_type_unencrypted;
  if(!decrypt_status){
    if(KARACELL_HASH_TYPE_NONE<hash_type){
      karacell_hash_get(0,karacell_base,thread_base,u32_idx_min,u32_list_base,tail_size);
    }
  }
  if(tail_size){
    u32_count_minus_1=(tail_size-1)>>U32_SIZE_LOG2;
    karacell_subblock_crypt(karacell_base,thread_base,KARACELL_TUMBLER_BIAS_CRYPT,u32_count_minus_1,u32_idx_min,u32_list_base);
  }
  if(decrypt_status){
    if(hash_type!=KARACELL_HASH_TYPE_NONE){
      karacell_hash_get(0,karacell_base,thread_base,u32_idx_min,u32_list_base,tail_size);
    }
  }
  if(KARACELL_HASH_TYPE_NONE<hash_type){
    hash_base=&thread_base->hash[0];
    hash_xor_all_base=&karacell_base->hash_xor_all[0];
    hash_u32_count_minus_1=karacell_base->hash_u32_count_minus_1_unencrypted;
    karacell_hash_xor(hash_base,hash_u32_count_minus_1,hash_xor_all_base);
    block_idx++;
/*
We need to ensure that the hash is sensitive to the byte-granular payload size, which is implied by size_following in the header. Unfortunately, the header is not protected by the hash. The reason is basically to save time, plus the fact that such protection would be of dubious utility, considering that a trusted peer could send a malformed header in the hopes of opening a security hole. So the next best thing is just to xor the tail_size to the base of karacell_base->master_key before we create the tumbler set. (block_idx already takes care of the higher bits of the implied payload size.) Yes, tail_size might be 0, but that doesn't undermine the intention here.
*/
    karacell_base->master_key[0]^=tail_size;
    karacell_tumbler_list_make(block_idx,karacell_base,thread_base);
/*
Restore the master key.
*/
    karacell_base->master_key[0]^=tail_size;
/*
Crypt the hash. Note that we must use KARACELL_TUMBLER_BIAS_CRYPT even though this is a hash, because we're not generating seeds.
*/
    karacell_subblock_crypt(karacell_base,thread_base,KARACELL_TUMBLER_BIAS_CRYPT,hash_u32_count_minus_1,0,hash_xor_all_base);
  }
  return;
}

u8
karacell_hash_type_get(karacell_t *karacell_base){
/*
Return the hash_type. The idea is to allow the caller to ensure that the hash is of an acceptable variety. Otherwise, a man-in-the-middle could replace a strong hash with a weak one, with some small chance of success.

In:

  *karacell_base has passed through karacell_header_decrypt(), but not karacell_header_make().

Out:

  Returns the hash_type field in the header.
*/
  return karacell_base->hash_type_unencrypted;
}

u8
karacell_hash_xor_all_compare(u32 hash_xor_all_base[KARACELL_HASH_U32_COUNT_MAX],karacell_t *karacell_base){
/*
Determine whether the hash_xor_all footer read from an encrypted file matches the computed one. Do so in a way which is less susceptible to leaky timing variances.

In:

  *karacell_base is defined as follows:

    hash_type_unencrypted has passed through karacell_header_decrypt(). If decrypting, the caller has invoked karacell_hash_type_get() to make sure that the hash type is acceptable.

    hash_u32_count_minus_1_unencrypted is as defined in karacell_header_decrypt():Out.

    *hash_xor_all is as defined in karacell_tail_crypt():Out.

  *hash_xor_all_base is the encrypted xor of all the hashes in a payload.

Out:

  Returns 1 if the hash is incorrect, else 0.
*/
  u8 hash_type;
  ULONG hash_u32_count_minus_1;
  u32 *hash_xor_all_computed_base;
  u32 hash_xor_or;
  u8 status;

  hash_type=karacell_base->hash_type_unencrypted;
  hash_xor_or=0;
  if(KARACELL_HASH_TYPE_NONE<hash_type){
    hash_xor_all_computed_base=&karacell_base->hash_xor_all[0];
    hash_u32_count_minus_1=karacell_base->hash_u32_count_minus_1_unencrypted;
    do{
      hash_xor_or|=hash_xor_all_base[hash_u32_count_minus_1]^hash_xor_all_computed_base[hash_u32_count_minus_1];
    }while(hash_u32_count_minus_1--);
  }
  status=!!hash_xor_or;
  return status;
}

void
karacell_hash_xor_all_get(u32 hash_xor_all_base[KARACELL_HASH_U32_COUNT_MAX],karacell_t *karacell_base){
/*
Export the encrypted hash_xor_all footer.

In:

  *karacell_base is as defined in karacell_hash_xor_all_compare():In.

Out:

  *hash_xor_all_base is karacell->hash_xor_all for (karacell_base->hash_u32_count_minus_1_unencrypted+1) (u32)s.
*/
  u8 hash_type;
  u32 hash_u32_count_minus_1;
  u32 *hash_xor_all_encrypted_base;

  hash_type=karacell_base->hash_type_unencrypted;
  if(KARACELL_HASH_TYPE_NONE<hash_type){
    hash_xor_all_encrypted_base=&karacell_base->hash_xor_all[0];
    hash_u32_count_minus_1=karacell_base->hash_u32_count_minus_1_unencrypted;
    karacell_u32_list_copy(0,hash_xor_all_encrypted_base,0,hash_xor_all_base,hash_u32_count_minus_1);
  }
  return;
}

void
karacell_thread_flush(karacell_t *karacell_base){
/*
Force all pending threads to finish. Then accumulate the xor of the xors of the hashes of all the blocks of every simulthread.

In:

  *karacell_base is as defined in karacell_init():Out or karacell_block_list_realloc():Out. Additionally:

    crypt_mode is defined.

    crypt_thread_idx_max was set by karacell_spawn().

    hash_type_unencrypted is as defined in karacell_header_decrypt():Out.

    hash_u32_count_minus_1_unencrypted is as defined in karacell_header_decrypt():Out.

    *hash_xor_all is 0.

    spawn_base points to a live spawn_t.

Out:

  karacell_base is as follows:

    *hash_xor_all contains its input value xored to karacell_thread->hash_xor_all of every simulthread, unless karacell_base->crypt_mode==KARACELL_MODE_PREFAB.

    *spawn_base is ready for reuse for crypting additional blocks in the same file.
*/
  u8 hash_type;
  u8 hash_u32_count_minus_1;
  u32 *hash_xor_all_base;
  u32 *hash_xor_base;
  u8 mode;
  ULONG simulthread_idx;
  spawn_t *spawn_base;
  u32 thread_idx_max;
  karacell_thread_t *thread_list_base;

  spawn_base=karacell_base->spawn_base;
  SPAWN_RETIRE_ALL(spawn_base);
  mode=karacell_base->crypt_mode;
/*
All threads have been retired. If need be, xor all the (hash_xor)s of all the threads to hash_xor_all.
*/
  if(mode!=KARACELL_MODE_PREFAB){
    hash_type=karacell_base->hash_type_unencrypted;
    if(KARACELL_HASH_TYPE_NONE<hash_type){
      thread_list_base=&karacell_base->thread_list_base[0];
      hash_xor_all_base=&karacell_base->hash_xor_all[0];
      hash_u32_count_minus_1=karacell_base->hash_u32_count_minus_1_unencrypted;
      thread_idx_max=karacell_base->crypt_thread_idx_max;
/*
This loop is finite because thread_idx_max<=KARACELL_SIMULTHREAD_IDX_MAX<U32_MAX, due to karacell_crypt_spawn().
*/
      for(simulthread_idx=0;simulthread_idx<=thread_idx_max;simulthread_idx++){
        hash_xor_base=&thread_list_base[simulthread_idx].hash_xor[0];
        karacell_hash_xor(hash_xor_base,hash_u32_count_minus_1,hash_xor_all_base);
      }
    }
  }
  return;
}

void
karacell_thread_execute(spawn_simulthread_context_t *spawn_simulthread_context_base){
/*
Crypt a set of blocks, create a set of xor masks, or apply xor masks to a set of blocks. (These are very similar operations, hence their combination here.) This function is intended to be called exclusively via Spawn.

In:

  *thread_context_base is as follows:

    readonly_string_base is karacell_base in disguise, which was initialized by karacell_crpyt_spawn(), karacell_prefab_spawn(), or karacell_xor_spawn().

    simulthread_idx==thread_idx. This is not required generally for Spawn clients, but is convenient here for Karacell's particular threading model.

Out:

  Returns via Spawn.

  Other effects are as documented in each of the parent functions above.
*/
  u32 *block_base;
  u32 **block_base_list_base;
  ULONG block_count;
  ULONG block_idx;
  u64 block_idx_absolute;
  u32 block_plus_seed_u32_count_minus_1;
  u32 block_remainder;
  u32 *hash_base;
  u32 *hash_seed_base;
  ULONG hash_seed_idx;
  u8 hash_seed_ready_status;
  u32 hash_seed_u32_count;
  u32 hash_seed_u32_count_minus_1;
  u8 hash_type;
  u8 hash_u32_count_minus_1;
  u32 *hash_xor_base;
  karacell_t *karacell_base;
  ULONG mask_idx;
  u8 mode;
  u32 simulthread_idx;
  karacell_thread_t *thread_base;
  u32 thread_count;
  u32 *u32_list_base;
  ULONG u32_idx;

  karacell_base=(karacell_t *)(spawn_simulthread_context_base->readonly_string_base);
  simulthread_idx=spawn_simulthread_context_base->simulthread_idx;
  block_base_list_base=karacell_base->block_base_list_base;
  thread_base=&karacell_base->thread_list_base[simulthread_idx];
  block_count=karacell_base->crypt_block_count;
/*
This add won't wrap because buildtime checks force KARACELL_SIMULTHREAD_IDX_MAX<U32_MAX, and karacell_spawn() forces crypt_thread_idx_max<=KARACELL_SIMULTHREAD_IDX_MAX.
*/
  thread_count=karacell_base->crypt_thread_idx_max+1;
/*
thread_count!=0 due to KARACELL_SIMULTHREAD_IDX_MAX<ULONG_MAX enforcement in tweak.h, combined with the fact that the number of threads and simulthreads are equal by design.
*/
  block_idx_absolute=karacell_base->crypt_block_idx_min_absolute;
  mode=karacell_base->crypt_mode;
/*
Allocate contiguous chunks of blocks as fairly as possible using remainder distribution, such that low threads may receive an additional block each.
*/
  block_remainder=block_count%thread_count;
  block_count/=thread_count;
  block_idx=block_count*simulthread_idx;
  if(simulthread_idx<block_remainder){
    block_idx+=simulthread_idx;
    block_count++;
  }else{
    block_idx+=block_remainder;
  }
  if(mode!=KARACELL_MODE_PREFAB){
    u32_list_base=karacell_base->crypt_u32_list_base;
    u32_idx=karacell_base->crypt_u32_idx_min+(block_idx<<KARACELL_BLOCK_U32_COUNT_LOG2);
  }
  hash_base=&thread_base->hash[0];
  hash_seed_base=&thread_base->hash_seed[0];
  hash_xor_base=&thread_base->hash_xor[0];
  block_idx_absolute+=block_idx;
  hash_seed_ready_status=0;
  if((mode==KARACELL_MODE_DECRYPT_FAST)||(mode==KARACELL_MODE_ENCRYPT_FAST)){
    hash_seed_ready_status=1;
  }
  hash_seed_u32_count=karacell_base->hash_seed_u32_count;
  hash_seed_u32_count_minus_1=hash_seed_u32_count-1;
  hash_type=karacell_base->hash_type_unencrypted;
  if(KARACELL_HASH_TYPE_NONE<hash_type){
    hash_u32_count_minus_1=karacell_base->hash_u32_count_minus_1_unencrypted;
    karacell_u32_list_zero(hash_u32_count_minus_1,0,hash_xor_base);
  }
  while(block_count--){
    if((mode!=KARACELL_MODE_DECRYPT_SLOW)&&(mode!=KARACELL_MODE_ENCRYPT_SLOW)){
      block_base=block_base_list_base[block_idx];
      block_idx++;
      if(mode==KARACELL_MODE_PREFAB){
        block_plus_seed_u32_count_minus_1=hash_seed_u32_count+(KARACELL_BLOCK_U32_COUNT-1);
        karacell_u32_list_zero(block_plus_seed_u32_count_minus_1,0,block_base);
      }
    }
    if((mode!=KARACELL_MODE_DECRYPT_FAST)&&(mode!=KARACELL_MODE_ENCRYPT_FAST)){
      karacell_tumbler_list_make(block_idx_absolute,karacell_base,thread_base);
      block_idx_absolute++;
    }else{
      if(hash_seed_u32_count){
        for(hash_seed_idx=0;hash_seed_idx<=hash_seed_u32_count_minus_1;hash_seed_idx++){
          hash_seed_base[hash_seed_idx]=block_base[hash_seed_idx+KARACELL_BLOCK_U32_COUNT];
        }
      }
    }
    if((mode==KARACELL_MODE_ENCRYPT_FAST)||(mode==KARACELL_MODE_ENCRYPT_SLOW)){
      if(KARACELL_HASH_TYPE_NONE<hash_type){
        karacell_hash_get(hash_seed_ready_status,karacell_base,thread_base,u32_idx,u32_list_base,KARACELL_BLOCK_SIZE);
      }
    }
    if((mode!=KARACELL_MODE_DECRYPT_FAST)&&(mode!=KARACELL_MODE_ENCRYPT_FAST)){
      if(mode!=KARACELL_MODE_PREFAB){
        karacell_subblock_crypt(karacell_base,thread_base,KARACELL_TUMBLER_BIAS_CRYPT,KARACELL_BLOCK_U32_COUNT-1,u32_idx,u32_list_base);
      }else{
        karacell_subblock_crypt(karacell_base,thread_base,KARACELL_TUMBLER_BIAS_CRYPT,KARACELL_BLOCK_U32_COUNT-1,0,block_base);
        if(hash_seed_u32_count){
          karacell_subblock_crypt(karacell_base,thread_base,KARACELL_TUMBLER_BIAS_HASH,hash_seed_u32_count_minus_1,KARACELL_BLOCK_U32_COUNT,block_base);
        }
      }
    }else{
      for(mask_idx=0;mask_idx<=(KARACELL_BLOCK_U32_COUNT-1);mask_idx++){
        u32_list_base[u32_idx+mask_idx]^=block_base[mask_idx];
      }
    }
    if((mode==KARACELL_MODE_DECRYPT_FAST)||(mode==KARACELL_MODE_DECRYPT_SLOW)){
      if(KARACELL_HASH_TYPE_NONE<hash_type){
        karacell_hash_get(hash_seed_ready_status,karacell_base,thread_base,u32_idx,u32_list_base,KARACELL_BLOCK_SIZE);
      }
    }
    if(mode!=KARACELL_MODE_PREFAB){
      u32_idx+=KARACELL_BLOCK_U32_COUNT;
      if(KARACELL_HASH_TYPE_NONE<hash_type){
        karacell_hash_xor(hash_base,hash_u32_count_minus_1,hash_xor_base);
      }
    }
  }
  return;
}

void
karacell_spawn(ULONG block_count_minus_1,karacell_t *karacell_base){
/*
Spawn karacell_thread_execute().

In:
  block_count_minus_1 is 1 less than the number of blocks to process.

  *karacell_base is as defined in karacell_tumbler_list_make():In. Additionally:

    hash_type_unencrypted is as defined in karacell_header_decrypt():Out.

    hash_u32_count_unencrypted is as defined in karacell_header_decrypt():Out.

Out:

  Threads are in flight which must be retired via karacell_thread_flush() before any other Karacell functions are called.

  *karacell_base is as follows, unless karacell_base->crypt_mode==KARACELL_MODE_PREFAB:

    *thread_list_base[N] is as follows:

      *hash_xor contains the xor of the hashes of all blocks in thread N.
*/
  u8 hash_type;
  u8 hash_u32_count_minus_1;
  u32 *hash_xor_base;
  u8 mode;
  u32 simulthread_idx;
  spawn_t *spawn_base;
  u32 thread_idx_max;
  karacell_thread_t *thread_list_base;

  spawn_base=karacell_base->spawn_base;
/*
thread_idx_max is normally a ULONG, but it's a u32 in this case because (KARACELL_SIMULTHREAD_IDX_MAX<U32_MAX) per buildtime verification. It just makes sense in this case to set the number of threads equal to at most the number of simulthreads, because all the threads are similar. It's also assumed by karacell_thread_execute(), so lots more verification would need to occur if thread_idx_max were allowed to exceed KARACELL_SIMULTHREAD_IDX_MAX.
*/
  thread_idx_max=KARACELL_SIMULTHREAD_IDX_MAX;
  if(block_count_minus_1<=thread_idx_max){
    thread_idx_max=(u32)(block_count_minus_1);
  }
  karacell_base->crypt_block_count=block_count_minus_1+1;
  karacell_base->crypt_thread_idx_max=thread_idx_max;
  mode=karacell_base->crypt_mode;
/*
If need be, zero all the (hash_xor)s of all the threads.
*/
  if(mode!=KARACELL_MODE_PREFAB){
    hash_type=karacell_base->hash_type_unencrypted;
    if(KARACELL_HASH_TYPE_NONE<hash_type){
      hash_u32_count_minus_1=karacell_base->hash_u32_count_minus_1_unencrypted;
      thread_list_base=&karacell_base->thread_list_base[0];
/*
This loop is finite because thread_idx_max<=KARACELL_SIMULTHREAD_IDX_MAX<U32_MAX, due to buildtime checks.
*/
      for(simulthread_idx=0;simulthread_idx<=thread_idx_max;simulthread_idx++){
        hash_xor_base=&thread_list_base[simulthread_idx].hash_xor[0];
        karacell_u32_list_zero(hash_u32_count_minus_1,0,hash_xor_base);
      }
    }
  }
  SPAWN(spawn_base,thread_idx_max);
  return;
}

void
karacell_crypt_spawn(ULONG block_count_minus_1,u64 block_idx_min_minus_1,u8 decrypt_status,karacell_t *karacell_base,ULONG u32_idx_min,u32 *u32_list_base){
/*
Crypt a series of whole blocks in a payload. This is NOT the recommended way to do things, unless you need to be monothreaded or you failed to prefabricate enough xor masks before the payload arrived. A better way is karacell_prefab_spawn() followed by karacell_xor_spawn().

In:

  block_count_minus_1 is 1 less than the number of blocks based at &u32_list_base[u32_idx_min]. Each block consists of KARACELL_BLOCK_U32_COUNT (u32)s.

  block_idx_min_minus_1 is the index of the least block to crypt.

  decrypt_status is 0 to encrypt or 1 to decrypt.

  *karacell_base is as defined in karacell_tumbler_list_make():In.

  u32_idx_min offsets u32_list_base.

  *u32_list_base the base of the hull containing the block(s) to crypt, which is writable for ((block_count_minus_1+1)<<KARACELL_U32_COUNT_LOG2) (u32)s starting at index u32_idx_min.

Out:

  See comment in karacell_spawn():Out.

  *u32_list_base is crypted over its required writable domain.
*/
  u8 mode;

  karacell_base->crypt_u32_list_base=u32_list_base;
  karacell_base->crypt_block_idx_min_absolute=block_idx_min_minus_1+1;
  karacell_base->crypt_u32_idx_min=u32_idx_min;
  if(decrypt_status){
    mode=KARACELL_MODE_DECRYPT_SLOW;
  }else{
    mode=KARACELL_MODE_ENCRYPT_SLOW;
  }
  karacell_base->crypt_mode=mode;
  karacell_spawn(block_count_minus_1,karacell_base);
  return;
}

void
karacell_prefab_spawn(ULONG block_count_minus_1,u64 block_idx_min_minus_1,karacell_t *karacell_base){
/*
Prefabricate a series of whole xor mask blocks while awaiting payload arrival.

In:

  block_count_minus_1 is 1 less than the number of xor mask blocks to prefabricate at bases given by karacell_list_base->block_base_list_base.

  block_idx_min_minus_1 is as defined in karacell_crypt_spawn():In.

  *karacell_base is as defined in karacell_tumbler_list_make():In.

Out:

  See comment in karacell_spawn():Out.

  *karacell_base is as follows:

    **block_base_list_base is updated such that the first (block_count_minus_1+1) blocks pointed to by block_base_list_base are filled with prefabricated xor masks followed by hash seeds, awaiting the arrival of a payload with which to xor.
*/
  karacell_base->crypt_block_idx_min_absolute=block_idx_min_minus_1+1;
  karacell_base->crypt_mode=KARACELL_MODE_PREFAB;
  karacell_spawn(block_count_minus_1,karacell_base);
  return;
}

void
karacell_xor_spawn(ULONG block_count_minus_1,u64 block_idx_min_minus_1,u8 decrypt_status,karacell_t *karacell_base,ULONG u32_idx_min,u32 *u32_list_base){
/*
Crypt a series of whole blocks of a payload by applying prefabricated xor masks.

In:

  block_count_minus_1 is as defined in karacell_crypt_spawn():In.

  block_idx_min_minus_1 is as defined karacell_crypt_spawn():In.

  decrypt_status is as defined karacell_crypt_spawn():In.

  *karacell_base is as defined in karacell_tumbler_list_make():In.

  u32_idx_min is as defined karacell_crypt_spawn():In.

  *u32_list_base is as defined karacell_crypt_spawn():In.

Out:

  See comment in karacell_spawn():Out.

  *u32_list_base has been crypted with the prefabricated xor masks generated by karacell_prefab_spawn() over its required writable domain.
*/
  u8 mode;

  karacell_base->crypt_u32_list_base=u32_list_base;
  karacell_base->crypt_block_idx_min_absolute=block_idx_min_minus_1+1;
  karacell_base->crypt_u32_idx_min=u32_idx_min;
  if(decrypt_status){
    mode=KARACELL_MODE_DECRYPT_FAST;
  }else{
    mode=KARACELL_MODE_ENCRYPT_FAST;
  }
  karacell_base->crypt_mode=mode;
  karacell_spawn(block_count_minus_1,karacell_base);
  return;
}

void
karacell_header_crypt(karacell_t *karacell_base){
/*
Crypt a Karacell header.

This function uses a karacell_thread_t, which means it should be serialized.

In:

  *karacell_base has passed through karacell_master_key_set() and karacell_iv_set(), and is otherwise as defined in karacell_init():Out or karacell_block_list_realloc():Out. *header_base is the header to crypt, with all fields defined (but encrypted, if decrypting).

Out:

  *karacell_base is as follows:

    *header_base contains a header which is completely crypted except for the IV.

    thread_list_base[0] is as defined in karacell_tumbler_list_make():Out:*thread_base.
*/
  u32 block_0_base[KARACELL_BLOCK0_U32_COUNT];
  karacell_header_t *header_base;
  karacell_thread_t *thread_base;

  header_base=karacell_base->header_base;
  thread_base=&karacell_base->thread_list_base[0];
/*
Generate tumblers, as we would before encrypting a normal payload block.
*/
  karacell_tumbler_list_make(0,karacell_base,thread_base);
/*
Compute the xor mask for block 0, which is a subset of the header.
*/
  karacell_u32_list_zero(KARACELL_BLOCK0_U32_COUNT-1,0,block_0_base);
  karacell_subblock_crypt(karacell_base,thread_base,KARACELL_TUMBLER_BIAS_CRYPT,KARACELL_BLOCK0_U32_COUNT-1,0,block_0_base);
/*
Manually crypt the header beyond the IV.
*/
  #if KARACELL_BLOCK0_U32_COUNT!=6
    #error "karacell_header_crypt() is not crypting all fields."
  #endif
  header_base->must_decrypt_to_zero^=((u64)(block_0_base[1])<<U32_BITS)+block_0_base[0];
  header_base->size_following^=((u64)(block_0_base[3])<<U32_BITS)+block_0_base[2];
  header_base->build_number_of_creator^=(u16)(block_0_base[4]);
  header_base->build_number_needed_to_decode^=(u16)(block_0_base[4]>>U16_BITS);
  header_base->reserved_zero^=(u16)(block_0_base[5]);
  header_base->hash_type^=(u8)(block_0_base[5]>>U16_BITS);
  header_base->hash_u32_count_minus_1^=(u8)(block_0_base[5]>>24);
  return;
}

u8
karacell_header_make(u32 *hash_size_base,u8 hash_type,karacell_t *karacell_base,u64 payload_size){
/*
Construct and encrypt a Karacell header, then export it.

This function uses a karacell_thread_t, which means it should be serialized.

In:

  hash_type is one of the constants defined in karacell_t.header_base->hash_type, except KARACELL_HASH_TYPE_UNKNOWN.

  *karacell_base is as defined in karacell_header_crypt():In.

  payload_size is the number of bytes in the payload, sandwiched between the header and hash_xor_all. 0 is allowed.

Out:

  Returns 1 if hash_type was an illegal value or the resulting file would exceed a 64-bit size, in which case all other outputs are undefined unless otherwise stated, else 0.

  *hash_size_base is as defined in karacell_header_decrypt():Out.

  *karacell_base is as specified in karacell_header_crypt():Out. Additionally:

    *header_base points to an encrypted copy of the header as defined in the Karacell specification revision to which this source code conforms.

    hash_seed_u32_count is as defined in karacell_header_decrypt():Out:karacell_base->hash_seed_u32_count.

    hash_type_unencrypted==hash_type.

    hash_u32_count_minus_1_unencrypted is consistent with hash_type.
*/
  u64 file_size;
  u32 hash_plus_header_size;
  u32 hash_seed_u32_count;
  u32 hash_size;
  u8 hash_u32_count_minus_1;
  karacell_header_t *header_base;
  u8 status;

  #if KARACELL_BLOCK0_U32_COUNT!=6
    #error "karacell_header_make() is unaware of changes to karacell_header_t."
  #endif
  status=0;
/*
You can expand this switch statement to deal with future hash types.
*/
  switch(hash_type){
  case KARACELL_HASH_TYPE_NONE:
    hash_seed_u32_count=0;
/*
This is the one case where 0 means 0 (no hash).
*/
    hash_u32_count_minus_1=0;
    break;   
  case KARACELL_HASH_TYPE_LMD8:
    hash_seed_u32_count=LMD8_U32_COUNT<<1;
    hash_u32_count_minus_1=LMD8_U32_COUNT-1;
    break;
  case KARACELL_HASH_TYPE_LMD7:
    hash_seed_u32_count=LMD7_U32_COUNT<<1;
    hash_u32_count_minus_1=LMD7_U32_COUNT-1;
    break;
  default:
    status=1;
  }
  if(!status){
    status=1;
    hash_size=0;
    if(hash_type!=KARACELL_HASH_TYPE_NONE){
      hash_size=((u32)(hash_u32_count_minus_1)+1)<<U32_SIZE_LOG2;
    }
    *hash_size_base=hash_size;
    hash_plus_header_size=hash_size+sizeof(karacell_header_t);
    file_size=payload_size+hash_plus_header_size;
    if(payload_size<file_size){
      karacell_base->hash_seed_u32_count=hash_seed_u32_count;
      header_base=karacell_base->header_base;
      header_base->must_decrypt_to_zero=0;
      header_base->size_following=file_size-(sizeof(karacell_header_t)-KARACELL_BLOCK0_SIZE_AFTER_SIZE_FOLLOWING);
      header_base->build_number_of_creator=BUILD_NUMBER;
      header_base->build_number_needed_to_decode=BUILD_NUMBER_NEEDED_TO_DECODE;
      header_base->reserved_zero=0;
      header_base->hash_type=hash_type;
      karacell_base->hash_type_unencrypted=hash_type;
      header_base->hash_u32_count_minus_1=hash_u32_count_minus_1;
      karacell_base->hash_u32_count_minus_1_unencrypted=hash_u32_count_minus_1;
      karacell_header_crypt(karacell_base);
      status=0;
    }
  }
  return status;
}

void
karacell_iv_set(karacell_t *karacell_base,u32 iv_base[KARACELL_IV_U32_COUNT],ULONG iv_idx_min){
/*
Copy an externally generated IV into a Karacell context. This is handy for managing multiple files with the master key.

In:

  karacell_base is as defined in karacell_init():Out or karacell_block_list_realloc():Out.

  *iv_base is the base of an IV to be used for cryption, consisting of KARACELL_IV_U32_COUNT (u32)s.

  iv_idx_min offsets iv_base.

Out:

  karacell_base->header_base.iv is iv_base[iv_idx_min] for KARACELL_IV_U32_COUNT (u32)s.
*/
  karacell_header_t *header_base;

  header_base=karacell_base->header_base;
  karacell_u32_list_copy(iv_idx_min,iv_base,0,&header_base->iv[0],KARACELL_IV_U32_COUNT-1);
  return;
}

u8
karacell_header_decrypt(u64 *file_size_base,u32 *hash_size_base,karacell_t *karacell_base){
/*
Import a Karacell header. Then decrypt then partially authenticate it.

In:

  *file_size_base has successfully passed thought karacell_header_size_check(). It's the size of the entire Karacell file including the header and hash_xor_all. This is what was actually received or read. It may be 0 or some other wrong value.

  *karacell_base is as defined in karacell_header_crypt():In. *header_base is the header to decrypt.

Out:

  Returns:

    0 if the decrypted header at karacell_base->header_base is intact, meaning that:

        1. must_decrypt_to_zero does indeed decrypt to 0.

        2. hash_type!=KARACELL_HASH_TYPE_UNKNOWN. (But karacell_base->hash_type_unencrypted will be set to this value if hash_type is unrecognized.)

        3. hash_u32_count_minus_1 is consistent with hash_type, to the best of our knowledge.

        4. size_following is sufficient for this file to contain the payload size returned at file_size_base, sandwiched between the header and hash_xor_all.

        5. In:*file_size_base is sufficient to contain the file size implied by size_following and hash_type.

        6. build_number_needed_to_decode<=build_number_of_creator.

        7. build_number_needed_to_decode<=BUILD_NUMBER.

        8. build_number_of_creator<=BUILD_NUMBER.

      See also the definition of karacell_header_t.

    1 if all the requirements for 0 pass, except #8. This is not an error condition, but it prompts the caller to inform the user.

    2 if all the requirements for 0 pass, except #7 and #8, in which case it appears as if the caller needs to update Karacell.

    3 if must_decrypt_to_zero does not in fact decrypt to 0. This probably indicates that the caller has tried the wrong master key. All other outputs are undefined unless otherwise stated. See also notes in error code 4 below.

    4 if the decrypted header is inconsistent, in that at least one of the requirements in #1 through #6 is not satisfied. All other outputs are undefined unless otherwise stated. It's possible that the encrypted header is legit, but came from a peer other than the presumed one, in which case the caller should try another master key -- but the odds are only about 1 in 2^64, given that must_decrypt_to_zero did indeed decrypt to 0. This is another reason why failed file transmission attempts should be retried using a different IV, even though it would be theoretically safe to use the previous IV because the payload is the same: very rarely, a receiver will conclude that a header is corrupt when in fact the wrong key was tried, due to must_decrypt_to_zero just happening to decrypt to 0.

  *file_size_base is the payload size. May be 0, but will always be less than its value on In.

  *hash_size_base is the size of the hash (0 for no hash). This value will be accurate even for unrecognized hash types.

  *karacell_base is as follows:

    hash_seed_u32_count is the number of (u32)s in the hash seed.

    *header_base is the decrypted version of *header_base on In, for size sizeof(karacell_header_t). There is no need to call karacell_iv_set().

    hash_type_unencrypted is header_base->hash_type.

    hash_u32_count_minus_1_unencrypted is header_base->hash_u32_count_minus_1.

    header_base->hash_type may have been adjusted: if the hash type is unrecogized, it's KARACELL_HASH_TYPE_UNKNOWN.
*/
  u16 build_number_needed_to_decode;
  u16 build_number_of_creator;
  u64 file_size;
  u64 file_size_min;
  u32 hash_seed_u32_count;
  u32 hash_size;
  u8 hash_type;
  u8 hash_u32_count_minus_1;
  u8 hash_u32_count_minus_1_expected;
  karacell_header_t *header_base;
  u64 payload_size;
  u64 size_following;
  u8 status;

  #if KARACELL_BLOCK0_U32_COUNT!=6
    #error "karacell_header_decrypt() is unaware of changes to karacell_header_t."
  #endif
  karacell_header_crypt(karacell_base);
  header_base=karacell_base->header_base;
  status=3;
  if(!header_base->must_decrypt_to_zero){
    status=4;
    hash_type=header_base->hash_type;
    hash_u32_count_minus_1=header_base->hash_u32_count_minus_1;
    hash_size=0;
    if(hash_type!=KARACELL_HASH_TYPE_NONE){
      hash_size=((u32)(hash_u32_count_minus_1)+1)<<U32_SIZE_LOG2;
    }
/*
You can expand this switch statement to deal with future hash types. If the hash type is unrecognized (which could legitimately happen if BUILD_NUMBER<build_number_of_creator), it's equivalent to having no hash, i.e. don't prevent decryption, but warn the user that there is no authentication available.
*/
    *hash_size_base=hash_size;
    switch(hash_type){
    case KARACELL_HASH_TYPE_UNKNOWN:
      hash_u32_count_minus_1_expected=hash_u32_count_minus_1+1;
      break;
    case KARACELL_HASH_TYPE_NONE:
      hash_seed_u32_count=0;
      hash_u32_count_minus_1_expected=0;
      break;
    case KARACELL_HASH_TYPE_LMD8:
      hash_seed_u32_count=LMD8_U32_COUNT<<1;
      hash_u32_count_minus_1_expected=LMD8_U32_COUNT-1;
      break;
    case KARACELL_HASH_TYPE_LMD7:
      hash_seed_u32_count=LMD7_U32_COUNT<<1;
      hash_u32_count_minus_1_expected=LMD7_U32_COUNT-1;
      break;
    default:
/*
Treat unrecognized hashes as unknown, but leave *hash_size_base intact so that the caller knows how long the footer is.
*/
      hash_type=KARACELL_HASH_TYPE_UNKNOWN;
      header_base->hash_type=hash_type;
      hash_seed_u32_count=0;
      hash_u32_count_minus_1_expected=hash_u32_count_minus_1;
    }
    if(hash_u32_count_minus_1==hash_u32_count_minus_1_expected){
      karacell_base->hash_seed_u32_count=hash_seed_u32_count;
      karacell_base->hash_type_unencrypted=hash_type;
      karacell_base->hash_u32_count_minus_1_unencrypted=hash_u32_count_minus_1;
      size_following=header_base->size_following;
      payload_size=size_following-KARACELL_BLOCK0_SIZE_AFTER_SIZE_FOLLOWING-hash_size;
      if(payload_size<size_following){
        file_size_min=size_following+sizeof(karacell_header_t)-KARACELL_BLOCK0_SIZE_AFTER_SIZE_FOLLOWING;
        if(size_following<file_size_min){
          file_size=*file_size_base;
          if(file_size_min<=file_size){
            build_number_of_creator=header_base->build_number_of_creator;
            build_number_needed_to_decode=header_base->build_number_needed_to_decode;
            if(build_number_needed_to_decode<=build_number_of_creator){
              *file_size_base=payload_size;
              if(BUILD_NUMBER<build_number_needed_to_decode){
                status=2;
              }else{
                status=(BUILD_NUMBER<build_number_of_creator);
              }
            }
          }
        }
      }
    }
  }
  return status;
}

u8
karacell_header_size_check(u64 file_size){
/*
Check whether a file is large enough to even possibly be a valid Karacell file, assuming no hash footer.

In:

  file_size is the size of the file, which may assume any value.

Out:

  Returns 1 if the file is not valid Karacell file, or 0 if it might be.
*/
  return (file_size<sizeof(karacell_header_t));
}

void
karacell_master_key_set(karacell_t *karacell_base,u32 master_key_base[KARACELL_MASTER_KEY_U32_COUNT_MAX],ULONG master_key_idx_min,u8 tumbler_idx_max){
/*
Copy a master key from a keyring into a Karacell context. This is handy for managing multiple peers with the same block allocation in order to save memory, or to try a decryption with multiple keys until something works.

In:

  *karacell_base is as defined in karacell_init():Out or karacell_block_list_realloc():Out.

  *master_key_base has passed through karacell_tumbler_idx_max_get().

  master_key_idx_min offsets master_key_base.

  tumbler_idx_max is the return value of karacell_tumbler_idx_max_get(), associated with *master_key_base.

Out:

  karacell_base->master_key is master_key_base[master_key_idx_min] for KARACELL_MASTER_KEY_U32_COUNT_MAX (u32)s.

  karacell_base->tumbler_idx_max is tumbler_idx_max.
*/
  karacell_base->tumbler_idx_max=tumbler_idx_max;
  karacell_u32_list_copy(master_key_idx_min,master_key_base,0,&karacell_base->master_key[0],KARACELL_MASTER_KEY_U32_COUNT_MAX-1);
  return;
}

void
karacell_master_key_xor(karacell_t *karacell_base,u32 master_key_xor_base[KARACELL_MASTER_KEY_U32_COUNT_MAX],ULONG master_key_xor_idx_min){
/*
Xor the master key to a mask, for example, an entropy pool for IV generation.

In:

  *karacell_base is as defined in karacell_master_key_set().

  *master_key_xor_base is as defined in karacell_master_key_set():In:*master_key_base.

  *master_key_xor_idx_min is as defined in karacell_master_key_set():In:master_key_idx_min.

Out:

  karacell_base is changed:

    *master_key is its the input value xored with *master_key_xor_base.
*/
  ULONG master_key_idx;

  for(master_key_idx=0;master_key_idx<=(KARACELL_MASTER_KEY_U32_COUNT_MAX-1);master_key_idx++){
    karacell_base->master_key[master_key_idx]^=master_key_xor_base[master_key_xor_idx_min];
    master_key_xor_idx_min++;
  }
  return;
}

u8
karacell_tumbler_idx_max_get(u32 master_key_base[KARACELL_MASTER_KEY_U32_COUNT_MAX]){
/*
Get the maximum 0-based tumbler index (tumbler_idx_max) implied by the master key. See mathematica.txt for a simulation.

In:

  *master_key_base is the master key, padded high with 0s.

Out:

  Returns 0 on failure, else the maximum 0-based tumbler index on [KARACELL_TUMBLER_COUNT_MIN-1,KARACELL_TUMBLER_COUNT_MAX-1]. The return value is 1 less than the number of unique tumblers required to implement Karacell with a cracking computational complexity at least as great as that implied by the master key. The idea is to use enough unique tumblers to ensure that the Horowitz & Sahni algo for Subset Sum has a solution space at least as large as 2^(floor(log2(master key))+1). (On average, the solution would be found after trying only half as many candidates, but the same can be said of trying every key, so this is moot. Likewise, other algos exist, but apparently offer no advantage to Karacell cracking.) If in the future this complexity estimate is shown to be insufficient, then obviously people will need to use longer keys. Otherwise, we want to do cryption as fast as possible, hence the desire to minimize this value. Note that it's always odd because the tumbler count is always even, on account of the difficulty of estimating computational complexity with odd numbers of tumblers; this fact might be used to accelerate cryption by applying 2 tumblers at a time.
*/
  u32 complexity_idx;
  u32 complexity_idx_max;
  u32 complexity_idx_min;
  u32 master_key_idx;
  u32 master_key_u32;
  u32 msb;
  u8 tumbler_idx_max;
/*
Find the MSB of the master key, which is a fundamental measure of its strength, assuming of course that it was otherwise randomly generated.
*/
  master_key_idx=KARACELL_MASTER_KEY_U32_COUNT_MAX-1;
  while((!master_key_base[master_key_idx])&&master_key_idx){
    master_key_idx--;
  }
  master_key_u32=master_key_base[master_key_idx];
  MSB_SMALL_GET(master_key_u32,msb);
  msb+=master_key_idx<<U32_BITS_LOG2;
/*
Find the number of tumblers we need in order to ensure sufficient cracking complexity.
*/
  complexity_idx_min=0;
  complexity_idx_max=KARACELL_COMPLEXITY_LOG2_LIST_U16_COUNT-1;
  do{
    complexity_idx=(complexity_idx_min+complexity_idx_max)>>1;
    if(msb<karacell_complexity_log2_list_base[complexity_idx]){
      complexity_idx_max=complexity_idx;
    }else{
      complexity_idx_min=complexity_idx+1;
    }
  }while(complexity_idx_min!=complexity_idx_max);
  tumbler_idx_max=(u8)((complexity_idx_max<<1)+1);
  if(tumbler_idx_max<(KARACELL_TUMBLER_COUNT_MIN-1)){
/*
The master key is weak. Fail out.
*/
    tumbler_idx_max=0;
  }
  return tumbler_idx_max;
}

void *
karacell_free(void *base){
/*
To maximize portability and debuggability, this is the only place where Karacell frees memory.

In:

  base is the return value of karacell_malloc() or karacell_realloc(). May be NULL.

Out:

  Returns NULL so that the caller can easily maintain the good practice of NULLing out invalid pointers.

  *base is freed.
*/
  #ifdef DEBUG
    if(base){
      karacell_allocation_count--;
    }
  #endif
  free(base);
  return NULL;
}

void *
karacell_malloc(ULONG size_minus_1){
/*
To maximize portability and debuggability, this is the only place where Karacell allocates memory.

In:

  size_minus_1 is the number of bytes to allocate, less 1 to ensure that we don't invite stupidity.

Out:

  base is the base of a memory allocation of (size_minus_1+1) bytes, or NULL if allocation failed.
*/
  #ifdef DEBUG
    u8 corruption;
  #endif
  void *base;
  ULONG size;

  size=size_minus_1+1;
  base=NULL;
  if(size){
    base=malloc((size_t)(size));
  }
  #ifdef DEBUG
    if(base){
      karacell_allocation_count++;
/*
Deterministically corrupt newly allocated memory in order to flush out bugs. If you see successive bytes different by the increment below, you can bet you're counting on uninitialized memory!
*/
      corruption=0;
      do{
        corruption+=0xA3;
        ((u8 *)(base))[size_minus_1]=corruption;
      }while(size_minus_1--);
    }
  #endif
  return base;
}

void *
karacell_realloc(void *base,ULONG size_minus_1){
/*
Change the size of a nonnull allocation to a new nonnull allocation. (This function cannot be used to allocate or free memory.)

In:

  base cannot be NULL, i.e. this function cannot be used like malloc, despite the fact that realloc() provides for such behavior. The reason for this provision is that we want to be able to count allocations and frees using exclusively karacell_malloc() and karacell_free(), in order to verify that all memory is eventually freed. Plus, why would you want to use this function to emulate karacell_malloc(), when you can just call karacell_malloc() itself?

  size_minus_1 is the number of bytes in the revised allocation, less 1.

Out:

  Returns a (void *) to the new allocation. If different from base, then base has been freed unless it's NULL, in which case nothing has changed and the reallocation failed.

  *base is only defined for the minimum of its previous and new sizes.
*/
  ULONG size;

  size=size_minus_1+1;
  if(size){
    base=realloc(base,(size_t)(size));
  }else{
    base=NULL;
  }
  return base;
}

void
karacell_block_list_free(karacell_t *karacell_base){
/*
Free the block list, probably because the caller doesn't plan to encrypt any more files.

In:

  karacell_base->block_base_list_base as defined in karacell_init():Out or karacell_block_list_realloc():Out. It may be NULL, in which case no blocks may be allocated.

Out:

  karacell_base->block_base_list_base is NULL for good practice. This list and all the blocks to which it points have been freed.

  karacell_base->block_idx_max is undefined.
*/
  u32 **block_base_list_base;
  ULONG block_idx;

  block_base_list_base=karacell_base->block_base_list_base;
  if(block_base_list_base){
    block_idx=karacell_base->block_idx_max;
    do{
      karacell_free(block_base_list_base[block_idx]);
    }while(block_idx--);
    karacell_base->block_base_list_base=karacell_free(block_base_list_base);
  }
  return;
}

ULONG
karacell_list_size_get(ULONG idx_max,u32 item_size_minus_1){
/*
Compute the size of a list while checking for overflow. The idea is to abstract the whole problem of 32-bit vs. 64-bit allocation limits.

In:

  idx_max is the maximum index of the list. All values are allowed.

  item_size_minus_1 is 1 less than the size of an item in the list, on [0,U32_MAX-1].

Out:

  Returns 0 if the list size is not representable in a ULONG, else ((idx_max+1)*(item_size_minus_1+1)).
*/
  ULONG idx_count;
  u32 item_size;
  ULONG list_size;

  item_size=item_size_minus_1+1;
  idx_count=idx_max+1;
  list_size=idx_count*item_size;
  if(list_size){
    if((list_size/item_size)!=idx_count){
      list_size=0;
    }
  }  
  return list_size;
}

ULONG
karacell_block_list_realloc(u64 block_idx_max_new,karacell_t *karacell_base){
/*
Change the maximum precryptable payload size, in order to save memory or handle larger files more efficiently. Note that increasing this value actually endangers the ability of the system to function at all, as doing so consumes memory which might be required for other vital functions.

In:

  block_idx_max_new is the number of KARACELL_BLOCK_SIZE blocks in the largest anticipated payload, less 1. (Larger payloads can still be crypted, but perhaps less efficiently.)

  *karacell_base is as defined in karacell_init():Out or karacell_block_list_realloc():Out.

Out:

  Returns the maximum precryptable payload size, in units of KARACELL_BLOCK_SIZE, less 1. On success, this value will be at least enough to accomodate payload_size_max. On failure, precryption is still possible. In the event of failure, which is a realistic eventuality in small embedded systems, the caller should be aware that substantially all of memory has been allocated, so emergency conversation measures may be warranted, possibly including further calls to this function. If expansion has been requested since the previous call, or relative to the precryptable payload size provided by karacell_init(), then the return value will not be less than the former allocation provided. Note that the return value is a ULONG rather than a u64 because the maximum precryptable payload size depends on the address space size, as distinct from file sizes, which could exceed the address space size and are thus handled as (u64)s.

  karacell_base->block_base_list_base has been updated. There is a theoretical possibility that this list is now overallocated, but this is not a threat to stability, except to the extend that it's consuming memory.

  karacell_base->block_idx_max has been updated to reflect the new allocation of precyption blocks implied by payload_size_max.
*/
  u32 *block_base;
  ULONG block_idx;
  ULONG block_idx_max_old;
  u32 **block_base_list_base_new;
  u32 **block_base_list_base_old;
  ULONG block_base_list_size;
  ULONG block_base_list_size_minus_1;
/*
Each "block" actually consists of KARACELL_BLOCK_SIZE bytes of xor mask followed by KARACELL_HASH_SEED_SIZE_MAX bytes of hash seeds. We do this because it saves us from needing 2 pointer lists -- one for the xor mask and one for the seeds.

Make sure the caller isn't asking for a number of blocks which would result in numerical overflow, and clip the request if so.
*/
  block_base_list_size=karacell_list_size_get(block_idx_max_new,KARACELL_BLOCK_SIZE+KARACELL_HASH_SEED_SIZE_MAX-1);
  if(!block_base_list_size){
    block_idx_max_new=(ULONG_MAX/(KARACELL_BLOCK_SIZE+KARACELL_HASH_SEED_SIZE_MAX))-1;
    block_base_list_size=(ULONG_MAX/(KARACELL_BLOCK_SIZE+KARACELL_HASH_SEED_SIZE_MAX))*(KARACELL_BLOCK_SIZE+KARACELL_HASH_SEED_SIZE_MAX);
  }
  block_base_list_size_minus_1=block_base_list_size-1;
  block_idx_max_old=karacell_base->block_idx_max;
  block_base_list_base_old=karacell_base->block_base_list_base;
  block_base_list_base_new=NULL;
  if(block_idx_max_old<block_idx_max_new){
/*
Expand the allocation to the extent possible.
*/
    block_base_list_base_new=(u32 **)(karacell_realloc(block_base_list_base_old,block_base_list_size_minus_1));
    if(block_base_list_base_new){
      block_idx=block_idx_max_old;
      do{
        block_idx++;
        block_base=karacell_malloc(KARACELL_BLOCK_SIZE+KARACELL_HASH_SEED_SIZE_MAX-1);
        block_base_list_base_new[block_idx]=block_base;
      }while(block_base&&(block_idx!=block_idx_max_new));
      if(!block_base){
/*
We're out of memory. Try to shrink *block_base_list_base_new in order to save whatever memory we can.
*/
        block_base_list_size_minus_1=(block_idx*sizeof(u32 *))-1;
        block_idx_max_new=block_idx-1;
        block_base_list_base_old=block_base_list_base_new;
        block_base_list_base_new=(u32 **)(karacell_realloc(block_base_list_base_new,block_base_list_size_minus_1));
/*
Technically realloc() can fail on a shrink. If this happened, then we're stuck with an overallocated base list, but it doesn't matter because we'll set block_base_list_base_new==block_base_list_base_old below.
*/
      }
    }else{
/*
We're out of memory. Perhaps some tiny further allocation is possible, but it's not worth pursuing because we can still function without it.
*/
      block_idx_max_new=block_idx_max_old;
    }
  }else if(block_idx_max_new<block_idx_max_old){
    block_idx=block_idx_max_old;
    do{
      karacell_free(block_base_list_base_old[block_idx]);
      block_idx--;
    }while(block_idx!=block_idx_max_new);
    block_base_list_base_new=(u32 **)(karacell_realloc(block_base_list_base_old,block_base_list_size_minus_1));
  }
  if(!block_base_list_base_new){
    block_base_list_base_new=block_base_list_base_old;
  }
  karacell_base->block_base_list_base=block_base_list_base_new;
  karacell_base->block_idx_max=block_idx_max_new;
  return block_idx_max_new;
}

karacell_t *
karacell_free_all(karacell_t *karacell_base){
/*
Free all Karacell data structures.

In:

  *karacell_base is as defined in karacell_init():Out or karacell_block_list_realloc():Out. May be NULL.

Out:

  Returns NULL for good practice.
*/
  if(karacell_base){
    SPAWN_FREE(karacell_base->spawn_base);
    karacell_free(karacell_base->thread_list_base);
    karacell_free(karacell_base->header_base);
    karacell_block_list_free(karacell_base);
    karacell_base=karacell_free(karacell_base);
  }
  return karacell_base;
}

karacell_thread_t *
karacell_thread_list_make(void){
/*
Prepare a list of (KARACELL_SIMULTHREAD_IDX_MAX+1) (karacell_thread_t)s to manage simulthreads for Spawn.

Out:

  Returns NULL on failure, else a pointer to the base of a list of (karacell_thread_t)s, with only tumbler_collision_bitmap initialized (to all 0s).
*/
  u32 bitmap_idx;
  karacell_thread_t *thread_list_base;
  u32 thread_idx;
  ULONG thread_list_size;
  u8 *tumbler_collision_bitmap_base;

  thread_list_size=karacell_list_size_get(KARACELL_SIMULTHREAD_IDX_MAX,(u32)(sizeof(karacell_thread_t)-1));
  thread_list_base=NULL;
  if(thread_list_size){
    thread_list_base=(karacell_thread_t *)(karacell_malloc(thread_list_size-1));
    if(thread_list_base){
      thread_idx=KARACELL_SIMULTHREAD_IDX_MAX+1;
      do{
        thread_idx--;
        tumbler_collision_bitmap_base=&thread_list_base[thread_idx].tumbler_collision_bitmap[0];
/*
We expect the compiler to unroll this loop and aggregate stores. memset() causes a host of compiler problems and is less portable.
*/
        for(bitmap_idx=0;bitmap_idx<=(KARACELL_TABLE_SIZE-1);bitmap_idx++){
          tumbler_collision_bitmap_base[bitmap_idx]=0;
        }
/*
Don't initialize any other fields because it's not required, and we want to expose badly written code which counts on undefined state.
*/
      }while(thread_idx);
    }
  }
  return thread_list_base;
}

u8
karacell_verify(void){
/*
Verify readonly data structures. In the process, increase the probability of them being cached, which thwarts timing analysis attacks.

Out:

  Returns 1 if an error is detected, else 0.
*/
  u64 lmd2_xor;

  lmd2_xor=LMD_STRING_LMD2_GET((u8 *)(karacell_complexity_log2_list_base),0,KARACELL_COMPLEXITY_LOG2_LIST_U16_COUNT<<1);
  lmd2_xor^=LMD_STRING_LMD2_GET((u8 *)(karacell_table_base),0,KARACELL_TABLE_SIZE);
  return (lmd2_xor!=0x6EE709B8E641CEEULL);
}

void
karacell_rewind(karacell_t *karacell_base){
/*
Reset the Karacell engine without entropizing the memory map.

In:

  *karacell_base is as defined in karacell_init():Out or karacell_block_list_realloc():Out.

Out:

  karacell_base->header_base->iv is undefined and unsuitable for use, awaiting a call to karacell_iv_set().

*/
  u32 *hash_xor_all_base;
  u32 *iv_base;
/*
The IV must be set by karacell_iv_set(). Expose bad code which attempts to reuse an IV!
*/
  iv_base=&karacell_base->header_base->iv[0];
  karacell_u32_list_zero(KARACELL_IV_U32_COUNT-1,0,iv_base);
/*
The xor of all hashes must start as 0.
*/
  hash_xor_all_base=&karacell_base->hash_xor_all[0];
  karacell_u32_list_zero(KARACELL_HASH_U32_COUNT_MAX-1,0,hash_xor_all_base);
  return;
}

karacell_t *
karacell_init(void){
/*
Allocate minimally sufficient memory for repeated use with file cryption transactions of any size.

Out:

  Returns NULL on failure, which could be due to insufficient memory or the failure or karacell_verify(), else the base of a karacell_t with use with other Karacell functions:

    block_base_list_base points to a single-entry block base list, which in turn points to the base of a single block of size (KARACELL_BLOCK_SIZE+KARACELL_HASH_SIZE_MAX) for precryption, implying a maximum precryptable payload size of KARACELL_BLOCK_SIZE -- the additional space being for a hash seed. It is expected that the caller will subsequently call karacell_block_list_realloc() in order to expand the list, if larger transactions are predicted to occur and memory space provides.

    header_base points to an undefined karacell_header_t, to be filled as needed in during cryption.

      iv is undefined and unsuitable for use, awaiting a call to karacell_iv_set().

    block_idx_max is 0, reflecting the allocation of a single block.

    master_key is undefined, awaiting a call to karacell_master_key_set().

    thread_list_base is as defined in the return value of karacell_thread_list_make().

    tumbler_idx_max is undefined, awaiting a call to karacell_master_key_set().
*/
  u32 *block_base;
  u32 **block_base_list_base;
  karacell_t *karacell_base;
  karacell_header_t *header_base;
  spawn_t *spawn_base;
  u8 status;
  karacell_thread_t *thread_list_base;

  karacell_base=(karacell_t *)(karacell_malloc(sizeof(karacell_t)-1));
  if(karacell_base){
    block_base_list_base=(u32 **)(karacell_malloc(sizeof(u32 *)-1));
    if(block_base_list_base){
      karacell_base->block_base_list_base=block_base_list_base;
/*
Allocate enough space for a block followed by a hash seed.
*/
      block_base=(u32 *)(karacell_malloc(KARACELL_BLOCK_SIZE+KARACELL_HASH_SIZE_MAX-1));
      status=!block_base;
      block_base_list_base[0]=block_base;
      header_base=(karacell_header_t *)(karacell_malloc(sizeof(karacell_header_t)-1));
      status|=!header_base;
      karacell_base->header_base=header_base;
      karacell_base->block_idx_max=0;
/*
Leave crypt_block_count_minus_1, crypt_block_idx_min, crypt_u32_idx_min, and crypt_xor_idx_min undefined, which will be set by karacell_crypt_spawn() or karacell_xor_spawn().

Leave master_key undefined, which must be set by karacell_master_key_set().
*/
      thread_list_base=karacell_thread_list_make();
      status|=!thread_list_base;
      karacell_base->thread_list_base=thread_list_base;
      spawn_base=SPAWN_INIT(&karacell_thread_execute,(u8 *)(karacell_base),KARACELL_SIMULTHREAD_IDX_MAX);
      status|=!spawn_base;
      karacell_base->spawn_base=spawn_base;
      if(!status){
        karacell_rewind(karacell_base);
/*
Other fields will be initialized by other functions. Cache and verify the important data tables before returning.
*/
        status=karacell_verify();
      }
      if(status){
        karacell_base=karacell_free_all(karacell_base);
      }
    }else{
      karacell_base=karacell_free(karacell_base);
    }
  }
  return karacell_base;
}
