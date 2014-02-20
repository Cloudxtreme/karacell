nasm -D_32_ -O0 -fwin32 -otemp\jytter.obj jytter.asm
nasm -D_32_ -O0 -fwin32 -otemp\timestamp.obj timestamp.asm
gcc -D_32_ -DDEBUG_OFF -DPTHREAD_OFF -DPIPELINE_OFF -O3 -fno-stack-protector -mtune=native temp\jytter.obj temp\timestamp.obj -otemp\karacell.exe main.c
gcc -D_32_ -DDEBUG -DPTHREAD_OFF -DPIPELINE_OFF -O3 -fno-stack-protector temp\jytter.obj temp\timestamp.obj -otemp\demo.exe demo.c
