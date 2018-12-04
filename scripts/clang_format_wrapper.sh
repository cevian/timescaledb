#!/bin/bash

# clang-format misunderstand sql function written in C because they have the signature
#   Datum my_func(PG_FUNCTION_ARGS)
# and clang-format does not interpret PG_FUNCTION_ARGS as a type, name pair.
# This script replaces PG_FUNCTION_ARGS with "PG_FUNCTION_ARGS fake_var_for_clang" to
# make it look like a proper function for clang and then converts it back after clang runs.

FILES="${@}"

cleanup() {
    echo "cleaning"
    for opt in ${FILES}
        do
            if [[ "${opt:0:1}" != "-" ]]
            then
                sed -e 's/PG_FUNCTION_ARGS fake_var_for_clang/PG_FUNCTION_ARGS/' $opt > /tmp/replace
                mv /tmp/replace $opt
            fi
        done
    return
}

trap cleanup EXIT SIGINT SIGTERM

for opt in ${FILES}
do
    if [[ "${opt:0:1}" != "-" ]]
    then
        # sed -i have different semantics on mac and linux, don't use
        sed -e 's/(PG_FUNCTION_ARGS)/(PG_FUNCTION_ARGS fake_var_for_clang)/' $opt > /tmp/replace
        mv /tmp/replace $opt
    fi
done

clang-format $@

exit 0;
