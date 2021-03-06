#!/bin/bash

#NAME: Dhruv Singhania
#EMAIL: singhania_dhruv@yahoo.com
#ID: 105125631

#Testing Error 0
echo "Testing Error 0"
`echo "hello" > input.txt`
`./lab0 --input=input.txt --output=output.txt`
if [ $? -eq 0 ]
then
    echo "Copy resulted in correct error code"
else
    echo "Copy resulted in incorrect error code"
fi

`cmp -l input.txt output.txt`
if [ $? -eq 0 ]
then
    echo "Copy was successful"
else
    echo "Copy was unsuccessful"
fi

#Testing Error 1
echo "Testing Error 1"
`./lab0 --incorrect  2> junk.txt`
if [ $? -eq 1 ]
then
    echo "Unrecognized argument resulted in correct error code"
else
    echo "Unrecognized argument resulted in incorrect error code"
fi

#Testing Error 2
echo "Testing Error 2"
`./lab0 --input=incorrect.txt 2> junk.txt`
if [ $? -eq 2 ]
then
    echo "Inability to open input file resulted in correct error code"
else
    echo "Inability to open input file resulted in incorrect error code"
fi

#Testing Error 3
echo "Testing Error 3"
`chmod u-w output.txt`
`./lab0 --input=input.txt --output=output.txt 2> junk.txt`
if [ $? -eq 3 ]
then
    echo "Inability to open output file resulted in correct error code"
else
    echo "Inability to open output file resulted in incorrect error code"
fi

#Testing Error 4
echo "Testing Error 4"
`./lab0 --segfault --catch 2> junk.txt`
if [ $? -eq 4 ]
then
    echo "Catching and receiving SIGSEGV resulted in correct error code"
else
    echo "Catching and receiving SIGSEV resulted in incorrect error code"
fi

#Removing created files
`rm -f input.txt output.txt junk.txt`
