TARGET=$(shell cc -dumpmachine)
BASICFLAGS=-O3 -fno-stack-protector
BITS=32
EXE=
JYTTEROBJ=
MEMFLAGS=-m32
NASM=
OBJ=o
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
	OBJ=obj
	SLASH=\\
	TRUERANDOM=JYTTER
	JYTTEROBJ=temp$(SLASH)jytter.$(OBJ)
	NASM=nasm -D_$(BITS)_ -O0 -fwin$(BITS) -o$(JYTTEROBJ) jytter.asm
endif
ifneq (,$(findstring linux,$(TARGET)))
	MEMFLAGS=-mtune=native
	TRUERANDOM=JYTTER
	JYTTEROBJ=temp$(SLASH)jytter.$(OBJ)
	NASM=nasm -D_$(BITS)_ -O0 -felf$(BITS) -o$(JYTTEROBJ) jytter.asm
	DELTEMP=$(RM) temp$(SLASH)$(WILDCARD)
endif
ifneq (,$(findstring darwin,$(TARGET)))
	TRUERANDOM=JYTTER
	JYTTEROBJ=temp$(SLASH)jytter.$(OBJ)
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
	cc -D_$(BITS)_ -DDEBUG -DPTHREAD_OFF -DPIPELINE_OFF -D$(TRUERANDOM) $(WARNFLAGS) $(BASICFLAGS) $(MEMFLAGS) $(JYTTEROBJ) -otemp$(SLASH)demo$(EXE) demo.c

karacell:
	$(NASM)
	cc -D_$(BITS)_ -DDEBUG_OFF -DPTHREAD_OFF -DPIPELINE_OFF -D$(TRUERANDOM) $(WARNFLAGS) $(BASICFLAGS) $(MEMFLAGS) $(JYTTEROBJ) -otemp$(SLASH)karacell$(EXE) main.c

karacell_debug:
	$(NASM)
	cc -D_$(BITS)_ -DDEBUG -DPTHREAD_OFF -DPIPELINE_OFF -D$(TRUERANDOM) $(WARNFLAGS) $(BASICFLAGS) $(MEMFLAGS) $(JYTTEROBJ) -otemp$(SLASH)karacell$(EXE) main.c

karacell_debug_pipeline:
	$(NASM)
	cc -D_$(BITS)_ -DDEBUG -DPTHREAD_OFF -DPIPELINE -D$(TRUERANDOM) $(WARNFLAGS) $(BASICFLAGS) $(MEMFLAGS) $(JYTTEROBJ) -otemp$(SLASH)karacell$(EXE) main.c

karacell_debug_pthread:
	$(NASM)
	cc -D_$(BITS)_ -DDEBUG -DPTHREAD -DPIPELINE_OFF -D$(TRUERANDOM) $(WARNFLAGS) $(BASICFLAGS) -pthread $(MEMFLAGS) $(JYTTEROBJ) -otemp$(SLASH)karacell$(EXE) main.c

karacell_debug_pipeline_pthread:
	$(NASM)
	cc -D_$(BITS)_ -DDEBUG -DPTHREAD -DPIPELINE -D$(TRUERANDOM) $(WARNFLAGS) $(BASICFLAGS) -pthread $(MEMFLAGS) $(JYTTEROBJ) -otemp$(SLASH)karacell$(EXE) main.c

karacell_pipeline:
	$(NASM)
	cc -D_$(BITS)_ -DDEBUG_OFF -DPTHREAD_OFF -DPIPELINE -D$(TRUERANDOM) $(WARNFLAGS) $(BASICFLAGS) $(MEMFLAGS) $(JYTTEROBJ) -otemp$(SLASH)karacell$(EXE) main.c

karacell_pipeline_pthread:
	$(NASM)
	cc -D_$(BITS)_ -DDEBUG_OFF -DPTHREAD -DPIPELINE -D$(TRUERANDOM) $(WARNFLAGS) $(BASICFLAGS) -pthread $(MEMFLAGS) $(JYTTEROBJ) -otemp$(SLASH)karacell$(EXE) main.c

karacell_pthread:
	$(NASM)
	cc -D_$(BITS)_ -DDEBUG_OFF -DPTHREAD -DPIPELINE_OFF -D$(TRUERANDOM) $(WARNFLAGS) $(BASICFLAGS) -pthread $(MEMFLAGS) $(JYTTEROBJ) -otemp$(SLASH)karacell$(EXE) main.c
