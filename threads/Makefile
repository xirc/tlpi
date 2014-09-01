TARGETS = simple_thread detached_attrib t_join_self \
          no_join_self wrong_reference thread_incr thread_incr_mutex \
          thread_mutex_type prod_no_condvar prod_condvar thread_multijoin \
          thread_incr2 t_ubt_test1 t_ubt_test1s t_ubt_test2 t_ubt_test2s \
          strerror_test strerror_test_tsd strerror_test_tls \
          one_time_init t_dirbasename \
          thread_cancel
OBJS = simple_thread.o detached_attrib.o t_join_self.o \
       no_join_self.o wrong_reference.o thread_incr.o thread_incr_mutex.o \
       thread_mutex_type.o prod_no_condvar.o prod_condvar.o thread_multijoin.o \
       thread_incr2.o t_ubt_test1.o t_ubt_test2.o ubt.o ubt_simple.o \
       strerror_test.o strerror.o strerror_tsd.o strerror_tls.o \
       one_time_init.o t_dirbasename.o m_dirbasename.a m_dirname.o m_basename.o \
       thread_cancel.o
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
thread_incr2: thread_incr2.o
thread_incr2.o: thread_incr2.c

t_ubt_test1: t_ubt_test1.o ubt.o
t_ubt_test1.o: t_ubt_test1.c ubt.h
t_ubt_test1s: t_ubt_test1.o ubt_simple.o
	$(CC) $(LDFLAGS) -o $@ $^
t_ubt_test2: t_ubt_test2.o ubt.o
t_ubt_test2.o: t_ubt_test2.c ubt.h
t_ubt_test2s: t_ubt_test2.o ubt_simple.o
	$(CC) $(LDFLAGS) -o $@ $^
ubt.o: ubt.c ubt.h
ubt_simple.o: ubt_simple.c ubt.h
strerror_test: strerror_test.o strerror.o
strerror_test.o: strerror_test.c
strerror.o: strerror.c
strerror_test_tsd: strerror_test.o strerror_tsd.o
	$(CC) $(LDFLAGS) -o $@ $^
strerror_tsd.o: strerror_tsd.c
strerror_test_tls: strerror_test.o strerror_tls.o
	$(CC) $(LDFLAGS) -o $@ $^
strerror_tls.o: strerror_tls.c
one_time_init: one_time_init.o
one_time_init.o: one_time_init.c
t_dirbasename: t_dirbasename.o m_dirbasename.a
t_dirbasename.o: t_dirbasename.c m_dirbasename.h
m_dirbasename.a: m_dirname.o m_basename.o
	$(AR) ru $@ $^
m_dirname.o: m_dirname.c m_dirbasename.h
m_basename.o: m_basename.c m_dirbasename.h
thread_cancel: thread_cancel.o
thread_cancel.o: thread_cancel.c
