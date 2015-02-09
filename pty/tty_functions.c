/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2014.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU Lesser General Public License as published   *
* by the Free Software Foundation, either version 3 or (at your option)   *
* any later version. This program is distributed without any warranty.    *
* See the files COPYING.lgpl-v3 and COPYING.gpl-v3 for details.           *
\*************************************************************************/
/*************************************************************************\
*                  Copyright (C) xirc, 2014.                              *
* See above.                                                              *
\*************************************************************************/


/* tty_functions.c

   Implement ttySetCbreak() and ttySetRaw().
*/


#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <termios.h>


/* Place terminal referred to by 'fd' in cbreak mode (noncanonical mode
   with echoing turned off). This function assumes that the terminal is
   currently in cooked mode (i.e., we shouldn't call it if the terminal
   is currently in raw mode, since it does not undo all of the changes
   made by the ttySetRaw() function below). Return 0 on success, or -1
   on error. If 'prevTermios' is non-NULL, then use the buffer to which
   it points to return the previous terminal settings. */
int
tty_set_cbreak(int fd, struct termios *prev_termios)
{
    struct termios t;

    if (tcgetattr(fd, &t) == -1) {
        return -1;
    }

    if (prev_termios != NULL) {
        *prev_termios = t;
    }

    t.c_lflag &= ~(ICANON | ECHO);
    t.c_lflag |=ISIG;

    t.c_iflag &= ~ICRNL;

    t.c_cc[VMIN] = 1;           /* Character-at-a-time input */
    t.c_cc[VTIME] = 0;          /* with blocking */

    if (tcsetattr(fd, TCSAFLUSH, &t) == -1) {
        return -1;
    }

    return 0;
}


/* Place terminal referred to by 'fd' in raw mode (noncanonical mode
   with all input and output processing disabled). Return 0 on success,
   or -1 on error. If 'prevTermios' is non-NULL, then use the buffer to
   which it points to return the previous terminal settings. */
int
tty_set_raw(int fd, struct termios *prev_termios)
{
    struct termios t;

    if (tcgetattr(fd, &t) == -1) {
        return -1;
    }

    if (prev_termios != NULL) {
        *prev_termios = t;
    }

    t.c_lflag &= ~(ICANON | ISIG | IEXTEN | ECHO);
            /* Noncanonical mode, disable signals, extended
             * input processing, and echoing */
    t.c_iflag &= ~(BRKINT | ICRNL | IGNBRK | IGNCR | INLCR |
                    INPCK | ISTRIP | IXON | PARMRK);
            /* Disable special handling of CR, NL, and BREAK.
             * No 8th-bit stripping or parity error handling.
             * Disable START/STOP output flow control. */
    t.c_oflag &= ~OPOST;
            /* Disable all output processing */

    t.c_cc[VMIN] = 1;           /* Character-at-a-time input */
    t.c_cc[VTIME] = 0;          /* with blocking */

    if (tcsetattr(fd, TCSAFLUSH, &t) == -1) {
        return -1;
    }

    return 0;
}