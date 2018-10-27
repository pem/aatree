#!/bin/sh

xit=0

tst() {
    name=$1
    shift
    file=$1
    shift
    ../aatree-test $* > /tmp/$file
    cs1=`cksum /tmp/$file | cut -d' ' -f1`
    cs2=`cksum $file | cut -d' ' -f1`
    if [ $cs1 != $cs2 ]; then
	xit=1
	echo "$name FAIL" >&2
    else
	echo "$name OK"
    fi
}

tst Empty aaempty.txt
tst 123 aa123.txt 1 2 3 1 2 3 1 2 3
tst "1-7 ordered" aa1to7ordered.txt -v 1 2 3 4 5 6 7 
tst "7-1 ordered" aa7to1ordered.txt -v 7 6 5 4 3 2 1
tst "1-7 balanced" aa1to7balanced.txt -v 4 2 6 1 3 5 7
tst "Values, unique keys" aavaluniquekey.txt -v f:6 g:7 a:1 c:3 b:2 d:4 e:5 i:9 h:8
tst "Values, dup. keys" aavaldupkey.txt -v f:9 a:1 e:6 c:4 c:3 b:2 c:5 d:7 e:8

# Examples from "Balanced Search Trees Made Simple" by Arne Andersson,
# http://user.it.uu.se/~arnea/ps/simp.pdf .
tst AA1 aa1.txt 04 10 02 08 12 01 03 05 09 11 13 07
tst AA2 aa2.txt 04 10 02 08 12 01 03 05 09 11 13 07 06
tst AA3 aa3.txt -d01 04 10 02 08 12 01 03 05 09 11 13 07 06

tst FindNone aafind0.txt -f x a
tst FindOne aafind1.txt -f a a
tst DeleteNone aadel0.txt -d x a
tst DeleteOne aadel1.txt -d a a

tst "Find no unique keys" aafinduniquekey0.txt -f z f:6 g:7 a:1 c:3 b:2 d:4 e:5 i:9 h:8
tst "Find a unique key 1" aafinduniquekey1.txt -f c f:6 g:7 a:1 c:3 b:2 d:4 e:5 i:9 h:8
tst "Find a unique key 2" aafinduniquekey2.txt -f d f:6 g:7 a:1 c:3 b:2 d:4 e:5 i:9 h:8
tst "Find no dup. keys" aafinddupkey0.txt -f z f:9 a:1 e:6 c:4 c:3 b:2 c:5 d:7 e:8
tst "Find a dup. key 1" aafinddupkey1.txt -f c f:9 a:1 e:6 c:4 c:3 b:2 c:5 d:7 e:8
tst "Find a dup. key 2" aafinddupkey2.txt -f e f:9 a:1 e:6 c:4 c:3 b:2 c:5 d:7 e:8

echo
if [ $xit -ne 0 ]; then
    echo "One or more tests failed"
else
    echo "All tests succeeded"
fi

exit $xit
