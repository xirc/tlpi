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


/* daemon_SIGHUP.c

   Demonstrate the use of SIGHUP as a mechanism to tell a daemon to
   reread its configuration file and reopen its log file.

   In the version of this code printed in the book, logOpen(), logClose(),
   logMessage(), and readConfigFile() were omitted for brevity. The version
   of the code in this file is complete, and can be compiled and run.
*/


#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <signal.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>

#include "become_daemon.h"


static const char *LOG_FILE = "/tmp/ds.log";
static const char *CONFIG_FILE = "/tmp/ds.conf";

/* Set nonzero on receipt of SIGHUP */
static volatile sig_atomic_t hup_received = 0;

static int log_open(const char *log_file);
static int log_close();
static int log_message(const char *format, ...);
static int read_config_file(const char *config_file);


static void
sighup_handler(int sig __attribute__((unused)))
{
    hup_received = 1;
}


int
main(int argc __attribute__((unused)),
     char *argv[] __attribute__((unused)))
{
    /* Time to sleep between messages */
    const int SLEEP_TIME = 15;

    /* Number of completed SLEEP_TIME intervals */
    int count = 0;

    /* Time remaining in sleep interval */
    int unslept;

    struct sigaction sa;

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = sighup_handler;
    if (sigaction(SIGHUP, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    if (become_daemon(0) == -1) {
        perror("become_daemon");
        exit(EXIT_FAILURE);
    }

    log_open(LOG_FILE);
    read_config_file(CONFIG_FILE);
    unslept = SLEEP_TIME;

    while (1) {
        unslept = sleep(unslept);   /* Returns > 0 if interrupted */
        if (hup_received) {
            hup_received = 0;
            log_close();
            log_open(LOG_FILE);
            read_config_file(CONFIG_FILE);
        }

        /* On completed interval */
        if (unslept == 0) {
            ++count;
            log_message("Main: %d", count);
            unslept = SLEEP_TIME;
        }
    }
    exit(EXIT_SUCCESS);
}


/* LOG FILE FUNCTIONS */
static FILE *logfp;


static int
log_message(const char *format, ...)
{
#define TS_BUF_SIZE 1024
    va_list arg_list;
    const char *TIMESTAMP_FMT = "%F %X";        /* = YYYY-MM-DD HH:MM:SS */
    char timestamp[TS_BUF_SIZE];
    time_t t;
    struct tm *loc;

    t = time(NULL);
    loc = localtime(&t);
    if (loc == NULL ||
           strftime(timestamp, TS_BUF_SIZE, TIMESTAMP_FMT, loc) == 0)
    {
        fprintf(logfp, "???Unknown time????: ");
    }
    else
    {
        fprintf(logfp, "%s: ", timestamp);
    }

    va_start(arg_list, format);
    vfprintf(logfp, format, arg_list);
    fprintf(logfp, "\n");
    va_end(arg_list);

    return 0;
}


static int
log_open(const char *log_file)
{
    mode_t m;

    m = umask(077);
    logfp = fopen(log_file, "a");
    umask(m);

    /* If opening the log fails we can't display a message... */
    if (logfp == NULL) {
        return -1;
    }

    /* Disable stdio buffering */
    setbuf(logfp, NULL);

    log_message("Opened log file");

    return 0;
}


static int
log_close(void)
{
    log_message("Closing log file");
    return fclose(logfp);
}


static int
read_config_file(const char *config_file)
{
#define SBUF_SIZE 100
    FILE *configfp;
    char str[SBUF_SIZE];

    configfp = fopen(config_file, "r");
    if (configfp == NULL) {                 /* Ignore nonexistent file */
        return -1;
    }

    if (fgets(str, SBUF_SIZE, configfp) == NULL) {
        str[0] = '\0';
    } else {
        str[strlen(str) - 1] = '\0';    /* Strip trailing '\n' */
    }
    log_message("Read config file: %s", str);
    fclose(configfp);

    return 0;
}
