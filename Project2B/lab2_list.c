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
#include <signal.h>
#include "SortedList.h"

int ret; //return value for various functions
int num_threads = 1; //number of parallel threads
long long num_iterations = 1; //number of iterations
int num_lists = 1; //number of lists
int opt_yield = 0; //option to cause specified yields
char sync_val = '\0'; //synchronization type
SortedList_t *list; //list used to add and delete
SortedListElement_t *element; //space for elements
int *positions; //position in element passed to thread
pthread_t *thread; //space for threads
pthread_mutex_t *m_lock; //lock used by sync_val m
int *lock; //lock used by sync_val s

void handle_error(char *msg, int code); //error handling
char* generate_string(); //generate random string as key
void *start_routine(void *arg); //thread routine
void handle_free(); //handle freeing space
void handle_segfault(); //segfault handling
int generate_hash(const char *key); //generate hash value

int main(int argc, char *const argv[]) {
  atexit(handle_free); //free allocated space before exit
  signal(SIGSEGV, handle_segfault); //catch segfaults

  char name[15] = "list-"; //name of test
  struct timespec start, end; //start and end time
  long long operations, run_time, average_time, average_wait; //output values
  long long wait_time; //total wait time
  int c; //holds value of getopt_long

  while(1) { //get options
    static struct option long_options[] = //holds key for options
      {
       {"threads", required_argument, 0, 't'},
       {"iterations", required_argument, 0, 'i'},
       {"lists", required_argument, 0, 'l'},
       {"yield", required_argument, 0, 'y'},
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
    case 'l': //--lists option
      num_lists = atoi(optarg);
      if(num_lists < 1) {
        handle_error("Number of lists must be greater than 0", 0);
      }
      break;
    case 'y': //--yield option
      if(strlen(optarg) > 3) {
        handle_error("Types of yield must not exceed 3", 0);
      }
      for(int i =0; i < (int)strlen(optarg); i++) {
        switch(optarg[i]) {
        case 'i':
          opt_yield |= INSERT_YIELD;
          break;
        case 'd':
          opt_yield |= DELETE_YIELD;
          break;
        case 'l':
          opt_yield |= LOOKUP_YIELD;
          break;
        default:
          handle_error("Types of yield must be 'i', 'd' or 'l'", 0);
        }
      }
      break;
    case 's': //--sync option
      sync_val = optarg[0];
      if(sync_val != '\0' && sync_val != 'm' && sync_val != 's') {
        handle_error("Type of synchronization must be 'm' or 's'", 0);
      }
      break;
    case '?': //invalid option
    default:
      handle_error("Usage: lab2_list [--threads=num] [--iterations=num] [--yield=vals] [--sync=val]", 0);
    }
  }

  if(opt_yield & INSERT_YIELD) { //update tag field
    strncat(name, "i", 2);
  }
  if(opt_yield & DELETE_YIELD) {
    strncat(name, "d", 2);
  }
  if(opt_yield & LOOKUP_YIELD) {
    strncat(name, "l", 2);
  }
  if(opt_yield == 0) {
    strncat(name, "none", 5);
  }
  strncat(name, "-", 2);
  switch(sync_val) {
  case '\0': //none
    strncat(name, "none", 5);
    break;
  default:
    strncat(name, &sync_val, 2);
    break;
  }

  list = (SortedList_t*) malloc(sizeof(SortedList_t) * num_lists); //allocate space for lists
  if(list == NULL) {
    handle_error("Error with malloc()", 1);
  }

  for(int i = 0; i < num_lists; i++) { //initialize lists
    list[i].key = NULL;
    list[i].next = &list[i];
    list[i].prev = &list[i];
  }

  long long num_elements = num_threads * num_iterations; //allocate space for elements
  element = (SortedListElement_t*) malloc(sizeof(SortedListElement_t) * num_elements);
  if(element == NULL) {
    handle_error("Error with malloc()", 1);
  }

  for(int i = 0; i < num_elements; i++) { //initialize elements
    element[i].key = generate_string();
    element[i].prev = &element[i];
    element[i].next = &element[i];
  }

  thread = (pthread_t*) malloc(sizeof(pthread_t) * num_threads); //allocate space for threads
  if(thread == NULL) {
    handle_error("Error with malloc()", 1);
  }

  if(sync_val == 'm') { //allocate space for and initialize mutexes
    m_lock = malloc(sizeof(pthread_mutex_t) * num_lists);
    if(m_lock == NULL) {
      handle_error("Error with malloc()", 1);
    }
    for(int i = 0; i < num_lists; i++) {
      ret = pthread_mutex_init(&m_lock[i], NULL);
      if(ret != 0) {
        handle_error("Error with pthread_mutex_init()", 1);
      }
    }
  }

  if(sync_val == 's') { //allocate space for and initialize spin locks
    lock = malloc(sizeof(int) * num_lists);
    if(lock == NULL) {
      handle_error("Error with malloc()", 1);
    }
    for(int i = 0; i < num_lists; i++) {
      lock[i] = 0;
    }
  }

  positions = (int *) malloc(sizeof(int) * num_threads); //allocate space of positions
  if(positions == NULL) {
    handle_error("Error with malloc()", 1);
  }

  ret = clock_gettime(CLOCK_MONOTONIC, &start); //starting time
  if(ret != 0) {
    handle_error("Error with clock_gettime()", 1);
  }

  for(int i = 0; i < num_threads; i++) { //create threads
    positions[i] = i * num_iterations;
    ret = pthread_create(&thread[i], NULL, start_routine, &positions[i]);
    if(ret != 0) {
      handle_error("Error with pthread_create()", 1);
    }
  }

  void **wait = (void *) malloc(sizeof(void **)); //allocate space for wait time

  for(int i = 0; i < num_threads; i++) { //wait for threads
    ret = pthread_join(thread[i], wait);
    if(ret != 0) {
      handle_error("Error with pthread_join()", 1);
    }
    wait_time += (long long) *wait;
  }

  free(wait); //free space for wait time

  ret = clock_gettime(CLOCK_MONOTONIC, &end); //ending time
  if(ret != 0) { 
    handle_error("Error with clock_gettime()", 1);
  }

  int len = 0; //check if lists are empty
  for(int i = 0; i < num_lists; i++) {
    len += SortedList_length(&list[i]);
  }
  if(len != 0) {
    handle_error("List length is not 0", 2);
  }

  operations = num_threads * num_iterations * 3; //output value calculations
  run_time = 1000000000 * (end.tv_sec - start.tv_sec) + end.tv_nsec - start.tv_nsec;
  average_time = run_time / operations;
  average_wait = wait_time / (operations * 30);
  fprintf(stdout, "%s,%d,%lld,%d,%lld,%lld,%lld,%lld\n", name, num_threads, num_iterations, num_lists, operations, run_time, average_time, average_wait); //print output

  if(sync_val == 'm') { //destroy mutexes
    for(int i = 0; i < num_lists; i++) {
      ret = pthread_mutex_destroy(&m_lock[i]);
      if(ret != 0) {
        handle_error("Error with pthread_mutex_destroy()", 1);
      }
    }
  }

  exit(0);
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
  return;
}

