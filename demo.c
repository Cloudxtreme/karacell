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
#include "simulthread.h"
#include "debug.h"
#include "table.h"
#ifdef JYTTER
  #include "jytter.h"
#endif
#include "spawn.h"
#include "spawn_xtrn.h"
#include "karacell.h"
#include "karacell_xtrn.h"
#include "entropy_xtrn.h"
#include "listcrypt_xtrn.h"
#include "lmd_xtrn.h"
#include "print.c"
#include "debug.c"

#define GARBAGE_U32_COUNT 12345

int
main(void){
  u64 block_idx;
  u32 entropy_list_base[KARACELL_MASTER_KEY_U32_COUNT_MAX];
  u64 file_idx;
  u32 garbage;
  u8 hash_type;
  u32 iv_base[KARACELL_IV_U32_COUNT];
  u64 lmd2;
  karacell_t *karacell_base;
  u32 master_key_base[KARACELL_MASTER_KEY_U32_COUNT_MAX];
  u8 status;
  karacell_thread_t *thread_base;
  u16 tumbler_bias;
  u8 tumbler_idx_max;
  u16 *tumbler_list_base;
  ULONG u16_count;
  ULONG u16_idx;
  ULONG u16_idx_min;
  u16 *u16_list_base;
  ULONG u32_count;
  ULONG u32_idx;
  ULONG u32_idx_min;
  u32 *u32_list_base;
  ULONG u8_count;
  ULONG u8_idx;
  ULONG u8_idx_min;
  u8 *u8_list_base;
  u8 unauthenticated_status;
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
  karacell_free_all(karacell_base);
/*
Now let's demo the use of cryption functions without the OS calls used in main.c. (There might be OS calls exclusively for multithreading, but even these can be disabled by building with PTHREAD_OFF as in flag.h.)

First, we need to generate some entropy, so read the warning in true_random_get(). The demo uses jytter, but modify trng.c as you see fit. Once this entropy pool has been generated, you can use it for all 2^64 possible values of file_idx, provided that no value is ever reused. In principle, you could save this entropy pool to secure storage, provided that it were ever only used with a single symmetric cryption peer. One direction of communication could then use even file_idx values, and the other could use odd values, but this is a higher level consideration beyond the scope of Karacell. Careful attention must be made to prevent replay attacks, for example, by xoring both peers' newly generated entropy pools each time file_idx falls out of sync.

Please note, unless there's some desynchronization of file_idx between peers, there is no reason to ever regenerate entropy for a given pair of peers. So please avoid redundant entropy generation in order to minimize communication latency.
*/
  entropy_make(entropy_list_base);
/*
Allocate a data structure for Karacell. Unless you want to use different master keys for both directions of communication, which is much more awkward than simply alternating (file_idx)s, you can reuse this data structure indefinitely, so long as it never gets copied to untrusted storage.
*/
  karacell_init();
  karacell_base=karacell_init();
/*
This next bit just allocates memory for cryption testing. You can delete this if you already have a string somewhere that you want to crypt. Let's allocate 12,345 (u32)s for the heck of it.
*/
  u32_list_base=(u32 *)(malloc(GARBAGE_U32_COUNT<<U32_SIZE_LOG2));
/*
Check to make sure memory allocation succeeded. This little section of code would have to be replaced by whatever exception path fits your particular application. I just stuck it here to make sure that you won't forget about this.
*/
  if(!(karacell_base&&u32_list_base)){
    print_error_no_memory();
    exit(1);
  }
/*
Now call karacell_tumbler_idx_max_get(). The best way is to do this once, upon master key (re)generation, in order to verify that it's a safe key to use, which is evidenced by nonzero tumbler_idx_max being returned. Then from that point onwards, just call it once after each karacell_init() in order to quickly recompute it without the hassle of reading it from secure storage. Needless to say, master_key_base must be filled in.
*/
  tumbler_idx_max=karacell_tumbler_idx_max_get(master_key_base);
/*
Here's the exception path to implement right after the first time a master key is (re)generated, as explained above. This code would not normally belong here, but at some do-it-once init that occurred long ago. So normally, we would just assume that tumbler_idx_max is safe, because it will always come back as the same value.
*/
  status=!tumbler_idx_max;
  if(status){
    print_error("Weak master key");
    exit(1);
  }
/*
End of tumbler_idx_max exception code.

Now, fill the demo string with deterministic garbage so we can verify correct behavior. This is obviously not what you would normally do.
*/
  garbage=0x87654321;
  for(u32_idx=0;u32_idx<=(GARBAGE_U32_COUNT-1);u32_idx++){
    u32_list_base[u32_idx]=garbage;
    garbage*=0xFEDCBA93;
  }
/*
This next bit is required, once per karacell_init(): call karacell_master_key_set() with the value of tumbler_idx_max obtained above and all KARACELL_MASTER_KEY_U32_COUNT_MAX defined (high 0s for padding). We're just going to use the existing demo master key used above.
*/
  karacell_master_key_set(karacell_base,master_key_base,0,tumbler_idx_max);
/*
We're now ready to crypt. Just make sure that file_idx is never ever reused until either the entropy pool or the master key is TOTALLY regenerated. But incrementing file_idx until that time is safe. Again, you may want to give one peer the evens, and the other, the odds. Or add 5s for a key shared by 5 people. Whatever. But if you see evidence that Karacell crashed and was restarted, you need a new entropy pool in order to avoid the possibility of file_idx reuse. So you need to atomically journal the start and end of a Karacell session on some secure storage, but that's beyond the scope of Karacell itself.

Let's just start at some random file_idx. You can start at 0.
*/
  file_idx=0x9E3EE13E409BE575;
/*
Let's use an LMD8 hash. You could just not use a hash, in the name of speed, but that probably wouldn't be wise except in the case of media streams with verified buffer overflow immunity. This probably makes the most practical sense, in the sense maximum speed with sufficient security. But choose another hash type if you wish.
*/
  hash_type=KARACELL_HASH_TYPE_LMD8;
/*
Let's encrypt the u32 list in the index range [51,9955]. In other words, u32_idx_min==51 and u32_count==9905. We don't need to worry about defining unauthenticated_status because we're not decrypting.
*/
  u32_idx_min=51;
  u32_count=9905;
/*
Take a quick LMD2 of the unencrypted substring, so we can verify that it decrypts properly after encryption.
*/
  lmd2=LMD_STRING_LMD2_GET((u8 *)(&u32_list_base[u32_idx_min]),0,(u32)(u32_count<<U32_SIZE_LOG2));
  status=listcrypt_u32_list_crypt(entropy_list_base,file_idx,karacell_base,&hash_type,&u32_count,u32_idx_min,u32_list_base,&unauthenticated_status);
/*
(Note that hash_type, u32_count, and unauthenticated_status are not modified by encryption, only decryption.)

Since encryption is not allowed to allocate memory, the following exception code should never occur, but in theory, it could if you provided listcrypt_u32_list_crypt() with bad inputs.
*/
  if(status){
    print_error("32-bit encryption failed");
    exit(1);
  }
/*
So now, u32_list_base[51,9955] is encrypted. But in order to avoid the hassles of memory reallocation, the header is at karacell_base->header_base and the hash is at karacell_base->hash_xor_all.

Normally, we would never encrypt, then decrypt, the same string in the same place. So an encrypted string would begin with its header and end with its hash. But in this test case, we're doing a roundtrip crypt. So hack up a copy of the header and hash in their expected places.

So all this is just to show that you need to write or send out 3 data structures in the correct order: (1) karacell_base->header_base, (2) u32_list_base[51,9955], (3) karacell_base->hash_xor_all, with #3 having the expected hash size (0 in the case of no hash, and LMD8_U32_COUNT in this case).

Apologies for the atrocious memcpy(). It's just a hack anyway, so relax.
*/
  memcpy((void *)(&u32_list_base[u32_idx_min-KARACELL_HEADER_U32_COUNT]),(const void *)(karacell_base->header_base),sizeof(karacell_header_t));
  karacell_u32_list_copy(0,&karacell_base->hash_xor_all[0],u32_idx_min+u32_count,u32_list_base,LMD8_U32_COUNT-1);
/*
Now decrypt this junk. Back up u32_idx_min and adjust u32_count to reflect an encrypted string.
*/
  u32_idx_min-=KARACELL_HEADER_U32_COUNT;
  u32_count+=KARACELL_HEADER_U32_COUNT+LMD8_U32_COUNT;
/*
Set hash_type==0 in order to say that we want to decrypt.
*/
  hash_type=0;
  status=listcrypt_u32_list_crypt(entropy_list_base,file_idx,karacell_base,&hash_type,&u32_count,u32_idx_min,u32_list_base,&unauthenticated_status);
/*
The following exception code could occur in real life, most likely due to karacell_header_decrypt() finding a consistency check failure in the header, or the hash failing to verify.
*/
  if(status){
    print_error("32-bit decryption failed");
    exit(1);
  }
/*
The hash remains encrypted, but the header has been decrypted at u32_list_base[u32_idx_min] for all KARACELL_HEADER_U32_COUNT (u32)s, so you can inspect it if you wish, being mindful that it does not come under hash protection (see karacell_header_decrypt() for full consistency checking discussion). In reality, though, hash_type, unauthenticated_status, and u32_count are all you need to check. u32_count might be 0, and needs to be checked in light of the receiver's specific expectations; in no event will a u32_count be returned which would result in buffer overflow, or which corresponds to a payload_size that is not a multiple of U32_SIZE.

Rewrite this exception code to suit your application:
*/
  if(unauthenticated_status||(hash_type!=KARACELL_HASH_TYPE_LMD8)){
    print_error("Expected LMD8 hash was not received");
    exit(1);
  }
/*
Check that the decrypted message is correct. Ignore the header and hash, because the hash has already verified.
*/
  u32_idx_min+=KARACELL_HEADER_U32_COUNT;
  lmd2-=LMD_STRING_LMD2_GET((u8 *)(&u32_list_base[u32_idx_min]),0,(u32)(u32_count<<U32_SIZE_LOG2));
  if(!lmd2){
    printf("32-bit cryption test passed.\n");
  }else{
    print_error("32-bit cryption test failed");
    exit(1);
  }
/*
Do the same thing, more or less, but with (u16)s instead of (u32)s.
*/
  u16_list_base=(u16 *)(malloc(GARBAGE_U32_COUNT<<U32_SIZE_LOG2));
  if(!u16_list_base){
    print_error_no_memory();
    exit(1);
  }
  garbage=0x87654321;
  for(u16_idx=0;u16_idx<=((GARBAGE_U32_COUNT<<1)-1);u16_idx++){
    u16_list_base[u16_idx]=(u16)(garbage);
    garbage*=0xFEDCBA93;
  }
/*
No need to repeat karacell_master_key_set(), as *karacell_base is still intact. It just requires karacell_rewind(), but that's done for us automatically by karacell_string_crypt_prepare().

Now, let's pretend that this key is shared by 5 people in some kind of private group. Then each person in the group has a file_idx which is a unique modulo of 5. So increment file_idx accordingly.
*/
  file_idx+=5;
/*
Let's change the hash to LMD7 for the heck of it.
*/
  hash_type=KARACELL_HASH_TYPE_LMD7;
/*
Set up some random base index and cryption region size.
*/
  u16_idx_min=29;
  u16_count=4597;
/*
Take an LMD2 for verification purposes below.
*/
  lmd2=LMD_STRING_LMD2_GET((u8 *)(&u16_list_base[u16_idx_min]),0,(u32)(u16_count<<U16_SIZE_LOG2));
/*
Notice how the format of listcrypt_u16_list_crypt() mirrors listcrypt_u32_list_crypt(), but for the trivial type difference.
*/
  status=listcrypt_u16_list_crypt(entropy_list_base,file_idx,karacell_base,&hash_type,&u16_count,u16_idx_min,u16_list_base,&unauthenticated_status);
  if(status){
    print_error("16-bit encryption failed");
    exit(1);
  }
/*
Do the same hack to copy the encrypted header and encrypted hash to the header and footer of the encrypted substring, just as in the u32 case above, but with LMD7 instead of LMD8.
*/
  memcpy((void *)(&u16_list_base[u16_idx_min-KARACELL_HEADER_U16_COUNT]),(const void *)(karacell_base->header_base),sizeof(karacell_header_t));
  memcpy((void *)(&u16_list_base[u16_idx_min+u32_count]),(const void *)(&karacell_base->hash_xor_all[0]),LMD7_SIZE);
/*
...and decrypt...
*/
  u16_idx_min-=KARACELL_HEADER_U16_COUNT;
  u16_count+=KARACELL_HEADER_U16_COUNT+(LMD7_U32_COUNT<<1);
  hash_type=0;
  status=listcrypt_u16_list_crypt(entropy_list_base,file_idx,karacell_base,&hash_type,&u16_count,u16_idx_min,u16_list_base,&unauthenticated_status);
  if(status){
    print_error("16-bit decryption failed");
    exit(1);
  }
  if(unauthenticated_status||(hash_type!=KARACELL_HASH_TYPE_LMD7)){
    print_error("Expected LMD7 hash was not received");
    exit(1);
  }
  u16_idx_min+=KARACELL_HEADER_U16_COUNT;
  lmd2-=LMD_STRING_LMD2_GET((u8 *)(&u16_list_base[u16_idx_min]),0,(u32)(u16_count<<U16_SIZE_LOG2));
  if(!lmd2){
    printf("16-bit cryption test passed.\n");
  }else{
    print_error("16-bit cryption test failed");
    exit(1);
  }
/*
Do it once more, this time for a u8 list with an LMD8 hash.
*/
  u8_list_base=malloc(GARBAGE_U32_COUNT<<U32_SIZE_LOG2);
  if(!u8_list_base){
    print_error_no_memory();
    exit(1);
  }
  garbage=0x87654321;
  for(u8_idx=0;u8_idx<=((GARBAGE_U32_COUNT<<U32_SIZE_LOG2)-1);u8_idx++){
    u8_list_base[u8_idx]=(u8)(garbage);
    garbage*=0xFEDCBA93;
  }
  file_idx+=5;
  hash_type=KARACELL_HASH_TYPE_LMD8;
  u8_idx_min=191;
  u8_count=9193;
/*
Take an LMD2 for verification purposes below.
*/
  lmd2=LMD_STRING_LMD2_GET((u8 *)(&u8_list_base[u8_idx_min]),0,(u32)(u8_count));
  status=listcrypt_u8_list_crypt(entropy_list_base,file_idx,karacell_base,&hash_type,&u8_count,u8_idx_min,u8_list_base,&unauthenticated_status);
  if(status){
    print_error("8-bit encryption failed");
    exit(1);
  }
  memcpy((void *)(&u8_list_base[u8_idx_min-KARACELL_HEADER_SIZE]),(const void *)(karacell_base->header_base),sizeof(karacell_header_t));
  memcpy((void *)(&u8_list_base[u8_idx_min+u32_count]),(const void *)(&karacell_base->hash_xor_all[0]),LMD8_SIZE);
  u8_idx_min-=KARACELL_HEADER_SIZE;
  u8_count+=KARACELL_HEADER_SIZE+LMD8_SIZE;
  hash_type=0;
  status=listcrypt_u8_list_crypt(entropy_list_base,file_idx,karacell_base,&hash_type,&u8_count,u8_idx_min,u8_list_base,&unauthenticated_status);
  if(status){
    print_error("8-bit decryption failed");
    exit(1);
  }
  if(unauthenticated_status||(hash_type!=KARACELL_HASH_TYPE_LMD8)){
    print_error("Expected LMD8 hash was not received");
    exit(1);
  }
  u8_idx_min+=KARACELL_HEADER_SIZE;
  lmd2-=LMD_STRING_LMD2_GET((u8 *)(&u8_list_base[u8_idx_min]),0,(u32)(u8_count));
  if(!lmd2){
    printf("8-bit cryption test passed.\n");
  }else{
    print_error("8-bit cryption test failed");
    exit(1);
  }
/*
Free karacell_base and associated structures. You don't need to do this for every file_idx incrementation -- just whenever file_idx wraps at some point, if ever, or simply when you don't mind repeating karacell_init() later because you want to shut down the peer conversation for now.

You might want to free some peer conversation strings as well, but that's your business. We won't bother to free the demo data structures.
*/
  karacell_free_all(karacell_base);
  return 0;
}
