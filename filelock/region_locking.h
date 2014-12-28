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


/* region_locking.h

   Header file for region_locking.c.
*/


#ifndef REGION_LOCKING_H
#define REGION_LOCKING_H


/* Lock a file region using nonblocking F_SETLK */
int lock_region(int fd, int type, int whence, int start, int len);


/* Lock a file region using blocking F_SETLKW */
int lock_region_wait(int fd, int type, int whence, int start, int len);


/* Test if a file region is lockable. Return 0 if lockable, or
 * PID of process holding incompatible lock, or -1 on error */
pid_t is_region_locked(int fd, int type, int whence, int start, int len);


#endif
