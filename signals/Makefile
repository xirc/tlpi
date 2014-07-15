TARGETS = ouch intquit t_kill
OBJS = ouch.o intquit.o t_kill.o
CFLAGS = -std=c99 -D_XOPEN_SOURCE=700 -g -Wall -Wextra
LDFLAGS =
RM = rm -f

all: $(TARGETS)
clean:
	$(RM) $(TARGETS) $(OBJS)
.c.o:
	$(CC) $(CFLAGS) $(LDFLAGS) -c -o $@ $<

ouch: ouch.o
ouch.o: ouch.c
intquit: intquit.o
intquit.o: intquit.c
t_kill: t_kill.o
t_kill.o: t_kill.c
