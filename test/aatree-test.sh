#!/bin/sh

xit=0

tst() {
    name=$1
    shift
    file=$1
    shift
#    valgrind --leak-check=full --show-leak-kinds=all ../aatree-test $* > /tmp/$file
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
tst 'aaaaaa' aaaaaa.txt -v a:1 a:2 a:3 a:4 a:5 a:6
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
tst aaaaDeleteOne aaadel.txt -d a a:1 a:2 a:3 a:4 a:5 a:6

tst "Find no unique keys" aafinduniquekey0.txt -f z f:6 g:7 a:1 c:3 b:2 d:4 e:5 i:9 h:8
tst "Find a unique key 1" aafinduniquekey1.txt -f c f:6 g:7 a:1 c:3 b:2 d:4 e:5 i:9 h:8
tst "Find a unique key 2" aafinduniquekey2.txt -f d f:6 g:7 a:1 c:3 b:2 d:4 e:5 i:9 h:8
tst "Find no dup. keys" aafinddupkey0.txt -f z f:9 a:1 e:6 c:4 c:3 b:2 c:5 d:7 e:8
tst "Find a dup. key 1" aafinddupkey1.txt -f c f:9 a:1 e:6 c:4 c:3 b:2 c:5 d:7 e:8
tst "Find a dup. key 2" aafinddupkey2.txt -f e f:9 a:1 e:6 c:4 c:3 b:2 c:5 d:7 e:8
# Built a peculiar tree where only the first x node as found
tst "Find a dup. key bug" aafinddupkeybug.txt -f x x:1 x:2 zip:zap foo:bar

tst "Insert unique in empty" aainsunique0.txt -u 1
tst "Insert unique in one ok" aainsunique1yes.txt -u 1 2
tst "Insert unique in one fail" aainsunique1no.txt -u 1 1
tst "Insert unique in 7 ok" aainsunique7yes.txt -u 1 2 3 4 5 6 7 8
tst "Insert unique in 7 fail" aainsunique7no.txt -u 1 2 3 4 5 6 7 6

tst "No Replace in empty" aareplace0.txt -r a:1
tst "No Replace in one" aareplace1no.txt -r a:1 b:2
tst "Replace in one" aareplace1yes.txt -r a:1 a:2
tst "No Replace in 7" aareplace7no.txt -r a:1 b:2 c:3 d:4 e:5 f:6 g:7 h:8
tst "Replace in 7" aareplace7yes.txt -r a:1 b:2 c:3 d:4 e:5 f:6 g:7 f:8
tst "Replace in aaa..." aaareplace1.txt -v -r a:1 a:2 a:3 a:4 a:5 a:6

tst "No rename in empty" aaren0.txt -R b/z
tst "No rename in one" aaren1no.txt -R b/z a:1
tst "Rename in one" aaren1yes.txt -R b/z b:1
tst "No rename in 6" aaren6no.txt -R x/z a:1 b:2 c:3 d:4 e:5 f:6
tst "Rename in 6" aaren6yes.txt -R d/z a:1 b:2 c:3 d:4 e:5 f:6
tst "No rename in aaa..." aarenaaano.txt -R d/z a:1 a:2 a:3 a:4 a:5 a:6
tst "Rename in aaa..." aarenaaayes.txt -R a/z a:1 a:2 a:3 a:4 a:5 a:6

tst "Abort at 0" aaabrta0.txt a:-1 b:2 c:-3 d:4 e:-5
tst "Abort at 2" aaabrta2.txt a:1 b:2 c:-3 d:4 e:-5
tst "Abort at 4" aaabrta4.txt a:1 b:2 c:3 d:4 e:-5

tst "No conditional delete in empty" aacondel0.txt -d b -C 3
tst "No conditional delete in one" aacondel1no.txt -d b -C 3 b:2
tst "Conditional delete in one" aacondel1yes.txt -d b -C 2 b:2
tst "No conditional delete in 6" aacondel6no.txt -d b -C 7 a:1 b:2 c:3 b:4 d:5 b:6
tst "Conditional delete one in 6" aacondel6one.txt -d b -C 4 a:1 b:2 c:3 b:4 d:5 b:6
tst "Conditional delete two in 6" aacondel6two.txt -d b -C 6 a:1 b:2 c:3 b:4 d:5 b:6

echo
if [ $xit -ne 0 ]; then
    echo "One or more tests failed"
else
    echo "All tests succeeded"
fi

exit $xit
