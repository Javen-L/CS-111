//NAME: Dhruv Singhania
//EMAIL: singhania_dhruv@yahoo.com
//ID: 105125631

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <getopt.h>
#include <termios.h>
#include <netdb.h>
#include <poll.h>
#include "zlib.h"
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>

int ret; //return value for system calls
int port_flag = 0; //mandatory flag set by --port
int shell_flag = 0; //flag set by --shell
int compress_flag = 0; //flag set by --compress
int port_no; //port num set by --port
char program_name[256]; //name of program in --shell
int sockfd, new_sockfd; //sock file descriptors set by sock() and accept()
int pipefd1[2], pipefd2[2]; //pipe file descriptors
int fork_ret; //return value for fork(), id of process
int exit_flag = 0; //exit status of process
z_stream send_stream, rec_stream; //compression stream set by --compress

void error(char *msg); //error handling
void handle_sig(int signum); //handles SIGPIPE and SIGINT
void get_options(int argc, char *argv[]); //performs getopt_long
void create_socket(); //opens connection to the client
void read_write(); //simple read and write if --shell is not used
void child_process(); //function for child process
void parent_process(); //function for parent process
void client_input();
void shell_input();
void handle_exit();

int main(int argc, char *argv[])
{
  get_options(argc, argv);
  create_socket();

  if(shell_flag == 0)
    {
      read_write();
    }
  
  ret = pipe(pipefd1); //creates two pipes between server and shell, one for each direction
  if(ret < 0) //error handling
    {
      error("Error with pipe()");
    }
  ret = pipe(pipefd2);
  if(ret < 0) //error handling
    {
      error("Error with pipe()");
    }

  signal(SIGPIPE, handle_sig); //activates the SIGPIPE handler
  signal(SIGINT, handle_sig);

  fork_ret = fork();
  if(fork_ret < 0) //error handling
    {
      error("Error with fork()");
    }
  else if(fork_ret == 0) //child process
    {
      child_process();
    }
  else //parent process
    {
      parent_process();
    }
  if(compress_flag)
    {
      inflateEnd(&send_stream);
      deflateEnd(&rec_stream);
    }
  exit(0);
}

void error(char *msg)
{
  perror(msg);
  exit(1);
}

void handle_sig(int signum)
{
  if(signum == SIGPIPE)
    {
      fprintf(stderr, "SIGPIPE recieved\n");
      exit(0);
    }
  if(signum == SIGINT)
    {
      ret = kill(fork_ret, SIGINT);
    if(ret < 0)
      {
        error("Error with kill()");
      }
    }
}

void get_options(int argc, char *argv[])
{
  int c; //holds value of getopt_long
  while(1) {
    static struct option long_options[] = //holds key for options
      {
       {"port", required_argument, 0, 'p'},
       {"shell", required_argument, 0, 's'},
       {"compress", no_argument, 0, 'c'},
       {0, 0, 0, 0}
      };
    int option_index = 0;
    c = getopt_long(argc, argv, "", long_options, &option_index);
    if(c == -1) //breaks when there are no more options
      {
	break;
      }
    switch(c)
      {
      case 'p': //--port case
	port_flag = 1;
	port_no = atoi(optarg);
	break;
      case 's': //--shell case
	shell_flag = 1;
	strcpy(program_name, optarg); //stores argument in program_name
	break;
      case 'c': //--compress case
	compress_flag = 1;
        send_stream.zalloc = Z_NULL;
	send_stream.zfree = Z_NULL;
	send_stream.opaque = Z_NULL;
	ret = inflateInit(&send_stream);
	if(ret != Z_OK)
	  {
	    error("Error with inflateInit()");
	  }
	rec_stream.zalloc = Z_NULL;
	rec_stream.zfree = Z_NULL;
	rec_stream.opaque = Z_NULL;
	ret = deflateInit(&rec_stream, Z_DEFAULT_COMPRESSION);
	if(ret != Z_OK)
	  {
	    error("Error with deflateInit()");
	  }
	break;
      case '?': //unrecognized option
	fprintf(stderr, "usage: lab1b-client --port='num' [--shell=program] [--compress]");
	exit(1);
      default:
	abort();
      }
  }
  if(port_flag == 0) //makes sure --port was used
    {
      fprintf(stderr, "usage: lab1b-client --port='num' [--shell=program] [--compress]");
      exit(1);
    }
}

void create_socket()
{
  unsigned int cli_len;
  struct sockaddr_in serv_addr, cli_addr;
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if(sockfd < 0)
    {
      error("Error with socket()");
    }
  bzero((char *)&serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(port_no);
  ret = bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
  if(ret < 0)
    {
      error("Error with bind()");
    }
  listen(sockfd, 5);
  cli_len = sizeof(cli_addr);
  new_sockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &cli_len);
  if(new_sockfd < 0)
    {
      error("Error with accept()");
    }
}

