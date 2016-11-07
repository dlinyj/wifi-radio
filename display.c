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
#include "term.h"


#ifndef __DEBUG__
#define COM_PORT_NAME "/dev/ttyACM0"
#else
#define COM_PORT_NAME "/dev/null"
#endif //__DEBUG__

#define STR_BUFFLEN 128

static int mainfd=0;				/* File descriptor of COM-port*/
static volatile int tun_disp_position=1; //Позиция символа настройки на экране
static volatile int tun_char_position=1; //Текущий символ настройки

static int playlist_len;			//Длинна плей листа (получается из mpc)
static int curent_song_pos;			//текущая позикия в плей листе (получается из mpc)

 /*
 * Напоминание 
 * tun_disp_position Позиция символа настройки на экране
 * tun_char_position Текущий символ настройки
 * Вычисление позиции станции идёт по следующей формуле
 * sation_pos=(tun_disp_position-1)*5+tun_char_position
 * */
 
int init_display_comport(void){
	struct termios options;
	mainfd = open(COM_PORT_NAME, O_RDWR | O_NOCTTY | O_NDELAY);
	if (mainfd == -1)
	{                                                       /* Could not open the port */
		fprintf(stderr, "open_port: Unable to open" COM_PORT_NAME "- %s\n",
		strerror(errno));
		exit (-1);
	}
 	/* Configure port reading */
	//fcntl(mainfd, F_SETFL, FNDELAY);  //read com-port not bloking
	fcntl(mainfd, F_SETFL, 0);  //read com-port is the bloking
/* Get the current options for the port */
	tcgetattr(mainfd, &options);
	cfsetispeed(&options, B9600);                     /* Set the baud rates to 9600 */
	//cfsetospeed(&options, B115200);
/* Enable the receiver and set local mode */
	options.c_cflag |= (CLOCAL | CREAD);
	options.c_cflag &= ~PARENB;                         /* Mask the character size to 8 bits, no parity */
	options.c_cflag &= ~CSTOPB;
	options.c_cflag &= ~CSIZE;
	options.c_cflag |=  CS8;                            /* Select 8 data bits */
	options.c_cflag &= ~CRTSCTS;                        /* Disable hardware flow control */ 
  
/* Enable data to be processed as raw input */
	options.c_lflag &= ~(ICANON | ECHO | ISIG);
/* Set the new options for the port */
	tcsetattr(mainfd, TCSANOW, &options);
	return mainfd;
}

void send_cmd_to_display (char * s) {
	int i;
	for (i=0;i<65535;i++) {
		if (s[i]!='\0') {
			write(mainfd,&s[i],1);
			/*
			if(s[i]==',') {
				write(mainfd,"\x1F,",2);
				continue;
			}
			if(s[i]=='.') {
				write(mainfd,"\x1F.",2);
				continue;
			}
			*/

		} else break;
	}
}

void clear_scr() {
	term_clear_scr(); //term
	char *s;
	s = strdup("\x0C"); 
	write(mainfd,s,strlen(s));
	free(s);
}

void home_scr() {
// *****************
	term_set_to_position_scr(1, 1);
// *****************
	char *s;
	s = strdup("\x1B[H"); 
	write(mainfd,s,strlen(s));
	free(s);
}

void set_to_position_scr(char col, char str){
	term_set_to_position_scr(col, str);
	static char s[4];
	s[0]='\x1f';
	s[1]='\x24';
	s[2]=col;
	s[3]=str;
	write(mainfd,s,4);
}

void print_to_scr (char * s) {
//terminal
	term_print_to_scr (s); 
	send_cmd_to_display (s); 

}

void reload_char(int i) {
	int j; //костыль от перезагрузки шрифта
	for(j=0;j<5;j++)
		switch (i)
		{
			case 1: send_cmd_to_display ("\x1B\x26\xA0\x21\x84\x10\x42\x80"); //Загрузка шрифта символ первой палочки OK 
					break;
			case 2: send_cmd_to_display ("\x1B\x26\xA0\x42\x08\x21\x84\x80"); //Загрузка шрифта символ 2 палочки OK
					break;
			case 3: send_cmd_to_display ("\x1B\x26\xA0\x84\x10\x42\x08\x81"); //Загрузка шрифта символ 3 палочки OK
					break;
			case 4: send_cmd_to_display ("\x1B\x26\xA0\x08\x21\x84\x10\x82"); //Загрузка шрифта символ 4 палочки OK
					break;
			case 5: send_cmd_to_display ("\x1B\x26\xA0\x10\x42\x08\x21\x84"); //Загрузка шрифта символ 5 палочки OK
			default:      ;
		
		}
}

