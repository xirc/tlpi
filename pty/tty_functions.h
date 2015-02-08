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


/* tty_functions.h

   Header file for tty_functions.c.
*/


#ifndef TTY_FUNCTIONS_H
#define TTY_FUNCTIONS_H


#include <termios.h>


int tty_set_cbreak(int fd, struct termios *prevTermios);


int tty_set_raw(int fd, struct termios *prevTermios);


#endif
