# $Id: $
#
# Per-Erik Martin (pem@pem.nu) 2018-10-15
#
#

CC=gcc -std=c11

CCOPTS=-Wpedantic -Wextra -Wall

CCDEFS=

CFLAGS=-g -DDEBUG $(CCOPTS) $(CCDEFS)
#CFLAGS=-O $(CCOPTS) $(CCDEFS)
LDFLAGS=
LDLIBS=

PROG=aatree-test

LIB=

SRC=aatree-test.c aatree.c

OBJ=$(SRC:%.c=%.o)

all:	$(PROG)

$(PROG):	$(OBJ)

#$(LIB):	$(COBJ) $(OBJ)
#	rm -f $(LIB)
#	$(AR) qc $(LIB) $(COBJ) $(OBJ)
#	ranlib $(LIB)

clean:
	$(RM) $(OBJ) core

cleanall:	clean
	$(RM) $(PROG) $(LIB) make.deps

make.deps:
	gcc -MM $(CFLAGS) $(SRC) > make.deps

include make.deps
