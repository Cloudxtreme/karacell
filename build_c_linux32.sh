nasm -D_32_ -O0 -felf32 -otemp/jytter.o jytter.asm
nasm -D_32_ -O0 -felf32 -otemp/timestamp.o timestamp.asm
gcc -D_32_ -D_FILE_OFFSET_BITS=64 -DDEBUG_OFF -DPTHREAD -DPIPELINE_OFF -O3 -pthread -fno-stack-protector -mtune=native temp/jytter.o temp/timestamp.o -otemp/karacell os.c
gcc -D_32_ -D_FILE_OFFSET_BITS=64 -DDEBUG -DPTHREAD_OFF -DPIPELINE_OFF -O3 -fno-stack-protector temp/jytter.o temp/timestamp.o -otemp/demo demo.c
