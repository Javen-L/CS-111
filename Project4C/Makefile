#NAME: Dhruv Singhania
#EMAIL: singhania_dhruv@yahoo.com
#ID: 105125631

default:
	gcc -o lab4c_tcp -Wall -Wextra -lmraa -lm -g lab4c_tcp.c
	gcc -o lab4c_tls -Wall -Wextra -lmraa -lm -lssl -lcrypto -g lab4c_tls.c

dist:
	tar -czvf lab4c-105125631.tar.gz lab4c_tcp.c lab4c_tls.c Makefile README

clean:
	rm -f lab4c-105125631.tar.gz lab4c_tcp lab4c_tls *.txt
