#!/bin/bash

#NAME: Dhruv Singhania
#EMAIL: singhania_dhruv@yahoo.com
#ID: 105125631

./lab4b --period=2 --scale=C --log=logfile <<-EOF
SCALE=F
PERIOD=1
START
STOP
LOG test
OFF
EOF

echo "Testing correct return value"
if [ $? -ne 0 ]
then
	echo "Failure"
else
	echo "Success"
fi

echo "Testing creation of logfile"
if [ ! -s logfile ]
then
	echo "Failure"
else
	echo "Success"
fi

echo "Testing command handling"
for c in SCALE=F PERIOD=1 START STOP OFF SHUTDOWN "LOG test"
do
	grep "$c" logfile > /dev/null
	if [ $? -ne 0 ]
	then
		echo "Failure"
	else
		echo "Success"
	fi
done

rm -f logfile
