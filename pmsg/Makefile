TARGETS = pmsg_unlink pmsg_create pmsg_getattr pmsg_send pmsg_receive \
          mq_notify_sig mq_notify_thread pmsg_receive2 \
          pmsg_seqnum_server pmsg_seqnum_client \
          pmsg_file_server pmsg_file_client m_talk mq_notify_sig2 \
          mq_notify_sigwaitinfo
OBJS = pmsg_unlink.o pmsg_create.o pmsg_getattr.o pmsg_send.o pmsg_receive.o \
       mq_notify_sig.o mq_notify_thread.o pmsg_receive2.o \
       pmsg_seqnum_server.o pmsg_seqnum_client.o \
       pmsg_file_server.o pmsg_file_client.o m_talk.o mq_notify_sig2.o \
       mq_notify_sigwaitinfo.o
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
pmsg_receive2: pmsg_receive2.o
pmsg_receive2.o: pmsg_receive2.c
pmsg_seqnum_server: pmsg_seqnum_server.o
pmsg_seqnum_server.o: pmsg_seqnum_server.c pmsg_seqnum.h
pmsg_seqnum_client: pmsg_seqnum_client.o
pmsg_seqnum_client.o: pmsg_seqnum_client.c pmsg_seqnum.h
pmsg_file_server: pmsg_file_server.o
pmsg_file_server.o: pmsg_file_server.c pmsg_file.h
pmsg_file_client: pmsg_file_client.o
pmsg_file_client.o: pmsg_file_client.c pmsg_file.h
m_talk: m_talk.o
m_talk.o: m_talk.c
mq_notify_sig2: mq_notify_sig2.o
mq_notify_sig2.o: mq_notify_sig2.c
mq_notify_sigwaitinfo: mq_notify_sigwaitinfo.o
mq_notify_sigwaitinfo.o: mq_notify_sigwaitinfo.c
