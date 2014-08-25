TARGETS = simple_thread detached_attrib t_join_self \
          no_join_self wrong_reference thread_incr thread_incr_mutex \
          thread_mutex_type prod_no_condvar prod_condvar thread_multijoin
OBJS = simple_thread.o detached_attrib.o t_join_self.o \
       no_join_self.o wrong_reference.o thread_incr.o thread_incr_mutex.o \
       thread_mutex_type.o prod_no_condvar.o prod_condvar.o thread_multijoin.o
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
thread_incr: thread_incr.o
thread_incr.o: thread_incr.c
thread_incr_mutex: thread_incr_mutex.o
thread_incr_mutex.o: thread_incr_mutex.c
thread_mutex_type: thread_mutex_type.o
thread_mutex_type.o: thread_mutex_type.c
prod_no_condvar: prod_no_condvar.o
prod_no_condvar.o: prod_no_condvar.c
prod_condvar: prod_condvar.o
prod_condvar.o: prod_condvar.c
thread_multijoin: thread_multijoin.o
thread_multijoin.o: thread_multijoin.c
