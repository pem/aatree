#!/bin/sh

generate=false

xit=0

name2file() {
    name=$1
    newname=`echo "aatree-$1" | tr -s ' ,.' '-'`
    echo "$newname.txt"
}

tst() {
    name=$1
    file=`name2file "$name"`
    shift
    if $generate ; then
	echo "Writing $file"
	../aatree-test $* > ./$file
    else
#        valgrind --leak-check=full --show-leak-kinds=all ../aatree-test $* > /tmp/$file
	../aatree-test $* > /tmp/$file
	cs1=`cksum /tmp/$file | cut -d' ' -f1`
	cs2=`cksum $file | cut -d' ' -f1`
	if [ "$cs1" != "$cs2" ]; then
	    xit=1
	    echo "$name FAIL" >&2
	else
	    echo "$name OK"
	fi
	rm /tmp/$file
    fi
}

if [ $# -gt 0 ]; then
    if [ $# -eq 1 -a "$1" = '-g' ]; then
	echo 'Generating new result files'
	echo 'This will overwrite existing files in this directory\!'
	echo 'Continue? [y/N]'
	read yesorno
	echo $yesorno | egrep -i '(y|yes)' > /dev/null 2>&1
	if [ $? -eq  0 ]; then
	    generate=true
	else
	    echo 'Aborted' >&2
	    exit 1
	fi
    else
	echo 'Usage: aatree-test.sh [-g]' >&2
	exit 1
    fi
fi

tst Empty
tst 'aaaaaa' -v a:1 a:2 a:3 a:4 a:5 a:6
tst 123 1 2 3 1 2 3 1 2 3
tst "1-7 ordered" -v 1 2 3 4 5 6 7 
tst "7-1 ordered" -v 7 6 5 4 3 2 1
tst "1-7 balanced" -v 4 2 6 1 3 5 7
tst "Values, unique keys" -v f:6 g:7 a:1 c:3 b:2 d:4 e:5 i:9 h:8
tst "Values, dup. keys" -v f:9 a:1 e:6 c:4 c:3 b:2 c:5 d:7 e:8

# Examples from "Balanced Search Trees Made Simple" by Arne Andersson,
# http://user.it.uu.se/~arnea/ps/simp.pdf .
tst AA1 04 10 02 08 12 01 03 05 09 11 13 07
tst AA2 04 10 02 08 12 01 03 05 09 11 13 07 06
tst AA3 -d01 04 10 02 08 12 01 03 05 09 11 13 07 06

tst FindNone -f x a
tst FindOne -f a a
tst DeleteNone -d x a
tst DeleteOne -d a a
tst aaaaDeleteOne -d a a:1 a:2 a:3 a:4 a:5 a:6

tst "Find no unique keys" -f z f:6 g:7 a:1 c:3 b:2 d:4 e:5 i:9 h:8
tst "Find a unique key 1" -f c f:6 g:7 a:1 c:3 b:2 d:4 e:5 i:9 h:8
tst "Find a unique key 2" -f d f:6 g:7 a:1 c:3 b:2 d:4 e:5 i:9 h:8
tst "Find no dup. keys" -f z f:9 a:1 e:6 c:4 c:3 b:2 c:5 d:7 e:8
tst "Find a dup. key 1" -f c f:9 a:1 e:6 c:4 c:3 b:2 c:5 d:7 e:8
tst "Find a dup. key 2" -f e f:9 a:1 e:6 c:4 c:3 b:2 c:5 d:7 e:8
# Built a peculiar tree where only the first x node as found
tst "Find a dup. key bug" -f x x:1 x:2 zip:zap foo:bar

tst "Insert unique in empty" -u 1
tst "Insert unique in one ok" -u 1 2
tst "Insert unique in one fail" -u 1 1
tst "Insert unique in 7 ok" -u 1 2 3 4 5 6 7 8
tst "Insert unique in 7 fail" -u 1 2 3 4 5 6 7 6

tst "No Replace in empty" -r a:1
tst "No Replace in one" -r a:1 b:2
tst "Replace in one" -r a:1 a:2
tst "No Replace in 7" -r a:1 b:2 c:3 d:4 e:5 f:6 g:7 h:8
tst "Replace in 7" -r a:1 b:2 c:3 d:4 e:5 f:6 g:7 f:8
tst "Replace in aaa..." -v -r a:1 a:2 a:3 a:4 a:5 a:6

tst "No rename in empty" -R b/z
tst "No rename in one" -R b/z a:1
tst "Rename in one" -R b/z b:1
tst "No rename in 6" -R x/z a:1 b:2 c:3 d:4 e:5 f:6
tst "Rename in 6" -R d/z a:1 b:2 c:3 d:4 e:5 f:6
tst "No rename in aaa..." -R d/z a:1 a:2 a:3 a:4 a:5 a:6
tst "Rename in aaa..." -R a/z a:1 a:2 a:3 a:4 a:5 a:6

tst "Abort at 0" a:-1 b:2 c:-3 d:4 e:-5
tst "Abort at 2" a:1 b:2 c:-3 d:4 e:-5
tst "Abort at 4" a:1 b:2 c:3 d:4 e:-5

tst "Find none conditionally in empty" -f a:3
tst "Find one conditionally in one" -f a:1 a:1
tst "Find none conditionally in 7" -f a:9 a:1 a:2 a:3 a:4 a:5 a:6 a:7
tst "Find 1 conditionally in 7" -f a:1 a:1 a:2 a:3 a:4 a:5 a:6 a:7
tst "Find 2 conditionally in 7" -f a:2 a:1 a:2 a:3 a:4 a:5 a:6 a:7
tst "Find 3 conditionally in 7" -f a:3 a:1 a:2 a:3 a:4 a:5 a:6 a:7
tst "Find 4 conditionally in 7" -f a:4 a:1 a:2 a:3 a:4 a:5 a:6 a:7
tst "Find 5 conditionally in 7" -f a:5 a:1 a:2 a:3 a:4 a:5 a:6 a:7
tst "Find 6 conditionally in 7" -f a:6 a:1 a:2 a:3 a:4 a:5 a:6 a:7
tst "Find 7 conditionally in 7" -f a:7 a:1 a:2 a:3 a:4 a:5 a:6 a:7

tst "No conditional delete in empty" -d b:3
tst "No conditional delete in one" -d b:3 b:2
tst "Conditional delete in one" -d b:2 b:2
tst "No conditional delete in 6" -d b:7 a:1 b:2 c:3 b:4 d:5 b:6
tst "Conditional delete one in 6" -d b:4 a:1 b:2 c:3 b:4 d:5 b:6
tst "Conditional delete two in 6" -d b:6 a:1 b:2 c:3 b:4 d:5 b:6

echo
if [ $xit -ne 0 ]; then
    echo "One or more tests failed"
else
    echo "All tests succeeded"
fi

exit $xit
