#include <stdio.h>   /* Standard input/output definitions */
#include <stdlib.h>  /* Standard General Utilities Library */
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */
#include <signal.h> 
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

#include <sys/poll.h>

#include <sys/types.h>
#include <sys/stat.h>

#define ENCODER_COM_PORT "/dev/ttyUSB0"
//#define ENCODER_COM_PORT "/dev/tty"



static int encoder_comport_fd =0;

int init_encoder_comport(void) {
	//int encoder_comport_fd=0;										/* File descriptor */

	struct termios options;
	encoder_comport_fd = open(ENCODER_COM_PORT, O_RDWR | O_NOCTTY | O_NDELAY);
	if (encoder_comport_fd == -1)
	{														/* Could not open the port */
		fprintf(stderr, "open_port: Unable to open %s - %s\n", ENCODER_COM_PORT,
		strerror(errno));
		exit (-1);
	}

	/* Configure port reading */
	//fcntl(encoder_comport_fd, F_SETFL, 0); 	//read com-port is the bloking
	fcntl(encoder_comport_fd, F_SETFL, FNDELAY);  //read com-port not bloking
	
/* Get the current options for the port */
	tcgetattr(encoder_comport_fd, &options);
	cfsetispeed(&options, B9600);						/* Set the baud rates to 9600 */
	 
/* Enable the receiver and set local mode */
	options.c_cflag |= (CLOCAL | CREAD);
	options.c_cflag &= ~PARENB; 						/* Mask the character size to 8 bits, no parity */
	options.c_cflag &= ~CSTOPB;
	options.c_cflag &= ~CSIZE;
	options.c_cflag |=  CS8;							/* Select 8 data bits */
	options.c_cflag &= ~CRTSCTS;						/* Disable hardware flow control */  
 
/* Enable data to be processed as raw input */
	//options.c_lflag &= ~(ICANON | ECHO | ISIG);
	options.c_lflag &= ~(ICANON | ECHO);
/* Set the new options for the port */
	tcsetattr(encoder_comport_fd, TCSANOW, &options);
	return encoder_comport_fd;
}

char encoder_read(void) {
	char chout;
/*
	ssize_t symbols;
	symbols=read(encoder_comport_fd, &chout, 1);
	if (symbols>0) {
		return chout;
	} else {
		return '\0';
	}
*/
	struct pollfd fds;
	fds.fd=encoder_comport_fd;
	fds.events = POLLIN;
	poll(&fds, 1, 500);
	if(fds.revents & POLLIN) {
		read(encoder_comport_fd, &chout, 1);
		return chout;
	}
	return '\0'; //timeout
}
