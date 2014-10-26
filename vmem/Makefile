TARGETS = t_mprotect
OBJS = t_mprotect.o
CFLAGS = -std=c99 -D_XOPEN_SOURCE=700 -g -Wall -Wextra
LDFLAGS =
RM = rm -f

all: $(TARGETS)
clean:
	$(RM) $(TARGETS) $(OBJS)
.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<

t_mprotect: t_mprotect.o
t_mprotect.o: t_mprotect.c
