//NAME: Dhruv Singhania
//EMAIL: singhania_dhruv@yahoo.com
//ID: 105125631

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <getopt.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <sched.h>

int ret; //return value for various functions
long long counter = 0; //counter to test race condition
int opt_yield = 0; //option to cause immediate yield
char sync_val = '\0'; //synchronization type
pthread_mutex_t m_lock; //lock used by add_m
int lock = 0; //lock used by add_s

void add(long long *pointer, long long value); //basic add routine
void add_m(long long *pointer, long long value); //add with mutex
void add_s(long long *pointer, long long value); //add with spin-lock
void add_c(long long *pointer, long long value); //add with compare-and-swap
void handle_error(char *msg, int code); //error handling
void *start_routine(void *arg); //thread routine

int main(int argc, char *const argv[]) {
  char name[15] = "add-"; //name of test
  struct timespec start, end; //start and end time
  long long operations, run_time, average_time; //output values
  int num_threads = 1; //number of parallel threads
  long long num_iterations = 1; //number of iterations
  pthread_t *thread; //space for threads
  int c; //holds value of getopt_long

  while(1) { //get options
    static struct option long_options[] = //holds key for options
      {
       {"threads", required_argument, 0, 't'},
       {"iterations", required_argument, 0, 'i'},
       {"yield", no_argument, 0, 'y'},
       {"sync", required_argument, 0, 's'},
       {0, 0, 0, 0}
      };
    int option_index = 0;
    c = getopt_long(argc, argv, "", long_options, &option_index);
    if(c == -1) { //breaks when there are no more options
      break;
    }
    switch(c) {
    case 't': //--threads option
      num_threads = atoi(optarg);
      if(num_threads < 1) {
        handle_error("Number of threads must be greater than 0", 0);
      }
      break;
    case 'i': //--iterations option
      num_iterations = atoi(optarg);
      if(num_iterations < 1) {
        handle_error("Number of iterations must be greater than 0", 0);
      }
      break;
    case 'y': //--yield option
      opt_yield = 1;
      strncat(name, "yield-", 7);
      break;
    case 's': //--sync option
      sync_val = optarg[0];
      if(sync_val != '\0' && sync_val != 'm' && sync_val != 's' && sync_val != 'c') {
        handle_error("Type of synchronization must be 'm', 's' or 'c'", 0);
      }
      break;
    case '?': //invalid option
    default:
      handle_error("Usage: lab2_add [--threads=num] [--iterations=num] [--yield] [--sync=val]", 0);
    }
  }

  switch(sync_val) { //update tag field
  case '\0': //none
    strncat(name, "none", 5);
    break;
  default:
    strncat(name, &sync_val, 2);
    break;
  }
  
  thread = (pthread_t*) malloc(sizeof(pthread_t) * num_threads); //allocate space
  if(thread == NULL) {
    handle_error("Error with malloc()", 1);
  }

  if(sync_val == 'm') { //initialize mutex
    ret = pthread_mutex_init(&m_lock, NULL);
    if(ret != 0) {
      handle_error("Error with pthread_mutex_init()", 1);
    }
  }

  ret = clock_gettime(CLOCK_MONOTONIC, &start); //starting time
  if(ret != 0) {
    handle_error("Error with clock_gettime()", 1);
  }

  for(int i = 0; i < num_threads; i++) { //create threads
    ret = pthread_create(&thread[i], NULL, start_routine, &num_iterations);
    if(ret != 0) {
      handle_error("Error with pthread_create()", 1);
    }
  }

  for(int i = 0; i < num_threads; i++) { //wait for threads
    ret = pthread_join(thread[i], NULL);
    if(ret != 0) {
      handle_error("Error with pthread_join()", 1);
    }
  }

  ret = clock_gettime(CLOCK_MONOTONIC, &end); //ending time
  if(ret != 0) { 
    handle_error("Error with clock_gettime()", 1);
  }

  operations = num_threads * num_iterations * 2; //output value calculations
  run_time = 1000000000 * (end.tv_sec - start.tv_sec) + end.tv_nsec - start.tv_nsec;
  average_time = run_time / operations;
  fprintf(stdout, "%s,%d,%lld,%lld,%lld,%lld,%lld\n", name, num_threads, num_iterations, operations, run_time, average_time, counter); //print output
  
  if(sync_val == 'm') { //destroy mutex
    ret = pthread_mutex_destroy(&m_lock);
    if(ret != 0) {
      handle_error("Error with pthread_mutex_destroy()", 1);
    }
  }

  free(thread); //free allocated memory
  exit(0);
}

void add(long long *pointer, long long value) {
  long long sum = *pointer + value;
  if(opt_yield) {
    ret = sched_yield();
    if(ret != 0) {
      handle_error("Error with sched_yield()", 1);
    }
  }
  *pointer = sum;
}

void add_m(long long *pointer, long long value) {
  ret = pthread_mutex_lock(&m_lock);
  if(ret != 0) {
    handle_error("Error with pthread_mutex_lock()", 1);
  }
  add(pointer, value);
  ret = pthread_mutex_unlock(&m_lock);
  if(ret != 0) {
    handle_error("Error with pthread_mutex_unlock()", 1);
  }
}

void add_s(long long *pointer, long long value) {
  while(__sync_lock_test_and_set(&lock, 1));
  add(pointer, value);
  __sync_lock_release(&lock);
}

void add_c(long long *pointer, long long value) {
  long long oldval, newval;
  do {
    oldval = counter;
    newval = oldval + value;
    if(opt_yield) {
      ret = sched_yield();
      if(ret != 0) {
        handle_error("Error with sched_yield()", 1);
      }
    }
  } while(__sync_val_compare_and_swap(pointer, oldval, newval) != oldval);
}

void handle_error(char *msg, int code) {
  switch(code) {
  case 0: //invalid command line parameter
    fprintf(stderr, "%s\n", msg);
    exit(1);
  case 1: //system call error
    perror(msg);
    exit(1);
  case 2: //other failures
    fprintf(stderr, "%s\n", msg);
    exit(2);
  } 
}

void *start_routine(void *arg) {
  long long *num_iter = (long long *) arg;
  for(int i = 0; i < *num_iter; i++) { //add 1 to counter
    switch(sync_val) {
    case '\0':
      add(&counter, 1);
      break;
    case 'm':
      add_m(&counter, 1);
      break;
    case 's':
      add_s(&counter, 1);
      break;
    case 'c':
      add_c(&counter, 1);
      break;
    }
  }
  for(int i = 0; i < *num_iter; i++) { //subtract 1 from counter
    switch(sync_val) {
    case '\0':
      add(&counter, -1);
      break;
    case 'm':
      add_m(&counter, -1);
      break;
    case 's':
      add_s(&counter, -1);
      break;
    case 'c':
      add_c(&counter, -1);
      break;
    }
  }
  return NULL;
}
