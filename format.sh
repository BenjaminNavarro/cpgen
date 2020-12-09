#!/usr/bin/env bash

root_folder=$(dirname $0)
src_files=`find $root_folder/include $root_folder/src $root_folder/tests $root_folder/apps -type f \( -name '*.cpp' -or -name '*.h*' \)`

if [ "$1" = "check" ]; then
    out=0
    tmpfile=$(mktemp /tmp/clang-format-check.XXXXXX)
    for f in ${src_files}; do
      clang-format -style=file $f > $tmpfile
      if ! [[ -z `diff $tmpfile $f` ]]; then
        echo "Wrong formatting in $f"
        out=1
      fi
    done
    rm -f $tmpfile
    if [[ $out -eq 1 ]]; then
      echo "You can run $0 to fix the issues locally, then commit/push again"
    fi
    exit $out
else
    clang-format -style=file -i $src_files
fi