void move_symb_left () {
	if ((tun_disp_position>1) || (tun_char_position>1)) {
		tun_char_position--;
		if (tun_char_position<1) {
			tun_char_position=5;
			tun_disp_position--;
			send_cmd_to_display ("\x08  \x08\x08"); //Move cursor left and space
			}
		reload_char(tun_char_position);
		send_cmd_to_display ("\xA0\x08"); //Печааем палку и возвращаем курсор взад
	}
}

void move_symb_right () {
	if ((tun_disp_position<20) || (tun_char_position<5)) {
		tun_char_position++;
		if (tun_char_position>5) {
			tun_char_position=1;
			tun_disp_position++;
			send_cmd_to_display ("\x08  "); //Move cursor left and space
			}
		reload_char(tun_char_position);
		send_cmd_to_display ("\xA0\x08"); //Печааем палку и возвращаем курсор взад
	}
}

/*Здесь идёт вызов из mpc!*/
void get_cur_position () {
	double delta_step;
	playlist_len=get_playlistlength(); //внешний вызов
	//curent_song_pos=get_number_curent_song(); //внешний вызов
/*
	delta_step=(double)100/(double)(playlist_len);
	tun_disp_position=(int)(delta_step*curent_song_pos/5)+1;
	tun_char_position=(int)(delta_step*curent_song_pos)%5+1;
*/
	delta_step=(double)99/(double)(playlist_len-1);
	tun_disp_position=(int)(delta_step*(curent_song_pos-1)/5)+1;
	tun_char_position=(int)(delta_step*(curent_song_pos-1)+0.5)%5+1;
	//terminal
	set_display_atrib(F_WHITE);
	gotoxy(0,15);
	printf("delta_step=%f \
			\ntun_disp_position=%d \
			\ntun_char_position=%d", 
			delta_step,tun_disp_position,tun_char_position);
}

void show_current_cursor_pos () {
	clear_scr();
	
	//home_scr();
	send_cmd_to_display ("\x1B\x25\x01"); //Разрешение юзверских шрифтов
	set_to_position_scr(8, 2);
	print_to_scr ("SEARCH"); //Поиск


	if (tun_disp_position !=10) {
		set_to_position_scr(tun_disp_position, 1); //устанавливаем курсор в текущую позицию
	} else {
		set_to_position_scr(9, 1);
		print_to_scr (" ");
	}

	reload_char(tun_char_position); //загружаем символ
	send_cmd_to_display ("\xA0\x08"); //Печааем палку и возвращаем курсор взад
	char ch;
	sprintf(&ch, "%d",tun_char_position);
	term_print_to_scr (&ch);

}
/*Здесь идёт вызов из mpc!*/
void tuning_movement (char left_right) {
	get_cur_position ();
	//right
/*
 * //Старая версия, для получеия позиции
	if (((left_right=='R') || (left_right=='d')) && (playlist_len!=(curent_song_pos+1))){
		printf("Right curent_song_pos=%d +2\n",curent_song_pos);
		set_play_list_position(curent_song_pos+2);
	}
	//left
	if (((left_right=='L') ||(left_right=='a')) && (curent_song_pos!=0)){
		printf("Left curent_song_pos=%d\n",curent_song_pos);
		set_play_list_position(curent_song_pos);
	}
*/
//новая версия
	if (((left_right=='R') || (left_right=='d')) && (playlist_len!=(curent_song_pos))){
		curent_song_pos++;
	}
	//left
	if (((left_right=='L') ||(left_right=='a')) && (curent_song_pos!=1)){
		curent_song_pos--;
	}
//****//	
	print_cur_pos_len (curent_song_pos, playlist_len); //term
	set_play_list_position(curent_song_pos);
//конец новой версии	
	show_current_cursor_pos (); 
}

int init_display () {
	if (init_display_comport()<0) {
		return -1;
	}
	clear_scr();
	curent_song_pos=get_number_curent_song()+1; //внешний вызов
	return 0;
}
