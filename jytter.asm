;
; Jytter
; Version 1
; http://jytter.blogspot.com
; Copyright 2012 Russell Leidich
; August 24, 2012
; 
; This file is part of the Jytter Library.
; 
; The Jytter Library is free software: you can redistribute it
; and/or modify it under the terms of the GNU Limited General Public License as
; published by the Free Software Foundation, version 3.
; 
; The Jytter Library is distributed in the hope that it will be
; useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Limited
; General Public License version 3 for more details.
;
; You should have received a copy of the GNU Limited General Public License
; version 3 along with the Leidich Message Digest Library (filename "COPYING").
; If not, see http://www.gnu.org/licenses/ .
;
;--------------------------------------------------------------------------
;
; Jytter True Random Number Generator
;
; Updated August 24, 2012 by Russell Leidich.
; Originally implemented on June 25, 2011.
;
; For use with:
;
;   32-bit X86 GCC and Microsoft compilers.
;
;   64-bit X64 GCC.
;
; Compile with:
;
;   32-bit: nasm -D_32_ -ojytter.o(bj) jytter.asm
;
;   64-bit: nasm -D_64_ -ojytter.o(bj) jytter.asm
;
; due to dependency on X86 and X64 calling conventions.
;
; See http://jytter.blogspot.com if you want to know why this algo produces
; high quality noise, as opposed to noise that appears to be of high quality,
; but in fact suffers from badly biased distribution over its 32-bit range of
; possibilities. This algo is very difficult to understand without first
; reading the blog.
;
%IFDEF _32_
%IFDEF _64_
%FATAL #error "You have defined both _32_ and _64_. Chose one only."
%ELSE
USE32
%ENDIF
%ELIF _64_
USE64
%ELSE
%FATAL "Use \"nasm -D_64_\" for 64-bit or \"nasm -D_32_\" for 32-bit code."
%ENDIF

SECTION .text
;
; UNIQUE_EVENT_DURATION_COUNT must be on [1,32]. It's the minimum number of
; unique event durations we must observe before concluding that we have noise
; of sufficient quality to return. Higher values imply more entropy and more
; execution time. 16 is about right, or maybe a bit fewer.
;
%DEFINE UNIQUE_EVENT_DURATION_COUNT 16

%MACRO PROC 1
  GLOBAL %1
  %1:
%ENDMACRO

%MACRO ENDPROC 1
%ENDMACRO

%MACRO INT64 1
  %1: RESQ 1
%ENDMACRO

STRUC jytter_scratch_space
  INT64 caller_rcx
  INT64 caller_rdx
  INT64 caller_rbx
  INT64 caller_rbp
  INT64 caller_rsi
ENDSTRUC

PROC jytter_true_random_get
PROC _jytter_true_random_get
;
; Do not execute this function in parallel with itself, as parallel cores
; might theoretically generate the similar random numbers simultaneously,
; due to sharing a common operating environment.
;
; Return a true random bitstring, based on the algorithm described at the web
; address above.
;
; This routine is fairly well optimized, so as to allow it to run in fewer
; clock cycles, and thus more predictable time (due to smaller variances),
; because we want to sample entropy from less predictable interrupting events
; which affect execution time, and not natural and predictable variances in
; execution time due to code path variances from iteration to iteration. In
; other words, we keep the code tight and predictable so that we don't mistake
; the pseudorandomness of expected variances in our own iteration time, for
; the true randomness of unpredictable interrupting events. For this reason in
; particular, there are neither branches (except for the very predictable
; loopback branch) nor memory accesses in the main loop.
;
; The C prototype is defined such that ECX/RDI will end up pointing to
; jytter_scratch_space. See
; http://en.wikipedia.org/wiki/X86_calling_conventions for more information.
;
; Prototype it like this under GCC. (Replace "__attribute__((fastcall))" with
; the equivalent under different compilers. This attribute causes parameter
; passing in ECX in 32-bit mode.) And note that although this procedure name
; starts with an underscore, it is omitted from the C prototype below:
;
; typedef struct {
;   uint64_t caller_rcx;
;   uint64_t caller_rdx;
;   uint64_t caller_rbx;
;   uint64_t caller_rbp;
;   uint64_t caller_rsi;
; } jytter_scratch_space_t;
;
; Followed by this in a 32-bit environment:
;
;   extern uint32_t jytter_true_random_get(jytter_scratch_space_t *jytter_scratch_space) __attribute__((fastcall));
;
; Or this in a 64-bit environment:
;
;   extern uint32_t jytter_true_random_get(jytter_scratch_space_t *jytter_scratch_space);
;
; Call it like this:
;
;   uint32_t true_random;
;   jytter_scratch_space_t jytter_scratch_space;
;
;   true_random=jytter_true_random_get(&jytter_scratch_space);
;
;-----------------------------------------------------------
;
; IN
;
;   ECX/RDI = Base of jytter_scratch_space (see STRUC scratch_space, above).
;    There is no need to intialize jytter_scratch_space itself.
;
;   DS = Segment of jytter_scratch_space (32-bit environment only).
;
; OUT
;
;   EAX = 32 bits of true random white noise.
;
;   jytter_scratch_space undefined.
;
;-----------------------------------------------------------
;
; We can't easily use the stack due to the possible presence of the compiler
; red zone below ESP or RSP. It doesn't matter because we have ECX/RDI as the
; base of jytter_scratch_space.
;
%IFDEF _32_
  mov [ecx+caller_rcx],edi
  mov [ecx+caller_rdx],edx
  mov [ecx+caller_rbx],ebx
  mov [ecx+caller_rbp],ebp
  mov [ecx+caller_rsi],esi
