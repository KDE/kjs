#!/bin/sh

if [ -z $ECMATEST_BASEDIR ]; then
    echo "WARNING: ECMATEST_BASEDIR not set."
fi

for i in bestPractice ch07 ch08 ch09 ch10 ch11 ch12 ch13 ch14; do
    ECMATEST_CHAPTER=$i ./ecmatest.shell | grep ^FAIL | sed 's,^[^(]*(,,;s,).*,,' > ecmatest_broken_$i;
done

for i in 15.1 15.2 15.3 15.4 15.5 15.6 15.7 15.8 15.9 15.10 15.11 15.12; do
    ECMATEST_CHAPTER=ch15/$i ./ecmatest.shell | grep ^FAIL | sed 's,^[^(]*(,,;s,).*,,' > ecmatest_broken_ch$i;
done
