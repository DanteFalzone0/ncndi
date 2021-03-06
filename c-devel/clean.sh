#!/bin/sh
# Script to delete object files and binaries

echo "Removing object files...";

{ for file in ./*.o; do
    rm $file;
done } 2> /dev/null;

echo "Removing executables...";

if [ -f test_lexer.exe ]; then
    rm test_lexer.exe;
fi

if [ -f test_binary_op_0.exe ]; then
    rm test_binary_op_0.exe;
fi

if [ -f test_binary_op_0 ]; then
    rm test_binary_op_0;
fi

if [ -f test_binary_op_1.exe ]; then
    rm test_binary_op_1.exe;
fi

if [ -f test_binary_op_1 ]; then
    rm test_binary_op_1;
fi

if [ -f test_lexer ]; then
    rm test_lexer;
fi

if [ -f test_calculate.exe ]; then
    rm test_calculate.exe;
fi

if [ -f test_calculate ]; then
    rm test_calculate;
fi

echo "Done cleaning.";
