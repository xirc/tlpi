TARGETS = dump_utmpx
OBJS = dump_utmpx.o
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
