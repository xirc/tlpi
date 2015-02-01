TARGETS = t_select poll_pipes
OBJS = t_select.o poll_pipes.o
CFLAGS = -std=c99 -D_XOPEN_SOURCE=700 -g -Wall -Wextra
LDFLAGS =
RM = rm -f

all: $(TARGETS)
clean:
	$(RM) $(TARGETS) $(OBJS)
.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<

t_select: t_select.o
t_select.o: t_select.c
poll_pipes: poll_pipes.o
poll_pipes.o: poll_pipes.c
