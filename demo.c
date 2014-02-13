/*
Mathematica Demo for Karacell, in C

The functions in this file are for demonstration purposes, and are not part of the Karacell specification.
*/
#include "flag.h"
#include "unix_include.h"
#include "constant.h"
#include "lmd2.h"
#include "lmd7.h"
#include "lmd8.h"
#include "tweak.h"
#include "debug.h"
#include "spawn.h"
#include "table.h"
#include "karacell.h"
#include "lmd2.c"
#include "lmd7.c"
#include "lmd8.c"
#include "print.c"
#include "debug.c"
#include "spawn.c"
#include "karacell.c"

int
main(void){
  u64 block_idx;
  u32 iv_base[KARACELL_IV_U32_COUNT];
  u64 lmd2;
  karacell_t *karacell_base;
  u32 master_key_base[KARACELL_MASTER_KEY_U32_COUNT_MAX];
  karacell_thread_t *thread_base;
  u16 tumbler_bias;
  u8 tumbler_idx_max;
  u16 *tumbler_list_base;
  u32 *xor_mask_base;
  u32 xor_mask_idx_max;
/*
Set the master key to the same value as in mathematica.txt.
*/
  master_key_base[0]=0x6E8A30BC;
  master_key_base[1]=0x4A3D13CA;
  master_key_base[2]=0xA6996DAA;
  master_key_base[3]=0x8B19DB57;
  master_key_base[4]=0xE939FE76;
  master_key_base[5]=0x8B03E2A1;
  master_key_base[6]=0x4EE9B198;
  master_key_base[7]=0x491B23FD;
  master_key_base[8]=0x6D56B544;
  master_key_base[9]=0x44EBCBC5;
  master_key_base[10]=0x88D530DF;
  master_key_base[11]=0x3B97BF68;
  master_key_base[12]=0x652EAA14;
  master_key_base[13]=0x2CCB6F2E;
  master_key_base[14]=0x28FC60A8;
  master_key_base[15]=0xD4349585;
/*
Get the number of tumblers sufficient to ensure that Subset Sum is as hard as counting to the master key.
*/
  tumbler_idx_max=karacell_tumbler_idx_max_get(master_key_base);
  if(tumbler_idx_max!=(KARACELL_TUMBLER_COUNT_MAX-1)){
    printf("Demo is broken. Oops!\n");
    exit(1);
  }
/*
Set the tumbler_bias to the same illegal value as in mathematica.txt, just to show how it functions.
*/
  tumbler_bias=0xAEDB;
/*
Set the block index to the same crazy but legal value as in mathematica.txt.
*/
  block_idx=0xCA298624A4A8E8D5;
/*
Set the IV to the same value as in mathematica.txt.
*/
  iv_base[0]=0xFBE33275;
  iv_base[1]=0x9257D1F9;
  iv_base[2]=0x4792D6DA;
  iv_base[3]=0xBB89F4B5;
  iv_base[4]=0x9335FCC5;
  iv_base[5]=0x242BC383;
  iv_base[6]=0xADFABE12;
  iv_base[7]=0x15A497E7;
  karacell_base=karacell_init();
/*
Grab a block of memory to use as plaintext.
*/
  xor_mask_base=karacell_malloc(KARACELL_BLOCK_SIZE-1);
  if(!(karacell_base&&xor_mask_base)){
    print_error_no_memory();
    exit(1);
  }
/*
Set the plaintext to 0 so the mask will show through.
*/
  karacell_u32_list_zero(KARACELL_BLOCK_U32_COUNT-1,0,xor_mask_base);
/*
Install the master key and the IV.
*/
  karacell_master_key_set(karacell_base,master_key_base,0,tumbler_idx_max);
  karacell_iv_set(karacell_base,iv_base,0);
/*
We're not doing anything multithreaded here, so we can use the first karacell_thread_t in the thread list for test purposes. (karacell_init() guarantees that this first thread structure is always present.)
*/
  thread_base=&karacell_base->thread_list_base[0];
/*
Make some tumblers. (As you can see in karacell.c, this is all abstracted for the caller. We're using this low-level interface to let you get closer to the engine.)
*/
  karacell_tumbler_list_make(block_idx,karacell_base,thread_base);
/*
Set the maximum u32 index to the same value as mathematica.txt.
*/
  xor_mask_idx_max=0x33A;
  karacell_subblock_crypt(karacell_base,thread_base,tumbler_bias,xor_mask_idx_max,0,xor_mask_base);
/*
Dump the xor mask  for comparison to Mathematica, which is just the ciphertext because the plaintext was 0. (We dump this first because it's huge and scrolls everything off the screen.)
*/
  DEBUG_STRING_PRINT("xor_mask",xor_mask_idx_max+1,(u8 *)(xor_mask_base),2);
/*
Dump the tumblers as well.
*/
  tumbler_list_base=&thread_base->tumbler_list[0];
  DEBUG_STRING_PRINT("tumbler_list",KARACELL_TUMBLER_COUNT_MAX,(u8 *)(tumbler_list_base),1);
/*
Because the mask is long and difficult to compare, just check its LMD2 against what Mathematica computed.
*/
  lmd2=LMD_STRING_LMD2_GET((u8 *)(xor_mask_base),0,(xor_mask_idx_max+1)<<U32_SIZE_LOG2);
  DEBUG_U64("xor_mask_lmd2",lmd2);
  if(lmd2==0xB7DCACFB939F6B48ULL){
    printf("Yay! It's correct.\n");
  }else{
    printf("Uh oh! It's wrong.\n");
  }
  return 0;
}
