#NAME: Dhruv Singhania
#EMAIL: singhania_dhruv@yahoo.com
#ID: 105125631

default:
	gcc lab2_list.c SortedList.c -g -Wall -Wextra -lpthread -lprofiler -L /u/ce/ugrad/dhruv/Desktop/CS111/Project2B/gperftools-2.7/lib -o lab2_list
	LD_LIBRARY_PATH=/u/ce/ugrad/dhruv/Desktop/CS111/Project2B/gperftools-2.1/lib

tests: default
	./lab2_list --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --threads=2 --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --threads=4 --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --threads=8 --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --threads=24 --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --iterations=1000 --sync=s >> lab2b_list.csv
	./lab2_list --threads=2 --iterations=1000 --sync=s >> lab2b_list.csv
	./lab2_list --threads=4 --iterations=1000 --sync=s >> lab2b_list.csv
	./lab2_list --threads=8 --iterations=1000 --sync=s >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=1000 --sync=s >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=1000 --sync=s >> lab2b_list.csv
	./lab2_list --threads=24 --iterations=1000 --sync=s >> lab2b_list.csv

	-./lab2_list --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --iterations=2 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --iterations=4 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --iterations=8 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --iterations=16 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=4 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=4 --iterations=2 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=4 --iterations=4 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=4 --iterations=8 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=4 --iterations=16 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=8 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=8 --iterations=2 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=8 --iterations=4 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=8 --iterations=8 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=8 --iterations=16 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=12 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=12 --iterations=2 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=12 --iterations=4 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=12 --iterations=8 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=12 --iterations=16 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=16 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=16 --iterations=2 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=16 --iterations=4 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=16 --iterations=8 --yield=id --lists=4 >> lab2b_list.csv
	-./lab2_list --threads=16 --iterations=16 --yield=id --lists=4 >> lab2b_list.csv

	./lab2_list --iterations=10 --yield=id --sync=m --lists=4 >> lab2b_list.csv
	./lab2_list --iterations=20 --yield=id --sync=m --lists=4 >> lab2b_list.csv
	./lab2_list --iterations=40 --yield=id --sync=m --lists=4 >> lab2b_list.csv
	./lab2_list --iterations=80 --yield=id --sync=m --lists=4 >> lab2b_list.csv
	./lab2_list --threads=4 --iterations=10 --yield=id --sync=m --lists=4 >> lab2b_list.csv
	./lab2_list --threads=4 --iterations=20 --yield=id --sync=m --lists=4 >> lab2b_list.csv
	./lab2_list --threads=4 --iterations=40 --yield=id --sync=m --lists=4 >> lab2b_list.csv
	./lab2_list --threads=4 --iterations=80 --yield=id --sync=m --lists=4 >> lab2b_list.csv
	./lab2_list --threads=8 --iterations=10 --yield=id --sync=m --lists=4 >> lab2b_list.csv
	./lab2_list --threads=8 --iterations=20 --yield=id --sync=m --lists=4 >> lab2b_list.csv
	./lab2_list --threads=8 --iterations=40 --yield=id --sync=m --lists=4 >> lab2b_list.csv
	./lab2_list --threads=8 --iterations=80 --yield=id --sync=m --lists=4 >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=10 --yield=id --sync=m --lists=4 >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=20 --yield=id --sync=m --lists=4 >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=40 --yield=id --sync=m --lists=4 >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=80 --yield=id --sync=m --lists=4 >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=10 --yield=id --sync=m --lists=4 >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=20 --yield=id --sync=m --lists=4 >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=40 --yield=id --sync=m --lists=4 >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=80 --yield=id --sync=m --lists=4 >> lab2b_list.csv

	./lab2_list --iterations=10 --yield=id --sync=s --lists=4 >> lab2b_list.csv
	./lab2_list --iterations=20 --yield=id --sync=s --lists=4 >> lab2b_list.csv
	./lab2_list --iterations=40 --yield=id --sync=s --lists=4 >> lab2b_list.csv
	./lab2_list --iterations=80 --yield=id --sync=s --lists=4 >> lab2b_list.csv
	./lab2_list --threads=4 --iterations=10 --yield=id --sync=s --lists=4 >> lab2b_list.csv
	./lab2_list --threads=4 --iterations=20 --yield=id --sync=s --lists=4 >> lab2b_list.csv
	./lab2_list --threads=4 --iterations=40 --yield=id --sync=s --lists=4 >> lab2b_list.csv
	./lab2_list --threads=4 --iterations=80 --yield=id --sync=s --lists=4 >> lab2b_list.csv
	./lab2_list --threads=8 --iterations=10 --yield=id --sync=s --lists=4 >> lab2b_list.csv
	./lab2_list --threads=8 --iterations=20 --yield=id --sync=s --lists=4 >> lab2b_list.csv
	./lab2_list --threads=8 --iterations=40 --yield=id --sync=s --lists=4 >> lab2b_list.csv
	./lab2_list --threads=8 --iterations=80 --yield=id --sync=s --lists=4 >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=10 --yield=id --sync=s --lists=4 >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=20 --yield=id --sync=s --lists=4 >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=40 --yield=id --sync=s --lists=4 >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=80 --yield=id --sync=s --lists=4 >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=10 --yield=id --sync=s --lists=4 >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=20 --yield=id --sync=s --lists=4 >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=40 --yield=id --sync=s --lists=4 >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=80 --yield=id --sync=s --lists=4 >> lab2b_list.csv

	./lab2_list --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --threads=2 --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --threads=4 --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --threads=8 --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --iterations=1000 --sync=m --lists=4 >> lab2b_list.csv
	./lab2_list --threads=2 --iterations=1000 --sync=m --lists=4 >> lab2b_list.csv
	./lab2_list --threads=4 --iterations=1000 --sync=m --lists=4 >> lab2b_list.csv
	./lab2_list --threads=8 --iterations=1000 --sync=m --lists=4 >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=1000 --sync=m --lists=4 >> lab2b_list.csv
	./lab2_list --iterations=1000 --sync=m --lists=8 >> lab2b_list.csv
	./lab2_list --threads=2 --iterations=1000 --sync=m --lists=8 >> lab2b_list.csv
	./lab2_list --threads=4 --iterations=1000 --sync=m --lists=8 >> lab2b_list.csv
	./lab2_list --threads=8 --iterations=1000 --sync=m --lists=8 >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=1000 --sync=m --lists=8 >> lab2b_list.csv
	./lab2_list --iterations=1000 --sync=m --lists=16 >> lab2b_list.csv
	./lab2_list --threads=2 --iterations=1000 --sync=m --lists=16 >> lab2b_list.csv
	./lab2_list --threads=4 --iterations=1000 --sync=m --lists=16 >> lab2b_list.csv
	./lab2_list --threads=8 --iterations=1000 --sync=m --lists=16 >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=1000 --sync=m --lists=16 >> lab2b_list.csv

	./lab2_list --iterations=1000 --sync=s >> lab2b_list.csv
	./lab2_list --threads=2 --iterations=1000 --sync=s >> lab2b_list.csv
	./lab2_list --threads=4 --iterations=1000 --sync=s >> lab2b_list.csv
	./lab2_list --threads=8 --iterations=1000 --sync=s >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=1000 --sync=s >> lab2b_list.csv
	./lab2_list --iterations=1000 --sync=s --lists=4 >> lab2b_list.csv
	./lab2_list --threads=2 --iterations=1000 --sync=s --lists=4 >> lab2b_list.csv
	./lab2_list --threads=4 --iterations=1000 --sync=s --lists=4 >> lab2b_list.csv
	./lab2_list --threads=8 --iterations=1000 --sync=s --lists=4 >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=1000 --sync=s --lists=4 >> lab2b_list.csv
	./lab2_list --iterations=1000 --sync=s --lists=8 >> lab2b_list.csv
	./lab2_list --threads=2 --iterations=1000 --sync=s --lists=8 >> lab2b_list.csv
	./lab2_list --threads=4 --iterations=1000 --sync=s --lists=8 >> lab2b_list.csv
	./lab2_list --threads=8 --iterations=1000 --sync=s --lists=8 >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=1000 --sync=s --lists=8 >> lab2b_list.csv
	./lab2_list --iterations=1000 --sync=s --lists=16 >> lab2b_list.csv
	./lab2_list --threads=2 --iterations=1000 --sync=s --lists=16 >> lab2b_list.csv
	./lab2_list --threads=4 --iterations=1000 --sync=s --lists=16 >> lab2b_list.csv
	./lab2_list --threads=8 --iterations=1000 --sync=s --lists=16 >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=1000 --sync=s --lists=16 >> lab2b_list.csv

graphs: tests
	chmod u+x lab2b_list.gp
	./lab2b_list.gp

profile:
	rm -f ./rawtxt profile.out
	LD_PRELOAD=/u/ce/ugrad/dhruv/Desktop/CS111/Project2B/gperftools-2.1/lib/libprofiler.so.0 CPUPROFILE=./rawtxt ./lab2_list --iterations=1000 --threads=12 --sync=s
	pprof --text ./lab2_list ./rawtxt > profile.out
	pprof --list=start_routine ./lab2_list ./rawtxt >> profile.out
	rm -f ./rawtxt

dist: graphs profile
	tar -czvf lab2b-105125631.tar.gz SortedList.h SortedList.c lab2_list.c Makefile lab2b_list.csv profile.out lab2b_1.png lab2b_2.png lab2b_3.png lab2b_4.png lab2b_5.png lab2b_list.gp README

clean:
	rm -f lab2_list lab2b-105125631.tar.gz
