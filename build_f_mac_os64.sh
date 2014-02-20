nasm -D_64_ -O0 -fmacho64 -otemp/jytter.o jytter.asm
nasm -D_64_ -O0 -fmacho64 -otemp/timestamp.o timestamp.asm
clang -D_64_ -DDEBUG_OFF -DPTHREAD -DPIPELINE_OFF -Wall -Wextra -Wconstant-conversion -Wint-conversion -Wconversion -O3 -pthread -fno-stack-protector -m64 temp/jytter.o temp/timestamp.o -o temp/karacell main.c
clang -D_64_ -DDEBUG -DPTHREAD_OFF -DPIPELINE_OFF -Wall -Wextra -Wconstant-conversion -Wint-conversion -Wconversion -O3 -fno-stack-protector temp/jytter.o temp/timestamp.o -o temp/demo demo.c
