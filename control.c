#include <stdio.h>   /* Standard input/output definitions */
#include <stdlib.h>  /* Standard General Utilities Library */
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <errno.h>   /* Error number definitions */
#include <time.h>

#include "mpc.h"
#include "display.h"
#include "uart.h"
#include "term.h"

#include "output.h"



#define CLRSCR() fputs("\033[H\033[J", stdout);
//#define home() 			fputs(ESC "[H", stdout) //Move cursor to the indicated row, column (origin at 1,1)
#define BUFF_LEN 1024

//#define ENCODER_COM_PORT "/dev/ttyUSB0"
#define ENCODER_COM_PORT "/dev/tty"
//#define ENCODER_COM_PORT "/dev/null"
#define ENCODER_COM_SPEED 9600


void show_current_track(output_t * output_st) {
		/*
		get_title (char * buf, int len)//название для всех
		get_name (char * buf, int len) //Это для радиостанций (название радио)
		get_artist (char * buf, int len)  //Это для артистов
		*/
		
		//У дисплея есть режим Specify horizontal scroll mode - разобраться. <1F><03>

		//char cmp_buf_title[BUFF_LEN]; 
		//char cmp_buf_title1[BUFF_LEN]; 
		char * cmp_buf_title; 
		static char curent_buf_title[BUFF_LEN];
		
		static int curent_title_position;
		static int strlen_title;
		
		
		//char cmp_buf_name[BUFF_LEN];
		char * cmp_buf_name;
		static char curent_buf_name[BUFF_LEN];
		static int curent_name_position;
		static int strlen_name;
		
		char show [21];
		show [20]='\0';

		//get_title (cmp_buf_title1);
		cmp_buf_title = output_st->title;
		if (strcmp(cmp_buf_title,curent_buf_title) != 0) {
			strcpy(curent_buf_title, cmp_buf_title);
			curent_title_position=0;
			strlen_title=strlen(curent_buf_title);
		}

		//get_name (cmp_buf_name);
		cmp_buf_name = output_st->name;
		if (strlen(cmp_buf_name) == 0) {
			//get_artist(cmp_buf_name);
			cmp_buf_name = output_st->artist;
		}
		
		if (strcmp(cmp_buf_name,curent_buf_name) != 0) {
			strcpy(curent_buf_name, cmp_buf_name);
			curent_name_position=0;
			strlen_name=strlen(curent_buf_name);
		}

	//Show title
		memset (show,' ',20);
		if (((curent_title_position==0) && (strlen_title<=20))) {
//Delete line
			home_scr();
			print_to_scr (show);
//
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
//Delete line
			set_to_position_scr(1, 2);
			print_to_scr (show);
//
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
}

int main() {
	char chout='\n';
	int action=1;
	time_t last_time_action, current_time, last_time;
	
	if (init_mpc () == 0) {
		printf("Error init mpc\n");
		exit (-1);
	}

	output_t * output_st = init_output_st ();

	while (init_display(output_st)) {
		printf("Error display\n");
		sleep (1);
	}

	set_cirilic ();
	clear_scr();


	int encoder_fd = init_comport(ENCODER_COM_PORT, ENCODER_COM_SPEED);
	init_term();
	last_time = time(NULL); 
	while (1) {
		get_all (output_st);
		read_com(encoder_fd, 1 , 100, &chout);
		if ((chout=='R') || (chout=='L') ||(chout=='a') || (chout=='d')) {
				action=0;
				last_time_action = time(NULL);
				get_cur_position (); 
				//Тут проблема!!!
				set_play_list_position(tuning_movement(chout)); 
				show_current_cursor_pos ();
		}
		if ((chout=='P') ||(chout=='s') ) {
				print_button_pressed();
				music_pause();
		}
		chout = '\0';
		current_time = time(NULL); 
		if (action==0) {
			if ((current_time-last_time_action)>3) {
				action++;
			}
		}
		if ((action) && ((current_time-last_time)>0)) {
			show_current_track(output_st);
			last_time = current_time; 
		}
	}

}




