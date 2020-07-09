//NAME: Dhruv Singhania
//EMAIL: singhania_dhruv@yahoo.com
//ID: 105125631

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>

int segfault_flag = 0; //flag set by --segfault
int catch_flag = 0; //flag set by --catch

void segfault() { //subroutine to force segfault
  char *seg;
  seg = NULL;
  *seg = 'c';
}

void handle_sigsegv() { //subroutine to handle segfault
  fprintf(stderr, "--segfault failed, the program could not execute because a segmentation fault was forced\n");
  exit(4);
}

int main(int argc, char *argv[]) {
  int c; //holds value of getopt
  int ifd, ofd; //holds file descriptors for input and output files
  char buf; //holds buffer for storing bytes read
  while(1) {
    static struct option long_options[] = //holds key for all options
      {
       {"input", required_argument, 0, 'i'},
       {"output", required_argument, 0, 'o'},
       {"segfault", no_argument, &segfault_flag, 1},
       {"catch", no_argument, &catch_flag, 1},
       {0, 0, 0, 0}
      };
    int option_index = 0; //stores option index for getopt_long
    c = getopt_long(argc, argv, "i:o:", long_options, &option_index);
    if(c == -1) { //breaks when there are no more options
      break;
    } 
    switch(c) {
    case 0: //segfault and catch cases
      break;
    case 'i': //input case
	ifd = open(optarg, O_RDONLY);
	if(ifd >= 0) { //make file new fd0
	close(0);
	dup(ifd);
	close(ifd);
      }
	else { //throw error
	  fprintf(stderr, "--%s failed, file %s cannot be opened: %s\n", long_options[option_index].name, optarg, strerror(errno));
	exit(2);
	  }
      break;
      case 'o': //output case
	ofd = open(optarg, O_WRONLY | O_CREAT, 0666);
	if(ofd >= 0) { //make file new fd1
	close(1);
	dup(ofd);
	close(ofd);
      }
	else { //throw error
	  fprintf(stderr, "--%s failed, file %s cannot be created: %s\n", long_options[option_index].name, optarg, strerror(errno));
	exit(3);
	  }
      break;
    case '?': //unrecognized option
      fprintf(stderr, "usage: lab0 [--input=file1] [--output==file2] [--segfault] [--catch]\n");
      exit(1);
      break;
    default:
      abort();
    }
  }
  
  if(segfault_flag == 1 && catch_flag == 1) { //both segfault and catch flags
    signal(SIGSEGV, handle_sigsegv);
    segfault();
      }
  else if(segfault_flag == 1) { //only segfault flag
    segfault();
      }
  else { //no flags, normal process
    while(read(0, &buf, 1) > 0) {
      write(1, &buf, 1);
	}
      }
  close(0);
  close(1);
  exit(0);
}
