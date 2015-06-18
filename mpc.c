#include <stdio.h>   /* Standard input/output definitions */
#include <stdlib.h>  /* Standard General Utilities Library */
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */

static int stations=0;

char station_name[100][256];

void send_cmd(char * buf, int len, char *cmd, int cat_len){
	FILE* ptr;
	//char *cmd = "echo \"currentsong\" | nc localhost 6600 | grep -e \"^Title: \""; //название для всех
	char *s;
	s = malloc(len+cat_len);
	memset (s,'\0',len+cat_len);
	if ((ptr = popen (cmd, "r")) != NULL)
		fgets (s, len+7, ptr);
	if (strlen(s)>0) s[strlen(s)-1]='\0';
	strcpy(buf,&s[cat_len]);
	free (s);
	pclose(ptr);
}

void get_title (char * buf, int len) { //название для всех
	send_cmd(buf, len, "echo \"currentsong\" | nc localhost 6600 | grep -e \"^Title: \"", 7);//название для всех
}

void get_name (char * buf, int len) { //Это для радиостанций
	send_cmd(buf, len, "echo \"currentsong\" | nc localhost 6600 | grep -e \"^Name: \"", 6);//Это для радиостанций
}

void get_artist (char * buf, int len) { //Это для артистов
	send_cmd(buf, len, "echo \"currentsong\" | nc localhost 6600 | grep -e \"^Artist: \"", 8);//Это для артистов
 }

int get_number_curent_song () {
	char buf[128];
	send_cmd(buf, 128, "echo \"status\" | nc localhost 6600 | grep -e \"^song: \"", 6);//Получаем текущую позицию
	return atoi(buf);
}

int get_playlistlength () {
	char buf[128];
	send_cmd(buf, 128, "echo \"status\" | nc localhost 6600 | grep -e \"^playlistlength: \"", 16);//Получаем длинну плей-листа
	return atoi(buf);
}

void set_play_list_position(int position) {
	char command_buff [255]={0};
	sprintf(command_buff,"mpc play %d", position);
	system(command_buff);
}


#define	MAX_LINE_SIZE 256

int parsing_addscript(char *filename)
{
	FILE *instream;
	char inbuf[MAX_LINE_SIZE];
	int  rv = 0;
	int lineno = 0;
	int i;
	instream = fopen(filename, "r");
	if (instream == NULL) {
		printf("Unable to open config file '%s': %m\n", filename);
		return -1;
	}

	while (fgets(inbuf, MAX_LINE_SIZE, instream) != NULL) {
		int len = strlen(inbuf);
		if (inbuf[len-1] != '\n') {
			lineno++;
			printf("line %d is too long in config file", lineno);
			continue;
		}
		/* Remove the '\n' */
		inbuf[len-1] = '\0';
		//printf("string=%s\n",inbuf);
		system(inbuf); //run the script!
		for (i=0;i<len;i++) 
			if((inbuf[0] !='#') && (inbuf[i]=='#') && (inbuf[i+1]=='*')) { //Если не комментарий, стоит решётка и затем звезда
				strcpy(station_name[stations],&inbuf[i+2]); 
				stations++;
				break;
			}

	}
	fclose(instream);
	return rv;
}