%ELSE
  mov [rdi+caller_rcx],rcx
  mov [rdi+caller_rdx],rdx
  mov [rdi+caller_rbx],rbx
  mov [rdi+caller_rbp],rbp
  mov [rdi+caller_rsi],rsi
  mov rcx,rdi
%ENDIF
;
; Clear the bitmap in ESI, as discussed below.
;
  xor esi,esi
;
; Set BL to the number of unique event durations that we must observe before
; issuing output, as discussed below.
;
  mov bl,UNIQUE_EVENT_DURATION_COUNT
;
; We need to JMP to the loop because it needs to have predictable alignment.
; Otherwise, we could create timing variances just due to the pseudorandomness
; of compiled alignment. (It's not like we can avoid all sources of
; pseudorandomness, but less is better.) Align to the maximum known cache line
; size, in case it matters.
;
  jmp SHORT read_timestamp_counter

ALIGN 64
read_timestamp_counter:
;
; Read the timestamp counter (TSC) into EDX:EAX.
;
  rdtsc
;
; Only EAX is rich in entropy, so we can ignore EDX. Add it to the accumulating
; true random number in EDI.
;
  add edi,eax
;
; Set EBP to the current timestamp, and EAX to the difference since the last
; one. EBP is initially undefined. We could fix that, at the cost of more
; pseudorandom timing behavior of this loop (bad idea), or just forget about
; it because it doesn't affect output randomness much. If you're really
; worried, then increment UNIQUE_EVENT_DURATION_COUNT.
;
  neg ebp
  xadd eax,ebp
;
; We don't want to touch memory because that would create pseudorandom
; execution timing. So we're stuck with registers for scratch space. We must
; spin until we have seen at least UNIQUE_EVENT_DURATION_COUNT unique events
; (memory stalls, interrupts, pipeline stalls, etc.) We measure this by
; looking for unique TSC delta values in EAX. Since there are 2^32 possible
; such values, we are forced to take only a 5-bit hash, corresponding to each
; bit in ESI. Just XOR bits [9:5] and [4:0] of EAX together. Nevermind higher
; bits, which won't affect the BTS instruction below.
;
  mov edx,eax
  shr edx,5
  xor eax,edx
;
; Set the bit in the bitmap of unique events. The same EAX (TSC delta) will
; always set the same bit in ESI
;
  bts esi,eax
;
; While the BTS is in flight, multiply EDI by 9. This makes it so that the
; random output will depend on the exact order of timestamp observations, and
; not merely the (commutative) sum of those observations, as the above ADD 
; to EDI would otherwise imply. Multiplication by 9 preserves information
; because it's a 1-to-1 function when done modulo 2^32, so we won't lose any
; entropy. It's also the largest factor that we can use without an IMUL
; instruction, which is much slower and may have pseudorandom timing. We want
; a large factor because it will spread the entropy faster throughout EDI.
;
%IFDEF _32_
  lea edi,[edi+8*edi]
%ELSE
  lea edi,[rdi+8*rdi]
%ENDIF
;
; If this is the first time we set a bit in ESI, then we've just observed a
; unique event, in which case decrement BL.
;
  adc bl,-1
;
; Spin until BL==0.
;
  jnz read_timestamp_counter
;
; Put the output into EAX with a 1-byte instruction.
;
  xchg eax, edi
;
; Restore the registers.
;
%IFDEF _32_
  mov esi,[ecx+caller_rsi]
  mov ebp,[ecx+caller_rbp]
  mov ebx,[ecx+caller_rbx]
  mov edx,[ecx+caller_rdx]
  mov edi,[ecx+caller_rcx]
%ELSE
  mov rdi,rcx
  mov rsi,[rcx+caller_rsi]
  mov rbp,[rcx+caller_rbp]
  mov rbx,[rcx+caller_rbx]
  mov rdx,[rcx+caller_rdx]
  mov rcx,[rcx+caller_rcx]
%ENDIF
  retn
ENDPROC _jytter_true_random_get
ENDPROC jytter_true_random_get

