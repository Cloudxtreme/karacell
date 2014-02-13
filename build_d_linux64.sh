nasm -D_64_ -O0 -felf64 -otemp/jytter.o jytter.asm
nasm -D_64_ -O0 -felf64 -otemp/timestamp.o timestamp.asm
gcc -D_64_ -DDEBUG_OFF -DPTHREAD -DPIPELINE_OFF -O3 -pthread -fno-stack-protector -mtune=native temp/jytter.o temp/timestamp.o -otemp/karacell os.c
gcc -D_64_ -DDEBUG -DPTHREAD_OFF -DPIPELINE_OFF -O3 -fno-stack-protector temp/jytter.o temp/timestamp.o -otemp/demo demo.c
