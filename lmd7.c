/*
Leidich Message Digest
http://leidich-message-digest.blogspot.com
Copyright 2013-2014 Russell Leidich
March 2, 2014

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
lmd_block_lmd7_get(u32 block_base[LMD7_BLOCK_U32_COUNT],ULONG block_idx_min,u32 p_base[LMD7_U32_COUNT<<1],u32 z_base[LMD7_U32_COUNT]){
/*
Compute the LMD7 of a block of size LMD7_BLOCK_SIZE. The algo is described at:

  http://leidich-message-digest.blogspot.com/2013/02/lmd7.html

Unrolling the loops and localizing the array variables might increase performance but would also expand the cache footprint.

In:

  *block_base is self-explanatory.

  block_idx_min offsets block_base.

  *p_base is as follows, with each of the following structures containing (LMD7_U32_COUNT>>1) (u32)s:

    x0 for LMD7_A0.
    c0 for LMD7_A0.
    x1 for LMD7_A1.
    c1 for LMD7_A1.

  *z_base is undefined.

Out:

  *p_base is undefined.

  *z_base contains z, which is the LMD7 of the block.
*/
  ULONG block_idx;
  u32 c;
  u32 carry0;
  u32 carry1;
  u32 d;
  u32 i;
  u32 j;
  u64 product;
  u32 x;
  u32 x_base[LMD7_U32_COUNT>>1];
  u32 x0;
  u64 z;

  for(i=0;i<=(LMD7_BLOCK_U32_COUNT-LMD7_U32_COUNT);i+=LMD7_U32_COUNT){
    block_idx=i+block_idx_min;
    for(j=0;j<=(LMD7_U32_COUNT-1);j++){
      p_base[j]^=block_base[block_idx]^p_base[j+LMD7_U32_COUNT];
      block_idx++;
    }
    x0=p_base[0];
    p_base[0]=p_base[LMD7_U32_COUNT>>1];
    c=p_base[(LMD7_U32_COUNT>>1)+1];
    d=LMD7_D0;
    product=x0*(u64)(d);
    x=(u32)(product);
    for(j=1;j<=((LMD7_U32_COUNT>>1)-1);j++){
      carry0=(c<x);
      carry0+=(u32)(product>>U32_BITS);
      x_base[j]=p_base[j];
      p_base[j]=c-x;
      c=p_base[j+(LMD7_U32_COUNT>>1)+1];
      product=(x_base[j]*(u64)(d))+carry0;
      x=(u32)(product);
    }
    carry0=(x0<x);
    carry0+=(u32)(product>>U32_BITS);
    p_base[LMD7_U32_COUNT>>1]=x0-x;
    for(j=1;j<=((LMD7_U32_COUNT>>1)-3);j+=2){
      carry1=(x_base[j]<carry0);
      p_base[j+(LMD7_U32_COUNT>>1)]=x_base[j]-carry0;
      carry0=(x_base[j+1]<carry1);
      p_base[j+(LMD7_U32_COUNT>>1)+1]=x_base[j+1]-carry1;
    }
    p_base[LMD7_U32_COUNT-1]=x_base[(LMD7_U32_COUNT>>1)-1]-carry0;
    block_idx=(i^(LMD7_BLOCK_U32_COUNT>>1))+block_idx_min;
    for(j=LMD7_U32_COUNT;j<=((LMD7_U32_COUNT<<1)-1);j++){
      p_base[j]^=block_base[block_idx];
      block_idx++;
    }
    x0=p_base[LMD7_U32_COUNT];
    p_base[LMD7_U32_COUNT]=p_base[LMD7_U32_COUNT+(LMD7_U32_COUNT>>1)];
    c=p_base[LMD7_U32_COUNT+(LMD7_U32_COUNT>>1)+1];
    d=LMD7_D1;
    product=x0*(u64)(d);
    x=(u32)(product);
    for(j=1;j<=((LMD7_U32_COUNT>>1)-2);j++){
      carry0=(c<x);
      carry0+=(u32)(product>>U32_BITS);
      x_base[j]=p_base[j+LMD7_U32_COUNT];
      p_base[j+LMD7_U32_COUNT]=c-x;
      c=p_base[j+LMD7_U32_COUNT+(LMD7_U32_COUNT>>1)+1];
      product=(x_base[j]*(u64)(d))+carry0;
      x=(u32)(product);
    }
    carry0=(c<x);
    carry0+=(u32)(product>>U32_BITS);
    x_base[(LMD7_U32_COUNT>>1)-1]=p_base[LMD7_U32_COUNT+(LMD7_U32_COUNT>>1)-1];
    p_base[LMD7_U32_COUNT+(LMD7_U32_COUNT>>1)-1]=c-x;
    product=(x_base[(LMD7_U32_COUNT>>1)-1]*(u64)(d))+carry0;
    x=(u32)(product);
    carry0=(x0<x);
    carry0+=(u32)(product>>U32_BITS);
    p_base[LMD7_U32_COUNT+(LMD7_U32_COUNT>>1)]=x0-x;
    for(j=1;j<=((LMD7_U32_COUNT>>1)-3);j+=2){
      carry1=(x_base[j]<carry0);
      p_base[j+LMD7_U32_COUNT+(LMD7_U32_COUNT>>1)]=x_base[j]-carry0;
      carry0=(x_base[j+1]<carry1);
      p_base[j+LMD7_U32_COUNT+(LMD7_U32_COUNT>>1)+1]=x_base[j+1]-carry1;
    }
    p_base[(LMD7_U32_COUNT<<1)-1]=x_base[(LMD7_U32_COUNT>>1)-1]-carry0;
  }
  z=0;
  for(i=0;i<=(LMD7_U32_COUNT-1);i++){
    z>>=U32_BITS;
    z+=p_base[i];
    z+=p_base[(i+LMD7_U32_COUNT)^(LMD7_U32_COUNT>>1)];
    z_base[i]=(u32)(z);
  }
  return;
}
