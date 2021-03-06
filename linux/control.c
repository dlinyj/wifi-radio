#include <stdio.h>   /* Standard input/output definitions */
#include <stdlib.h>  /* Standard General Utilities Library */
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <errno.h>   /* Error number definitions */
#include <time.h>
#include <sys/time.h> // for clock_gettime()

#include "mpc.h"
#include "display.h"
#include "uart.h"
#include "term.h"

#include "output.h"



#define CLRSCR() fputs("\033[H\033[J", stdout);
//#define home() 			fputs(ESC "[H", stdout) //Move cursor to the indicated row, column (origin at 1,1)
#define BUFF_LEN 1024

#define ENCODER_COM_PORT			"/dev/ttyACM1"
//#define ENCODER_COM_PORT 			"/dev/ttyUSB0"
#define DEFAULT_ENCODER_COM_PORT 	"/dev/tty"

#define ENCODER_COM_SPEED 9600

void show_current_time_and_date (void) {
	time_t t ; 
	struct tm *tmp ; 

	char show [21];
	char tmp_str[21];

	show [20]='\0';
	

	time( &t ); 
  	tmp = localtime( &t );
	
	memset (show,' ',20);
	strftime(tmp_str, sizeof(tmp_str), "%d.%m.%Y", tmp); 
	strncpy (&show[(20 - strlen(tmp_str))/2], tmp_str, strlen(tmp_str));
	home_scr();
	print_to_scr (show);

	memset (show,' ',20);
	strftime(tmp_str, sizeof(tmp_str), "%H:%M:%S", tmp); 
	strncpy (&show[(20 - strlen(tmp_str))/2], tmp_str, strlen(tmp_str));
	set_to_position_scr(1, 2);
	print_to_scr (show);
}

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
		char spacestr []=  "                    ";
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
			if (strlen(output_st->artist) != 0) {
				cmp_buf_name = output_st->artist;
			} else {
				cmp_buf_name = spacestr;
			}
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

unsigned int usec_used(struct timeval * t_start, struct timeval * t_end) {
	return (((t_end->tv_sec - t_start->tv_sec) * 1000000) + t_end->tv_usec) - (t_start->tv_usec);
}

int main() {
	char chout='\n';
	int action=1;
	time_t last_time_action, current_time, last_time;
	
	if ( init_mpc () == 0) {
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
	
	int encoder_fd = 0;
	if (cfileexists(ENCODER_COM_PORT)) {
		encoder_fd = init_comport(ENCODER_COM_PORT, ENCODER_COM_SPEED);
	} else {
		encoder_fd = init_comport(DEFAULT_ENCODER_COM_PORT, ENCODER_COM_SPEED);
	}

	init_term();
	last_time = time(NULL); 

	int count_click = 0;
	struct timeval t_start, t_end;
	int show_time = 0;
	while (1) {
		get_all (output_st);
		int ret_read = read_com(encoder_fd, 1 , 100, &chout);
		if ((chout=='R') || (chout=='L') ||(chout=='a') || (chout=='d')) {
				action=0;
				last_time_action = time(NULL);
				set_play_list_position(tuning_movement(chout)); 
				get_cur_position (); 
				show_current_cursor_pos ();
		}
		if ((chout=='P') ||(chout=='s') ) {
			gettimeofday(&t_end, NULL);
			if(!count_click) {
				t_start = t_end;
				count_click++;
			} else {
				if (usec_used (&t_start, &t_end) <= 500000) {
					double_click_button();
					if (show_time) {
						show_time--;
					} else {
						show_time++;
					}
				} 
				count_click = 0;
			}
		}

		if (count_click) {
			gettimeofday(&t_end, NULL);
			if (usec_used (&t_start, &t_end) > 500000) 	{
				print_button_pressed();
				music_pause();
				count_click = 0;
			}
		}

		chout = '\0';
		current_time = time(NULL); 
		if (action==0) {
			if ((current_time-last_time_action)>3) {
				action++;
			}
		}
		if ((action) && ((current_time-last_time)>0)) {
			if (!show_time) {
				show_current_track(output_st);
			} else {
				show_current_time_and_date();
			}
			last_time = current_time; 
		}
	}
}
