#include <stdio.h>
#include <unistd.h>
#include <errno.h>

#include "mpc.h"
#include "display.h"
#include "control.h"

int main() {

/*	if (init_display()<0) 
		return -1;
*/
	while (init_display()) {
		printf("Error display\n");
		sleep (1);
	}
	set_cirilic ();
	clear_scr();
	tunning () ;
	return 0;
}
