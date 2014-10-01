TARGETS = douser
OBJS = douser.o
CFLAGS = -std=c99 -D_XOPEN_SOURCE=700 -g -Wall -Wextra
LDFLAGS =
RM = rm -f

all: $(TARGETS)
clean:
	$(RM) $(TARGETS) $(OBJS)
.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<

douser: douser.o
	$(CC) $(LDFLAGS) -lcrypt -o $@ $^
douser.o: douser.c
