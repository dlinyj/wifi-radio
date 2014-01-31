 #include <stdio.h>   /* Standard input/output definitions */
 #include <stdlib.h>  /* Standard General Utilities Library */
 #include <string.h>  /* String function definitions */
 #include <unistd.h>  /* UNIX standard function definitions */
 #include <fcntl.h>   /* File control definitions */
 #include <errno.h>   /* Error number definitions */
 #include <termios.h> /* POSIX terminal control definitions */
 #include <signal.h> 
 #include <pthread.h>
 #include <semaphore.h>
 #include <time.h>
 
 #include <curses.h> //ВНИМАНИЕ, либа!!! Для отладки

//#define HOME

#define STR_BUFFLEN 128
static int mainfd=0;                                       /* File descriptor of COM-port*/
static int tun_disp_position=1; //Позиция символа настройки на экране
static int tun_char_position=1; //Текущий символ настройки
static int stations=0;
char station_name[100][256];
int search_status=0;			//А был ли поиск?

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;



int initcomport(void)
{
//*****************************************************************************
//******************init COM-port**********************************************
	//int mainfd=0;                                       /* File descriptor */
 
	struct termios options;
	mainfd = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY | O_NDELAY);
	if (mainfd == -1)
	{                                                       /* Could not open the port */
		fprintf(stderr, "open_port: Unable to open /dev/ttyACM0 - %s\n",
		strerror(errno));
		exit (-1);
	}
 
	/* Configure port reading */
	//fcntl(mainfd, F_SETFL, FNDELAY);  read com-port not bloking
	fcntl(mainfd, F_SETFL, 0);  //read com-port is the bloking
	 
	 
/* Get the current options for the port */
	tcgetattr(mainfd, &options);
	cfsetispeed(&options, B9600);                     /* Set the baud rates to 9600 */
	//cfsetospeed(&options, B115200);
	 
/* Enable the receiver and set local mode */
	options.c_cflag |= (CLOCAL | CREAD);
	options.c_cflag &= ~PARENB;                         /* Mask the character size to 8 bits, no parity */
	options.c_cflag &= ~CSTOPB;
	options.c_cflag &= ~CSIZE;
	options.c_cflag |=  CS8;                            /* Select 8 data bits */
	options.c_cflag &= ~CRTSCTS;                        /* Disable hardware flow control */ 
  
/* Enable data to be processed as raw input */
	options.c_lflag &= ~(ICANON | ECHO | ISIG);
/* Set the new options for the port */
	tcsetattr(mainfd, TCSANOW, &options);
//*****************************************************************************
//*****************************************************************************
	return mainfd;
}


void clear_scr() {
	char *s;
	s = strdup("\x0C"); 
	write(mainfd,s,strlen(s));
	free(s);
}

void home_scr() {
	char *s;
	s = strdup("\x1B[H"); 
	write(mainfd,s,strlen(s));
	free(s);
}

void set_to_position_scr(char col, char str){
	static char s[4];
	s[0]='\x1f';
	s[1]='\x24';
	s[2]=col;
	s[3]=str;
	write(mainfd,s,4);
}

void print_to_scr (char * s) {
	int i;
	for (i=0;i<65535;i++) {
		if (s[i]!='\0') {
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

		} else break;
	}
}

void reload_char(int i) {
	int j; //костыль от перезагрузки шрифта
	for(j=0;j<3;j++)
		switch (i)
		{
			case 1: print_to_scr ("\x1B\x26\xA0\x21\x84\x10\x42\x80"); //Загрузка шрифта символ первой палочки OK 
					break;
			case 2: print_to_scr ("\x1B\x26\xA0\x42\x08\x21\x84\x80"); //Загрузка шрифта символ 2 палочки OK
					break;
			case 3: print_to_scr ("\x1B\x26\xA0\x84\x10\x42\x08\x81"); //Загрузка шрифта символ 3 палочки OK
					break;
			case 4: print_to_scr ("\x1B\x26\xA0\x08\x21\x84\x10\x82"); //Загрузка шрифта символ 4 палочки OK
					break;
			case 5: print_to_scr ("\x1B\x26\xA0\x10\x42\x08\x21\x84"); //Загрузка шрифта символ 5 палочки OK
			default:      ;
		
		}
}

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

