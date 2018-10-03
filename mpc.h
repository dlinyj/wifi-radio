#ifndef __MPC_H__
#define __MPC_H__

void send_cmd(char * buf, int len, char *cmd, int cat_len);
void get_title (char * buf, int len);
void get_name (char * buf, int len);
void get_artist (char * buf, int len);
int get_number_curent_song (void);
int get_playlistlength (void);
int parsing_addscript(char *filename);
void set_play_list_position(int position);

#define MUSIC_PAUSE() system("mpc toggle > /dev/null");
#endif //__MPC_H__
