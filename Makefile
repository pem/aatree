# $Id: $
#
# Per-Erik Martin (pem@pem.nu) 2018-10-15
#
#

CC=gcc -std=c11

CCOPTS=-Wpedantic -Wextra -Wall

CCDEFS=-D_POSIX_C_SOURCE=200809L

CFLAGS=-g -DDEBUG $(CCOPTS) $(CCDEFS)
#CFLAGS=-O $(CCOPTS) $(CCDEFS)
LDFLAGS=
LDLIBS=

PROG=aatree-test

LIB=libaatree.a

SRC=aatree-test.c
LSRC=aatree.c aatreem.c

OBJ=$(SRC:%.c=%.o)
LOBJ=$(LSRC:%.c=%.o)

all:	$(PROG) $(LIB)

$(PROG):	$(OBJ) $(LIB)

$(LIB):	$(LOBJ)
	rm -f $(LIB)
	$(AR) qc $(LIB) $(LOBJ)
	ranlib $(LIB)

clean:
	$(RM) $(OBJ) $(LOBJ) core

cleanall:	clean
	$(RM) $(PROG) $(LIB) make.deps

make.deps:
	gcc -MM $(CFLAGS) $(SRC) > make.deps

include make.deps
