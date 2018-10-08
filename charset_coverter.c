#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <iconv.h>

#define BUF_SZ 512

char *convert_charset(char *str, const char *from, const char *to) {
	char    *buf = NULL, *out;
	size_t   buf_sz, out_left, in_left;
	iconv_t  conv = (iconv_t) -1;
	size_t   converted;
	if (!str) {
		goto on_error;
	}
	in_left = strlen(str);

	/* Initial buffer for conversion. */
	buf = malloc(BUF_SZ);
	if (!buf) {
		goto on_error;
	}
	buf_sz = BUF_SZ;
	out = buf;
	out_left = BUF_SZ - 1 /* '\0' */;

	/* Create iconv descriptor. */
	conv = iconv_open(to, from);
	if (((iconv_t) -1) == conv)
		goto on_error;

	while (1) {
		converted = iconv(conv, &str, &in_left, &out, &out_left);
		if (((size_t) -1) == converted) {
			if (E2BIG == errno) {
				/* We need more space. */
				char *new_buf;

				buf_sz += BUF_SZ;
				new_buf = realloc(buf, buf_sz);
				if (!new_buf)
					goto on_error;
				out = new_buf + (out - buf);
				buf = new_buf;
				out_left += BUF_SZ;
			} else {
				goto on_error;
			}
		}

		/* All done. */
		if (!in_left)
		{
			buf[buf_sz - out_left - 1] = '\0';
			break;
		}
	}
	iconv_close(conv);
	return buf;
on_error:
	free(buf);
	if (((iconv_t) -1) != conv)
		iconv_close(conv);

	return NULL;
}

void mistake_correctioin (char * buf) {
	while (* buf != '\0') {
		switch (*buf) {
			case (char)0x9A: //Ь -> Ъ
				*buf = 0x9C;
				break;
			case (char)0x9C: //Ъ -> Ь
				*buf = 0x9A;
				break;
			case (char)0xEA: //ь -> ъ
				*buf = 0xEC;
				break;
			case (char)0xEC: //ъ -> ь
				*buf = 0xEA;
				break;
			case (char)0xF1: //хз (ё) -> е
				*buf = 0xA5;
				break;
			case (char)0xF0: //хз (Ё) -> Е
				*buf = 0x85;
				break;
			default:
				break;
		}
		buf++;
	}
}
