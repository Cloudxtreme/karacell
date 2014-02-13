nasm -D_32_ -O0 -fmacho32 -otemp/jytter.o jytter.asm
nasm -D_32_ -O0 -fmacho32 -otemp/timestamp.o timestamp.asm
clang -D_32_ -D_FILE_OFFSET_BITS=64 -DDEBUG_OFF -DPTHREAD -DPIPELINE_OFF -Wall -Wextra -Wconstant-conversion -Wint-conversion -Wconversion -O3 -pthread -fno-stack-protector -m32 temp/jytter.o temp/timestamp.o -o temp/karacell os.c
clang -D_32_ -D_FILE_OFFSET_BITS=64 -DDEBUG -DPTHREAD_OFF -DPIPELINE_OFF -Wall -Wextra -Wconstant-conversion -Wint-conversion -Wconversion -O3 -fno-stack-protector temp/jytter.o temp/timestamp.o -o temp/demo demo.c
