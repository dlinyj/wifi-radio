#ifndef __MPC_H__
#define __MPC_H__
#include "output.h"
void send_cmd(char * buf, int len, char *cmd, int cat_len);
void get_title (char * buf);
void get_name (char * buf);
void get_artist (char * buf);
int get_number_curent_song (void);
int get_playlistlength (void);
void set_play_list_position(int position);
void music_pause (void);
int init_mpc (void);

void get_all (output_t * output_st);
output_t * init_output_st (void );

//#define MUSIC_PAUSE() system("mpc toggle > /dev/null");
#endif //__MPC_H__
