#! /bin/bash

TOOL=~/obj/kdelibs/kjs/testkjs
DIR=.

if [ $# == 1 ]; then
    TOOL=$1
fi

echo "Starting to run tests"

# test tool available ?
if [ ! -x $TOOL ]; then
    echo "Can't execute test tool $TOOL"
    exit 1
fi

# can the shell test functions be parsed ?
$TOOL $DIR/shell.js >/dev/null 2>&1
if [ $? != 0 ]; then
    echo "Can't evaluate shell test functions in $DIR/shell.js"
    exit 2
fi

tests=`ls $DIR/*.js`
for t in $tests; do
    if [ `echo $t | grep -- '-n.js$'` ]; then
        $TOOL $t >/dev/null 2>&1
        if [ $? != 0 ]; then
            echo "PASS Expected failure in $t"
        else
            echo "FAIL Unexpected pass in $t"
        fi
    else
        echo "Executing $t"
        $TOOL $DIR/shell.js $t
    fi
done
echo "Done"
