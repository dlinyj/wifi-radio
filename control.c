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

#define CLRSCR() printf("\033[H\033[J");
#define BUFF_LEN 256


void show_current_track() {
		/*
		get_title (char * buf, int len)//название для всех
		get_name (char * buf, int len) //Это для радиостанций (название радио)
		get_artist (char * buf, int len)  //Это для артистов
		*/

		/*
		char cmp_buf_title[BUFF_LEN]; 
		static char curent_buf_title[BUFF_LEN];
		static int curent_title_position;
		static int strlen_title;
		
		
		char cmp_buf_name[BUFF_LEN];
		static char curent_buf_name[BUFF_LEN];
		static int curent_name_position;
		static int strlen_name;
		
		char show [21];
		show [20]='\0';
		
		get_title (cmp_buf_title, BUFF_LEN);
		if (strcmp(cmp_buf_title,curent_buf_title) != 0) {
			strcpy(curent_buf_title, cmp_buf_title);
			curent_title_position=0;
			strlen_title=strlen(curent_buf_title);
		}

		get_name (cmp_buf_name, BUFF_LEN);
		if (strlen(cmp_buf_name)==0)
			get_artist(cmp_buf_name, BUFF_LEN);

		if (strcmp(cmp_buf_name,curent_buf_name) != 0) {
			strcpy(curent_buf_name, cmp_buf_name);
			curent_name_position=0;
			strlen_name=strlen(curent_buf_name);
		}

	//Show title
		memset (show,' ',20);
		if (((curent_title_position==0) && (strlen_title<=20))) {
			home_scr();
			print_to_scr (show);
			strncpy (show, curent_buf_title, 20);
			curent_title_position++;
			home_scr();
			print_to_scr (show);
		}
		if (strlen_title>20) {
			if ((strlen_title-curent_title_position)>=20) {
				strncpy (show, &curent_buf_title[curent_title_position], 20);
			} else strncpy (show, &curent_buf_title[curent_title_position], (strlen_title-curent_title_position));
			curent_title_position++;
			if (curent_title_position==strlen_title)
				curent_title_position=0;
			home_scr();
			print_to_scr (show);
		}
		
	//Show name
		memset (show,' ',20);
		if (((curent_name_position==0) && (strlen_name<=20)) ) {
			set_to_position_scr(1, 2);
			print_to_scr (show);
			strncpy (show, curent_buf_name, 20);
			curent_name_position++;
			set_to_position_scr(1, 2);
			print_to_scr (show);
		}
		if (strlen_name>20) {
			if ((strlen_name-curent_name_position)>=20) {
				strncpy (show, &curent_buf_name[curent_name_position], 20);
			} else strncpy (show, &curent_buf_name[curent_name_position], (strlen_name-curent_name_position));
			curent_name_position++;
			if (curent_name_position==strlen_name)
				curent_name_position=0;
			set_to_position_scr(1, 2);
			print_to_scr (show);
		}
		usleep (500000);
		*/
		int search_status=0;
		
		char cmp_buf_title[BUFF_LEN]; 
		static char curent_buf_title[BUFF_LEN];
		static int curent_title_position;
		static int strlen_title;
		
		
		char cmp_buf_name[BUFF_LEN];
		static char curent_buf_name[BUFF_LEN];
		static int curent_name_position;
		static int strlen_name;
		
		char show [21];
		show [20]='\0';
		
		get_title (cmp_buf_title, BUFF_LEN);
		if (strcmp(cmp_buf_title,curent_buf_title) != 0) {
			strcpy(curent_buf_title, cmp_buf_title);
			curent_title_position=0;
			strlen_title=strlen(curent_buf_title);
		}

		get_name (cmp_buf_name, BUFF_LEN);
		if (strlen(cmp_buf_name)==0)
			get_artist(cmp_buf_name, BUFF_LEN);

		if (strcmp(cmp_buf_name,curent_buf_name) != 0) {
			strcpy(curent_buf_name, cmp_buf_name);
			curent_name_position=0;
			strlen_name=strlen(curent_buf_name);
		}

	//Show title
		memset (show,' ',20);
		if (((curent_title_position==0) && (strlen_title<=20)) || (search_status==1)) {
			home_scr();
			print_to_scr (show);
			strncpy (show, curent_buf_title, 20);
			curent_title_position++;
			home_scr();
			print_to_scr (show);
		}
		if (strlen_title>20) {
			if ((strlen_title-curent_title_position)>=20) {
				strncpy (show, &curent_buf_title[curent_title_position], 20);
			} else strncpy (show, &curent_buf_title[curent_title_position], (strlen_title-curent_title_position));
			curent_title_position++;
			if (curent_title_position==strlen_title)
				curent_title_position=0;
			home_scr();
			print_to_scr (show);
		}
		
	//Show name
		memset (show,' ',20);
		if (((curent_name_position==0) && (strlen_name<=20)) || (search_status==1)) {
			set_to_position_scr(1, 2);
			print_to_scr (show);
			strncpy (show, curent_buf_name, 20);
			curent_name_position++;
			set_to_position_scr(1, 2);
			print_to_scr (show);
		}
		if (strlen_name>20) {
			if ((strlen_name-curent_name_position)>=20) {
				strncpy (show, &curent_buf_name[curent_name_position], 20);
			} else strncpy (show, &curent_buf_name[curent_name_position], (strlen_name-curent_name_position));
			curent_name_position++;
			if (curent_name_position==strlen_name)
				curent_name_position=0;
			set_to_position_scr(1, 2);
			print_to_scr (show);
		}
	usleep(500000);

}

void tunning () {
	char chout;
	int action=1;
	time_t last_time_action, current_time;
	init_encoder_comport();
	while (1) {
		chout=encoder_read();
		printf("%c", chout);
		if ((chout=='R') || (chout=='L')) {
				action=0;
				last_time_action = time(NULL);
				get_cur_position ();
				tuning_movement(chout);
				show_current_cursor_pos (); //не пашет это место
		}
		if (chout=='P') {
				//CLRSCR();
				printf("Button pressed\n");
				MUSIC_PAUSE();
		}
		if (action==0) {
			current_time = time(NULL); 
			if ((current_time-last_time_action)>3) {
				action++;
			}
		}
		if (action) {
			show_current_track();
		}
	}

}

int main() {
	if (init_display()<0) 
		return -1;
	clear_scr();
	tunning () ;

	//for (;;) show_current_track();
	//pause();
	
	//print_to_scr ("\x1B\x25\x01"); //Разрешение юзверских шрифтов
	//print_to_scr ("\x1B\x25\x00"); //Запрещение юзверских шрифтов
	//print_to_scr ("Abc\xA0\xA1\xA2\xA3\xA4\x20\x21\x22\x23\x24\x25\x26\x27"); //Печать юзверских шрифтов
	//print_to_scr ("\x1F\x55"); //Display Counter
	return 0;
}


