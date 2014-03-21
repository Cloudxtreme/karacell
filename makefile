TARGET=$(shell cc -dumpmachine)
BASICFLAGS=-D_FILE_OFFSET_BITS=64 -O3 -fno-stack-protector
BITS=32
EXE=
JYTTEROBJ=
MEMFLAGS=-m32
NASM=
OBJ=.o
RM=rm
SLASH=/
TRUERANDOM=
WARNFLAGS=-Wall -Wextra -Wconversion
WILDCARD=*
DELTEMP=@echo clean does not work, so you should delete everything in the temp subfolder manually.
ifneq (,$(findstring 64,$(TARGET)))
	BITS=64
	MEMFLAGS=-m64
endif
ifneq (,$(findstring mingw,$(TARGET)))
	BITS=32
	EXE=.exe
	MEMFLAGS=-mtune=native
	OBJ=.obj
	SLASH=\\
	TRUERANDOM=JYTTER
	JYTTEROBJ=temp$(SLASH)jytter$(OBJ)
	NASM=nasm -D_$(BITS)_ -O0 -fwin$(BITS) -o$(JYTTEROBJ) jytter.asm
endif
ifneq (,$(findstring linux,$(TARGET)))
	MEMFLAGS=-mtune=native
	TRUERANDOM=JYTTER
	JYTTEROBJ=temp$(SLASH)jytter$(OBJ)
	NASM=nasm -D_$(BITS)_ -O0 -felf$(BITS) -o$(JYTTEROBJ) jytter.asm
	DELTEMP=$(RM) temp$(SLASH)$(WILDCARD)
endif
ifneq (,$(findstring darwin,$(TARGET)))
	TRUERANDOM=JYTTER
	JYTTEROBJ=temp$(SLASH)jytter$(OBJ)
	WARNFLAGS+= -Wconstant-conversion -Wint-conversion
	NASM=nasm -D_$(BITS)_ -O0 -fmacho$(BITS) -o$(JYTTEROBJ) jytter.asm
	DELTEMP=$(RM) temp$(SLASH)$(WILDCARD)
else
	ifneq (,$(findstring arm,$(TARGET)))
		TRUERANDOM=SECRANDOM
		WARNFLAGS+= -Wconstant-conversion -Wint-conversion
		DELTEMP=$(RM) temp$(SLASH)$(WILDCARD)
	endif
endif

clean:
	$(DELTEMP)	

demo:
	$(NASM)
	cc -c -fpic -D_$(BITS)_ -DDEBUG -DPTHREAD_OFF $(WARNFLAGS) $(BASICFLAGS) $(MEMFLAGS) -otemp$(SLASH)debug$(OBJ) debug_obj.c
	cc -c -fpic -D_$(BITS)_ -DDEBUG -DPTHREAD_OFF -D$(TRUERANDOM) $(WARNFLAGS) $(BASICFLAGS) $(MEMFLAGS) -otemp$(SLASH)entropy$(OBJ) entropy_obj.c
	cc -c -fpic -D_$(BITS)_ -DDEBUG -DPTHREAD_OFF $(WARNFLAGS) $(BASICFLAGS) $(MEMFLAGS) -otemp$(SLASH)karacell$(OBJ) karacell_obj.c
	cc -c -fpic -D_$(BITS)_ -DDEBUG -DPTHREAD_OFF $(WARNFLAGS) $(BASICFLAGS) $(MEMFLAGS) -otemp$(SLASH)listcrypt$(OBJ) listcrypt_obj.c
	cc -c -fpic -D_$(BITS)_ -DDEBUG -DPTHREAD_OFF $(WARNFLAGS) $(BASICFLAGS) $(MEMFLAGS) -otemp$(SLASH)lmd$(OBJ) lmd_obj.c
	cc -c -fpic -D_$(BITS)_ -DDEBUG -DPTHREAD_OFF $(WARNFLAGS) $(BASICFLAGS) $(MEMFLAGS) -otemp$(SLASH)print$(OBJ) print_obj.c
	cc -c -fpic -D_$(BITS)_ -DDEBUG -DPTHREAD_OFF $(WARNFLAGS) $(BASICFLAGS) $(MEMFLAGS) -otemp$(SLASH)spawn$(OBJ) spawn_obj.c
	cc -D_$(BITS)_ -DDEBUG -DPTHREAD_OFF $(WARNFLAGS) $(BASICFLAGS) $(MEMFLAGS) $(JYTTEROBJ) temp$(SLASH)debug$(OBJ) temp$(SLASH)entropy$(OBJ) temp$(SLASH)karacell$(OBJ) temp$(SLASH)listcrypt$(OBJ) temp$(SLASH)lmd$(OBJ) temp$(SLASH)print$(OBJ) temp$(SLASH)spawn$(OBJ) -otemp$(SLASH)demo$(EXE) demo.c

