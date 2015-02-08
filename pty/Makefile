TARGETS = t_pty_master_open t_pty_fork
OBJS = t_pty_master_open.o pty_master_open.o t_pty_fork.o pty_fork.o
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
