#! /usr/local/cs/bin/gnuplot

#NAME: Dhruv Singhania
#EMAIL: singhania_dhruv@yahoo.com
#ID: 105125631

# purpose:
#	 generate data reduction graphs for the multi-threaded list project

# input: lab2_list.csv
#	1. test name
#	2. # threads
#	3. # iterations per thread
#	4. # lists
#	5. # operations performed (threads x iterations x (ins + lookup + delete))
#	6. run time (ns)
#	7. run time per operation (ns)
#	8. wait time per operation (ns)

# output:
#	lab2b_1.png ... throughput vs. number of threads for mutex and spin-lock synchronized list operations
#	lab2b_2.png ... mean time per mutex wait and mean time per operation for mutex-synchronized list operations
#	lab2b_3.png ... successful iterations vs. threads for each synchronization method
#	lab2b_4.png ... throughput vs. number of threads for mutex synchronized partitioned lists
#	lab2b_5.png ... throughput vs. number of threads for spin-lock-synchronized partitioned lists

# general plot parameters
set terminal png
set datafile separator ","

#lab2b_1.png
set title "Throughput vs. Number of Threads (Mutex and Spin-lock Unpartitioned List)"
set xlabel "Threads"
set logscale x 2
set ylabel "Throughput"
set logscale y
set autoscale
set output 'lab2b_1.png'
plot \
     "< grep -e 'list-none-m,[0-9]*,1000,1,' lab2b_list.csv" using ($2):(1000000000)/($7) \
	title 'list w/ mutex' with linespoints lc rgb 'blue', \
     "< grep -e 'list-none-s,[0-9]*,1000,1,' lab2b_list.csv" using ($2):(1000000000)/($7) \
	title 'list w/ spin-lock' with linespoints lc rgb 'green'

#lab2b_2.png
set title "Mean Time per Mutex Wait and Mean Time per Operation"
set xlabel "Threads"
set logscale x 2
set ylabel "Time"
set logscale y
set autoscale
set output 'lab2b_2.png'
plot \
     "< grep -e 'list-none-m,[0-9]*,1000,1,' lab2b_list.csv" using ($2):($7) \
	title 'total completion time' with linespoints lc rgb 'blue', \
     "< grep -e 'list-none-m,[0-9]*,1000,1,' lab2b_list.csv" using ($2):($8) \
	title 'wait-for-lock time' with linespoints lc rgb 'green'

#lab2b_3.png
set title "Successful Iterations vs Threads"
set xlabel "Threads"
set logscale x 2
set ylabel "Successful Iterations"
set logscale y
set autoscale
set output 'lab2b_3.png'
plot \
     "< grep -e 'list-id-none,[0-9]*,[0-9]*,4,' lab2b_list.csv" using ($2):($3) \
	title 'unprotected' with points lc rgb 'blue', \
     "< grep -e 'list-id-m,[0-9]*,[0-9]*,4,' lab2b_list.csv" using ($2):($3) \
	title 'mutex' with points lc rgb 'green', \
     "< grep -e 'list-id-s,[0-9]*,[0-9]*,4,' lab2b_list.csv" using ($2):($3) \
	title 'spin-lock' with points lc rgb 'red'

#lab2b_4.png
set title "Throughput vs Number of Threads (Mutex Partitioned Lists)"
set xlabel "Threads"
set logscale x 2
set ylabel "Throughput"
set logscale y
set autoscale
set output 'lab2b_4.png'
plot \
     "< grep -e 'list-none-m,[0-9]*,1000,1,' lab2b_list.csv" using ($2):(1000000000)/($7) \
	title '1 list' with linespoints lc rgb 'blue', \
     "< grep -e 'list-none-m,[0-9]*,1000,4,' lab2b_list.csv" using ($2):(1000000000)/($7) \
	title '4 lists' with linespoints lc rgb 'green', \
     "< grep -e 'list-none-m,[0-9]*,1000,8,' lab2b_list.csv" using ($2):(1000000000)/($7) \
	title '8 lists' with linespoints lc rgb 'red', \
     "< grep -e 'list-none-m,[0-9]*,1000,16,' lab2b_list.csv" using ($2):(1000000000)/($7) \
	title '16 lists' with linespoints lc rgb 'yellow'


#lab2b_5.png
set title "Throughput vs Number of Threads (Spin-lock Partitioned Lists)"
set xlabel "Threads"
set logscale x 2
set ylabel "Throughput"
set logscale y
set autoscale
set output 'lab2b_5.png'
plot \
     "< grep -e 'list-none-s,[0-9]*,1000,1,' lab2b_list.csv" using ($2):(1000000000)/($7) \
	title '1 list' with linespoints lc rgb 'blue', \
     "< grep -e 'list-none-s,[0-9]*,1000,4,' lab2b_list.csv" using ($2):(1000000000)/($7) \
	title '4 lists' with linespoints lc rgb 'green', \
     "< grep -e 'list-none-s,[0-9]*,1000,8,' lab2b_list.csv" using ($2):(1000000000)/($7) \
	title '8 lists' with linespoints lc rgb 'red', \
     "< grep -e 'list-none-s,[0-9]*,1000,16,' lab2b_list.csv" using ($2):(1000000000)/($7) \
	title '16 lists' with linespoints lc rgb 'yellow'
