TARGETS = real_timer timed_read t_nanosleep
OBJS = real_timer.o timed_read.o t_nanosleep.o
CFLAGS = -std=c99 -D_XOPEN_SOURCE=700 -g -Wall -Wextra
LDFLAGS =
RM = rm -f

all: $(TARGETS)
clean:
	$(RM) $(TARGETS) $(OBJS)
.c.o:
	$(CC) $(CFLAGS) $(LDFLAGS) -c -o $@ $<

real_timer: real_timer.o
real_timer.o: real_timer.c
timed_read: timed_read.o
timed_read.o: timed_read.c
t_nanosleep: t_nanosleep.o
t_nanosleep.o: t_nanosleep.c
