TARGETS = dump_utmpx t_getlogin
OBJS = dump_utmpx.o t_getlogin.o
CFLAGS = -std=c99 -D_XOPEN_SOURCE=700 -g -Wall -Wextra
LDFLAGS =
RM = rm -f

all: $(TARGETS)
clean:
	$(RM) $(TARGETS) $(OBJS)
.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<

dump_utmpx: dump_utmpx.o
dump_utmpx.o: dump_utmpx.c
t_getlogin: t_getlogin.o
t_getlogin.o: t_getlogin.c
