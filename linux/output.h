#ifndef __OUTPUT_H__
#define __OUTPUT_H__

#include <stdint.h>

#define BUF_SIZE 1024

 /*
 * Напоминание 
 * tun_disp_position Позиция символа настройки на экране
 * tun_char_position Текущий символ настройки
 * Вычисление позиции станции идёт по следующей формуле
 * sation_pos=(tun_disp_position-1)*5+tun_char_position
 * */

typedef struct{
	char	title	[BUF_SIZE];
	char	name	[BUF_SIZE];
	char	artist	[BUF_SIZE];
	int		currentsong;
	int		playlistlength;
	double	delta_step;
	int		tun_disp_position;
	int		tun_char_position;
} output_t;

#endif
