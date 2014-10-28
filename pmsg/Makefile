TARGETS = pmsg_unlink pmsg_create pmsg_getattr pmsg_send pmsg_receive \
          mq_notify_sig mq_notify_thread
OBJS = pmsg_unlink.o pmsg_create.o pmsg_getattr.o pmsg_send.o pmsg_receive.o \
       mq_notify_sig.o mq_notify_thread.o
CFLAGS = -std=c99 -D_XOPEN_SOURCE=700 -g -Wall -Wextra
LDFLAGS = -lrt
RM = rm -f

all: $(TARGETS)
clean:
	$(RM) $(TARGETS) $(OBJS)
.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<

pmsg_unlink: pmsg_unlink.o
pmsg_unlink.o: pmsg_unlink.c
pmsg_create: pmsg_create.o
pmsg_create.o: pmsg_create.c
pmsg_getattr: pmsg_getattr.o
pmsg_getattr.o: pmsg_getattr.c
pmsg_send: pmsg_send.o
pmsg_send.o: pmsg_send.c
pmsg_receive: pmsg_receive.o
pmsg_receive.o: pmsg_receive.c
mq_notify_sig: mq_notify_sig.o
mq_notify_sig.o: mq_notify_sig.c
mq_notify_thread: mq_notify_thread.o
mq_notify_thread.o: mq_notify_thread.c