void move_symb_left () {
	if ((tun_disp_position>1) || (tun_char_position>1)) {
		tun_char_position--;
		if (tun_char_position<1) {
			tun_char_position=5;
			tun_disp_position--;
			print_to_scr ("\x08  \x08\x08"); //Move cursor left and space
			}
		reload_char(tun_char_position);
		print_to_scr ("\xA0\x08"); //Печааем палку и возвращаем курсор взад
	}
}


void move_symb_right () {
	if ((tun_disp_position<20) || (tun_char_position<5)) {
		tun_char_position++;
		if (tun_char_position>5) {
			tun_char_position=1;
			tun_disp_position++;
			print_to_scr ("\x08  "); //Move cursor left and space
			}
		reload_char(tun_char_position);
		print_to_scr ("\xA0\x08"); //Печааем палку и возвращаем курсор взад
	}
}

void tuning_action() {
 /*
 * Напоминание 
 * tun_disp_position Позиция символа настройки на экране
 * tun_char_position Текущий символ настройки
 * Вычисление позиции станции идёт по следующей формуле
 * sation_pos=(tun_disp_position-1)*5+tun_char_position
 * */
	pthread_mutex_lock(&mutex);

	clear_scr();
	home_scr();

	print_to_scr ("\x1B\x25\x01"); //Разрешение юзверских шрифтов
	set_to_position_scr(8, 2);
	print_to_scr ("SEARCH"); //Поиск

//Здеся мы и должны получить позицию!
	int playlist_len, curent_pos;
	playlist_len=get_playlistlength();
	curent_pos=get_number_curent_song();
//И тута будем много-много ДУМАТЬ	
	
	
	set_to_position_scr(tun_disp_position, 1); //устанавливаем курсор в текущую позицию
	reload_char(tun_char_position); //загружаем символ
	print_to_scr ("\xA0\x08"); //Печааем палку и возвращаем курсор взад
	

}



int action=1;
time_t last_time_action;


void *KOSTYLI() {
	time_t current_time;
	while (1)
	if (action==0) {
		current_time = time(NULL); 
		if ((current_time-last_time_action)>3) {
			action++;
			search_status++;
			pthread_mutex_unlock(&mutex);
		}
	}
	sleep(1);
}

