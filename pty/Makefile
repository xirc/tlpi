TARGETS = t_pty_master_open t_pty_fork \
          script
OBJS = t_pty_master_open.o pty_master_open.o t_pty_fork.o pty_fork.o script.o \
       script.o tty_functions.o pty_master_open_bsd.o
CFLAGS = -std=c99 -D_XOPEN_SOURCE=700 -g -Wall -Wextra
LDFLAGS =
RM = rm -f

all: $(TARGETS)
clean:
	$(RM) $(TARGETS) $(OBJS)
.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<

t_pty_master_open: t_pty_master_open.o pty_master_open.o
t_pty_master_open.o: t_pty_master_open.c pty_master_open.h
pty_master_open.o: pty_master_open.c pty_master_open.h
t_pty_fork: t_pty_fork.o pty_fork.o pty_master_open.o
t_pty_fork.o: t_pty_fork.c pty_fork.h
pty_fork.o: pty_fork.c pty_fork.h pty_master_open.h
script: script.o tty_functions.o pty_fork.o pty_master_open.o
# script: script.o tty_functions.o pty_fork.o pty_master_open_bsd.o
script.o: script.c tty_functions.h
tty_functions.o: tty_functions.c tty_functions.h
pty_master_open_bsd.o: pty_master_open_bsd.c pty_master_open.h
