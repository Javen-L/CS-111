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

const int B = 4275; // B value of the thermistor
const int R0 = 100000; // R0 = 100k
int log_fd = 0; //log file descriptor
int num_period = 1; //period length
char scale_val = 'F'; //scale value
int ret; //return value
mraa_result_t res; //result value
time_t rawtime; //time value
struct tm *info; //time struct
mraa_aio_context sensor; //temperature sensor
mraa_gpio_context button; //button
int run_flag = 1; //run condition

void handle_error(const char *msg); //handle all errors
void handle_signal(int sig); //handle all signals
void handle_button(); //handle when button is pressed
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
			{0, 0, 0, 0}
		};
		int option_index = 0;
		c = getopt_long(argc, argv, "psl", long_options, &option_index);
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
		case '?':
		default:
			abort();
		}
	}
	
	sensor = mraa_aio_init(1); //initialize sensor variables
	if(sensor == NULL) {
		handle_error("Error with mraa_aio_init()");
	}
	button = mraa_gpio_init(60);
	if(button == NULL) { 
		handle_error("Error with mraa_gpio_init()");
	}

	atexit(handle_exit); //close variables at exit

	res = mraa_gpio_dir(button, MRAA_GPIO_IN); //set direction of data flow
	if(res != MRAA_SUCCESS) {
		handle_error("Error with mraa_dir()");
	}

	mraa_gpio_isr(button, MRAA_GPIO_EDGE_RISING, &handle_button, NULL); //handle button press
	signal(SIGINT, handle_signal);

	struct timeval curr_time, next_time; //time helper variables
	next_time.tv_sec = 0;

	struct pollfd poll_in; //define poll
	poll_in.fd = STDIN_FILENO;
	poll_in.events = POLLIN;

	int bytes_read, i, index = 0; //read helper variables
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

		ret = poll(&poll_in, 1, 0);
		if(ret < 0) {
			handle_error("Error with poll()");
		}
		else if(ret) {
			bytes_read = read(STDIN_FILENO, buf, 256);
			if(bytes_read < 0) {
				handle_error("Error with read()");
			}
			for(i = 0; i < bytes_read; i++) {
				switch(buf[i]) {
				case '\n':
					command[index] = '\0';
					handle_command(command);
					index = 0;
					memset(command, 0, 256);
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

void handle_button() {
	rawtime = time(NULL);
	info = localtime(&rawtime);
	fprintf(stdout, "%.2d:%.2d:%.2d SHUTDOWN\n", info->tm_hour, info->tm_min, info->tm_sec);
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
	res = mraa_gpio_close(button);
	if(res != MRAA_SUCCESS) {
		fprintf(stderr, "Error with mraa_gpio_close()");
	}
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
	fprintf(stdout, "%.2d:%.2d:%.2d %0.1lf\n", info->tm_hour, info->tm_min, info->tm_sec, temp);
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
		handle_button();
	}
}
