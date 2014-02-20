/*
Leidich Message Digest
http://leidich-message-digest.blogspot.com
Copyright 2013 Russell Leidich
October 7, 2013

This file is part of the Leidich Message Digest Library.

The Leidich Message Digest Library is free software: you can redistribute it
and/or modify it under the terms of the GNU Limited General Public License as
published by the Free Software Foundation, version 3.

The Leidich Message Digest Library is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Limited
General Public License version 3 for more details.

You should have received a copy of the GNU Limited General Public License
version 3 along with the Leidich Message Digest Library (filename "COPYING").
If not, see http://www.gnu.org/licenses/ .
*/
void
lmd_block_lmd8_get(u32 block_base[LMD8_BLOCK_U32_COUNT],ULONG block_idx_min,u32 p_base[LMD8_U32_COUNT<<1],u32 z_base[LMD8_U32_COUNT]){
/*
Compute the LMD8 of a block of size LMD8_BLOCK_SIZE. The algo is described at:

  http://leidich-message-digest.blogspot.com/2013/03/lmd8.html

Unrolling the loops and localizing the array variables might increase performance but would also expand the cache footprint.

In:

  *block_base is self-explanatory.

  block_idx_min offsets block_base.

  *p_base is as follows, with each of the following structures containing (LMD8_U32_COUNT>>1) (u32)s:

    x0 for LMD8_A0.
    c0 for LMD8_A0.
    x1 for LMD8_A1.
    c1 for LMD8_A1.

  *z_base is undefined.

Out:

  *p_base is undefined.

  *z_base contains z, which is the LMD8 of the block.
*/
  ULONG block_idx;
  u32 c;
  u32 carry0;
  u32 carry1;
  u32 d;
  u32 i;
  u32 j;
  u32 p;
  u64 product;
  u32 x;
  u32 x_base[LMD8_U32_COUNT>>1];
  u64 z;

  for(i=0;i<=(LMD8_BLOCK_U32_COUNT-LMD8_U32_COUNT);i+=LMD8_U32_COUNT){
    block_idx=i+block_idx_min;
    for(j=0;j<=(LMD8_U32_COUNT-1);j++){
      p_base[j]^=block_base[block_idx]^p_base[j+LMD8_U32_COUNT];
      block_idx++;
    }
    carry0=0;
    d=LMD8_D0;
    for(j=0;j<=((LMD8_U32_COUNT>>1)-1);j++){
      p=p_base[j];
      product=(p*(u64)(d))+carry0;
      x_base[j]=p;
      c=p_base[j+(LMD8_U32_COUNT>>1)];
      x=(u32)(product);
      carry0=(c<x);
      carry0+=product>>U32_BITS;
      p_base[j]=c-x;
    }
    x=x_base[0];
    carry1=(x<carry0);
    p_base[LMD8_U32_COUNT>>1]=x-carry0;
    x=x_base[1];
    carry0=(x<carry1);
    p_base[(LMD8_U32_COUNT>>1)+1]=x-carry1;
    x=x_base[2];
    carry1=(x<carry0);
    p_base[(LMD8_U32_COUNT>>1)+2]=x-carry0;
    x=x_base[3];
    carry0=(x<carry1);
    p_base[(LMD8_U32_COUNT>>1)+3]=x-carry1;
    x=x_base[4];
    carry1=(x<carry0);
    p_base[(LMD8_U32_COUNT>>1)+4]=x-carry0;
    x=x_base[5];
    carry0=(x<carry1);
    p_base[(LMD8_U32_COUNT>>1)+5]=x-carry1;
    x=x_base[6];
    carry1=(x<carry0);
    p_base[(LMD8_U32_COUNT>>1)+6]=x-carry0;
    x=x_base[7];
    p_base[(LMD8_U32_COUNT>>1)+7]=x-carry1;
    block_idx=(i^(LMD8_BLOCK_U32_COUNT>>1))+block_idx_min;
    for(j=LMD8_U32_COUNT;j<=((LMD8_U32_COUNT<<1)-1);j++){
      p_base[j]^=block_base[block_idx];
      block_idx++;
    }
    carry0=0;
    d=LMD8_D1;
    for(j=LMD8_U32_COUNT;j<=(LMD8_U32_COUNT+(LMD8_U32_COUNT>>1)-1);j++){
      p=p_base[j];
      product=(p*(u64)(d))+carry0;
      x_base[j-LMD8_U32_COUNT]=p;
      c=p_base[j+(LMD8_U32_COUNT>>1)];
      x=(u32)(product);
      carry0=(c<x);
      carry0+=product>>U32_BITS;
      p_base[j]=c-x;
    }
    x=x_base[0];
    carry1=(x<carry0);
    p_base[LMD8_U32_COUNT+(LMD8_U32_COUNT>>1)]=x-carry0;
    x=x_base[1];
    carry0=(x<carry1);
    p_base[LMD8_U32_COUNT+(LMD8_U32_COUNT>>1)+1]=x-carry1;
    x=x_base[2];
    carry1=(x<carry0);
    p_base[LMD8_U32_COUNT+(LMD8_U32_COUNT>>1)+2]=x-carry0;
    x=x_base[3];
    carry0=(x<carry1);
    p_base[LMD8_U32_COUNT+(LMD8_U32_COUNT>>1)+3]=x-carry1;
    x=x_base[4];
    carry1=(x<carry0);
    p_base[LMD8_U32_COUNT+(LMD8_U32_COUNT>>1)+4]=x-carry0;
    x=x_base[5];
    carry0=(x<carry1);
    p_base[LMD8_U32_COUNT+(LMD8_U32_COUNT>>1)+5]=x-carry1;
    x=x_base[6];
    carry1=(x<carry0);
    p_base[LMD8_U32_COUNT+(LMD8_U32_COUNT>>1)+6]=x-carry0;
    x=x_base[7];
    p_base[LMD8_U32_COUNT+(LMD8_U32_COUNT>>1)+7]=x-carry1;
  }
  z=0;
  for(i=0;i<=(LMD8_U32_COUNT-1);i++){
    z>>=U32_BITS;
    z+=p_base[i];
    z+=p_base[(i+LMD8_U32_COUNT)^(LMD8_U32_COUNT>>1)];
    z_base[i]=(u32)(z);
  }
  return;
}
