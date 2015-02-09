TARGETS = t_pty_master_open t_pty_fork \
          script script2 script3 \
          script4 script_replay unbuffer longrunner
OBJS = t_pty_master_open.o pty_master_open.o t_pty_fork.o pty_fork.o script.o \
       script.o tty_functions.o pty_master_open_bsd.o script2.o script3.o \
       script4.o script_replay.o unbuffer.o longrunner.o
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
script2: script2.o tty_functions.o pty_fork.o pty_master_open.o
script2.o: script2.c tty_functions.h
script3: script3.o tty_functions.o pty_fork.o pty_master_open.o
script3.o: script3.c tty_functions.h
script4: script4.o tty_functions.o pty_fork.o pty_master_open.o
	$(CC) $(CFLAGS) -lrt -o $@ $^
script4.o: script4.c tty_functions.h
script_replay: script_replay.o tty_functions.o
	$(CC) $(CFLAGS) -lrt -o $@ $^
script_replay.o: script_replay.c tty_functions.h
unbuffer: unbuffer.o tty_functions.o pty_fork.o pty_master_open.o
unbuffer.o: unbuffer.c tty_functions.h
longrunner: longrunner.o
longrunner.o: longrunner.c
