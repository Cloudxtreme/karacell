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
Operating System Interface for Karacell

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
#include "file_sys.h"
#include "table.h"
#include "jytter.h"
#include "karacell.h"
#include "lmd2.c"
#include "lmd7.c"
#include "lmd8.c"
#include "print.c"
#include "debug.c"
#include "file_sys.c"
#include "spawn.c"
#include "karacell.c"
#include "trng.c"
#include "entropy.c"
#include "ascii.c"
#ifdef DEBUG
  #ifdef _32_
    extern void timestamp_get(uint64_t *timestamp_base) __attribute__((fastcall));
  #else
    extern void timestamp_get(uint64_t *timestamp_base);
  #endif
#endif

int
main(int argc, char *argv[]){
  u8 authentication_fail_status;
  u64 block_idx_minus_1;
  ULONG burst_size;
  u8 decrypt_status;
  u32 entropy_list_base[KARACELL_MASTER_KEY_U32_COUNT_MAX];
  u8 entropy_spawn_status;
  FILE *handle_in;
  FILE *handle_out;
  u8 hash_type;
  u32 hash_size;
  u32 hash_xor_all_base[KARACELL_HASH_U32_COUNT_MAX];
  karacell_header_t *header_base;
  u8 header_status;
  u32 *iv_base;
  karacell_t *karacell_base;
  u32 master_key_base[KARACELL_MASTER_KEY_U32_COUNT_MAX];
  u8 *mode_base;
  u8 mode;
  u8 overwrite_status;
  u8 *overwrite_status_base;
  u32 *payload_base;
  u64 payload_block_idx_max;
  u64 payload_size;
  ULONG precrypt_block_idx_max;
  u8 status;
  spawn_t *spawn_base;
  #ifdef DEBUG
    u64 timestamp_delta;
    u64 timestamp_end;
    u64 timestamp_start;
  #endif
  u8 tumbler_idx_max;

  #ifdef DEBUG
    timestamp_get(&timestamp_start);
  #endif
  status=1;
  authentication_fail_status=0;
  entropy_spawn_status=0;
  handle_in=NULL;
  handle_out=NULL;
  karacell_base=NULL;
  payload_base=NULL;
  do{
    #ifndef DEBUG
      if(argc!=6){
    #endif
        printf("Karacell 3, build %d\nA file encryption/decryption utility.\nCopyright 2013 TigerSpike Products Pte Ltd, all rights reserved.\n\n",BUILD_NUMBER);
    #ifndef DEBUG
      }
    #endif
    fflush(stdout);
    if(argc!=6){
      printf("build_switches=\"");
      #ifdef DEBUG
        printf("DEBUG");
      #endif
      #ifdef PIPELINE
        #ifdef DEBUG
          printf(",");
        #endif
        printf("PIPELINE");
      #endif
      #ifdef PTHREAD
        #if defined(DEBUG)||defined(PIPELINE)
          printf(",");
        #endif
        printf("PTHREAD");
      #endif
      printf("\"\n\n");
      printf("Syntax:\n\n  karacell [mode] [overwrite] [key] [input file] [output file]\n\nwhere:\n\n");
      printf("  [mode] is:\n");
      print_new_line();
      printf("    0 to decrypt.\n");
      printf("    1 to encrypt with no hash (untrustworthy but private and fast).\n");
      printf("    2 to encrypt with LMD8 hash (512 bits).\n");
      printf("    3 to encrypt with LMD7 hash (1024 bits).\n");
      print_new_line();
      printf("  [overwrite] is:\n");
      print_new_line();
      printf("    0 to not overwrite [output file].\n");
      printf("    1 to automatically overwrite [output file].\n");
      print_new_line();
      printf("  [key] is at least 2^%d and consists of up to %d hexadecimal digits.\n\n",karacell_complexity_log2_list_base[(KARACELL_TUMBLER_COUNT_MIN>>1)-2],KARACELL_MASTER_KEY_U32_COUNT_MAX<<3);
      #ifdef WINDOWS
        printf("  [input file] is the input file, for example, \"c:\\My Documents\\document.pdf\".\n\n");
      #else
        printf("  [input file] is the input file, for example, \"/home/username/document.pdf\".\n\n");
      #endif
      printf("  [output file] is the output file, which must differ from [input file].\n");
      break;
    }
    mode_base=(u8 *)(argv[1]);
    mode=mode_base[0];
    status=!mode;
    mode-='0';
/*
If mode was null then we can't read the memory at mode_base[1], so be careful. If it's out of range, set status.
*/
    if((!status)&&((KARACELL_HASH_TYPE_MAX<mode)||(mode_base[1]))){
      status=1;
    }
    hash_type=mode;
    switch(mode){
    case 0:
      decrypt_status=1;
      break;
    case KARACELL_HASH_TYPE_NONE:
    case KARACELL_HASH_TYPE_LMD8:
    case KARACELL_HASH_TYPE_LMD7:
      decrypt_status=0;
      break;
    default:
      status=1;
    }
    overwrite_status_base=(u8 *)(argv[2]);
    overwrite_status=overwrite_status_base[0];
    status|=!overwrite_status;
    overwrite_status-='0';
/*
If overwrite_status was null then we can't read the memory at overwrite_status_base[1], so be careful. If it's out of range, set status.
*/
    if((!status)&&((1<overwrite_status)||(overwrite_status_base[1]))){
      status=1;
    }
    status|=ascii_master_key_get(master_key_base,(u8 *)(argv[3]));
    tumbler_idx_max=karacell_tumbler_idx_max_get(master_key_base);
    status|=!tumbler_idx_max;
    if(status){
      print_error("Bad syntax. (Illegal or weak key, perhaps?) Run again with no parameters");
      break;
    }
    status=1;
    DEBUG_STRING_PRINT("master_key",KARACELL_MASTER_KEY_U32_COUNT_MAX,(u8 *)(master_key_base),2);
    DEBUG_U8("tumbler_count",tumbler_idx_max+1);
    if(!overwrite_status){
      handle_out=file_open((u8 *)(argv[5]),FILE_READ);
      if(handle_out){
        print_error("Output file already exists");
        break;
      }
    }
    handle_in=file_open((u8 *)(argv[4]),FILE_READ);
    if(!handle_in){
      print_error("Input file not found");
      break;
    }
    handle_out=file_open((u8 *)(argv[5]),FILE_WRITE);
    if(!handle_out){
      print_error("Cannot open output file");
      break;
    }
    karacell_base=karacell_init();
    if(!karacell_base){
      print_error("Out of memory or internal data structures corrupt");
      break;
    }
    header_base=karacell_base->header_base;
    karacell_master_key_set(karacell_base,master_key_base,0,tumbler_idx_max);
    if(!decrypt_status){
/*
Go make *entropy_list_base in another thread, which will cause it to run faster due to increased timing entropy.
*/
      spawn_base=entropy_make_spawn(entropy_list_base);
      if(!spawn_base){
        print_error("Spawn failed");
        break;
      }
      entropy_spawn_status=1;
    }
    status=file_size_get_rewind(handle_in,&payload_size);
    if(status){
      print_error_input_file();
      break;
    }
    if(decrypt_status){
      status=karacell_header_size_check(payload_size);
      if(status){
        print_error("Input file is smaller than a Karacell header");
        break;
      }
      status=file_read(sizeof(karacell_header_t),(u8 *)(header_base),handle_in);
      if(status){
        print_error_input_file();
        break;
      }
      header_status=karacell_header_decrypt(&payload_size,&hash_size,karacell_base);
      status=1;
      switch(header_status){
      case 0:
        status=0;
        break;
      case 1:
/*
This is not an error case; there's no requirement that we report the apparent availability of a new version of Karacell, but it's nice thing to do.
*/
        status=0;
        printf("NOTE: This file indicates that a newer Karacell build is now available.\n");
        break;
      case 2:
        print_error("File was encrypted with a more recent Karacell build");
        break;
      case 3:
        print_error("Wrong key");
        break;
      case 4:
        print_error("It's almost certain that the key is correct but the file is corrupt");
        break;
      }
      if(status){
        break;
      }
    }
    status=1;
    burst_size=KARACELL_BLOCK_SIZE;
    if(burst_size<payload_size){
/*
Do the cryption in a series of multithreaded bursts in parallel with file IO. First, compute the burst size in blocks, not to exceed the lesser of KARACELL_BLOCKS_PER_BURST or the number of whole blocks in the payload.
*/
      payload_block_idx_max=(payload_size-1)>>KARACELL_BLOCK_SIZE_LOG2;
/*
Don't count the tail block, which we'll crypt serially below.
*/
      payload_block_idx_max--;
/*
0<KARACELL_BLOCKS_PER_BURST<=ULONG_MAX, so precrypt_block_idx_max won't wrap. Find out how many blocks we can burst, before hitting the tail block.
*/
      precrypt_block_idx_max=KARACELL_BLOCKS_PER_BURST-1;
      if(payload_block_idx_max<=precrypt_block_idx_max){
        precrypt_block_idx_max=payload_block_idx_max;
      }
/*
Try to allocate enough space for the desired burst size. Just take whatever we can get.
*/
      precrypt_block_idx_max=karacell_block_list_realloc(precrypt_block_idx_max,karacell_base);
      burst_size=(precrypt_block_idx_max+1)<<KARACELL_BLOCK_SIZE_LOG2;
    }
    payload_base=(u32 *)(karacell_malloc(burst_size-1));
    if(!payload_base){
      print_error_no_memory();
      break;
    }   
    iv_base=&header_base->iv[0];
    if(!decrypt_status){
/*
We're now prepared to crypt, so it's time to finalize *entropy_list_base.
*/
      entropy_make_flush(spawn_base);
      entropy_spawn_status=0;
      DEBUG_STRING_PRINT("entropy_list",KARACELL_MASTER_KEY_U32_COUNT_MAX,(u8 *)(entropy_list_base),2);
/*
Set file_idx==0 because this is the only file that will be encrypted with this entropy pool. In real life, it would be incremented from one file to the next using the same *entropy_list_base.
*/
      entropy_iv_make(entropy_list_base,0,iv_base,0,karacell_base);
      karacell_iv_set(karacell_base,iv_base,0);
    }
    DEBUG_STRING_PRINT("iv",KARACELL_IV_U32_COUNT,(u8 *)(iv_base),2);
    if(!decrypt_status){
      status=karacell_header_make(&hash_size,hash_type,karacell_base,payload_size);
      if(status){
        print_error("Input file size invalid");
        break;
      }
      status=file_write(sizeof(karacell_header_t),(u8 *)(header_base),handle_out);
      if(status){
        print_error_output_file();
        break;
      }
    }
    status=0;
    block_idx_minus_1=0;
    while(KARACELL_BLOCK_SIZE<payload_size){
      if(payload_size<burst_size){
        precrypt_block_idx_max=(payload_size-1)>>KARACELL_BLOCK_SIZE_LOG2;
/*
Don't count the tail block, which we'll crypt serially below.
*/
        burst_size=precrypt_block_idx_max<<KARACELL_BLOCK_SIZE_LOG2;
        precrypt_block_idx_max--;
      }
      #ifdef PIPELINE
        karacell_prefab_spawn(precrypt_block_idx_max,block_idx_minus_1,karacell_base);
      #endif
      status=file_read(burst_size,(u8 *)(payload_base),handle_in);
      #ifdef PIPELINE
/*
Flush all active threads before we check on status, and potentially exit the while.
*/
        karacell_thread_flush(karacell_base);
      #endif
      if(status){
        print_error_input_file();
        break;
      }
      #ifdef PIPELINE
        karacell_xor_spawn(precrypt_block_idx_max,block_idx_minus_1,decrypt_status,karacell_base,0,payload_base);
      #else
        karacell_crypt_spawn(precrypt_block_idx_max,block_idx_minus_1,decrypt_status,karacell_base,0,payload_base);
      #endif
      karacell_thread_flush(karacell_base);
      status=file_write(burst_size,(u8 *)(payload_base),handle_out);
      if(status){
        print_error_output_file();
        break;
      }
      block_idx_minus_1+=precrypt_block_idx_max;
      block_idx_minus_1++;
      payload_size-=burst_size;
    }
    if(status){
      break;
    }
    if(payload_size){
      status=file_read(payload_size,(u8 *)(payload_base),handle_in);
      if(status){
        print_error_input_file();
        break;
      }
    }
/*
Crypt the tail. We need to do this even if the tail is null (which implies that the payload is null), in order to evaluate the encrypted hash, if any.
*/
    karacell_tail_crypt(decrypt_status,karacell_base,block_idx_minus_1,(u32)(payload_size),0,payload_base);
    if(payload_size){
/*
Write the crypted tail block.
*/
      status=file_write(payload_size,(u8 *)(payload_base),handle_out);
      if(status){
        print_error_input_file();
        break;
      }
    }
    if(decrypt_status){
      hash_type=karacell_hash_type_get(karacell_base);
      status=1;
      switch(hash_type){
      case KARACELL_HASH_TYPE_NONE:
        print_warning("Output file is untrustworthy because input file has no hash");
        break;
      case KARACELL_HASH_TYPE_LMD8:
      case KARACELL_HASH_TYPE_LMD7:
/*
In practice, tune this switch statement so that only acceptably strong hashes result in status==0. This is purely an implementation decision and has nothing to do with Karacell.
*/
        status=0;
        break;
      default:
        print_warning("Unrecognized hash type. Output file is untrustworthy");
      }
      if(!status){
/*
Read the encrypted hash.
*/
        status=file_read(hash_size,(u8 *)(hash_xor_all_base),handle_in);
        if(status){
          print_error_input_file();
          break;
        }
        status=karacell_hash_xor_all_compare(hash_xor_all_base,karacell_base);
        if(status){
          print_error("Hash does not match! Output file is corrupt");
          break;
        }
      }else{
/*
The absence of an evaluable hash isn't a fatal error, but it's a problem that should be reported back to the OS.
*/
        status=0;
        authentication_fail_status=1;
      }
    }else{
      if(hash_size){
/*
Write the encrypted hash.
*/
        karacell_hash_xor_all_get(hash_xor_all_base,karacell_base);
        status=file_write(hash_size,(u8 *)(hash_xor_all_base),handle_out);
        if(status){
          print_error_output_file();
          break;
        }
      }
    }
/*
If we were going to crypt another file, we would need to call karacell_rewind() at this point.
*/
  }while(0);
  if(entropy_spawn_status){
    entropy_make_flush(spawn_base);
  }
  karacell_free(payload_base);
  karacell_free_all(karacell_base);
  status|=file_close(&handle_out);
  status|=file_close(&handle_in);
  #ifdef DEBUG
    if(karacell_allocation_count){
      print_error("karacell_malloc() and karacell_free() are not in balance:");
      DEBUG_U64("mallocs_minus_frees",karacell_allocation_count);
      status=1;
    }
    timestamp_get(&timestamp_end);
    timestamp_delta=timestamp_end-timestamp_start;
    DEBUG_U64("timer_ticks_elapsed",timestamp_delta);
  #endif
  fflush(stdout);
  if((!status)&&authentication_fail_status){
    status=2;
  }
/*
status is one of:

  0: Success.

  1: Failure.

  2: Decryption succeeded, but with no hash, an unacceptably weak hash, or a hash known to Karacell but not this file.

Depending on the circumstances, you might want to add code which clears this process' memory, including the command line invokation, prior to exit. In theory, it might also be necessary to discretize timing (via a variable delay here) in a very coarse manner, in order to hide potentially revealing cache misses, etc. But most likely this would only be an issue for microtransactions.
*/
  return status;
}
