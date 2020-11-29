# Copyright 2020 Brett M. Leedy
#!/bin/bash

echo "run_lint.bash: LINTING START=========================================================="


cpplint --recursive \
        --exclude=deps/ \
        --exclude=*.gitignore \
        --exclude=test/googletest/ \
        --exclude=test/build  \
        --exclude=cmake-modules  \
        ./* 2>&1

MESSAGE=`cpplint --recursive \
         --exclude=deps/ \
         --exclude=*.gitignore \
         --exclude=test/googletest/ \
         --exclude=test/build \
         --exclude=cmake-modules  \
         2>&1 ./* | grep "Total errors found"`

if [ "${MESSAGE}" = "" ]; then
    echo "run_lint.bash: ======================================================================="
    echo "run_lint.bash: linting success!";
    echo "run_lint.bash: ======================================================================="
    exit 0;
else
    echo "run_lint.bash: ======================================================================="
    echo "run_lint.bash: linting failure :      ${MESSAGE}";
    echo "run_lint.bash: ======================================================================="
    exit 1;
fi
