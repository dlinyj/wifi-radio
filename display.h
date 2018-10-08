#ifndef __DISLPAY_H__
#define __DISLPAY_H__
#include "output.h"
void clear_scr(void);
void home_scr(void);
void set_to_position_scr(char col, char str);
void print_to_scr (char * s);
void reload_char(int i);
void move_symb_right (void);
void move_symb_left (void);
void get_cur_position (void);
void show_current_cursor_pos (void);
int tuning_movement (char left_right);
//int init_display(void);
int init_display (output_t * output_st_ext);
void set_cirilic (void);
void set_overwrite_mode (void);
#endif //__DISLPAY_H__
