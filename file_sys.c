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
FILE *
file_open(u8 *name_base,u8 mode){
/*
Open a file.

In:

  *name_base is the path and filename in UNIX form, terminated with NULL.

  mode is one of the FILE_ constants from file_sys.h

Out:

  Returns NULL on failure, else a file handle.
*/
  char *mode_string_base;

  switch(mode){
  case FILE_READ:
    mode_string_base="rb";
    break;
  case FILE_WRITE:
    mode_string_base="wb";
    break;
  case FILE_APPEND:
    mode_string_base="ab";
  }
  return fopen((const char *)(name_base),(const char *)(mode_string_base));
}

u8
file_read(u64 string_size,u8 *string_base,FILE *handle){
/*
Read part of a file.

In:

  string_size is the number of bytes to read.

  *string_base is the target buffer.

  handle is the return value of file_open().

Out:

  Returns 0 if string_size bytes have been read to *string_base, else 1.
*/
  u64 transfer_size;

  #ifdef _32_
    if((string_size>>32)==0){
  #endif
      transfer_size=(u64)(fread(string_base,(size_t)(U8_SIZE),(size_t)(string_size),handle));
  #ifdef _32_
    }else{
      transfer_size=0;
    }
  #endif
  return (string_size!=transfer_size);
}

u8
file_write(u64 string_size,u8 *string_base,FILE *handle){
/*
Write part of a file.

In:

  string_size is the number of bytes to write.

  *string_base is the source buffer.

  handle is the return value of file_open().

Out:

  Returns 0 if string_size bytes have been written from *string_base, else 1.
*/
  u64 transfer_size;

  #ifdef _32_
    if((string_size>>32)==0){
  #endif
    transfer_size=(u64)(fwrite(string_base,(size_t)(U8_SIZE),(size_t)(string_size),handle));
  #ifdef _32_
    }else{
      transfer_size=0;
    }
  #endif
  return (string_size!=transfer_size);
}

u8
file_close(FILE **handle_base){
/*
Close a file.

In:

  handle is the return value of file_open().

Out:

  Returns 1 on failure, else 0.

  *handle_base==NULL because it's no longer valid, regardless of whether fclose() succeeded.
*/
  u8 status;

  status=0;
  if(*handle_base){
    status=(fclose(*handle_base)!=0);
    *handle_base=NULL;
  }
  return status;
}

u8
file_rewind(FILE *handle){
/*
Seek to the base of a file. (We use FSEEKO() instead of rewind() because the latter was never formally defined for 64-bit stream offsets.)

In:

  handle is the return value of file_open().

Out:

  Returns 1 on failure, else 0.
*/
  return !!FSEEKO(handle,(off_t)(0),SEEK_SET);
}

u8
file_size_get_rewind(FILE *handle,u64 *size_base){
/*
Get the size of a file, then seek to offset 0.

In:

  handle is the return value of file_open().

Out:

  Returns 0 if *size_base is updated with the file size and the file is seeked to 0, else 1.
*/
  u64 offset;
  u8 status;

  status=!!FSEEKO(handle,(off_t)(0),SEEK_END);
  if(!status){
    offset=(u64)(FTELLO(handle));
    if(offset!=U64_MAX){
      status=file_rewind(handle);
      if(!status){
        *size_base=offset;
      }
    }
  }
  return status;
}
