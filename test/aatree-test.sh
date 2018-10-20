#!/bin/sh

tst() {
    name=$1
    shift
    file=$1
    shift
    ../aatree-test $* > /tmp/$file
    cs1=`cksum /tmp/$file | cut -d' ' -f1`
    cs2=`cksum $file | cut -d' ' -f1`
    if [ $cs1 != $cs2 ]; then
	echo "$name FAIL" >&2
    else
	echo "$name OK"
    fi
}

tst Empty aaempty.txt
tst 123 aa123.txt 1 2 3 1 2 3 1 2 3
tst "1-7 ordered" aa1to7ordered.txt 1 2 3 4 5 6 7 
tst "7-1 ordered" aa7to1ordered.txt 7 6 5 4 3 2 1
tst "1-7 balanced" aa1to7balanced.txt 4 2 6 1 3 5 7
tst AA1 aa1.txt 4 10 2 8 12 1 3 5 9 11 13 7
tst AA2 aa2.txt 4 10 2 8 12 1 3 5 9 11 13 7 6
