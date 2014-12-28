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


/* create_pid_file.h

   Header file for create_pid_file.c.
*/


#ifndef CREATE_PID_FILE_H
#define CREATE_PID_FILE_H


#define CPF_CLOEXEC 1

/* Open/create the file named in 'pid_file', lock it, optionally
 * set the close-on-exec flag for the file descriptor, write our PID
 * into the file, and (in case the caller is interested) return the file
 * descriptor referring to the locked file. The caller is responsible for
 * deleting 'pid_file' file (just) before process termination.
 * 'prog_name' should be the name of the calling program (i.e., argv[0] or
 * similar), and is used only for diagnostic messages. If we can't open
 * 'pid_file', or we encounter some other error, then we print an appropriate
 * diagnostic and terminate.
 */
int
create_pid_file(const char *prog_name, const char *pid_file, int flags);


#endif
