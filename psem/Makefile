TARGETS = psem_create psem_unlink psem_wait psem_post psem_getvalue \
          thread_incr_psem psem_xfr psem_timedwait svsem_spec psem_spec
OBJS = psem_create.o psem_unlink.o psem_wait.o psem_post.o psem_getvalue.o \
       thread_incr_psem.o psem_xfr.o psem_timedwait.o svsem_spec.o psem_spec.o
CFLAGS = -std=c99 -D_XOPEN_SOURCE=700 -g -Wall -Wextra
LDFLAGS = -lrt
RM = rm -f

all: $(TARGETS)
clean:
	$(RM) $(TARGETS) $(OBJS)
.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<

psem_create: psem_create.o
psem_create.o: psem_create.c
psem_unlink: psem_unlink.o
psem_unlink.o: psem_unlink.c
psem_wait: psem_wait.o
psem_wait.o: psem_wait.c
psem_post: psem_post.o
psem_post.o: psem_post.c
psem_getvalue: psem_getvalue.o
psem_getvalue.o: psem_getvalue.c
thread_incr_psem: thread_incr_psem.o
thread_incr_psem.o: thread_incr_psem.c
psem_xfr: psem_xfr.o
psem_xfr.o: psem_xfr.c
psem_timedwait: psem_timedwait.o
psem_timedwait.o: psem_timedwait.c
svsem_spec: svsem_spec.o
svsem_spec.o: svsem_spec.c semun.h
psem_spec: psem_spec.o
psem_spec.o: psem_spec.c
