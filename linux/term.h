#ifndef __TERM_EXAMPLE__
#define __TERM_EXAMPLE__

#define ESC "\033"

#define home() 			fputs(ESC "[H", stdout) //Move cursor to the indicated row, column (origin at 1,1)
#define clrscr()		fputs(ESC "[2J", stdout) //lear the screen, move to (1,1)
#define gotoxy(x,y)		printf(ESC "[%d;%dH", y, x);
#define eraceline()	fputs(ESC "[K", stdout)
/* 
Set Display Attribute Mode	<ESC>[{attr1};...;{attrn}m
*/
#define resetcolor() printf(ESC "[0m")
#define set_display_atrib(color) 	printf(ESC "[%dm",color)
#define hide_the_cursor() fputs("\e[?25l", stdout) /* hide the cursor */
#define show_the_cursor() fputs("\e[?25h", stdout) /* show the cursor */

#define LEFT -1
#define RIGHT 1
#define UP -1
#define DOWN 1

//Format text
#define RESET 		0
#define BRIGHT 		1
#define DIM			2
#define UNDERSCORE	3
#define BLINK		4
#define REVERSE		5
#define HIDDEN		6

//Foreground Colours (text)

#define F_BLACK 	30
#define F_RED		31
#define F_GREEN		32
#define F_YELLOW	33
#define F_BLUE		34
#define F_MAGENTA 	35
#define F_CYAN		36
#define F_WHITE		37

//Background Colours
#define B_BLACK 	40
#define B_RED		41
#define B_GREEN		42
#define B_YELLOW	44
#define B_BLUE		44
#define B_MAGENTA 	45
#define B_CYAN		46
#define B_WHITE		47


void init_term (void);
void exit_term (void);
void print_cur_pos_len (int position, int len);
void print_button_pressed (void);
void double_click_button (void);
void print_title_name_artist(int pos, char * mess);
void print_display_table(void);

#define print_title(mess) 		print_title_name_artist(5,mess)
#define print_name(mess) 		print_title_name_artist(6,mess)
#define print_artist(mess) 		print_title_name_artist(7,mess)

void term_set_to_position_scr(int x, int y);
void term_print_to_scr (char * message);
void term_clear_scr(void);


#endif //__TERM_EXAMPLE__
