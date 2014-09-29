TARGETS = test_become_daemon
OBJS = test_become_daemon.o become_daemon.o
CFLAGS = -std=c99 -D_XOPEN_SOURCE=700 -g -Wall -Wextra
LDFLAGS =
RM = rm -f

all: $(TARGETS)
clean:
	$(RM) $(TARGETS) $(OBJS)
.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<

test_become_daemon: test_become_daemon.o become_daemon.o
test_become_daemon.o: test_become_daemon.c become_daemon.h
become_daemon.o: become_daemon.c become_daemon.h
