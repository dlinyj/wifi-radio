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

#include "mpc.h"
#include "display.h"
#include "encoder.h"
#include "control.h"

int main() {
	if (init_display()<0) 
		return -1;
	clear_scr();
	tunning () ;
	return 0;
}