//For PC with ncurses and keyboard!!!
void tunning () {
	int ch;
	int result;
	//Ncurses КОСТЫЛИ!!!
	pthread_t thread1;
	result = pthread_create( &thread1, NULL, &KOSTYLI, NULL);
	if (result != 0) {
		perror("Creating the first thread");
		return EXIT_FAILURE;
	}
	
	initscr();
	raw();
	keypad(stdscr, TRUE);
	noecho();
	while (1) {
		ch = getch();
		if(ch == KEY_UP) {
			break;
		} else {
			if (action) {
				tuning_action();
				action--;
				last_time_action = time(NULL); 
			} else {
				if(ch == KEY_LEFT) {
					last_time_action = time(NULL);
					move_symb_left (); //сдвинуть символ влево
				}
				if(ch == KEY_RIGHT) {
					last_time_action = time(NULL);
					move_symb_right(); //сдвинуть символ вправо
				}
				if(ch == KEY_DOWN) {
					action++;
					search_status++;
					pthread_mutex_unlock(&mutex);

				}
			}
		}
		refresh();
	}
	endwin();
	pthread_mutex_unlock(&mutex);
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


#define BUFF_LEN 256

//void show_current_track(int number_signal) {
void *show_current_track() {
	while (1) {
		/*
		get_title (char * buf, int len)//название для всех
		get_name (char * buf, int len) //Это для радиостанций (название радио)
		get_artist (char * buf, int len)  //Это для артистов
		*/
		pthread_mutex_lock(&mutex);
		
		char cmp_buf_title[BUFF_LEN]; 
		static char curent_buf_title[BUFF_LEN];
		static int curent_title_position;
		static int strlen_title;
		
		
		char cmp_buf_name[BUFF_LEN];
		static char curent_buf_name[BUFF_LEN];
		static int curent_name_position;
		static int strlen_name;
		
		char show [21];
		show [20]='\0';
		
		get_title (cmp_buf_title, BUFF_LEN);
		if (strcmp(cmp_buf_title,curent_buf_title) != 0) {
			strcpy(curent_buf_title, cmp_buf_title);
			curent_title_position=0;
			strlen_title=strlen(curent_buf_title);
		}

		get_name (cmp_buf_name, BUFF_LEN);
		if (strlen(cmp_buf_name)==0)
			get_artist(cmp_buf_name, BUFF_LEN);

		if (strcmp(cmp_buf_name,curent_buf_name) != 0) {
			strcpy(curent_buf_name, cmp_buf_name);
			curent_name_position=0;
			strlen_name=strlen(curent_buf_name);
		}

	//Show title
		memset (show,' ',20);
		if (((curent_title_position==0) && (strlen_title<=20)) || (search_status==1)) {
			home_scr();
			print_to_scr (show);
			strncpy (show, curent_buf_title, 20);
			curent_title_position++;
			home_scr();
			print_to_scr (show);
		}
		if (strlen_title>20) {
			if ((strlen_title-curent_title_position)>=20) {
				strncpy (show, &curent_buf_title[curent_title_position], 20);
			} else strncpy (show, &curent_buf_title[curent_title_position], (strlen_title-curent_title_position));
			curent_title_position++;
			if (curent_title_position==strlen_title)
				curent_title_position=0;
			home_scr();
			print_to_scr (show);
		}
		
	//Show name
		memset (show,' ',20);
		if (((curent_name_position==0) && (strlen_name<=20)) || (search_status==1)) {
			set_to_position_scr(1, 2);
			print_to_scr (show);
			strncpy (show, curent_buf_name, 20);
			curent_name_position++;
			set_to_position_scr(1, 2);
			print_to_scr (show);
		}
		if (strlen_name>20) {
			if ((strlen_name-curent_name_position)>=20) {
				strncpy (show, &curent_buf_name[curent_name_position], 20);
			} else strncpy (show, &curent_buf_name[curent_name_position], (strlen_name-curent_name_position));
			curent_name_position++;
			if (curent_name_position==strlen_name)
				curent_name_position=0;
			set_to_position_scr(1, 2);
			print_to_scr (show);
		}
		if (search_status==1) search_status--;
		pthread_mutex_unlock(&mutex);
		usleep (500000);
	}
}


int main() {
/*
Смотри внизу полезняшки!!!
 */
	printf("Trololo!\n");
	if (initcomport()<0) 
		return -1;
	clear_scr();
/*
	parsing_addscript("addradio.sh");
	printf("\n");
	for(i=0;i<stations;i++) {
		printf("%s\n",station_name[i]);
	}
*/

/*
	struct sigaction act;
	act.sa_handler = show_current_track; //Передаём параметр сигнала
	sigaction (SIGALRM, & act, 0);
*/

/*
	signal(SIGALRM, show_current_track);
	signal(SIGALRM, SIG_IGN);          // ignore this signal      
	ualarm (500000, 0);
*/
	int result;
	pthread_t thread1;
	result = pthread_create( &thread1, NULL, &show_current_track, NULL);
	if (result != 0) {
		perror("Creating the first thread");
		return EXIT_FAILURE;
	}
	tunning () ;

	//print_to_scr ("\x1B\x25\x01"); //Разрешение юзверских шрифтов
	//print_to_scr ("\x1B\x25\x00"); //Запрещение юзверских шрифтов
	//print_to_scr ("Abc\xA0\xA1\xA2\xA3\xA4\x20\x21\x22\x23\x24\x25\x26\x27"); //Печать юзверских шрифтов
	//print_to_scr ("\x1F\x55"); //Display Counter

	close (mainfd);
	return 0;
}




/*
	char *s, *s1, *s2;
	s1 = strdup(" text0 ");
	s2 = strdup(" text1 ");
	s = malloc(STR_BUFFLEN);
	memset (s,'\0',STR_BUFFLEN);
	// *s="\0";    //Я инициализирую строки так это - 
	//намного быстрее вызовов strcpy 
	strcat(s,s1); //Функция strcat
	strcat(s,s2); 
	printf("%s\n", s);

	free(s1);
	free(s2);
	free(s);
*/








/*
 #include <stdio.h>
#include <iconv.h>
#include <errno.h>
#include <err.h>
int main() {
  iconv_t cd;
  size_t k, f, t;
  int se;
  const char *code = "Вопрос!";
  const char* in = code;
  char buf[100];
  char* out = buf;

  cd = iconv_open("cp1251", "koi8-r");
  if( cd == (iconv_t)(-1) )
	err( 1, "iconv_open" );
  f = strlen(code);
  t = sizeof buf;
  memset( &buf, 0, sizeof buf );
  errno = 0;
  k = iconv(cd, &in, &f, &out, &t);
  se = errno;
  printf( "converted: %u,error=%d\n", (unsigned) k, se );

  printf("string: %s\n", buf);

  iconv_close(cd);
  return 0;
}

 * 
 */
