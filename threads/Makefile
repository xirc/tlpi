TARGETS = simple_thread detached_attrib t_join_self \
          no_join_self wrong_reference
OBJS = simple_thread.o detached_attrib.o t_join_self.o \
       no_join_self.o wrong_reference.o
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
t_join_self: t_join_self.o
t_join_self.o: t_join_self.c
no_join_self: no_join_self.o
no_join_self.o: no_join_self.c
wrong_reference: wrong_reference.o
wrong_reference.o: wrong_reference.c
