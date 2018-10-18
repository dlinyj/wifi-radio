#ifndef __UART_H__
#define __UART_H__
#include <stdbool.h>
#include <stdint.h>

/*
 * Check if a file exist using stat() function
 * return 1 if the file exist otherwise return 0
 */
int cfileexists(const char* filename);

/**
	* @brief  inittialize comport
	* @param  comport : string value represents com device, examle: "/dev/ttyUSB0"
	* @retval file decriptor of initialized port (<0 if failed)
	*/
int init_comport(const char *comport, int baud);

/**
	* @brief  reads from comport into ring buffer
	* @param  fd : comport file descriptor
	* @param  len : 
	* @param  timeout : time to wait for data, ms
	* @buff  buffer for read
	* @retval size of data peing placed in ringbuffer
	*/
int read_com(int fd, int len , int timeout, char * buff);
/**
	* @brief  writes into comport
	* @param  fd : comport file descriptor
	* @param  buf : pointer to data
	* @param  size : data size
	* @param  timeout : time to wait for port to be free, ms
	* @retval 1 - success, 0 - fail
	*/
int write_com(int fd, char * buf, size_t size, int timeout);

int init_rs485 (int fd);
int set_rts(int fd, int on);

void set_blocking (int fd, int should_block);

int com_bytes_available (int fd);
#endif //__UART_H__


