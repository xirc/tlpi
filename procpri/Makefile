TARGETS = t_setpriority
OBJS = t_setpriority.o
CFLAGS = -std=c99 -D_XOPEN_SOURCE=700 -g -Wall -Wextra
LDFLAGS =
RM = rm -f

all: $(TARGETS)
clean:
	$(RM) $(TARGETS) $(OBJS)
.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<

t_setpriority: t_setpriority.o
t_setpriority.o: t_setpriority.c
