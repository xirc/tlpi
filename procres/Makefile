TARGETS = t_getrusage
OBJS = t_getrusage.o
CFLAGS = -std=c99 -D_XOPEN_SOURCE=700 -g -Wall -Wextra
LDFLAGS =
RM = rm -f

all: $(TARGETS)
clean:
	$(RM) $(TARGETS) $(OBJS)
.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<

t_getrusage: t_getrusage.o
t_getrusage.o: t_getrusage.c
