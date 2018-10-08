#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <unistd.h>
#include <fcntl.h>

#include "charset_coverter.h"
#include "output.h"

#define HOSTNAME "localhost"
#define PORT 6600

static int sock;

int init_mpc (void) {
	int port = PORT;
	struct sockaddr_in serv_addr;
	struct hostent *server;
	char buf[BUF_SIZE];

	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0) {
		fprintf(stderr, "socket() failed: %d", errno);
		exit(1);
	}
	//
	server = gethostbyname(HOSTNAME);
	if (server == NULL) {
		fprintf(stderr, "Host not found\n");
		return EXIT_FAILURE;
	}
	memset((char *) &serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);
	serv_addr.sin_port = htons(port);
	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
		fprintf(stderr, "connect() failed: %d", errno);
		exit(1);
	}
	memset(buf, 0, BUF_SIZE);
	read(sock, buf, BUF_SIZE-1);
	fprintf(stderr, "%s\n",buf);
	return sock;
}

char * foud_text_in_string (char * where_found, char * word) {
	static char tmp_buf[BUF_SIZE] = {0};
	char * ret = tmp_buf;
	char * tmp = strstr(where_found, word );
	if (tmp != NULL) {
		tmp += strlen(word);
		while ((* tmp != '\n') && (* tmp != '\r')) {
			*ret = *tmp;
			ret ++;
			tmp ++;
		}
		*ret = '\0';
	} else {
		tmp_buf[0] = '\0';
	}
	return tmp_buf;

}

static void send_cmd_get_request (char * cmd, char * request) {
	write(sock, cmd, strlen(cmd));
	read(sock, request, BUF_SIZE-1);
}

void get_all (output_t * output_st) {
	fprintf(stderr, "get_all\n\r");
	static char tmp_buff [BUF_SIZE];
	send_cmd_get_request("currentsong\n", tmp_buff);
	
	char * tmp;
	tmp = convert_charset(foud_text_in_string(tmp_buff,"Title: "), "utf-8", "cp866");
	if (tmp != NULL) {
		mistake_correctioin(tmp);
		strcpy(output_st->title, tmp);
		free(tmp);
	} else {
		memset(output_st->title, ' ', 20);
		output_st->title[21] = '\0';
	}
	tmp = convert_charset(foud_text_in_string(tmp_buff,"Name: "), "utf-8", "cp866");
	if (tmp != NULL) {
		mistake_correctioin(tmp);
		strcpy(output_st->name, tmp);
		free(tmp);
	} else {
		memset(output_st->name, ' ', 20);
		output_st->name[21] = '\0';
	}
	tmp = convert_charset(foud_text_in_string(tmp_buff,"Artist: "), "utf-8", "cp866");
	if (tmp != NULL) {
		mistake_correctioin(tmp);
		strcpy(output_st->artist, tmp);
		free(tmp);
	}else {
		memset(output_st->artist, ' ', 20);
		output_st->artist[21] = '\0';
	}
	send_cmd_get_request("status\n", tmp_buff);
	tmp = foud_text_in_string(tmp_buff,"playlistlength: ");
	if (tmp != NULL) {
		output_st->playlistlength = atoi(tmp);
	} else {
		output_st->playlistlength = 0;
	}
	tmp = foud_text_in_string(tmp_buff,"song: ");
	if (tmp != NULL) {
		output_st->currentsong = atoi(tmp);
	} else  {
		output_st->currentsong = 0;
	}
}

output_t * init_output_st (void ){
	static output_t output_st = {0};
	memset(output_st.title, '\0', BUF_SIZE-1);
	memset(output_st.name, '\0', BUF_SIZE-1);
	memset(output_st.artist, '\0', BUF_SIZE-1);
	output_st.currentsong = 0;
	output_st.playlistlength = 0;
	return &output_st;
}

#ifdef OLD_VERSION
static void get_from_cursong (char * buf, char * position ) {
	static char tmp_buff [BUF_SIZE];
	send_cmd_get_request("currentsong\n", tmp_buff);
	char * tmp = convert_charset(foud_text_in_string(tmp_buff,position), "utf-8", "cp866");
	strcpy(buf,tmp);
	mistake_correctioin(buf);
	free(tmp);
}

static void get_title (char * buf) { //название для всех
	get_from_cursong(buf, "Title: ");
}

static void get_name (char * buf) { //Это для радиостанций
	get_from_cursong(buf, "Name: ");
}

static void get_artist (char *buf) {
	get_name(buf);
}

static int get_from_status(char * type_str) {
	static char tmp_buff[BUF_SIZE];
	send_cmd_get_request("status\n", tmp_buff);
	char * tmp = foud_text_in_string(tmp_buff,type_str);
	if (tmp != NULL) {
		return atoi(tmp);
	} else {
		return -1;
	}
}

static int get_number_curent_song () {
	return get_from_status("song: ");
}

static int get_playlistlength () {
	return get_from_status("playlistlength: ");
}
#endif //OLD_VERSION

void set_play_list_position(int position) {
	char command_buff [BUF_SIZE];
	char tmp_buff [BUF_SIZE];
	sprintf(command_buff,"play %d\n", position);
	send_cmd_get_request(command_buff, tmp_buff);
}

void music_pause (void)  {
	char tmp_buff [BUF_SIZE];
	send_cmd_get_request("pause\n", tmp_buff);
}


