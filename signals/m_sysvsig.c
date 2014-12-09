/*************************************************************************\
*                  Copyright (C) xirc, 2014.                              *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU Affero General Public License as published   *
* by the Free Software Foundation, either version 3 or (at your option)   *
* any later version. This program is distributed without any warranty.    *
* See the file COPYING.agpl-v3 for details.                               *
\*************************************************************************/


#define _GNU_SOURCE
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>


static int
m_sighold(int sig)
{
    sigset_t set;
    if (sigemptyset(&set) == -1) {
        return -1;
    }
    if (sigaddset(&set, sig) == -1) {
        return -1;
    }
    if (sigprocmask(SIG_BLOCK, &set, NULL) == -1) {
        return -1;
    }
    return 0;
}


static int
m_sigrelse(int sig)
{
    sigset_t set;
    if (sigemptyset(&set) == -1) {
        return -1;
    }
    if (sigaddset(&set, sig) == -1) {
        return -1;
    }
    if (sigprocmask(SIG_UNBLOCK, &set, NULL) == -1) {
        return -1;
    }
    return 0;
}


static int
m_sigignore(int sig)
{
    struct sigaction sa;
    sa.sa_handler = SIG_IGN;
    if (sigaction(sig, &sa, NULL) == -1) {
        return -1;
    }
    return 0;
}

static int
m_sigpause(int sig)
{
    sigset_t mask;
    if (sigprocmask(SIG_BLOCK, NULL, &mask) == -1) {
        return -1;
    }
    if (sigdelset(&mask, sig) == -1) {
        return -1;
    }
    return sigsuspend(&mask);
}


void (*m_sigset(int sig, void (*handler)(int)))(int)
{
    void (*retval)(int);
    struct sigaction new_sa, old_sa;
    sigset_t block;

    if (sigprocmask(SIG_BLOCK, NULL, &block) == -1) {
        return (void*)-1;
    }
    if (sigaction(sig, NULL, &old_sa) == -1) {
        return (void*)-1;
    }
    if (sigismember(&block, sig)) {
        retval = SIG_HOLD;
    } else {
        retval = old_sa.sa_handler;
    }

    if (handler == SIG_HOLD) {
        if (m_sighold(sig) == -1) {
            return (void*)-1;
        }
        return retval;
    } else {
        new_sa.sa_handler = handler;
        sigemptyset(&new_sa.sa_mask);
        new_sa.sa_flags = 0;
        if (sigaction(sig, &new_sa, NULL) == -1) {
            return (void*)-1;
        }
        if (m_sigrelse(sig) == -1) {
            return (void*)-1;
        }
        return retval;
    }
}


int
main(int argc __attribute__((unused)),
     char *argv[] __attribute__((unused)))
{
    /* TODO TEST sigset */
    (void) m_sigset;
    /* TODO TEST sighold */
    (void) m_sighold;
    /* TODO TEST sigrelse */
    (void) m_sigrelse;
    /* TODO TEST sigignore */
    (void) m_sigignore;
    /* TODO TEST sigpause */
    (void) m_sigpause;

    exit(EXIT_SUCCESS);
}
