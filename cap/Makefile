TARGETS = douser check_password_caps
OBJS = douser.o check_password_caps.o caplib.o
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
check_password_caps: check_password_caps.o caplib.o
	$(CC) $(LDFLAGS) -lcrypt -lcap -o $@ $^
check_password_caps.o: check_password_caps.c caplib.h
caplib.o: caplib.c caplib.h