void read_write()
{
  while(1)
    {
      char buf[16], temp[2]; //to help with read write
      int bytes_read; //stores num of bytes read
      bytes_read = read(0, buf, 16);
      if(bytes_read < 0) //error handling
        {
          error("Error with read()");
        }
      for(int i = 0; i < bytes_read; i++)
        {
          switch(buf[i])
            {
              case 4: //^D case
	        temp[0] = '^';
	        temp[1] = 'D';
	        write(1, temp, 2);
	        exit(0);
              case 10: //<lf> case
              case 13: //<cr> case
	        temp[0] = 13;
	        temp[1] = 10;
	        write(1, temp, 2);
	        break;
              default: //default case
	        write(1, &buf[i], 1);
	        break;
            }
        }
    }
  exit(0);
}

void child_process()
{
  close(pipefd1[1]); //sets up pipes
  close(pipefd2[0]);
  dup2(pipefd1[0], 0);
  dup2(pipefd2[1], 1);
  dup2(pipefd2[1], 2);
  close(pipefd1[0]);
  close(pipefd2[1]);
  char *myargs[2]; //to pass to execvp
  myargs[0] = strdup(program_name);
  myargs[1] = NULL;
  execvp(myargs[0], myargs); //executed specified program
  error("Error with execvp()"); //error handling
}

void parent_process()
{
  close(pipefd1[0]); //sets up pipes
  close(pipefd2[1]);
  struct pollfd fds[] = //holds key for poll
    {
     {new_sockfd, POLLIN, 0},
     {pipefd2[0], POLLIN, 0}
    };
  atexit(handle_exit);
  while(!exit_flag)
    {
      ret = poll(fds, 2, 0);
      if(ret < 0) //error handling
	{
	  error("Error with poll()");
	}
      if(fds[0].revents & POLLIN)
	{
	  client_input();
	}
      else if(fds[0].revents & POLLERR)
	{
	  error("Error with poll()");
	}
      else if(fds[1].revents & POLLIN)
	{
	  shell_input();
	}
      else if(fds[1].revents & POLLERR || fds[1].revents & POLLHUP)
	{
	  exit_flag = 1;
	}
    }
  close(sockfd);
  close(new_sockfd);
  close(pipefd1[1]);
  close(pipefd2[0]);
}

void client_input()
{
  char buf[256], temp;
  int bytes_read = read(new_sockfd, buf, 256);
  if(bytes_read < 0)
    {
      error("Error with read()");
    }
  if(compress_flag)
    {
      char compress_buf[256];
      send_stream.avail_in = bytes_read;
      send_stream.next_in = (Bytef *)buf;
      send_stream.avail_out = 256;
      send_stream.next_out = (Bytef *)compress_buf;
      do
        {
          inflate(&send_stream, Z_SYNC_FLUSH);
        } while(send_stream.avail_in > 0);
      for(unsigned int i = 0; i < 256 - send_stream.avail_out; i++)
        {
          switch(compress_buf[i])
            {
              case 3: //^C case
                kill(fork_ret, SIGINT);
		break;
              case 4: //^D case
                exit_flag = 1;
                break;
              case 10: //<lf> case
              case 13: //<cr> case
                temp = 10;
                write(pipefd1[1], &temp, sizeof(char));
                break;
              default:
                write(pipefd1[1], &compress_buf[i], sizeof(char));
                break;
            }
        }
    }
  else
    {
      for(int i = 0; i < bytes_read; i++)
        {
          switch(buf[i])
            {
              case 3: //^C case
                kill(fork_ret, SIGINT);
		break;
              case 4: //^D case
                exit_flag = 1;
                break;
              case 10: //<lf> case
              case 13: //<cr> case
                temp = 10;
                write(pipefd1[1], &temp, sizeof(char));
                break;
              default:
                write(pipefd1[1], &buf[i], sizeof(char));
                break;
            }
        }
    }
}

void shell_input()
{
  char buf[256];
  int bytes_read = read(pipefd2[0], buf, 256);
  if(bytes_read < 0)
    {
      error("Error with read()");
    }
  if(compress_flag)
    {
      char compress_buf[256];
      rec_stream.avail_in = bytes_read;
      rec_stream.next_in = (Bytef *)buf;
      rec_stream.avail_out = 256;
      rec_stream.next_out = (Bytef *)compress_buf;
      do
        {
          deflate(&send_stream, Z_SYNC_FLUSH);
        } while(send_stream.avail_in > 0);
      write(new_sockfd, compress_buf, 256 - rec_stream.avail_out);
    }
  else
    {
      write(new_sockfd, buf, 256);
    }
}

void handle_exit()
{
  int status; //holds status of pid;
  ret = waitpid(fork_ret, &status, 0);
  if(ret < 0) //error handling
    {
      error("Error with waitpid()");
    }
  fprintf(stderr, "SHELL EXIT SIGNAL=%d STATUS=%d\n\r", WIFSIGNALED(status), WEXITSTATUS(status)); //program termination
}
