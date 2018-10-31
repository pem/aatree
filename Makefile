#
# Per-Erik Martin (pem@pem.nu) 2018-10-15
#
#

CC=gcc -std=c11

CCOPTS=-Wpedantic -Wextra -Wall

CCDEFS=-D_POSIX_C_SOURCE=200809L

#CFLAGS=-g -DDEBUG $(CCOPTS) $(CCDEFS)
CFLAGS=-O2 -fomit-frame-pointer $(CCOPTS) $(CCDEFS)
LDFLAGS=
LDLIBS=

PROG=aatree-test

LIB=libaatree.a
MLIB=libaatreem.a

SRC=aatree-test.c
LSRC=aatree.c
MLSRC=aatree.c aatreem.c

OBJ=$(SRC:%.c=%.o)
LOBJ=$(LSRC:%.c=%.o)
MLOBJ=$(MLSRC:%.c=%.o)

all:	$(PROG) $(LIB) $(MLIB)

$(PROG):	$(OBJ) $(MLIB)

$(LIB):	$(LOBJ)
	rm -f $(LIB)
	$(AR) qc $(LIB) $(LOBJ)
	ranlib $(LIB)

$(MLIB):	$(MLOBJ)
	rm -f $(MLIB)
	$(AR) qc $(MLIB) $(MLOBJ)
	ranlib $(MLIB)

clean:
	$(RM) $(OBJ) $(LOBJ) $(MLOBJ) core

cleanall:	clean
	$(RM) $(PROG) $(LIB) $(MLIB) make.deps

make.deps:
	gcc -MM $(CFLAGS) $(SRC) $(MSRC) > make.deps

include make.deps
