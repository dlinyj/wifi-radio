#include <stdio.h>   /* Standard input/output definitions */
#include <stdlib.h>  /* Standard General Utilities Library */
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */

#include "mpc.h"
#include "display.h"
#include "uart.h"

#define _TERMINAL_DEBUG_

#ifdef _TERMINAL_DEBUG_
#include "term.h"
#endif

#ifndef __DEBUG__
#define COM_PORT_NAME "/dev/ttyACM0"
#else
#define COM_PORT_NAME "/dev/null"
#endif //__DEBUG__
#define DISP_COM_SPEED 9600

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



//Этот пиздец надо переписать!!!
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


void set_cirilic (void) {
	send_cmd_to_display("\x1B\x74\x06");
}

void clear_scr() {
#ifdef _TERMINAL_DEBUG_
	term_clear_scr(); //term
#endif
	char *s;
	s = strdup("\x0C"); 
	write(mainfd,s,strlen(s));
	free(s);
}

void home_scr() {
#ifdef _TERMINAL_DEBUG_
	term_set_to_position_scr(1, 1);
#endif
	char *s;
	s = strdup("\x1B[H"); 
	write(mainfd,s,strlen(s));
	free(s);
}

void set_to_position_scr(char col, char str){
#ifdef _TERMINAL_DEBUG_
	term_set_to_position_scr(col, str);
#endif
	static char s[4];
	s[0]='\x1f';
	s[1]='\x24';
	s[2]=col;
	s[3]=str;
	write(mainfd,s,4);
}

void print_to_scr (char * s) {
#ifdef _TERMINAL_DEBUG_
	term_print_to_scr (s); 
#endif
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
	delta_step=(double)99/(double)(playlist_len-1);
	tun_disp_position=(int)(delta_step*(curent_song_pos-1)/5)+1;
	tun_char_position=(int)(delta_step*(curent_song_pos-1)+0.5)%5+1;

#ifdef _TERMINAL_DEBUG_
	set_display_atrib(F_WHITE);
	gotoxy(0,15);
	printf("delta_step=%f \
			\n\rtun_disp_position=%d \
			\n\rtun_char_position=%d", 
			delta_step,tun_disp_position,tun_char_position);
#endif
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
	//sprintf(&ch, "%d",tun_char_position);
	ch = (char)tun_char_position + 0x30;
#ifdef _TERMINAL_DEBUG_
	term_print_to_scr (&ch);
#endif

}
/*Здесь идёт вызов из mpc!*/
void tuning_movement (char left_right) {
	get_cur_position ();
	if (((left_right=='R') || (left_right=='d')) && (playlist_len!=(curent_song_pos))){
		curent_song_pos++;
	}
	if (((left_right=='L') ||(left_right=='a')) && (curent_song_pos!=1)){
		curent_song_pos--;
	}
	print_cur_pos_len (curent_song_pos, playlist_len); //term
	set_play_list_position(curent_song_pos);
	show_current_cursor_pos (); 
}

int init_display () {
/*	
	if (init_display_comport()<0) {
		return -1;
	}
*/
	mainfd = init_comport(COM_PORT_NAME,DISP_COM_SPEED);
	if (mainfd < 0) {
		return -1;
	}
	clear_scr();
	curent_song_pos=get_number_curent_song()+1; //внешний вызов
	return 0;
}