karacell:
	$(NASM)
	cc -D_$(BITS)_ -DDEBUG_OFF -DPIPELINE_OFF -DPTHREAD_OFF -D$(TRUERANDOM) $(WARNFLAGS) $(BASICFLAGS) $(MEMFLAGS) $(JYTTEROBJ) -otemp$(SLASH)karacell$(EXE) main.c

karacell_debug:
	$(NASM)
	cc -D_$(BITS)_ -DDEBUG -DPIPELINE_OFF -DPTHREAD_OFF -D$(TRUERANDOM) $(WARNFLAGS) $(BASICFLAGS) $(MEMFLAGS) $(JYTTEROBJ) -otemp$(SLASH)karacell$(EXE) main.c

karacell_debug_pipeline:
	$(NASM)
	cc -D_$(BITS)_ -DDEBUG -DPIPELINE -DPTHREAD_OFF -D$(TRUERANDOM) $(WARNFLAGS) $(BASICFLAGS) $(MEMFLAGS) $(JYTTEROBJ) -otemp$(SLASH)karacell$(EXE) main.c

karacell_debug_pipeline_pthread:
	$(NASM)
	cc -D_$(BITS)_ -DDEBUG -DPIPELINE -DPTHREAD -D$(TRUERANDOM) $(WARNFLAGS) $(BASICFLAGS) -pthread $(MEMFLAGS) $(JYTTEROBJ) -otemp$(SLASH)karacell$(EXE) main.c

test:
	$(NASM)
	cc -D_$(BITS)_ -DDEBUG -DPIPELINE -DPTHREAD -D$(TRUERANDOM) $(WARNFLAGS) $(BASICFLAGS) -pthread $(MEMFLAGS) $(JYTTEROBJ) -otemp$(SLASH)test$(EXE) test.c

karacell_debug_pthread:
	$(NASM)
	cc -D_$(BITS)_ -DDEBUG -DPIPELINE_OFF -DPTHREAD -D$(TRUERANDOM) $(WARNFLAGS) $(BASICFLAGS) -pthread $(MEMFLAGS) $(JYTTEROBJ) -otemp$(SLASH)karacell$(EXE) main.c

