TARGETS = t_stat
OBJS = file_perms.o t_stat.o
CFLAGS = -std=c99 -D_XOPEN_SOURCE=700 -g -Wall -Wextra
LDFLAGS =
RM = rm -f

all: $(TARGETS)
clean:
	$(RM) $(TARGETS) $(OBJS)
.c.o:
	$(CC) $(CFLAGS) $(LDFLAGS) -c -o $@ $<

t_stat: t_stat.o file_perms.o
t_stat.o: t_stat.c file_perms.h
file_perms.o: file_perms.c file_perms.h
