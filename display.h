#ifndef __DISLPAY_H__
#define __DISLPAY_H__
void clear_scr(void);
void home_scr(void);
void set_to_position_scr(char col, char str);
void print_to_scr (char * s);
void reload_char(int i);
void move_symb_right (void);
void move_symb_left (void);
void get_cur_position (void);
void show_current_cursor_pos (void);
void tuning_movement (char left_right);
int init_display(void);
void set_cirilic (void);
#endif //__DISLPAY_H__
