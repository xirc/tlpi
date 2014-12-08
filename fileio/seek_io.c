/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2014.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU Affero General Public License as published   *
* by the Free Software Foundation, either version 3 or (at your option)   *
* any later version. This program is distributed without any warranty.    *
* See the file COPYING.agpl-v3 for details.                               *
\*************************************************************************/
/*************************************************************************\
*                  Copyright (C) xirc, 2014.                              *
* See above.                                                              *
\*************************************************************************/


/* seek_io.c
   Demonstrate the use of lseek() and file I/O system calls.

   Usage: seek_io file {r<length>|R<length>|w<string>|s<offset>}...

   This program opens the file named on its command line, and then
   performs the file I/O operations specified by its remaining
   command-line arguments:

           r<length>    Read 'length' bytes from the file at current
                        file offset, displaying them as text.

           R<length>    Read 'length' bytes from the file at current
                        file offset, displaying them in hex.

           w<string>    Write 'string' at current file offset.

           s<offset>    Set the file offset to 'offset'.

   Example:

        seek_io myfile wxyz s1 r2
*/


#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>


static void
usage(FILE *fp, const char *progname)
{
    fprintf(fp,
        "Usage: %s file {r<length>|R<length>|w<string>|s<offset>}...\n",
        progname);
}

int
main(int argc, char *argv[])
{
    size_t len;
    off_t offset;
    int fd, ap, j;
    char *buf;
    ssize_t num_read, num_written;

    if (argc == 2 && strcmp(argv[1], "--help") == 0) {
        usage(stdout, argv[0]);
        exit(EXIT_SUCCESS);
    }
    if (argc < 3) {
        usage(stderr, argv[0]);
        exit(EXIT_FAILURE);
    }

    fd = open(argv[1], O_RDWR | O_CREAT,
                S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP |
                S_IROTH | S_IWOTH);                     /* rw-rw-rw- */
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    for (ap = 2; ap < argc; ap++) {
        switch (argv[ap][0]) {
        case 'r':   /* Display bytes at current offset, as text */
        case 'R':   /* Display bytes at current offset, in hex */
            len = strtol(&argv[ap][1], NULL, 0);

            buf = malloc(len);
            if (buf == NULL) {
                perror("malloc");
                exit(EXIT_FAILURE);
            }

            num_read = read(fd, buf, len);
            if (num_read == -1) {
                perror("read");
                exit(EXIT_FAILURE);
            }

            if (num_read == 0) {
                printf("%s: end-of-file\n", argv[ap]);
            } else {
                printf("%s: ", argv[ap]);
                for (j = 0; j < num_read; j++) {
                    if (argv[ap][0] == 'r') {
                        printf("%c",
                            isprint((unsigned char) buf[j]) ? buf[j] : '?');
                    } else {
                        printf("%02x ", (unsigned int) buf[j]);
                    }
                }
                printf("\n");
            }

            free(buf);
            break;

        case 'w':   /* Write string at current offset */
            num_written = write(fd, &argv[ap][1], strlen(&argv[ap][1]));
            if (num_written == -1) {
                perror("write");
                exit(EXIT_FAILURE);
            }
            printf("%s: wrote %ld bytes\n", argv[ap], (long) num_written);
            break;

        case 's':   /* Change file offset */
            offset = strtol(&argv[ap][1], NULL, 0);
            if (lseek(fd, offset, SEEK_SET) == -1) {
                perror("lseek");
                exit(EXIT_FAILURE);
            }
            printf("%s: seek succeeded\n", argv[ap]);
            break;

        default:
            fprintf(stderr,
                    "Argument must start with [rRws]: %s\n", argv[ap]);
            usage(stderr, argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    exit(EXIT_SUCCESS);
}
