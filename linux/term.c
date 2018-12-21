#include <stdio.h>   /* Standard input/output definitions */
#include <stdlib.h>  /* Standard General Utilities Library */
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <termios.h> 

#include "term.h"
#include "charset_coverter.h"
/*
1. Curent position + playlist len
2-4 - mpc
5. Curent title
6. Curent name 
7. Curent artist
8. Buton pressed
********************** 9
*                    * 10
*                    * 11
********************** 12
*12345678901234567890* 13
6-10 display_emulator
┌────────────────────┐
│                    │
└────────────────────┘
*/
static int x=0,y=0;
static struct termios oldt;


void print_cur_pos_len (int position, int len) {
	home();
	eraceline();
	set_display_atrib(F_CYAN);
	printf("Curent position = %d playlist len = %d\n", position,len);
	set_display_atrib(F_BLUE);
	fflush(stdout);
}

void print_button_pressed () {
	static int button = 0;
	gotoxy(0,8);
	set_display_atrib(F_RED);
	if (!button) {
		fputs("Button pressed             ", stdout);
		button++;
	} else {
		fputs("Button unpressed           ", stdout);
		button--;
	}
	fflush(stdout);
}

void double_click_button () {
	static int button = 0;
	gotoxy(0,8);
	set_display_atrib(F_RED);
	if (!button) {
		fputs("Double click press            ", stdout);
		button++;
	} else {
		fputs("Double click unpressed        ", stdout);
		button--;
	}
	fflush(stdout);
}



void print_display_table () {
	gotoxy(0,9);
	set_display_atrib(F_YELLOW);
	/*
	fputs("*12345678901234567890*\n"
			"┌────────────────────┐\n"
			"│                    │\n"
			"│                    │\n"
			"└────────────────────┘\n"
			"*12345678901234567890*\n", stdout);
	*/
		puts("*12345678901234567890*");
		gotoxy(0,10);
		puts("┌────────────────────┐");
		gotoxy(0,11);
		puts("│");
		gotoxy(22,11);
		puts("│"); 
		gotoxy(0,12);
		puts("│");
		gotoxy(22,12);
		puts("│");
		gotoxy(0,13);
		puts("└────────────────────┘");
		gotoxy(0,14);
		puts("*12345678901234567890*");
}


void term_set_to_position_scr(int xt, int yt) { //(8, 2);
	gotoxy(xt+1,yt+10);
}

void term_print_to_scr (char * message) {
	set_display_atrib(F_CYAN);
	char * buf = convert_charset(message, "cp866","utf-8" );
	printf("%s", buf);
	fflush(stdout);
	free(buf);
}

void print_title_name_artist(int pos, char * mess) {
	gotoxy(0,pos);
	eraceline();
	set_display_atrib(F_GREEN);
	fputs(mess, stdout);
	fflush(stdout);
}

void term_clear_scr() {
	term_set_to_position_scr(1,1);
	term_print_to_scr("                    ");
	term_set_to_position_scr(1,2);
	term_print_to_scr("                    ");
}

void clearchar() {
		gotoxy(x,y);
		putchar(' ');
}

void movex(int moveto) {
	if(((x>0) && (moveto<0)) || ((x<80) && (moveto>0))) {
		//clearchar();
		x+=moveto;
		gotoxy(x,y);
		putchar('*');
	}
}

void movey(int moveto) {
	if(((y>0) && (moveto<0)) || ((y<20) && (moveto>0))) {
		//clearchar();
		y+=moveto;
		gotoxy(x,y);
		putchar('*');
	}
}

void init_term () {
	static struct termios newt;
	tcgetattr( STDIN_FILENO, &oldt);
	newt = oldt;
	newt.c_lflag &= ~(ICANON|ECHO);            
	tcsetattr( STDIN_FILENO, TCSANOW, &newt);

	home();
	clrscr();
	set_display_atrib(BRIGHT);
	set_display_atrib(F_RED);
	hide_the_cursor(); 
	print_display_table ();
}

void exit_term () {
	home();
	clrscr();
	resetcolor();
	show_the_cursor(); 
	tcsetattr( STDIN_FILENO, TCSANOW, &oldt);
}

