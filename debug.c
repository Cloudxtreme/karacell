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
#ifdef DEBUG
#ifndef PRINT_PRESENT
  #include "print_xtrn.h"
#endif

void
debug_print_flush(void){
  fflush(stdout);
  return;
}

void
debug_print(char *string_base){
  printf("%s",string_base);
  debug_print_flush();
  return;
}

void
debug_print_if(u8 status,char *string_base){
  if(status){
    debug_print(string_base);
  }
  return;
}

void
debug_list_print_custom(char *context_string_base,u32 chunk_count,u8 *list_base,u8 chunk_size_log2,u8 backwards,u8 compilable){
  u8 char_array_base[2+(8*11)+4];
  u8 char_array_idx;
  u8 char_array_idx_delta;
  u8 column_count;
  u32 i;
  u8 j;
  u32 k;
  u32 k_delta;
  u32 row_count;

  if(chunk_size_log2<=U64_SIZE_LOG2){
    if(compilable==0){
      printf("%s[%08X]={",context_string_base,chunk_count);
    }else{
      printf("%s \\",context_string_base);
    }
    print_new_line();
    if(list_base){
      row_count=4;
      column_count=0x10;
      if(chunk_size_log2>1){
        row_count++;
        column_count=(u8)(column_count<<1);
      }
      row_count-=chunk_size_log2;
      row_count=(chunk_count>>row_count)+(!!(chunk_count&((u32)(1<<row_count)-1)));
      column_count=(u8)(column_count>>chunk_size_log2);
      k_delta=(u32)(1<<chunk_size_log2);
      char_array_idx_delta=(u8)((k_delta<<1)+1);
      if(backwards==0){
        k=0;
      }else{
        k_delta=-k_delta;
        k=(u32)((chunk_count<<chunk_size_log2)+k_delta);
      }
      for(i=0;i<row_count;i++){
        if(i==(row_count-1)){
          column_count=(u8)(chunk_count);
        }
        char_array_idx=0;
        for(j=0;j<column_count;j++){
          if(compilable){
            char_array_base[char_array_idx]='0';
            char_array_base[char_array_idx+1]='x';
            char_array_idx=(u8)(char_array_idx+2);
          }
          switch(chunk_size_log2){
          case 0:
            sprintf((char *)(char_array_base+char_array_idx),"%02X,",list_base[k]);
            break;
          case 1:
            sprintf((char *)(char_array_base+char_array_idx),"%04X,",*(u16 *)(list_base+k));
            break;
          case 2:
            sprintf((char *)(char_array_base+char_array_idx),"%08X,",*(u32 *)(list_base+k));
            break;
          case 3:
            sprintf((char *)(char_array_base+char_array_idx),"%08X%08X,",*(u32 *)(list_base+k+U32_SIZE),*(u32 *)(list_base+k));
            break;
          }
          char_array_idx=(u8)(char_array_idx+char_array_idx_delta);
          k=k+k_delta;
        }
        chunk_count-=column_count;
        if(i==(row_count-1)){
          char_array_idx--;
        }
        if(compilable&&(i!=(row_count-1))){
          char_array_base[char_array_idx]=' ';
          char_array_base[char_array_idx+1]='\\';
          char_array_idx=(u8)(char_array_idx+2);
        }
        char_array_base[char_array_idx]='\n';
        char_array_base[char_array_idx+1]=0;
        printf("  %s",(char *)(char_array_base));
      }
      if(compilable==0){
        printf("}");
      }
      print_new_line();
    }else{
      printf("\ndebug_list_print_custom(): List has NULL base.\n");
      exit(1);
    }
  }else{
    printf("\ndebug_list_print_custom(): chunk_size_log2>%d.\n",U64_SIZE_LOG2);
    exit(1);
  }
  debug_print_flush();
  return;
}

void
debug_list_print(char *context_string_base,u32 chunk_count,u8 *list_base,u8 chunk_size_log2){
  debug_list_print_custom(context_string_base,chunk_count,list_base,chunk_size_log2,0,0);
  return;
}

void
debug_u8(char *name_base,u8 value){
  printf("%s=%02X\n",name_base,value);
  debug_print_flush();
  return;
}

void
debug_u16(char *name_base,u16 value){
  printf("%s=%04X\n",name_base,value);
  debug_print_flush();
  return;
}

void
debug_u32(char *name_base,u32 value){
  printf("%s=%08X\n",name_base,value);
  debug_print_flush();
  return;
}

void
debug_u64(char *name_base,u64 value){
  printf("%s=%08X%08X\n",name_base,(u32)(value>>32),(u32)(value));
  debug_print_flush();
  return;
}

void
debug_ptr(char *name_base,void *base){
#ifdef _64_
  debug_u64(name_base,(u64)(base));
#else
  debug_u32(name_base,(u32)(base));
#endif
  return;
}
#endif
