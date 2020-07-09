//NAME: Dhruv Singhania
//EMAIL: singhania_dhruv@yahoo.com
//ID: 105125631

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <mraa.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>
#include <string.h>
#include <signal.h>
#include <poll.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

const int B = 4275; // B value of the thermistor
const int R0 = 100000; // R0 = 100k
int log_fd = 0; //log file descriptor
int num_period = 1; //period length
char scale_val = 'F'; //scale value
char *id_num = ""; //ID number
char *host_name = ""; //host name
int port_num = 0; //port number
int ret; //return value
mraa_result_t res; //result value
time_t rawtime; //time value
struct tm *info; //time struct
mraa_aio_context sensor; //temperature sensor
int run_flag = 1; //run condition
int sock_fd = 0; //socket file descriptor
struct sockaddr_in server_addr; //server address
struct hostent *server; //server
SSL *ssl = NULL; //SSL
SSL_CTX *ssl_context = NULL; //SSL context

void handle_error(const char *msg); //handle all errors
void handle_signal(int sig); //handle all signals
void handle_off(); //handle OFF
void handle_exit(); //handle all exit actions
void handle_sample(); //handle printing each sample
void handle_command(const char *msg); //handle all commands

int main(int argc, char *const argv[]) {
	int c; //getopt_long ret value

	while(1) { //get options
		static struct option long_options[] = {
			{"period", required_argument, 0, 'p'},
			{"scale", required_argument, 0, 's'},
			{"log", required_argument, 0, 'l'},
			{"id", required_argument, 0, 'i'},
			{"host", required_argument, 0, 'h'},
			{0, 0, 0, 0}
		};
		int option_index = 0;
		c = getopt_long(argc, argv, "pslih", long_options, &option_index);
		if(c == -1) {
			break;
		}
		switch(c) {
		case 'p':
			num_period = atoi(optarg);
			if(num_period < 1) {
				handle_error("Period cannot be less than 1");
			}
			break;
		case 's':
			scale_val = optarg[0];
			if(scale_val != 'C' && scale_val != 'F') {
				handle_error("Scale must be 'C' or 'F'");
			}
			break;
		case 'l':
			log_fd = open(optarg, O_WRONLY | O_CREAT, 0666);
			if(log_fd < 0) {
				handle_error("Error with open()");
			}
			break;
		case 'i':
			id_num = optarg;
			if(strlen(id_num) != 9) {
				handle_error("ID must be 9 digits");
			}
			break;
		case 'h':
			host_name = optarg;
			break;
		case '?':
		default:
			handle_error("Usage: lab4c_tcp --id=num --host=addr --log=file [--period=num] [--scale=char] portnum");
			abort();
		}
	}

	if(optind >= argc) { //get port num
		handle_error("Port option is mandatory");
	}
	port_num = atoi(argv[optind]);
	if(port_num < 1) {
		handle_error("Port cannot be less than 1");
	}

	if(log_fd == 0) { //confirm mandatory options
		handle_error("Log option is mandatory");
	}
	if(strlen(id_num) == 0) {
		handle_error("ID option is mandatory");
	}
	if(strlen(host_name) == 0) {
		handle_error("Host option is mandatory");
	}

	sock_fd = socket(AF_INET, SOCK_STREAM, 0); //create and open socket
	if(sock_fd < 0) {
		handle_error("Error with socket()");
	}
	server = gethostbyname(host_name);
	if(server == NULL) {
		handle_error("Error with gethostbyname()");
	}
	bzero((char *)&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, (char *)&server_addr.sin_addr.s_addr, server->h_length);
	server_addr.sin_port = htons(port_num);
	if(connect(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
		handle_error("Error with connect()");
	}

	SSL_library_init(); //initialize SSL
	SSL_load_error_strings();
	OpenSSL_add_all_algorithms();
	ssl_context = SSL_CTX_new(TLSv1_client_method());
	if(ssl_context == NULL) {
		handle_error("Error with SSL_CTX_new()");
	}
	ssl = SSL_new(ssl_context);
	if(ssl == NULL) {
		handle_error("Error with SSL_new()");
    	}
	ret = SSL_set_fd(ssl, sock_fd);
	if(ret == 0) {
		handle_error("Error with SSL_set_fd()");
	}
	ret = SSL_connect(ssl);
	if(ret != 1) {
		handle_error("Error with SSL_connect()");
	}

	char id[15]; //print ID num
	sprintf(id, "ID=%s\n", id_num);
	ret = SSL_write(ssl, id, strlen(id));
	if(ret < 0) {
		handle_error("Error with SSL_write()");
	}
	if(log_fd > 0) {
		dprintf(log_fd, "ID=%s\n", id_num);
	}

	sensor = mraa_aio_init(1); //initialize sensor variable
	if(sensor == NULL) {
		handle_error("Error with mraa_aio_init()");
	}

	atexit(handle_exit); //close variables at exit
	signal(SIGINT, handle_signal); //handle ^C

	struct timeval curr_time, next_time; //time helper variables
	next_time.tv_sec = 0;

	struct pollfd poll_in; //define poll
	poll_in.fd = sock_fd;
	poll_in.events = POLLIN;

	int poll_ret, bytes_read, i, index = 0; //read helper variables
	char buf[256];
	char command[256];
	memset(buf, 0, 256);
	memset(buf, 0, 256);

	while(1) {
		ret = gettimeofday(&curr_time, NULL);
		if(ret < 0) {
			handle_error("Error with gettimeofday()");
		}
		if(curr_time.tv_sec >= next_time.tv_sec) {
			if(run_flag) {
				handle_sample();
				next_time = curr_time;
				next_time.tv_sec += num_period;
			}
		}

		poll_ret = poll(&poll_in, 1, 0);
		if(poll_ret < 0) {
			handle_error("Error with poll()");
		}
		else if(poll_ret) {
			bytes_read = SSL_read(ssl, buf, 256);
			if(bytes_read < 0) {
				handle_error("Error with SSL_read()");
			}
			for(i = 0; i < bytes_read; i++) {
				switch(buf[i]) {
				case '\n':
					command[index] = '\0';
					handle_command(command);
					index = 0;
					//memset(command, 0, 256);
					break;
				default:
					command[index] = buf[i];
					index++;
					break;
				}
			}
		}
	}

	return 0;
}

void handle_error(const char *msg) {
	perror(msg);
	exit(1);
}

void handle_signal(int sig) {
	if(sig == SIGINT) {
		exit(1);
	}
}

void handle_off() {
	rawtime = time(NULL);
	info = localtime(&rawtime);
	char sample[24];
	sprintf(sample, "%.2d:%.2d:%.2d SHUTDOWN\n", info->tm_hour, info->tm_min, info->tm_sec);
	SSL_write(ssl, sample, strlen(sample));
	if(log_fd > 0) {
		dprintf(log_fd, "%.2d:%.2d:%.2d SHUTDOWN\n", info->tm_hour, info->tm_min, info->tm_sec);
	}
	exit(0);
}


void handle_exit() {
	res = mraa_aio_close(sensor); //close variables
	if(res != MRAA_SUCCESS) {
		fprintf(stderr, "Error with mraa_aio_close()");
	}
	SSL_shutdown(ssl);
	SSL_free(ssl);
}

void handle_sample() {
	rawtime = time(NULL);
	info = localtime(&rawtime);
	int a  = mraa_aio_read(sensor); //calculate temperature		
	float R = 1023.0 / a - 1.0;
	R = R0 * R;
	float temp = 1.0 / (log(R / R0) / B + 1 / 298.15) - 273.15;
	if(scale_val == 'F') {
		temp = temp * 1.8 + 32;
	}
	char sample[24];
	sprintf(sample, "%.2d:%.2d:%.2d %0.1lf\n", info->tm_hour, info->tm_min, info->tm_sec, temp);
	SSL_write(ssl, sample, strlen(sample));
	if(log_fd > 0) {
		dprintf(log_fd, "%.2d:%.2d:%.2d %0.1lf\n", info->tm_hour, info->tm_min, info->tm_sec, temp);
	}
}

void handle_command(const char *msg) {
	if(log_fd > 0) {
		dprintf(log_fd, "%s\n", msg);
	}
	if(strcmp(msg, "SCALE=F") == 0) {
		scale_val = 'F';
	}
	else if(strcmp(msg, "SCALE=C") == 0) {
		scale_val = 'C';
	}
	else if(strncmp(msg, "PERIOD=", strlen("PERIOD=")) == 0) {
		num_period = atoi(msg + strlen("PERIOD="));
	}
	else if(strcmp(msg, "STOP") == 0) {
		run_flag = 0;
	}
	else if(strcmp(msg, "START") == 0) {
		run_flag = 1;
	}
	else if(strcmp(msg, "OFF") == 0) {
		handle_off();
	}
}