karacell_lib:
	$(NASM)
	cc -c -fpic -D_$(BITS)_ -DDEBUG_OFF -DPTHREAD_OFF -D$(TRUERANDOM) $(WARNFLAGS) $(BASICFLAGS) $(MEMFLAGS) -otemp$(SLASH)entropy$(OBJ) entropy_obj.c
	cc -c -fpic -D_$(BITS)_ -DDEBUG_OFF -DPTHREAD_OFF $(WARNFLAGS) $(BASICFLAGS) $(MEMFLAGS) -otemp$(SLASH)karacell$(OBJ) karacell_obj.c
	cc -c -fpic -D_$(BITS)_ -DDEBUG_OFF -DPTHREAD_OFF $(WARNFLAGS) $(BASICFLAGS) $(MEMFLAGS) -otemp$(SLASH)listcrypt$(OBJ) listcrypt_obj.c
	cc -c -fpic -D_$(BITS)_ -DDEBUG_OFF -DPTHREAD_OFF $(WARNFLAGS) $(BASICFLAGS) $(MEMFLAGS) -otemp$(SLASH)lmd$(OBJ) lmd_obj.c
	cc -c -fpic -D_$(BITS)_ -DDEBUG_OFF -DPTHREAD_OFF $(WARNFLAGS) $(BASICFLAGS) $(MEMFLAGS) -otemp$(SLASH)spawn$(OBJ) spawn_obj.c
	cc -shared -otemp$(SLASH)libkaracell.so $(JYTTEROBJ) temp$(SLASH)entropy$(OBJ) temp$(SLASH)karacell$(OBJ) temp$(SLASH)listcrypt$(OBJ) temp$(SLASH)lmd$(OBJ) temp$(SLASH)spawn$(OBJ)

karacell_lib_pthread:
	$(NASM)
	cc -c -fpic -D_$(BITS)_ -DDEBUG_OFF -DPTHREAD -D$(TRUERANDOM) $(WARNFLAGS) $(BASICFLAGS) -pthread $(MEMFLAGS) -otemp$(SLASH)entropy$(OBJ) entropy_obj.c
	cc -c -fpic -D_$(BITS)_ -DDEBUG_OFF -DPTHREAD $(WARNFLAGS) $(BASICFLAGS) -pthread $(MEMFLAGS) -otemp$(SLASH)karacell$(OBJ) karacell_obj.c
	cc -c -fpic -D_$(BITS)_ -DDEBUG_OFF -DPTHREAD $(WARNFLAGS) $(BASICFLAGS) -pthread $(MEMFLAGS) -otemp$(SLASH)listcrypt$(OBJ) listcrypt_obj.c
	cc -c -fpic -D_$(BITS)_ -DDEBUG_OFF -DPTHREAD $(WARNFLAGS) $(BASICFLAGS) -pthread $(MEMFLAGS) -otemp$(SLASH)lmd$(OBJ) lmd_obj.c
	cc -c -fpic -D_$(BITS)_ -DDEBUG_OFF -DPTHREAD $(WARNFLAGS) $(BASICFLAGS) -pthread $(MEMFLAGS) -otemp$(SLASH)spawn$(OBJ) spawn_obj.c
	cc -shared -otemp$(SLASH)libkaracell.so $(JYTTEROBJ) temp$(SLASH)entropy$(OBJ) temp$(SLASH)karacell$(OBJ) temp$(SLASH)listcrypt$(OBJ) temp$(SLASH)lmd$(OBJ) temp$(SLASH)spawn$(OBJ)

karacell_pipeline:
	$(NASM)
	cc -D_$(BITS)_ -DDEBUG_OFF -DPIPELINE -DPTHREAD_OFF -D$(TRUERANDOM) $(WARNFLAGS) $(BASICFLAGS) $(MEMFLAGS) $(JYTTEROBJ) -otemp$(SLASH)karacell$(EXE) main.c

karacell_pipeline_pthread:
	$(NASM)
	cc -D_$(BITS)_ -DDEBUG_OFF -DPIPELINE -DPTHREAD -D$(TRUERANDOM) $(WARNFLAGS) $(BASICFLAGS) -pthread $(MEMFLAGS) $(JYTTEROBJ) -otemp$(SLASH)karacell$(EXE) main.c

karacell_pthread:
	$(NASM)
	cc -D_$(BITS)_ -DDEBUG_OFF -DPIPELINE_OFF -DPTHREAD -D$(TRUERANDOM) $(WARNFLAGS) $(BASICFLAGS) -pthread $(MEMFLAGS) $(JYTTEROBJ) -otemp$(SLASH)karacell$(EXE) main.c
