#include <unistd.h>
#include "py/mpconfig.h"

int user_getc(void);
void user_putc(char c);
void user_write_console(const char *s,int len);

// Receive single character
int mp_hal_stdin_rx_chr(void) {
	return user_getc();
//    unsigned char c = 0;
//    int r = read(0, &c, 1);
//    (void)r;
//    return c;
}

// Send string of given length
void mp_hal_stdout_tx_strn(const char *str, mp_uint_t len) {
	user_write_console(str,len);
//    int r = write(1, str, len);
//    (void)r;
}
