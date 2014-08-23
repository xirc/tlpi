TARGETS = simple_thread detached_attrib
OBJS = simple_thread.o detached_attrib.o
CFLAGS = -std=c99 -D_XOPEN_SOURCE=700 -g -Wall -Wextra
LDFLAGS = -pthread
RM = rm -f

all: $(TARGETS)
clean:
	$(RM) $(TARGETS) $(OBJS)
.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<

simple_thread: simple_thread.o
simple_thread.o: simple_thread.c
detached_attrib: detached_attrib.o
detached_attrib.o: detached_attrib.c
