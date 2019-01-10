// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#include <stdio.h>   /* Standard input/output definitions */
#include <stdlib.h>  /* Standard General Utilities Library */
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h>


#include "mpc.h"
#include "display.h"
#include "uart.h"
#include "output.h"

#define _TERMINAL_DEBUG_
//#define __DEBUG__

#ifdef _TERMINAL_DEBUG_
#include "term.h"
#endif

#define COM_PORT_NAME 			"/dev/ttyACM0"
#define DEFINE_COM_PORT_NAME 	"/dev/null"


#define DISP_COM_SPEED 9600
#define WRITE_TIMEOUT 1000

static int mainfd=0;
output_t * output_st = NULL;

static int curent_song_pos = 1;

void send_cmd_to_display (char * s) {
	int i=0;
	while (s[i]!='\0') {
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
		i++;
	}
}

void set_overwrite_mode (void) {
	send_cmd_to_display("\x1F\x03");
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
	write_com(mainfd, s, strlen(s), WRITE_TIMEOUT);
	free(s);
}

void home_scr() {
#ifdef _TERMINAL_DEBUG_
	term_set_to_position_scr(1, 1);
#endif
	char *s;
	s = strdup("\x1B[H"); 
	write_com(mainfd, s, strlen(s), WRITE_TIMEOUT);
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
	write_com(mainfd, s, 4, WRITE_TIMEOUT);
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
			case 1: send_cmd_to_display ("\x1B\x26\xFD\x21\x84\x10\x42\x80"); //Загрузка шрифта символ первой палочки OK 
					break;
			case 2: send_cmd_to_display ("\x1B\x26\xFD\x42\x08\x21\x84\x80"); //Загрузка шрифта символ 2 палочки OK
					break;
			case 3: send_cmd_to_display ("\x1B\x26\xFD\x84\x10\x42\x08\x81"); //Загрузка шрифта символ 3 палочки OK
					break;
			case 4: send_cmd_to_display ("\x1B\x26\xFD\x08\x21\x84\x10\x82"); //Загрузка шрифта символ 4 палочки OK
					break;
			case 5: send_cmd_to_display ("\x1B\x26\xFD\x10\x42\x08\x21\x84"); //Загрузка шрифта символ 5 палочки OK
			default:      ;
		
		}
}

void move_symb_left () {
	if ((output_st->tun_disp_position>1) || (output_st->tun_char_position>1)) {
		output_st->tun_char_position--;
		if (output_st->tun_char_position<1) {
			output_st->tun_char_position=5;
			output_st->tun_disp_position--;
			send_cmd_to_display ("\x08  \x08\x08"); //Move cursor left and space
			}
		reload_char(output_st->tun_char_position);
		send_cmd_to_display ("\xFD\x08"); //Печааем палку и возвращаем курсор взад
	}
}

void move_symb_right () {
	if ((output_st->tun_disp_position<20) || (output_st->tun_char_position<5)) {
		output_st->tun_char_position++;
		if (output_st->tun_char_position>5) {
			output_st->tun_char_position=1;
			output_st->tun_disp_position++;
			send_cmd_to_display ("\x08  "); //Move cursor left and space
			}
		reload_char(output_st->tun_char_position);
		send_cmd_to_display ("\xFD\x08"); //Печааем палку и возвращаем курсор взад
	}
}

void get_cur_position () {
/*
	double delta_step;
	playlist_len=get_playlistlength(); //внешний вызов
	//curent_song_pos=get_number_curent_song(); //внешний вызов
	delta_step=(double)99/(double)(playlist_len-1);
	tun_disp_position=(int)(delta_step*(curent_song_pos-1)/5)+1;
	tun_char_position=(int)(delta_step*(curent_song_pos-1)+0.5)%5+1;
*/
/*
	output_st->delta_step = (double)99/(double)(output_st->playlistlength-1);
	output_st->tun_disp_position=(int)(output_st->delta_step*(output_st->currentsong)/5)+1;
	output_st->tun_char_position=(int)(output_st->delta_step*(output_st->currentsong)+0.5)%5+1;
*/

	output_st->delta_step = (double)99/(double)(output_st->playlistlength-1);
	output_st->tun_disp_position=(int)(output_st->delta_step*(curent_song_pos)/5)+1;
	output_st->tun_char_position=(int)(output_st->delta_step*(curent_song_pos)+0.5)%5+1;

#ifdef _TERMINAL_DEBUG_
	set_display_atrib(F_WHITE);
	gotoxy(0,15);
	printf("delta_step=%f \
			\n\rtun_disp_position=%d \
			\n\rtun_char_position=%d", 
			output_st->delta_step,output_st->tun_disp_position,output_st->tun_char_position);
	fflush(stdout);
#endif
}

void show_current_cursor_pos () {
	clear_scr();
	//home_scr();
	send_cmd_to_display ("\x1B\x25\x01"); //Разрешение юзверских шрифтов
	set_to_position_scr(8, 2);
	print_to_scr ("SEARCH"); //Поиск
	if (output_st->tun_disp_position !=10) {
		set_to_position_scr(output_st->tun_disp_position, 1); //устанавливаем курсор в текущую позицию
	} else {
		set_to_position_scr(9, 1);
		print_to_scr (" ");
	}

	reload_char(output_st->tun_char_position); //загружаем символ
	send_cmd_to_display ("\xFD\x08"); //Печааем палку и возвращаем курсор взад
#ifdef _TERMINAL_DEBUG_
	char ch;
	ch = (char)output_st->tun_char_position + 0x30;
	term_print_to_scr (&ch);
#endif

}
//Какая-то лажня с вычислением позиций в крайнем правом положении
int tuning_movement (char left_right) {
	//int curent_song_pos = output_st->currentsong;
/*
	if (((left_right=='R') || (left_right=='d')) && (output_st->playlistlength!=(output_st->currentsong+1))){
		curent_song_pos++;
	}
	if (((left_right=='L') ||(left_right=='a')) && (output_st->currentsong!=0)){
		//curent_song_pos--;
		curent_song_pos-=2;
	}
*/
	if (((left_right=='R') || (left_right=='d')) && (output_st->playlistlength!=(curent_song_pos+1))){
		curent_song_pos++;
	}
	//left
	if (((left_right=='L') ||(left_right=='a')) && (curent_song_pos!=0)){
		curent_song_pos--;
	}
#ifdef _TERMINAL_DEBUG_
	print_cur_pos_len (curent_song_pos+1, output_st->playlistlength); //term
	fflush(stdout);
#endif
	return curent_song_pos;
}

int init_display (output_t * output_st_ext) {
	if (cfileexists(COM_PORT_NAME)) {
		mainfd = init_comport(COM_PORT_NAME,DISP_COM_SPEED);
		if (mainfd < 0) {
			return -1;
		}
	} else {
		mainfd = init_comport(DEFINE_COM_PORT_NAME,DISP_COM_SPEED);
	}
	output_st = output_st_ext;
	clear_scr();
	//curent_song_pos=get_number_curent_song()+1; //внешний вызов
	return 0;
}
