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


/* pty_fork.h

   Header file for pty_fork.c.
*/


#ifndef FORK_PTY_H
#define FORK_PTY_H


#include <termios.h>
#include <sys/ioctl.h>
#include <sys/types.h>


pid_t pty_fork(int *master_fd, char *slave_name, size_t snlen,
        const struct termios *slave_termios, const struct winsize *slave_ws);


#endif