char* generate_string() {
  static const char chars[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
  int len = rand() % 5;
  char *temp = (char*) malloc(sizeof(char) * len);
  for(int i = 0; i < len; i++) {
    temp[i] = chars[rand() % (sizeof(chars) - 1)];
  }
  return temp;
}

void *start_routine(void *arg) {
  long long wait = 0;
  struct timespec start, end;
  int first = *((int *) arg);

  for(int i = first; i < first + num_iterations; i++) { //insert elements
    int hash = generate_hash(element[i].key);
    switch(sync_val) {
    case 'm':
      clock_gettime(CLOCK_MONOTONIC, &start);
      pthread_mutex_lock(&m_lock[hash]);
      clock_gettime(CLOCK_MONOTONIC, &end);
      wait += 1000000000 * (end.tv_sec - start.tv_sec) + end.tv_nsec - start.tv_nsec;
      break;
    case 's':
      clock_gettime(CLOCK_MONOTONIC, &start);
      while(__sync_lock_test_and_set(&lock[hash], 1));
      clock_gettime(CLOCK_MONOTONIC, &end);
      wait += 1000000000 * (end.tv_sec - start.tv_sec) + end.tv_nsec - start.tv_nsec;
      break;
    }
    SortedList_insert(&list[hash], &element[i]);
    switch(sync_val) {
    case 'm':
      pthread_mutex_unlock(&m_lock[hash]);
      break;
    case 's':
      __sync_lock_release(&lock[hash]);
      break;
    }
  }

  int list_len = 0; //check list lengths
  for(int i = 0; i < num_lists; i++) {
    switch(sync_val) {
    case 'm':
      clock_gettime(CLOCK_MONOTONIC, &start);
      pthread_mutex_lock(&m_lock[i]);
      clock_gettime(CLOCK_MONOTONIC, &end);
      wait += 1000000000 * (end.tv_sec - start.tv_sec) + end.tv_nsec - start.tv_nsec;
      break;
    case 's':
      clock_gettime(CLOCK_MONOTONIC, &start);
      while(__sync_lock_test_and_set(&lock[i], 1));
      clock_gettime(CLOCK_MONOTONIC, &end);
      wait += 1000000000 * (end.tv_sec - start.tv_sec) + end.tv_nsec - start.tv_nsec;
      break;
    }
    list_len += SortedList_length(&list[i]);
    switch(sync_val) {
    case 'm':
      pthread_mutex_unlock(&m_lock[i]);
      break;
    case 's':
      __sync_lock_release(&lock[i]);
      break;
    }
  }
  if(list_len < num_iterations) {
    handle_error("Error inserting elements in list", 2);
  }

  for(int i = first; i < first + num_iterations; i++) { //find and delete elements
    int hash = generate_hash(element[i].key);
    switch(sync_val) {
    case 'm':
      clock_gettime(CLOCK_MONOTONIC, &start);
      pthread_mutex_lock(&m_lock[hash]);
      clock_gettime(CLOCK_MONOTONIC, &end);
      wait += 1000000000 * (end.tv_sec - start.tv_sec) + end.tv_nsec - start.tv_nsec;
      break;
    case 's':
      clock_gettime(CLOCK_MONOTONIC, &start);
      while(__sync_lock_test_and_set(&lock[hash], 1));
      clock_gettime(CLOCK_MONOTONIC, &end);
      wait += 1000000000 * (end.tv_sec - start.tv_sec) + end.tv_nsec - start.tv_nsec;
      break;
    }
    SortedListElement_t *temp = SortedList_lookup(&list[hash], element[i].key);
    if(temp == NULL) {
      handle_error("Error with SortedList_lookup()", 2);
    }
    ret = SortedList_delete(temp);
    if(ret != 0) {
      handle_error("Error with SortedList_delete()", 2);
    }
    switch(sync_val) {
    case 'm':
      pthread_mutex_unlock(&m_lock[hash]);
      break;
    case 's':
      __sync_lock_release(&lock[hash]);
      break;
    }
  }

  return (void *) wait;
}

void handle_free() {
  free(list);
  free(element);
  free(thread);
  free(positions);
  free(m_lock);
  free(lock);
  return;
}

void handle_segfault() {
  handle_error("Segmentation fault", 2);
  return;
}

int generate_hash(const char *key) {
  return key[0] % num_lists;
}