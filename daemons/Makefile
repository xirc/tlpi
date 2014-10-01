TARGETS = test_become_daemon daemon_SIGHUP m_logger
OBJS = test_become_daemon.o become_daemon.o daemon_SIGHUP.o m_logger.o
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
daemon_SIGHUP: daemon_SIGHUP.o become_daemon.o
daemon_SIGHUP.o: daemon_SIGHUP.c become_daemon.h
m_logger: m_logger.o
m_logger.o: m_logger.c
